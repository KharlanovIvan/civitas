#include "gallery.h"
#include <qapplication.h>

Gallery::Gallery(QWidget *parent) : QDockWidget(parent) {
    try {
        // Устанавливаем политику размера для галереи
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        thumbnailList = new ThumbnailListWidget(this);

        thumbnailList->setViewMode(QListView::IconMode);  // Отображение в виде значков
        thumbnailList->setResizeMode(QListWidget::Adjust); // Подстройка элементов списка при изменении размеров
        thumbnailList->setSpacing(20); // Устанавливаем расстояние между миниатюрами
        thumbnailList->setSelectionMode(QAbstractItemView::SingleSelection); // Позволяет выбирать только одну миниатюру
        thumbnailList->setMovement(QListView::Static); // Фиксированное положение элементов
        thumbnailList->setWrapping(false);

        thumbnailList->setDragEnabled(true); // Разрешаем перетаскивание элементов

        thumbnailList->setAcceptDrops(true);


        // Устанавливаем thumbnailList как виджет для доквиджета
        setWidget(thumbnailList);


        connect(thumbnailList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            try {
                if (!item) {
                    qWarning() << "Ошибка: itemClicked получен nullptr";
                    return;
                }
                QString seriesUID = item->data(Qt::UserRole).toString();
                emit thumbnailClicked(seriesUID); // Отправляем сигнал с UID выбранной миниатюры
            } catch (const std::exception &e) {
                qCritical() << "Исключение в itemClicked: " << e.what();
            }
        });

        connect(thumbnailList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
            try {
                if (!item) {
                    qWarning() << "Ошибка: itemDoubleClicked получен nullptr";
                    return;
                }
                QString seriesUID = item->data(Qt::UserRole).toString();
                emit thumbnailDoubleClicked(seriesUID); // Отправляем сигнал с UID выбранной миниатюры при двойном щелчке
            } catch (const std::exception &e) {
                qCritical() << "Исключение в itemDoubleClicked: " << e.what();
            }
        });

    } catch (const std::exception &e) {
        qCritical() << "Ошибка в конструкторе Gallery: " << e.what();
    }

}


// Деструктор класса Gallery
Gallery::~Gallery() {
    delete thumbnailList; // Освобождаем память, предотвращая утечки
}


void Gallery::resizeEvent(QResizeEvent *event) {
    qDebug() << " Gallery::resizeEvent";

        // Получаем новые размеры главного окна
        int newWidth = event->size().width();
        int newHeight = event->size().height();

        // Проверяем, если панель прикреплена (не в режиме "topLevel")
        if (!isFloating()) {
            QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget());
            // Получаем позицию панели в главном окне
            Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
            if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
                // Устанавливаем ограничения для ширины
                int newWidthLimit = qMax(200, static_cast<int>(newWidth * 0.1));
                setMinimumWidth(newWidthLimit);
                setMaximumWidth(newWidthLimit);
                // Ограничиваем только ширину, оставляем высоту гибкой
                setMinimumHeight(0);
                setMaximumHeight(QWIDGETSIZE_MAX);
            } else if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
                // Устанавливаем ограничения для высоты
                int newHeightLimit = qMax(200, static_cast<int>(newHeight * 0.1));
                setMinimumHeight(newHeightLimit);
                setMaximumHeight(newHeightLimit);
                // Ограничиваем только высоту, оставляем ширину гибкой
                setMinimumWidth(0);
                setMaximumWidth(QWIDGETSIZE_MAX);
            }
        }
        updateThumbnailsSize();

        // Вызов стандартного поведения
        QDockWidget::resizeEvent(event);
}



void Gallery::updateThumbnailsSize() {
    if (!thumbnailList) return;

    QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget());
    if (!mainWindow) return;

    Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
    thumbnailList->updateThumbnailsSize(area);
}



void Gallery::addThumbnail(const QString &title, const QImage *image, const QString &seriesUID) {
    if (!image || image->isNull()) return;

    thumbnailList->addThumbnail(title, image, seriesUID);

    updateThumbnailsSize();
}


// Метод получения UID выбранной миниатюры
QString Gallery::getSelectedThumbnailUID() const {
    QListWidgetItem *item = thumbnailList->currentItem(); // Получаем текущий выбранный элемент
    if (item) {
        return item->data(Qt::UserRole).toString(); // Возвращаем UID из пользовательских данных
    }
    return QString(); // Если ничего не выбрано, возвращаем пустую строку
}

// Получаем название для отображения
QString Gallery::getSelectedThumbnailTitle() const {
    QListWidgetItem *item = thumbnailList->currentItem();
    if (item) {
        return item->data(Qt::UserRole + 1).toString();  // Извлекаем название для пользователя
    }
    return QString();
}

// Метод очистки всех миниатюр
void Gallery::clearThumbnails() {
    thumbnailList->clear();
}


void Gallery::sortThumbnails() {
    if (!thumbnailList) return;
    thumbnailList->sortThumbnails();
}


