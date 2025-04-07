// VTKViewerManager.h
#pragma once

#include <QWidget>
#include <QVector>
#include <QSharedPointer>
#include <QSplitter>
#include <QMap>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QApplication>

#include "vtkpipelineviewer.h"

class VTKViewerManager : public QWidget {
    Q_OBJECT
public:
    explicit VTKViewerManager(QWidget* parent = nullptr);

    void addViewer(QSharedPointer<VTKPipelineViewer> viewer);
    void replaceViewer(int index, QSharedPointer<VTKPipelineViewer> viewer);
    void removeViewer(int index);
    void clearAllViewers();

    void setDataForViewer(int index, QSharedPointer<DataDICOM> data);
    void activateViewer(int index);
    void compareWithViewer(int index);

    QVector<QSharedPointer<VTKPipelineViewer>> getViewers() const;

signals:
    void viewerClicked(int index);
    void viewerCompareRequested(int index);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:

    QSplitter* mainSplitter;
    QSplitter* leftSplitter;
    QSplitter* rightSplitter;




    QVector<QSharedPointer<VTKPipelineViewer>> viewers;
    QSplitter* splitter;
    int activeViewerIndex = -1;
    int compareViewerIndex = -1;

    void updateLayout();
    void highlightActiveViewer();
    void connectViewerSignals(QSharedPointer<VTKPipelineViewer> viewer, int index);

    int findViewerIndex(QWidget* widget);
};
