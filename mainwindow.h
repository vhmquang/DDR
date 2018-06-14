#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vtkDICOMImageReader.h>
#include <vtkThreshold.h>
#include <vtkImageData.h>
#include <vtkSimplePointsWriter.h>
#include <vtkImageViewer2.h>
#include <vtkOpenGLRenderer.h>
#include <vtkRenderingOpenGL2ObjectFactory.h>
#include <vtkSimplePointsWriter.h>
#include <vtkSmartPointer.h>
#include <vtkRenderingOpenGL2Module.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderingCoreModule.h>
#include <vtkImageSliceMapper.h>
#include <vtkRenderingOpenGL2ObjectFactory.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkWin32OutputWindow.h>
#include <vtkPolyData.h>
#include <vtkDelaunay3D.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkCleanPolyData.h>
#include <vtkSimplePointsReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkDataSet.h>
#include <vtkPointSet.h>
#include <vtkAutoInit.h>
#include <vtkXMLImageDataWriter.h>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>
#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include <math.h>
#include <linefunction.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_Folder_triggered();

    void on_actionSave_as_xyz_triggered();

    void on__2D_Slider_valueChanged(int value);

    void on_actionGenerate_3D_Model_triggered();

    int getOffSet(int x, int y, int z, int* dims);

    bool isBoundaryPoint(int x, int y , int z, double lowerValue, double upperValue, int* dims);

    int* indexTo3D(int index, int* dims);

    double bilinearInterpolation(double q11, double q12, double q21, double q22, double x1, double x2, double y1, double y2, double x, double y);

    double calPointValue(int beginX,int beginY,double pointX,double pointY,int Oz, double xSpacing, double ySpacing,double* data);

    QVector<int>  getAllIndexFromKDistance(int position, int distance, int startingPoint, int z);

    void clockWiseTrace(int z);

    int findNextPoint(int distance, int currPoint, int prevPoint, int z);

    int getOffSet2D(int x, int y, int* dims);

    int checkStartPosition(int prevPoint, int currPoint);

    void printXYZfile(QString filename, QVector<int> data, int* dims, double *spacing);

    void filterVectorByThreshold(double  lower, double upper, int *dims, int z);

    double startingAngle(int prevPoint, int currPoint);

    int findFirstPoint(int z);

    void extractDICOMData();

    void findBoundaryPoint();
private:
    Ui::MainWindow *ui;

    void drawDCMSeries(std::string folderDCM);
    int sliceMin;
    int finalIndex = 0;
    int sliceMax;
    int* imageDims;
    double* result;
    int firstPoint;
    int resultSize;
    int* finalResult;
    double lowerBound = 90;
    double defaultLowerBound = 90;
    double upperBound = 300;
    double maxLowerBound = 150;
    double stepBound = 5;
    QVector<int> resultVector;
    QVector<QVector<double>> modifiedDataVector;
    QVector<int> tempVector;
    QVector<QVector<int>> finalVector;
    QVector<int> filterDataVector;
    QVector<int> boundaryDataVector;
    QVector<int> externalBoundaryVector;
    QVector<int> blackListVector;
    int* resultDims;
    int   numberOfAddedPoint = 0;
    double* dataArray;
    vtkSmartPointer<vtkDICOMImageReader> readerDCMSeries = vtkSmartPointer<vtkDICOMImageReader>::New();
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkThreshold> threshHold = vtkSmartPointer<vtkThreshold>::New();
    vtkSmartPointer<vtkOpenGLRenderer> renderer = vtkSmartPointer<vtkOpenGLRenderer>::New();
    vtkSmartPointer<vtkSimplePointsWriter> xyzWriter = vtkSmartPointer<vtkSimplePointsWriter>::New();
    vtkSmartPointer<vtkImageViewer2> imageViewerDCMSeries = vtkSmartPointer<vtkImageViewer2>::New();
    vtkSmartPointer<vtkImageDataGeometryFilter> imageFilter = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
    vtkSmartPointer<vtkDelaunay3D> delaunay3D = vtkSmartPointer<vtkDelaunay3D>::New();
    enum startPosition { left = 0, right = 1};

};

#endif // MAINWINDOW_H
