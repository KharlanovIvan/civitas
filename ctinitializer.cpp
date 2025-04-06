#include "ctinitializer.h"

    void CTInitializer::initializePipeline(QSharedPointer<VTKPipelineViewer> viewer, const QSharedPointer<DataDICOM>& data) {
        qDebug() << "CTInitializer: Инициализация пайплайна для CT";

        viewer->initializePipeline(data);

        // Настройка фильтров

        viewer->setFlipFilter(1);

        viewer->setWindowLevelFilter(data->getWindowWidth(), data->getWindowCenter());

        vtkSmartPointer<CustomInteractorStyle2D> CTInteractorStyle = vtkSmartPointer<CustomInteractorStyle2D>::New();
        viewer->setInteractorStyle(CTInteractorStyle);


    }

    void CTInitializer::initializeUI(MainWindow* mainWindow) {
        qDebug() << "CTInitializer: Инициализация UI для CT:";

        qDebug() << "\tИнициализация базовых инструментов КТ...";
        // Инициализация базовых инструментов КТ
        mainWindow->initCTBasicToolbar();
    }
