#pragma once

#include <QSharedPointer>

// Предварительные объявления классов, которые могут понадобиться
class VTKPipelineViewer;
class DataDICOM;
class MainWindow;

class IModalityInitializer {
public:
    virtual ~IModalityInitializer() {}

    // Метод для инициализации VTK-пайплайна с учётом особенностей модальности
    virtual void initializePipeline(VTKPipelineViewer* viewer, const QSharedPointer<DataDICOM>& data) = 0;

    // Метод для инициализации интерфейса (панели инструментов, меню, кнопки) для данной модальности
    virtual void initializeUI(MainWindow* mainWindow) = 0;
};
