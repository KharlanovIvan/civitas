#pragma once

#include <vtkImageMapToWindowLevelColors.h>

#include "IModalityInitializer.h"
#include "vtkpipelineviewer.h"
#include "mainwindow.h"
#include "custominteractorstyle2d.h"

#include <QDebug>

class CTInitializer : public IModalityInitializer {
public:
    virtual ~CTInitializer() override {}

    virtual void initializePipeline(QSharedPointer<VTKPipelineViewer> viewer, const QSharedPointer<DataDICOM>& data) override;

    virtual void initializeUI(MainWindow* mainWindow) override;
};
