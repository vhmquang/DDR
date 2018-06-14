#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog.h>
#include <QMessageBox>
#define _USE_MATH_DEFINES
#include <math.h>
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
    int* result = new int[3];
    int z = index /(dims[0]*dims[1]);
    index -= (z * dims[0] * dims[1]);
    int y = index / dims[0];
    int x = index % dims[0];
    result[0] = x;
    result[1] = y;
    result[2] = z;
    return result;
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

QVector<int> MainWindow::getAllIndexFromKDistance(int position, int distance, int startingPoint, int z){
    QVector<int> resultArray ;
    QVector<int> topLeft ;
    QVector<int> topRight ;
    QVector<int> bottomLeft ;
    QVector<int> bottomRight;
    int* location = indexTo3D(position,resultDims);
    int x = location[0];
    int y = location[1];
    int width = resultDims[0];
    int height = resultDims[0];
    int result = 0;
    int tempX;
    int tempY;
    /*
     Case 1: Top Left
    */
    tempX = x - distance;
    for (int i = 0; i <= distance; i++){
        tempY = y - i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,z,resultDims);
            topLeft.append(result);
        }
    }
    tempY = y - distance;
    for (int i = (distance - 1); i > 0; i--){
        tempX = x - i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,z,resultDims);
            topLeft.append(result);
        }
    }
    /*
    Case 2: Top Right
    */
    tempY = y - distance;
    for (int i = 0 ; i <= distance; i++){
        tempX = x + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,z,resultDims);
            topRight.append(result);
        }
    }
    tempX = x + distance;
    for (int i = (distance - 1); i > 0; i--){
        tempY = y - i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,z,resultDims);
            topRight.append(result);
        }
    }
    /*
    Case 3: Bottom Right
    */
    tempX = x + distance;
    for (int i = 0 ; i <= distance; i++ ){
        tempY = y + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,z,resultDims);
            bottomRight.append(result);
        }
    }
    tempY = y + distance;
    for (int i = (distance - 1) ; i > 0; i-- ){
        tempX = x + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,z,resultDims);
            bottomRight.append(result);
        }
    }

    /*
    Case 4: Bottom Left
    */
    tempY = y + distance;
    for (int i = 0; i <= distance ; i++ ){
        tempX = x - i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,z,resultDims);
            bottomLeft.append(result);
        }
    }
    tempX = x - distance;
    for (int i = (distance - 1 ); i > 0 ; i-- ){
        tempY = y + i;
        if (tempX >= 0 && tempX < width && tempY >= 0 && tempY < height ){
            result = getOffSet(tempX,tempY,z,resultDims);
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
    delete []location;
    return resultArray;
}

int MainWindow::checkStartPosition(int prevPoint,int currPoint){
    int* currPosition = indexTo3D(currPoint,resultDims);
    int* prevPosition = indexTo3D(prevPoint,resultDims);
    int prevX = prevPosition[0];
    int currX = currPosition[0];
    int prevY = prevPosition[1];
    int currY = currPosition[1];
    if (prevPoint == 0){
        delete []currPosition;
        delete []prevPosition;
        return 1;
    }
    else if (prevX == currX && prevY > currY){
        delete []currPosition;
        delete []prevPosition;
        return 0;
    }
    else if (prevX == currX && prevY < currY){
        delete []currPosition;
        delete []prevPosition;
        return 1;
    }
    else if (prevX > currX){
        delete []currPosition;
        delete []prevPosition;
        return 1;
    }
    else{
        delete []currPosition;
        delete []prevPosition;
        return 0;
    }
}
int MainWindow::findNextPoint(int distance, int currPoint, int prevPoint, int z){
    lineFunction line;
    double angle;
    double stepAngle = 0.5;
    int* currPointLocation = indexTo3D(currPoint,resultDims);
    int* prevPointLocation = indexTo3D(prevPoint,resultDims);
    int position;
    int nextPointX;
    int nextPointY;
    int currentX = currPointLocation[0];
    int currentY = currPointLocation[1];
    int prevX = prevPointLocation[0];
    int prevY = prevPointLocation[1];
    int nextPoint;
    bool isPass;
    QVector<int> allIndexFromKDistance;
    if (prevX == currentX){
        if (prevY < currentY)
            angle = -90;
        else
            angle = 90;
    }
    else{
        angle = startingAngle(prevPoint,currPoint);
    }
    if (prevPoint == 0){
        angle = 89.5;
    }
    position = checkStartPosition(prevPoint,currPoint);
    for (int i = 0; i < 720; i++)
    {
        angle = angle + stepAngle;
        if (angle == -90 || angle == 90 || angle == 270 || angle == -270 || angle == 450  || angle == 630)
        {
            for (int k = 1 ; k <= distance; k++)
            {
                if (position == 0)
                {
                    nextPointX = currPointLocation[0];
                    nextPointY = currPointLocation[1] - k;
                    nextPoint = getOffSet(nextPointX,nextPointY,currPointLocation[2],resultDims);
                }
                else {
                    nextPointX = currPointLocation[0];
                    nextPointY = currPointLocation[1] + k;
                    nextPoint = getOffSet(nextPointX,nextPointY,currPointLocation[2],resultDims);
                }
                if (boundaryDataVector.contains(nextPoint))
                {
                    if (nextPoint == firstPoint)
                    {
                        delete []currPointLocation;
                        delete []prevPointLocation;
                        return -1;
                    }
                    else if(!tempVector.contains(nextPoint)){
                        // qDebug() <<"Return point x: \t" << nextPointX << "\t y: \t" << nextPointY;
                        delete []currPointLocation;
                        delete []prevPointLocation;
                        return nextPoint;
                    }
                }
            }
            if (position == 0){
                position = 1;
            }
            else {
                position = 0;
            }
        }
        else
        {
            line.setNewValue(angle,currPointLocation[0],currPointLocation[1]);
            for (int k = 1 ; k <= distance; k++)
            {
                allIndexFromKDistance = getAllIndexFromKDistance(currPoint,k,1,z);
                for (int vectorIndex = 0 ; vectorIndex < allIndexFromKDistance.size();vectorIndex++)
                {
                    nextPoint = allIndexFromKDistance.at(vectorIndex);
                    int* nextPointLocation = indexTo3D(nextPoint,resultDims);
                    isPass = line.isPassThroughPoint(nextPointLocation[0], nextPointLocation[1]);
                    nextPointX = nextPointLocation[0];
                    if (position == 0)
                    {
                        if (isPass && boundaryDataVector.contains(nextPoint) && nextPointX < currPointLocation[0])
                        {
                            if (nextPoint == firstPoint)
                            {
                                delete []currPointLocation;
                                delete []prevPointLocation;
                                delete []nextPointLocation;
                                return -1;
                            }
                            else if(!tempVector.contains(nextPoint)){
                                //   qDebug() <<"Return point x: \t" << nextPointX << "\t y: \t" << nextPointY;
                                delete []currPointLocation;
                                delete []prevPointLocation;
                                delete []nextPointLocation;
                                return nextPoint;
                            }
                        }
                    }
                    else {
                        if (isPass && boundaryDataVector.contains(nextPoint) && nextPointX  > currPointLocation[0])
                        {
                            if (nextPoint == firstPoint)
                            {
                                delete []currPointLocation;
                                delete []prevPointLocation;
                                delete []nextPointLocation;
                                return -1;
                            }
                            else if(!tempVector.contains(nextPoint)){
                                //   qDebug() <<"Return point x: \t" << nextPointX << "\t y: \t" << nextPointY;
                                delete []currPointLocation;
                                delete []prevPointLocation;
                                delete []nextPointLocation;
                                return nextPoint;
                            }
                        }
                    }
                    delete []nextPointLocation;
                }
            }
        }
    }
    qDebug() << "Can't find next point at distance" << distance;
    delete []currPointLocation;
    delete []prevPointLocation;
    return -2;
}

int MainWindow::findFirstPoint(int z){
    int index;
    for (int y = imageDims[1] - 1; y >=0  ;y--)
    {
        for (int x = 0; x < imageDims[0]; x++)
        {
            index = getOffSet(x,y,z,resultDims);
            if (boundaryDataVector.contains(index) && !blackListVector.contains(index))
            {
                return index;
            }
        }
    }
}
// clockWiseTrace function, dung de tinh nextPoint index
void MainWindow:: clockWiseTrace(int z){
    //TODO: Find first point, then find starting angle. For each stepAngle = 0.5, calculate lineQuation
    // then find all point around 0..k distance and check with equation.
    // if true then return point, compare with first point. If == FP, exist
    // else continue to find next point
    bool isRunning = true;
    bool isFirstPoint = true;
    int currPoint;
    int prevPoint;
    int nextPoint;
    int distance = 1;
    int maxDistance = 2;
    int countRetry = 0;
    int numberOfRetry = 2;
    while(isRunning){
        if (isFirstPoint)
        {
            currPoint = findFirstPoint(z);
            // qDebug() << "Find first point: " << currPoint << "at distance" << distance;
            firstPoint = currPoint;
            tempVector.append(firstPoint);
            isFirstPoint = false;
            nextPoint = findNextPoint(distance,currPoint,0,z);
            //qDebug() << "Find next point: " << nextPoint << "at distance" << distance;
            if (nextPoint == -1)
            {
                if (tempVector.size() < 15)
                {
                    qDebug() << "Retry count:" << countRetry;
                    countRetry++;
                    isFirstPoint = true;
                    tempVector.clear();
                    blackListVector.append(firstPoint);
                }
                else
                {
                    externalBoundaryVector.append(tempVector);
                    qDebug() << "Fi nish extract boundary of slice: " << z << "with external point: " << externalBoundaryVector.size();
                    return;
                }
            }
            if (nextPoint == -2)
            {
                if(countRetry >= numberOfRetry)
                {
                    qDebug() << "Retry count:" << countRetry;
                    distance++;
                    isFirstPoint = true;
                    tempVector.clear();
                    countRetry = 0;
                    //qDebug() << "Retry: " << countRetry;
                }
                else
                {
                    qDebug() << "Retry count:" << countRetry;
                    countRetry++;
                    isFirstPoint = true;
                    tempVector.clear();
                    blackListVector.append(firstPoint);
                }
            }
            if (nextPoint >= 0)
            {
                tempVector.append(nextPoint);
                prevPoint = currPoint;
                currPoint = nextPoint;
            }
        }
        else
        {
            nextPoint = findNextPoint(distance,currPoint,prevPoint,z);
           // qDebug() << "Find next point: " << nextPoint << "at distance" << distance;
            if (nextPoint == -1)
            {
                if (tempVector.size() < 15)
                {
                    qDebug() << "Retry count:" << countRetry;
                    countRetry++;
                    isFirstPoint = true;
                    tempVector.clear();
                    blackListVector.append(firstPoint);
                }
                else
                {
                    externalBoundaryVector.append(tempVector);
                    qDebug() << "Finish extract boundary of slice: " << z << "with external point: " << externalBoundaryVector.size();
                    return;
                }
            }
            if (nextPoint == -2)
            {
                if(countRetry >= numberOfRetry)
                {
                    qDebug() << "Retry count:" << countRetry;
                    distance++;
                    isFirstPoint = true;
                    tempVector.clear();
                    countRetry = 0;
                }
                else
                {
                    qDebug() << "Retry count:" << countRetry;
                    countRetry++;
                    isFirstPoint = true;
                    tempVector.clear();
                    blackListVector.append(firstPoint);
                }
            }
            if (nextPoint >= 0)
            {
                tempVector.append(nextPoint);
                prevPoint = currPoint;
                currPoint = nextPoint;
            }

        }
        if (distance > maxDistance)
        {
            if (lowerBound > maxLowerBound)
            {
                qDebug() << "Can't extract external boundary of slice: " << z;
                return;
            }
            else
            {
                lowerBound = lowerBound + stepBound;
                distance = 1;
                isFirstPoint = true;
                countRetry = 0;
                filterDataVector.clear();
                boundaryDataVector.clear();
                tempVector.clear();
                blackListVector.clear();
                filterVectorByThreshold(lowerBound, upperBound, resultDims,z);
                findBoundaryPoint();
                qDebug() << "Increase lowerBound by 5 to: " << lowerBound;
            }
        }
    }
}

void MainWindow:: extractDICOMData(){
    qDebug() <<"Extract DICOM data";
    int width = *imageDims;
    int height = *(imageDims +1);
    int depth = *(imageDims+2);
    double temp;
    int index;
    dataArray = new double[width*height*depth] ;
    for (int z = 0; z < imageDims[2]; z++ ){
        for (int y = imageDims[1] - 1; y >=0  ;y--){
            for (int x = 0; x < imageDims[0]; x++){
                index = getOffSet(x,y,z,imageDims);
                temp = imageData->GetScalarComponentAsDouble(x,y,z,0);
                dataArray[index] = temp;
            }
        }
    }
}

void MainWindow::findBoundaryPoint(){
    int filterDataIndex;
    int x;
    int y;
    int z;
    for (int i = 0; i < filterDataVector.size(); i++)
    {
        filterDataIndex = filterDataVector.at(i);
        int* filterDataLocation = indexTo3D(filterDataIndex,resultDims);
        x = filterDataLocation[0];
        y = filterDataLocation[1];
        z = filterDataLocation[2];
        bool isBoundary = isBoundaryPoint(x,y,z,lowerBound,upperBound,resultDims);
        if (isBoundary){
            boundaryDataVector.append(filterDataIndex);
        }
        delete []filterDataLocation;
    }
    qDebug() << "Size of boundary point" << boundaryDataVector.size();
}
double MainWindow::startingAngle(int prevPoint, int currPoint){
    double angle = 0;
    if (prevPoint == 0){
        return angle = 90;
    }
    int* prevLocation = indexTo3D(prevPoint,resultDims);
    int* currLocation = indexTo3D(currPoint,resultDims);
    angle =  atan2(prevLocation[1] - currLocation[1], prevLocation[0] - currLocation[0])* 180 / M_PI;
    delete []prevLocation;
    delete []currLocation;
    return angle;
}

bool MainWindow::isBoundaryPoint(int x, int y ,int z,double lowerValue, double upperValue, int* dims){
    int x1 = x + 1;
    int x2 = x - 1 ;
    int y1 = y + 1;
    int y2 = y -1 ;
    bool isValid = false;
    int tempIndex = 0;
    if (x1 < dims[0] && x2 >= 0 && y1 < dims[1] && y2 >= 0){
        tempIndex = getOffSet(x2,y1,z,dims);
        if (dataArray[tempIndex] < lowerValue || dataArray[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }
    if (x1 < dims[0] && x2 >= 0 && y1 < dims[1] && y2 >= 0){
        tempIndex = getOffSet(x2,y,z,dims);
        if (dataArray[tempIndex] < lowerValue || dataArray[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }
    if (x1 < dims[0] && x2 >= 0 && y1 < dims[1] && y2 >= 0){
        tempIndex = getOffSet(x2,y2,z,dims);
        if (dataArray[tempIndex] < lowerValue || dataArray[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }
    if (x1 < dims[0] && x2 >= 0 && y1 < dims[1] && y2 >= 0){
        tempIndex = getOffSet(x,y2,z,dims);
        if (dataArray[tempIndex] < lowerValue || dataArray[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }
    if (x1 < dims[0] && x2 >= 0 && y1 < dims[1] && y2 >= 0){
        tempIndex = getOffSet(x,y1,z,dims);
        if (dataArray[tempIndex] < lowerValue || dataArray[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }
    if (x1 < dims[0] && x2 >= 0 && y1 < dims[1] && y2 >= 0){
        tempIndex = getOffSet(x1,y,z,dims);
        if (dataArray[tempIndex] < lowerValue || dataArray[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }
    if (x1 < dims[0] && x2 >= 0 && y1 < dims[1] && y2 >= 0){
        tempIndex = getOffSet(x1,y1,z,dims);
        if (dataArray[tempIndex] < lowerValue || dataArray[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }
    if (x1 < dims[0] && x2 >= 0 && y1 < dims[1] && y2 >= 0){
        tempIndex = getOffSet(x1,y2,z,dims);
        if (dataArray[tempIndex] < lowerValue || dataArray[tempIndex] > upperValue)
        {   isValid = true;
            return isValid;
        }
    }
    return isValid;
}
void MainWindow::on_actionSave_as_xyz_triggered()
{ 
    int width = *imageDims;
    int height = *(imageDims +1);
    int depth = *(imageDims+2);
    double* imageSpacing = imageData->GetSpacing();
    double xSpacing = *imageSpacing;
    double ySpacing = *(imageSpacing +1);
    double zSpacing = *(imageSpacing +2);
    numberOfAddedPoint = 0;
    double indexRatio = 0.0;
    indexRatio = zSpacing/xSpacing;
    numberOfAddedPoint = int(xSpacing*width/zSpacing);
    // buffer 20 points
    resultSize = numberOfAddedPoint*numberOfAddedPoint*depth;
    result = new double[resultSize];
    finalResult = new int[resultSize];
    int simple[3] = {numberOfAddedPoint,numberOfAddedPoint,1};
    resultDims = imageDims;
    vtkIdType a = imageData->GetNumberOfPoints();
    double* spacing = readerDCMSeries->GetPixelSpacing();
    int maxDimension = numberOfAddedPoint*numberOfAddedPoint*depth;
    int _pointAdd = int(spacing[2]/spacing[1])+10;
    //Extract DICOM data
    extractDICOMData();
    /*
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
    }*/
    for (int z = 0; z < resultDims[2]; z++){
        filterDataVector.clear();
        boundaryDataVector.clear();
        tempVector.clear();
        // qDebug() << "Size of filter boundary tempo external: " << filterDataVector.size() << boundaryDataVector.size()<<tempVector.size() <<externalBoundaryVector.size();
        lowerBound = defaultLowerBound;
        blackListVector.clear();
        filterVectorByThreshold(lowerBound, upperBound, resultDims, z);
        findBoundaryPoint();
        clockWiseTrace(z);
    }
    //qDebug() << externalBoundaryVector.size();
    printXYZfile("filterDataVector.xyz",filterDataVector,resultDims,imageSpacing);
    printXYZfile("boundaryDataVector.xyz",boundaryDataVector,resultDims,imageSpacing);
    printXYZfile("externalBoundaryData.xyz",externalBoundaryVector,resultDims,imageSpacing);

}

void MainWindow::printXYZfile(QString filename, QVector<int> data, int *dims, double *spacing){
    QString filename1 = filename;
    QFile file( filename1 );
    double localX;
    double localY;
    double localZ;
    if ( file.open(QIODevice::ReadWrite | QIODevice::Text) )
    {
        QTextStream stream( &file );
        for (int i = 0; i < data.size() ; i++)
        {
            int* location = indexTo3D(data.value(i),dims);
            localX = location[0] * spacing[0];
            localY= location[1] * spacing[1];
            localZ = location[2] * spacing[2];
            stream << localX << ' ';
            stream << localY << ' ';
            stream << localZ << ' ';
            stream << '\n';
            delete []location;
        }
    }
}
void MainWindow::filterVectorByThreshold(double  lower, double upper, int* dims,int z){
    for (int y = dims[1] - 1; y >=0 ; y--)
    {
        for (int x = 0; x < dims[0]; x++)
        {
            int tempIndex = getOffSet(x,y,z,dims);
            if (dataArray[tempIndex] <= upper && dataArray[tempIndex] >= lower)
            {
                filterDataVector.append(tempIndex);
            }
        }
    }
    qDebug() << "Size of filterDataVector" << filterDataVector.size();
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
