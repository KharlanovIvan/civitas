#pragma once

#include <QDockWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QResizeEvent>
#include <QMainWindow>  // Добавляем заголовочный файл

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

signals:
    // Сигнал, который будет отправляться при выборе миниатюры
    void thumbnailClicked(const QString &seriesUID);
    void thumbnailDoubleClicked(const QString &seriesUID);

private:
    QListWidget *thumbnailList; // Список миниатюр

    void updateThumbnailsSize();

    void resizeEvent(QResizeEvent *event) override;

};
