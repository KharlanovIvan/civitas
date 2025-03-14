#include "gallery.h"

// Конструктор класса Gallery
Gallery::Gallery(QWidget *parent) : QDockWidget(parent) {
    try {
        // Устанавливаем политику размера для галереи
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // Создаем виджет списка миниатюр
        thumbnailList = new QListWidget(this);
        thumbnailList->setViewMode(QListView::IconMode);  // Отображение в виде значков
        thumbnailList->setResizeMode(QListWidget::Adjust); // Подстройка элементов списка при изменении размеров
        thumbnailList->setSpacing(20); // Устанавливаем расстояние между миниатюрами
        thumbnailList->setSelectionMode(QAbstractItemView::SingleSelection); // Позволяет выбирать только одну миниатюру
        thumbnailList->setMovement(QListView::Static); // Фиксированное положение элементов
        thumbnailList->setFlow(QListWidget::LeftToRight); // Горизонтальное отображение элементов
        thumbnailList->setWrapping(false);

        // Устанавливаем thumbnailList как виджет для доквиджета
        setWidget(thumbnailList);

        // Восстанавливаем коннектор
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
    } catch (const std::exception &e) {
        qCritical() << "Ошибка в конструкторе Gallery: " << e.what();
    }
}

// Деструктор класса Gallery
Gallery::~Gallery() {
    delete thumbnailList; // Освобождаем память, предотвращая утечки
}

// Переопределяем обработчик изменения размера окна
void Gallery::resizeEvent(QResizeEvent *event) {

    if (isResizing) {
        // Если уже выполняется изменение размеров, просто выходим
        return;
    }

    // Устанавливаем флаг, чтобы предотвратить рекурсию
    isResizing = true;

    static int counter = 0;
    qDebug() << "resizeEvent called:" << ++counter;

    // Сброс минимальных и максимальных значений
    setMinimumWidth(0);
    setMaximumWidth(QWIDGETSIZE_MAX);
    setMinimumHeight(0);
    setMaximumHeight(QWIDGETSIZE_MAX);

    if (parentWidget()) { // Проверяем, есть ли родительский виджет
        // Проверяем, является ли родитель QMainWindow
        QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget());
        Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
        if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
            // Устанавливаем политику размера для галереи
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            // Вычисляем новую ширину галереи: 10% от ширины родительского окна, но не менее 200px
            int newWidth = qMax(200, static_cast<int>(parentWidget()->width() * 0.10));
            setMinimumWidth(newWidth);
            setMaximumWidth(newWidth);

            // Устанавливаем минимальную высоту в 2 раза больше ширины, но не менее 400px
            int newHeight = newWidth * 2;
            setMinimumHeight(newHeight);

            thumbnailList->setMaximumHeight(QWIDGETSIZE_MAX);

            thumbnailList->setMaximumWidth(newWidth );


        } else if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
            // Устанавливаем политику размера для галереи
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            // Вычисляем новую высоту галереи: 10% от высоты родительского окна, но не менее 200px
            int newHeight = qMax(200, static_cast<int>(parentWidget()->height() * 0.10));
            setMinimumHeight(newHeight);
            setMaximumHeight(newHeight);

            // Устанавливаем минимальную высоту в 2 раза больше ширины, но не менее 400px
            int newWidth = newHeight * 2;
            setMinimumWidth(newWidth);

            thumbnailList->setMaximumWidth(QWIDGETSIZE_MAX);

            thumbnailList->setMaximumHeight(newHeight - 10);
        }
    }

    // Обновляем размеры миниатюр после изменения размеров галереи
    updateThumbnailsSize();

    // Сбрасываем флаг после завершения изменения размеров
    isResizing = false;

    // Вызываем стандартную обработку события изменения размера
    QDockWidget::resizeEvent(event);
}



void Gallery::updateThumbnailsSize() {
    if (!thumbnailList) return; // Если списка нет, просто выходим

    // Проверяем, является ли родитель QMainWindow
    QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget());
    if (!mainWindow) {
        qDebug() << "Parent is not a QMainWindow!";
        return;
    }

    Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);

    // Если галерея в левом или правом положении (вертикальное положение)
    if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
        qDebug() << "Галерея в левом или правом положении (вертикальное положение)";

        // Устанавливаем Flow для вертикального размещения
        thumbnailList->setFlow(QListView::TopToBottom);  // Элементы будут идти сверху вниз

        // Получаем текущую ширину Gallery
        int listWidth = width();
        // Миниатюры занимают 85% от ширины Gallery
        int itemSize = static_cast<int>(listWidth * 0.65);

        // Устанавливаем новый размер значков
        thumbnailList->setIconSize(QSize(itemSize, itemSize));


        // Проходим по всем элементам списка и устанавливаем им новый размер
        for (int i = 0; i < thumbnailList->count(); ++i) {
            QListWidgetItem *item = thumbnailList->item(i);
            item->setSizeHint(QSize(itemSize, itemSize));
        }
    }
    // Если галерея в верхнем или нижнем положении (горизонтальное положение)
    else if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        qDebug() << "Галерея в верхнем или нижнем положении (горизонтальное положение)";

        // Устанавливаем Flow для горизонтального размещения
        thumbnailList->setFlow(QListView::LeftToRight);  // Элементы будут идти слева направо

        // Получаем текущую высоту Gallery
        int listHeight = height();
        // Миниатюры занимают 85% от высоты Gallery
        int itemSize = static_cast<int>(listHeight * 0.65);

        // Устанавливаем новый размер значков
        thumbnailList->setIconSize(QSize(itemSize, itemSize));

        // Проходим по всем элементам списка и устанавливаем им новый размер
        for (int i = 0; i < thumbnailList->count(); ++i) {
            QListWidgetItem *item = thumbnailList->item(i);
            item->setSizeHint(QSize(itemSize, itemSize));
        }
    }
    // Принудительно обновляем виджет
    thumbnailList->update();
}



// Метод добавления миниатюры
void Gallery::addThumbnail(const QString &titleThumbnail, const QImage *thumbnail, const QString &seriesUID) {
    if (!thumbnail->isNull()) { // Проверяем, не пустое ли изображение
        int itemSize = static_cast<int>(width() * 0.85); // Вычисляем размер миниатюры

        // Создаем новый элемент списка с миниатюрой
        QListWidgetItem *item = new QListWidgetItem(QIcon(QPixmap::fromImage(*thumbnail)), titleThumbnail);

        // Сохраняем UID для внутреннего использования (например, идентификация серии)
        item->setData(Qt::UserRole, seriesUID);  // UID серии

        // Сохраняем название для отображения пользователю
        item->setData(Qt::UserRole + 1, titleThumbnail);  // Название для пользователя

        item->setSizeHint(QSize(itemSize, itemSize + 20)); // Увеличиваем высоту для текста
        item->setTextAlignment(Qt::AlignHCenter); // Центрируем текст
        item->setSizeHint(QSize(itemSize, itemSize)); // Устанавливаем размер элемента

        thumbnailList->addItem(item); // Добавляем элемент в список
    }
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
    try {
        thumbnailList->clear();
    } catch (const std::exception &e) {
        qCritical() << "Ошибка в clearThumbnails: " << e.what();
    }
}


void Gallery::sortThumbnails() {
    qDebug() << "Сортировка миниатюр запущена...";

    QString serviceKeywordTop = "topogram";
    QString serviceKeywordBottom = "protocol";

    // Создаем новую очередь миниатюр
    QList<QListWidgetItem*> topSeries;
    QList<QListWidgetItem*> mainSeries;
    QList<QListWidgetItem*> bottomSeries;

    for (int i = thumbnailList->count(); i > 0; --i) {
        QListWidgetItem *item = thumbnailList->takeItem(0); // Удаляем элемент из списка, но не из памяти
        QString description = item->data(Qt::UserRole + 1).toString().toLower(); // seriesDescription
        if(description.contains(serviceKeywordTop)) {
            topSeries.append(item);

        } else if(description.contains(serviceKeywordBottom)) {
            bottomSeries.append(item);

        } else {
            mainSeries.append(item);
        }
    }

    // Добавляем элементы обратно в нужном порядке
    for (QListWidgetItem *item : topSeries) thumbnailList->addItem(item);
    for (QListWidgetItem *item : mainSeries) thumbnailList->addItem(item);
    for (QListWidgetItem *item : bottomSeries) thumbnailList->addItem(item);

    qDebug() << "Сортировка миниатюр завершена.";

}



