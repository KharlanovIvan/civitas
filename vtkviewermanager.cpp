// VTKViewerManager.cpp
#include "vtkviewermanager.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDebug>
#include <qapplication.h>

VTKViewerManager::VTKViewerManager(QWidget* parent) : QWidget(parent) {

    setAcceptDrops(true);
    splitter = new QSplitter(Qt::Horizontal, this);
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(splitter);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void VTKViewerManager::addViewer(QSharedPointer<VTKPipelineViewer> viewer) {
    if (viewers.size() >= 10) return;
    viewers.append(viewer);
    splitter->addWidget(viewer->getVTKWidget());
    connectViewerSignals(viewer, viewers.size() - 1);
    updateLayout();
}

void VTKViewerManager::replaceViewer(int index, QSharedPointer<VTKPipelineViewer> viewer) {
    if (index < 0 || index >= viewers.size()) return;
    QWidget* oldWidget = viewers[index]->getVTKWidget();
    splitter->replaceWidget(index, viewer->getVTKWidget());
    viewers[index] = viewer;
    connectViewerSignals(viewer, index);
    delete oldWidget;
    updateLayout();
}

void VTKViewerManager::removeViewer(int index) {
    if (index < 0 || index >= viewers.size()) return;
    QWidget* widget = viewers[index]->getVTKWidget();
    splitter->widget(index)->setParent(nullptr);
    widget->deleteLater();
    viewers.removeAt(index);
    updateLayout();
}

void VTKViewerManager::clearAllViewers() {
    for (auto& viewer : viewers) {
        viewer->getVTKWidget()->deleteLater();
    }
    viewers.clear();
    updateLayout();
}

void VTKViewerManager::setDataForViewer(int index, QSharedPointer<DataDICOM> data) {
    if (index >= 0 && index < viewers.size()) {
        viewers[index]->initializePipeline(data);
    }
}

void VTKViewerManager::activateViewer(int index) {
    if (index >= 0 && index < viewers.size()) {
        activeViewerIndex = index;
        compareViewerIndex = -1;
        updateLayout();
    }
}

void VTKViewerManager::compareWithViewer(int index) {
    if (index >= 0 && index < viewers.size() && index != activeViewerIndex) {
        compareViewerIndex = index;
        updateLayout();
    }
}

QVector<QSharedPointer<VTKPipelineViewer>> VTKViewerManager::getViewers() const {
    return viewers;
}

void VTKViewerManager::updateLayout() {
    QList<int> sizes;
    int total = width();
    int minSize = 100;

    if (viewers.size() <= 2) {
        for (int i = 0; i < viewers.size(); ++i)
            sizes << total / viewers.size();
    } else {
        int small = total / (viewers.size() + 3); // большой + n малых
        int large = total - (small * (viewers.size() - 1));

        for (int i = 0; i < viewers.size(); ++i) {
            if (i == activeViewerIndex || (i == compareViewerIndex))
                sizes << large / (compareViewerIndex != -1 ? 2 : 1);
            else
                sizes << small;
        }
    }
    splitter->setSizes(sizes);
    highlightActiveViewer();
}

void VTKViewerManager::highlightActiveViewer() {
    for (int i = 0; i < viewers.size(); ++i) {
        QString border = (i == activeViewerIndex || i == compareViewerIndex)
        ? "2px solid red" : "1px solid gray";
        viewers[i]->getVTKWidget()->setStyleSheet("border:" + border);
    }
}

void VTKViewerManager::connectViewerSignals(QSharedPointer<VTKPipelineViewer> viewer, int index) {
    QObject::connect(viewer.data(), &VTKPipelineViewer::clicked, this, [=]() {
        if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
            compareWithViewer(index);
        } else {
            activateViewer(index);
        }
        emit viewerClicked(index);
    });
}

int VTKViewerManager::findViewerIndex(QWidget* widget) {
    for (int i = 0; i < viewers.size(); ++i) {
        if (viewers[i]->getVTKWidget() == widget)
            return i;
    }
    return -1;
}

void VTKViewerManager::mousePressEvent(QMouseEvent* event) {
    QWidget* clicked = childAt(event->pos());
    int index = findViewerIndex(clicked);
    if (index != -1) {
        if (event->modifiers() & Qt::ControlModifier) {
            compareWithViewer(index);
        } else {
            activateViewer(index);
        }
    }
}

void VTKViewerManager::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-seriesuid")) {
        event->acceptProposedAction();
    }
}

void VTKViewerManager::dropEvent(QDropEvent* event) {
    if (!event->mimeData()->hasFormat("application/x-seriesuid")) return;
    QByteArray uidData = event->mimeData()->data("application/x-seriesuid");
    QString seriesUID(uidData);
    qDebug() << "Drop series: " << seriesUID;
    // emit signal or load here (implementation specific)
    event->acceptProposedAction();
}
