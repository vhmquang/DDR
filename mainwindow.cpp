#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog.h>
#include <QMessageBox>
VTK_MODULE_INIT(vtkRenderingOpenGL2) //initialize object factory for Marching Cubes
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2) //initialize object factory for Ray Caster
VTK_MODULE_INIT(vtkInteractionStyle) //initialize object factory for default interation style in 2D

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QString lastUsedDir;
    QFileDialog *dir = new QFileDialog(this);
    dir->setParent(this);
    dir->setFileMode(QFileDialog::DirectoryOnly);
    QDir folderNameDCM;
    if (lastUsedDir.isNull()){
        dir->setDirectory(QDir::currentPath());
        if (dir->exec()){
            folderNameDCM = dir->directory();
            lastUsedDir = folderNameDCM.absolutePath();
            std::string stdstrFolderNameDCM = folderNameDCM.absolutePath().toUtf8().constData();
            drawDCMSeries(stdstrFolderNameDCM);
        }
    }
    else{
        dir->setDirectory(lastUsedDir);
        if(dir->exec()){
            folderNameDCM = dir->directory();
            lastUsedDir = folderNameDCM.absolutePath();
            std::string stdstrFolderNameDCM = folderNameDCM.absolutePath().toUtf8().constData();
            drawDCMSeries(stdstrFolderNameDCM);
        }
    };
}
void MainWindow::drawDCMSeries(std::string folderDCM)
{
    try{
        // folderDCM = "C:\\Users\\vhmquang\\Documents\\build-Master3D-Desktop_Qt_5_9_2_MSVC2017_64bit-Debug\\Marching Man";
        readerDCMSeries->SetDirectoryName(folderDCM.c_str()); //read the directory
        readerDCMSeries->Update(); //update directory
        imageViewerDCMSeries->SetInputConnection(readerDCMSeries->GetOutputPort()); //get dataset to viewer
        sliceMin = imageViewerDCMSeries->GetSliceMin();
        sliceMax = imageViewerDCMSeries->GetSliceMax();
        /* ui->_2D_Slider->setMinimum(sliceMin);
        ui->_2D_Slider->setMaximum(sliceMax);
        ui->_2D_Render_Window->GetRenderWindow()->AddRenderer(renderer);
        imageViewerDCMSeries->SetRenderWindow(ui->_2D_Render_Window->GetRenderWindow()); //set renderwindow to display
        imageViewerDCMSeries->SetSlice(sliceMin);
        imageViewerDCMSeries->Render();*/
        imageData = readerDCMSeries->GetOutput();
        imageDims = imageData->GetDimensions();
        double* _pixelSpacing = readerDCMSeries->GetPixelSpacing();
        qDebug() << imageDims[0];
        qDebug() << *(imageDims+1);
        qDebug() << *(imageDims+2);
        qDebug() << readerDCMSeries->GetPixelRepresentation();
        qDebug() << *_pixelSpacing;
        qDebug() << *(_pixelSpacing+1);
        qDebug() << *(_pixelSpacing+2);
        qDebug() << readerDCMSeries->GetBitsAllocated();
        qDebug() << readerDCMSeries->GetRescaleSlope();
    }
    catch (int err){ //catch error if there are no DICOM files
        if (err <=0){
            QMessageBox::critical(this, "DRR",
                                  "Folder you open does not contain any DICOM images, please retry .",
                                  QMessageBox::Ok);
        }
    }

}
double MainWindow::calPointValue(int beginX,int beginY,double pointX,double pointY,int Oz, double xSpacing, double ySpacing,double* data){
    double q11,  q12,  q21,  q22,  x1,  x2,  y1,  y2,x,y ;
    x = pointX * xSpacing;
    y = pointY * ySpacing;
    x1 = beginX * xSpacing;
    y1 = beginY * ySpacing;
    y2 = (beginY + 1) * xSpacing;
    x2 = (beginX + 1) * ySpacing;
    int q11Index = getOffSet(beginX,beginY,Oz,imageDims);
    int q12Index = getOffSet(beginX,beginY+1,Oz,imageDims);
    int q21Index = getOffSet(beginX+1,beginY,Oz,imageDims);
    int q22Index = getOffSet(beginX+1,beginY+1,Oz,imageDims);
    q11 = data[q11Index];
    q12 = data[q12Index];
    q21 = data[q21Index];
    q22 = data[q22Index];
    double result = bilinearInterpolation(q11,q12,q21,q22,x1,x2,y1,y2,x,y);
    return result;
}
int MainWindow::getOffSet(int x, int y, int z, int* dims){
    int offset = x + y * dims[1] + z * dims[1] * dims[0];
    return offset;
}

int MainWindow::getOffSet2D(int x, int y,int* dims){
    int offset = x+ y * dims[0];
    return offset;
}

int* MainWindow::indexTo3D (int index, int* dims){
    int z = index /(dims[0]*dims[1]);
    index -= (z * dims[0] * dims[1]);
    int y = index / dims[0];
    int x = index % dims[0];
    return new int[3]{x,y,z};
}

double MainWindow::bilinearInterpolation(double q11, double q12, double q21, double q22, double x1, double x2, double y1, double y2, double x, double y)
{
    double x2x1, y2y1, x2x, y2y, yy1, xx1;
    x2x1 = x2 - x1;
    y2y1 = y2 - y1;
    x2x = x2 - x;
    y2y = y2 - y;
    yy1 = y - y1;
    xx1 = x - x1;
    return 1.0 / (x2x1 * y2y1) * (
                q11 * x2x * y2y +
                q21 * xx1 * y2y +
                q12 * x2x * yy1 +
                q22 * xx1 * yy1
                );
}

void MainWindow::boundaryTrace(bool isFirst, int prevPoint, int currentPoint, QVector<double> inputDataVector){
    int* currentPointDims;
    int prevPointTemp = prevPoint;
    int currentPointTemp = currentPoint;
    currentPointDims = indexTo3D(currentPoint,resultDims);
    // check neu diem do nam trong vung` da~ trace roi thi return
    // check xem co phai la diem dau tien trong viec traceBoundary
    if (isFirst){
        resultVector.append(currentPoint);
        currentPointTemp = currentPoint;
        firstPoint = currentPoint;
        //qDebug() << finalIndex;
    }

    // tinh nextPoint = cach chay clockWiseTrace;
    int nextPoint;
    bool running = true;
    while (running){
        if (isFirst){
            nextPoint = clockWiseTrace(prevPointTemp,currentPointTemp,inputDataVector);
            prevPointTemp = currentPointTemp;
            currentPointTemp = nextPoint;
            resultVector.append(nextPoint);
            qDebug() << "Out of clockWiseTrace with CurrPoint = " << nextPoint  << "Prev Point" <<prevPointTemp << "First Time";
            isFirst = false;
        }
        else{
            nextPoint = clockWiseTrace(prevPointTemp,currentPointTemp,inputDataVector);
            if (nextPoint == -1){
                return;
            }
            prevPointTemp = currentPointTemp;
            currentPointTemp = nextPoint;
            resultVector.append(nextPoint);
            qDebug() << "Out of clockWiseTrace with CurrPoint = " << nextPoint  << "Prev Point" <<prevPointTemp;
        }
    }
    //boundaryTrace(xmin, xmax, ymin, ymax, false, currentPoint, nextPoint);
}


QVector<int> MainWindow::getAllIndexFromKDistance(int position, int distance, int startingPoint){
    QVector<int> resultArray ;
    QVector<int> topLeft ;
    QVector<int> topRight ;
    QVector<int> bottomLeft ;
    QVector<int> bottomRight;
    int* location = indexTo3D(position,resultDims);
    int x = location[0];
    int y = location[1];
    int width = numberOfAddedPoint;
    int height = numberOfAddedPoint;
    int result = 0;
    int tempX;
    int tempY;
    /*
     Case 1: Top Left
    */
    tempX = x - distance;
    for (int i = 0; i <= distance; i++){
        tempY = y + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,0,resultDims);
            topLeft.append(result);
        }
    }
    tempY = y + distance;
    for (int i = (-distance + 1); i < 0; i++){
        tempX = x + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,0,resultDims);
            topLeft.append(result);
        }
    }
    /*
    Case 2: Top Right
    */
    tempY = y + distance;
    for (int i = 0 ; i <= distance; i++){
        tempX = x + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,0,resultDims);
            topRight.append(result);
        }
    }
    tempX = x + distance;
    for (int i = (distance - 1); i > 0; i--){
        tempY = y + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,0,resultDims);
            topRight.append(result);
        }
    }
    /*
    Case 3: Bottom Right
    */
    tempX = x + distance;
    for (int i = 0 ; i <= distance; i++ ){
        tempY = y - i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,0,resultDims);
            bottomRight.append(result);
        }
    }
    tempY = y - distance;
    for (int i = (distance - 1) ; i > 0; i-- ){
        tempX = x + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,0,resultDims);
            bottomRight.append(result);
        }
    }

    /*
    Case 4: Right Line
    */
    tempY = y - distance;
    for (int i = 0; i <= distance ; i++ ){
        tempX = x - i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,0,resultDims);
            bottomLeft.append(result);
        }
    }
    tempX = x - distance;
    for (int i = (distance - 1 ); i > 0 ; i-- ){
        tempY = y - i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,0,resultDims);
            bottomLeft.append(result);
        }
    }
    switch (startingPoint){
    case 1:
        resultArray.append(bottomLeft);
        resultArray.append(topLeft);
        resultArray.append(topRight);
        resultArray.append(bottomRight);
        break;
    case 3:
        resultArray.append(topLeft);
        resultArray.append(topRight);
        resultArray.append(bottomRight);
        resultArray.append(bottomLeft);
        break;
    case 7:
        resultArray.append(topRight);
        resultArray.append(bottomRight);
        resultArray.append(bottomLeft);
        resultArray.append(topLeft);
        break;
    case 5:
        resultArray.append(bottomRight);
        resultArray.append(bottomLeft);
        resultArray.append(topLeft);
        resultArray.append(topRight);
        break;
    }
    return resultArray;
}

int MainWindow::checkStartingQuadra(int prevPoint, int currentPoint){
    int* prevLocation =  indexTo3D(prevPoint,resultDims);
    int* curLocation  = indexTo3D(currentPoint,resultDims);
    if (prevLocation[0] == 0 && prevLocation[1] == 0)
        return 1;
    if (prevLocation[1] == curLocation[1]){
        if ( prevLocation[0] < curLocation[0])
            return 3;
        if ( prevLocation[0] > curLocation[0])
            return 5;
    }
    if (prevLocation[1] < curLocation[1]){
        if (prevLocation[0] == curLocation[0])
            return 1;
        if (prevLocation[0] < curLocation[0])
            return 3;
        if (prevLocation[0] > curLocation[0])
            return 5;
    }
    if (prevLocation[1] > curLocation[1]){
        if (prevLocation[0] == curLocation[0])
            return 7;
        if (prevLocation[0] < curLocation[0])
            return 3;
        if (prevLocation[0] > curLocation[0])
            return 5;
    }
}
bool MainWindow::checkExistingPoint(int pointValue){
    bool testResult = true;
    int temp = finalIndex;
    int temp2;
    for (int i = 0; i < finalIndex; i++ ){
        temp2 = finalResult[i];
        //qDebug() << "Compare: " << temp2 <<"With point: " << pointValue;
        if ( temp2 == pointValue){
            testResult = false;
        }
    }
    return testResult;
}
// clockWiseTrace function, dung de tinh nextPoint index
int MainWindow::clockWiseTrace(int prevPoint, int currentPoint, QVector<double> inputDataVector){
    int startPosition = 0;
    QVector<int> allIndex;
    while (true){
        /*
        Check start position
        */
        startPosition = checkStartingQuadra(prevPoint,currentPoint);
        /*
        Obtain all index with each k, then query the result to determine interesting point
        */
        for (int i = 1; i<= numberOfAddedPoint;i++){
            allIndex = getAllIndexFromKDistance(currentPoint,i,startPosition);
            /*
             * query inside all adjacent neighbors
             */
            for (int k = 0; k < allIndex.size(); k++){
                //TODO: implement assertsion
                double value = inputDataVector.value(allIndex.value(k));
                if (value >= lowerBound && value <= upperBound){
                    if (allIndex.value(k) == firstPoint)
                        return -1;
                    QVector<int> test = resultVector;
                    if (!resultVector.contains(allIndex.value(k)))
                        return allIndex.value(k);
                }
            }
        }
    }
}

bool MainWindow::isBoundaryPoint(int x, int y ,int z, double lowerValue, double upperValue, int* dims){
    int x1 = x + 1;
    int x2 = x - 1 ;
    int y1 = y + 1;
    int y2 = y -1 ;
    bool isValid = false;
    int tempIndex = 0;
    try {
        tempIndex = getOffSet(x2,y1,z,dims);
        if (result[tempIndex] < lowerValue || result[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }  catch(int e) {
        qDebug() << e;
    };
    try{
        tempIndex = getOffSet(x2,y,z,dims);
        if (result[tempIndex] < lowerValue || result[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }  catch(int e) {
        qDebug() << e;
    }  ;
    try {
        tempIndex = getOffSet(x2,y2,z,dims);
        if (result[tempIndex] < lowerValue || result[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }  catch(int e) {
        qDebug() << e;
    };
    try {
        tempIndex = getOffSet(x,y2,z,dims);
        if (result[tempIndex] < lowerValue || result[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }  catch(int e) {
        qDebug() << e;
    };
    try{
        tempIndex = getOffSet(x,y1,z,dims);
        if (result[tempIndex] < lowerValue || result[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }  catch(int e) {
        qDebug() << e;
    };
    try{
        tempIndex = getOffSet(x1,y,z,dims);
        if (result[tempIndex] < lowerValue || result[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }  catch(int e) {
        qDebug() << e;
    };
    try {
        tempIndex = getOffSet(x1,y1,z,dims);
        if (result[tempIndex] < lowerValue || result[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }  catch(int e) {
        qDebug() << e;
    }
    try {
        tempIndex = getOffSet(x1,y2,z,dims);
        if (result[tempIndex] < lowerValue || result[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }  catch(int e) {
        qDebug() << e;
    };
    return isValid;

}
void MainWindow::on_actionSave_as_xyz_triggered()
{
    int width = *imageDims;
    int height = *(imageDims +1);
    int depth = *(imageDims+2);
    qDebug() << "depth" << depth;
    double* imageSpacing = imageData->GetSpacing();


    double xSpacing = *imageSpacing;
    double ySpacing = *(imageSpacing +1);
    double zSpacing = *(imageSpacing +2);
    numberOfAddedPoint = 0;
    double indexRatio = 0.0;
    indexRatio = zSpacing/xSpacing;
    numberOfAddedPoint = int(xSpacing*width/zSpacing);
    double* dataArray = new double[width*height*depth] ;
    // buffer 20 points
    resultSize = numberOfAddedPoint*numberOfAddedPoint*depth;
    qDebug() <<" added point" << numberOfAddedPoint;
    result = new double[resultSize];
    finalResult = new int[resultSize];
    qDebug() << "Result size " << resultSize;
    int simple[3] = {numberOfAddedPoint,numberOfAddedPoint,1};
    resultDims = simple;
    vtkIdType a = imageData->GetNumberOfPoints();
    double* spacing = readerDCMSeries->GetPixelSpacing();
    int maxDimension = numberOfAddedPoint*numberOfAddedPoint*depth;
    int _pointAdd = int(spacing[2]/spacing[1])+10;
    //Extract DICOM data
    for (int z = 0; z < imageDims[2]; z++ ){
        qDebug("Running Inside Image");
        for (int y = 0; y < imageDims[1];y++){
            for (int x = 0; x < imageDims[0]; x++){
                int index = getOffSet(x,y,z,imageDims);
                // int* result = indexTo3D(index,imageDims);
                // qDebug () << *result;
                double temp = imageData->GetScalarComponentAsDouble(x,y,z,0);
                dataArray[index] = temp;
                //qDebug () << imageData->GetScalarComponentAsDouble(x,y,z,0);
                //qDebug() << "Index at: " << index <<"\n";
                //qDebug () << "Data: " << dataArray[index];
            }
        }
    }

    //Calculate data for new grid
    for (int z = 0; z < imageDims[2]; z++ ){
        //    qDebug("Running Inside Image");
        QVector<double> tempVector;
        for (int y = 0; y < numberOfAddedPoint;y++){
            for (int x = 0; x < numberOfAddedPoint; x++){
                if ((fmod((x * indexRatio), xSpacing) == 0) && (fmod((y * indexRatio), ySpacing) == 0)){
                    int resultIndex = getOffSet(x,y,z,resultDims);
                    int dataIndex = getOffSet(x * indexRatio,y * indexRatio,z,imageDims);
                    result[resultIndex] =  dataArray[dataIndex];
                    tempVector.append(dataArray[dataIndex]);
                    //qDebug() << "x: " << x <<"\t y: "<< y << "\t z: "<<z <<"\n";
                    qDebug() << "Index: " << resultIndex << "Data: " << result[resultIndex];
                }
                else {
                    double pointX = x * indexRatio;
                    double pointY = y * indexRatio;
                    int i = static_cast<int> (floor(pointX));
                    int j = static_cast<int> (floor(pointY));
                    int resultIndex = getOffSet(x,y,z,resultDims);
                    result[resultIndex] = calPointValue(i,j,pointX,pointY,z,xSpacing,ySpacing,dataArray);
                    tempVector.append( calPointValue(i,j,pointX,pointY,z,xSpacing,ySpacing,dataArray));
                    //qDebug() << "Index: " << resultIndex << "Data: " << result[resultIndex];
                }
            }
        }
        modifiedDataVector.append(tempVector);
        QVector<QVector<double>> same = modifiedDataVector;
    }

    //filter data on new grid - old way
    /*   for (int i = 0; i < finalVector.size(); i++)
        for (int k = 0; k < tempVector.size(); k++)
            qDebug() << finalVector.value(i).value(k);
*/
    threshHold->SetInputData(imageData);
    //threshHold->ThresholdByUpper(400);
    for (int z = 0; z < resultDims[2]; z++){
        QVector<double> inputDataVector = modifiedDataVector.value(z);
        for (int i = 0; i < inputDataVector.size(); i++){
            if (inputDataVector.value(i) < upperBound && inputDataVector.value(i) > lowerBound){
                boundaryTrace(true,0,i,inputDataVector);
                break;
            }
        }
    }
    int count = 0;

    QString filename1 ="finalSolution200-1200.xyz";
    QFile file( filename1 );
    if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        QTextStream stream( &file );
        for (int i = 0; i < modifiedDataVector.value(0).size() ; i++){
            int* location = indexTo3D(modifiedDataVector.value(0).value(i),resultDims);
            double localX = location[0]*zSpacing;
            double localY = location[1]*zSpacing;
            double localZ = zSpacing;
            stream << localX << ' ';
            stream << localY << ' ';
            stream << localZ << ' ';
            count++;
            if(count == 1){
                stream << "\n";
                count = 0;
            }
        }
    }
}


void MainWindow::on__2D_Slider_valueChanged(int value)
{
    imageViewerDCMSeries->SetSlice(value);
    imageViewerDCMSeries->Render();
}

void MainWindow::on_actionGenerate_3D_Model_triggered()
{
    /* vtkSmartPointer<vtkImageDataGeometryFilter> imageFilter = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
    imageFilter->SetInputConnection(readerDCMSeries->GetOutputPort());
    imageFilter->SetExtent(0,300,0,300,0,300);
    imageFilter->SetThresholdValue(500);
    imageFilter->SetThresholdCells(500);
   // imageFilter->SetExtent(0,400,0,400,0,400);
    imageFilter->Update();*/
    qDebug("generate data");
    vtkSmartPointer<vtkXMLPolyDataReader> reader =
            vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName("range400.vtp");
    reader->Update();
    vtkSmartPointer<vtkDelaunay3D> delaunay3D =
            vtkSmartPointer<vtkDelaunay3D>::New();
    delaunay3D->SetInputConnection(reader->GetOutputPort());
    delaunay3D->SetAlphaTets(1);

    vtkSmartPointer<vtkPolyDataMapper> delaunayMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    delaunayMapper->SetInputConnection(delaunay3D->GetOutputPort());

    vtkSmartPointer<vtkActor> delaunayActor =
            vtkSmartPointer<vtkActor>::New();
    vtkSmartPointer<vtkRenderer> delaunayRenderer =
            vtkSmartPointer<vtkRenderer>::New();
    delaunayActor->SetMapper(delaunayMapper);
    // Create a renderer, render window, and interactor
    delaunayRenderer->AddActor(delaunayActor);
    delaunayRenderer->SetBackground(.4, .6, .3);
    ui->_2D_Render_Window->GetRenderWindow()->AddRenderer(delaunayRenderer);

    // Render and interact
    ui->_2D_Render_Window->GetRenderWindow()->Start();
    ui->_2D_Render_Window->GetRenderWindow()->Render();
}
