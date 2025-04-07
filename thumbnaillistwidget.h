#pragma once

#include <QListWidget>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QStyledItemDelegate>
#include <QMainWindow>
#include <QDockWidget>


class ThumbnailListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit ThumbnailListWidget(QWidget *parent = nullptr);

    void addThumbnail(const QString &titleThumbnail, const QImage *thumbnail, const QString &seriesUID);
    QString selectedSeriesUID() const;

    void setActiveSeriesUID(const QString &uid); // для подсветки активной

    void sortThumbnails();

    void updateThumbnailsSize(Qt::DockWidgetArea area);





protected:
    void mousePressEvent(QMouseEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPoint m_dragStartPosition;
    QString m_activeUID;



signals:
    void requestShowInMainView(const QString &seriesUID);
    void requestDeleteSeries(const QString &seriesUID);
};
