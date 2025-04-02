#include "ctinitializer.h"

    void CTInitializer::initializePipeline(VTKPipelineViewer* viewer, const QSharedPointer<DataDICOM>& data) {
        qDebug() << "CTInitializer: Инициализация пайплайна для CT";
        // Здесь можно задать базовые фильтры, окно/уровень и прочее для CT
        // Например:
        // viewer->setWindowLevel(400, 40);
        // viewer->addFilterToFront("CT_Sharpen", someCTSpecificFilter);
        //viewer->initializePipeline(data);  // базовая инициализация
    }

    void CTInitializer::initializeUI(MainWindow* mainWindow) {
        qDebug() << "CTInitializer: Инициализация UI для CT";
        // Настраиваем меню, кнопки, панели инструментов для CT
        // mainWindow->setupCTSpecificTools();
    }
