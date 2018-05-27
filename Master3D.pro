#-------------------------------------------------
#
# Project created by QtCreator 2017-12-29T10:15:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Master3D
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = Master3D
TEMPLATE = app

SOURCES += main.cpp \
        mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui
INCLUDEPATH += "C:\Program Files\VTK\include\vtk-8.0"
LIBS += "-LD:\VTK\VTK-Build\lib\Debug"
LIBS += -lvtkzlib-8.0
LIBS += -lvtkalglib-8.0
LIBS += -lvtkChartsCore-8.0
LIBS += -lvtkCommonColor-8.0
LIBS += -lvtkCommonComputationalGeometry-8.0
LIBS += -lvtkCommonCore-8.0
LIBS += -lvtkCommonDataModel-8.0
LIBS += -lvtkCommonExecutionModel-8.0
LIBS += -lvtkCommonMath-8.0
LIBS += -lvtkCommonMisc-8.0
LIBS += -lvtkCommonSystem-8.0
LIBS += -lvtkCommonTransforms-8.0
LIBS += -lvtkDICOMParser-8.0
LIBS += -lvtkDomainsChemistry-8.0
LIBS += -lvtkDomainsChemistryOpenGL2-8.0
LIBS += -lvtkexoIIc-8.0
LIBS += -lvtkexpat-8.0
LIBS += -lvtkFiltersAMR-8.0
LIBS += -lvtkFiltersCore-8.0
LIBS += -lvtkFiltersExtraction-8.0
LIBS += -lvtkFiltersFlowPaths-8.0
LIBS += -lvtkFiltersGeneral-8.0
LIBS += -lvtkFiltersGeneric-8.0
LIBS += -lvtkFiltersGeometry-8.0
LIBS += -lvtkFiltersHybrid-8.0
LIBS += -lvtkFiltersHyperTree-8.0
LIBS += -lvtkFiltersImaging-8.0
LIBS += -lvtkFiltersModeling-8.0
LIBS += -lvtkFiltersParallel-8.0
LIBS += -lvtkFiltersParallelImaging-8.0
LIBS += -lvtkFiltersPoints-8.0
LIBS += -lvtkFiltersProgrammable-8.0
LIBS += -lvtkFiltersSelection-8.0
LIBS += -lvtkFiltersSMP-8.0
LIBS += -lvtkFiltersSources-8.0
LIBS += -lvtkFiltersStatistics-8.0
LIBS += -lvtkFiltersTexture-8.0
LIBS += -lvtkFiltersTopology-8.0
LIBS += -lvtkFiltersVerdict-8.0
LIBS += -lvtkfreetype-8.0
LIBS += -lvtkGeovisCore-8.0
LIBS += -lvtkgl2ps-8.0
LIBS += -lvtkglew-8.0
LIBS += -lvtkGUISupportQt-8.0
LIBS += -lvtkGUISupportQtSQL-8.0
LIBS += -lvtkhdf5_hl-8.0
LIBS += -lvtkhdf5-8.0
LIBS += -lvtkImagingColor-8.0
LIBS += -lvtkImagingCore-8.0
LIBS += -lvtkImagingFourier-8.0
LIBS += -lvtkImagingGeneral-8.0
LIBS += -lvtkImagingHybrid-8.0
LIBS += -lvtkImagingMath-8.0
LIBS += -lvtkImagingMorphological-8.0
LIBS += -lvtkImagingSources-8.0
LIBS += -lvtkImagingStatistics-8.0
LIBS += -lvtkImagingStencil-8.0
LIBS += -lvtkInfovisCore-8.0
LIBS += -lvtkInfovisLayout-8.0
LIBS += -lvtkInteractionImage-8.0
LIBS += -lvtkInteractionStyle-8.0
LIBS += -lvtkInteractionWidgets-8.0
LIBS += -lvtkIOAMR-8.0
LIBS += -lvtkIOCore-8.0
LIBS += -lvtkIOEnSight-8.0
LIBS += -lvtkIOExodus-8.0
LIBS += -lvtkIOExport-8.0
LIBS += -lvtkIOExportOpenGL2-8.0
LIBS += -lvtkIOGeometry-8.0
LIBS += -lvtkIOImage-8.0
LIBS += -lvtkIOImport-8.0
LIBS += -lvtkIOInfovis-8.0
LIBS += -lvtkIOLegacy-8.0
LIBS += -lvtkIOLSDyna-8.0
LIBS += -lvtkIOMINC-8.0
LIBS += -lvtkIOMovie-8.0
LIBS += -lvtkIONetCDF-8.0
LIBS += -lvtkIOParallel-8.0
LIBS += -lvtkIOParallelXML-8.0
LIBS += -lvtkIOPLY-8.0
LIBS += -lvtkIOSQL-8.0
LIBS += -lvtkIOTecplotTable-8.0
LIBS += -lvtkIOVideo-8.0
LIBS += -lvtkIOXML-8.0
LIBS += -lvtkIOXMLParser-8.0
LIBS += -lvtkjpeg-8.0
LIBS += -lvtkjsoncpp-8.0
LIBS += -lvtklibharu-8.0
LIBS += -lvtklibxml2-8.0
LIBS += -lvtklz4-8.0
LIBS += -lvtkmetaio-8.0
LIBS += -lvtknetcdf_c++
LIBS += -lvtkNetCDF-8.0
LIBS += -lvtkoggtheora-8.0
LIBS += -lvtkParallelCore-8.0
LIBS += -lvtkpng-8.0
LIBS += -lvtkproj4-8.0
LIBS += -lvtkRenderingAnnotation-8.0
LIBS += -lvtkRenderingContext2D-8.0
LIBS += -lvtkRenderingContextOpenGL2-8.0
LIBS += -lvtkRenderingCore-8.0
LIBS += -lvtkRenderingFreeType-8.0
LIBS += -lvtkRenderingGL2PSOpenGL2-8.0
LIBS += -lvtkRenderingImage-8.0
LIBS += -lvtkRenderingLabel-8.0
LIBS += -lvtkRenderingLOD-8.0
LIBS += -lvtkRenderingOpenGL2-8.0
LIBS += -lvtkRenderingQt-8.0
LIBS += -lvtkRenderingVolume-8.0
LIBS += -lvtkRenderingVolumeOpenGL2-8.0
LIBS += -lvtksqlite-8.0
LIBS += -lvtksys-8.0
LIBS += -lvtktiff-8.0
LIBS += -lvtkverdict-8.0
LIBS += -lvtkViewsContext2D-8.0
LIBS += -lvtkViewsCore-8.0
LIBS += -lvtkViewsInfovis-8.0
LIBS += -lvtkViewsQt-8.0
