#pragma once

#include <QDockWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QResizeEvent>
#include <QMainWindow>  // Добавляем заголовочный файл
#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QApplication>

#include "thumbnaillistwidget.h"

class Gallery : public QDockWidget {
    Q_OBJECT

    bool isResizing = false; // Флаг для предотвращения рекурсии
public:
    explicit Gallery(QWidget *parent = nullptr);

    ~Gallery();

    // Метод для добавления миниатюры в галерею
    void addThumbnail(const QString &titleThumbnail, const QImage *thumbnail, const QString &seriesUID);

    // Метод для получения выбранного элемента
    QString getSelectedThumbnailUID() const;

    QString getSelectedThumbnailTitle() const;

    // Метод для очистки списка миниатюр
    void clearThumbnails();

    void sortThumbnails();

    ThumbnailListWidget* getthumbnailList(){ return thumbnailList; };

signals:
    // Сигнал, который будет отправляться при выборе миниатюры
    void thumbnailClicked(const QString &seriesUID);
    void thumbnailDoubleClicked(const QString &seriesUID);
    // Сигнал для обработки перетаскивания серии
    void seriesDropped(const QString &seriesUID, const QPoint &dropPosition);



private:
    QPoint m_ptDragPos;


    ThumbnailListWidget* thumbnailList;

    void updateThumbnailsSize();

    void resizeEvent(QResizeEvent *event) override;

protected:

};
