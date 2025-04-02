#include "mrinitializer.h"



void MRInitializer::initializePipeline(VTKPipelineViewer* viewer, const QSharedPointer<DataDICOM>& data)  {
    qDebug() << "MRInitializer: Инициализация пайплайна для MR";
    // Здесь можно задать другие фильтры или параметры для MR
    //viewer->initializePipeline(data);  // базовая инициализация
}

void MRInitializer::initializeUI(MainWindow* mainWindow)  {
    qDebug() << "MRInitializer: Инициализация UI для MR";
    // Настраиваем меню, кнопки, панели инструментов для MR
    // mainWindow->setupMRSpecificTools();
}
