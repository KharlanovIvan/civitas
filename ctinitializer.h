#pragma once

#include "IModalityInitializer.h"
#include <QDebug>

class CTInitializer : public IModalityInitializer {
public:
    virtual ~CTInitializer() override {}

    virtual void initializePipeline(VTKPipelineViewer* viewer, const QSharedPointer<DataDICOM>& data) override;

    virtual void initializeUI(MainWindow* mainWindow) override;
};
