#pragma once

#include "IModalityInitializer.h"
#include <QDebug>

class MRInitializer : public IModalityInitializer {
public:
    virtual ~MRInitializer() override {}

    virtual void initializePipeline(QSharedPointer<VTKPipelineViewer> viewer, const QSharedPointer<DataDICOM>& data) override;

    virtual void initializeUI(MainWindow* mainWindow) override;
};
