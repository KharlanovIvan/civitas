#include "thumbnaillistwidget.h"

ThumbnailListWidget::ThumbnailListWidget(QWidget *parent)
    : QListWidget(parent) {
    setViewMode(QListView::IconMode);
    setIconSize(QSize(128, 128));
    setResizeMode(QListView::Adjust);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setContextMenuPolicy(Qt::DefaultContextMenu);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



void ThumbnailListWidget::resizeEvent(QResizeEvent *event) {
    // Определяем размеры родителя
    int parentWidth = parentWidget()->width();
    int parentHeight = parentWidget()->height();

    // Если ширина больше высоты, то отображаем элементы горизонтально
    if (parentWidth > parentHeight) {
        setFlow(QListView::LeftToRight);  // Горизонтальный поток
    } else {
        setFlow(QListView::TopToBottom);  // Вертикальный поток
    }

    // Вызовем стандартную обработку изменения размера
    QListWidget::resizeEvent(event);
}


// Метод добавления миниатюры
void ThumbnailListWidget::addThumbnail(const QString &titleThumbnail, const QImage *thumbnail, const QString &seriesUID) {
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

        addItem(item); // Добавляем элемент в список
    }
}


/*
void ThumbnailListWidget::addThumbnail(const QString &title, const QImage &image, const QString &seriesUID) {
    if (!image.isNull()) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(QPixmap::fromImage(image)), title);
        item->setData(Qt::UserRole, seriesUID);
        item->setSizeHint(QSize(140, 160));
        addItem(item);

        clearSelection();
        item->setSelected(true);
    }
}
*/
QString ThumbnailListWidget::selectedSeriesUID() const {
    if (selectedItems().isEmpty())
        return {};
    return selectedItems().first()->data(Qt::UserRole).toString();
}

void ThumbnailListWidget::setActiveSeriesUID(const QString &uid) {
    m_activeUID = uid;

    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *currentItem = this->item(i);
        const QString itemUID = currentItem->data(Qt::UserRole).toString();

        if (itemUID == uid) {
            currentItem->setBackground(Qt::yellow); // активная подсветка
        } else {
            currentItem->setBackground(Qt::white);
        }
    }
}



void ThumbnailListWidget::sortThumbnails() {
    qDebug() << "ThumbnailListWidget::sortThumbnails: Сортировка миниатюр запущена...";

    const QString serviceKeywordTop = "topogram";
    const QString serviceKeywordBottom = "protocol";

    // Списки для разных типов серий
    QList<QListWidgetItem*> topSeries;
    QList<QListWidgetItem*> mainSeries;
    QList<QListWidgetItem*> bottomSeries;

    // Удаляем все элементы из списка, но не из памяти, и распределяем их по категориям
    for (int i = count() - 1; i >= 0; --i) {
        QListWidgetItem *item = takeItem(i); // Удаляем, но не уничтожаем элемент
        QString description = item->data(Qt::UserRole + 1).toString().toLower();

        if (description.contains(serviceKeywordTop)) {
            topSeries.append(item);
        } else if (description.contains(serviceKeywordBottom)) {
            bottomSeries.append(item);
        } else {
            mainSeries.append(item);
        }
    }

    // Добавляем элементы обратно в список в нужном порядке
    for (QListWidgetItem *item : topSeries) addItem(item);
    for (QListWidgetItem *item : mainSeries) addItem(item);
    for (QListWidgetItem *item : bottomSeries) addItem(item);

    // Обновляем виджет для правильного отображения
    update();

    qDebug() << "ThumbnailListWidget::sortThumbnails: Сортировка миниатюр завершена.";
}



void ThumbnailListWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QListWidget::mousePressEvent(event);
}

void ThumbnailListWidget::startDrag(Qt::DropActions supportedActions) {
    qDebug() << "ThumbnailListWidget::startDrag";
    QListWidgetItem *item = currentItem();
    if (!item)
        return;

    QMimeData *mimeData = new QMimeData;
    mimeData->setText(item->data(Qt::UserRole).toString());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(item->icon().pixmap(128, 128));
    drag->exec(supportedActions);
}

void ThumbnailListWidget::contextMenuEvent(QContextMenuEvent *event) {
    QListWidgetItem *item = itemAt(event->pos());
    if (!item)
        return;

    QString uid = item->data(Qt::UserRole).toString();
    QMenu menu(this);

    QAction *showInMainView = menu.addAction("Показать в главном окне");
    QAction *deleteSeries = menu.addAction("Удалить серию");

    QAction *selectedAction = menu.exec(event->globalPos());
    if (selectedAction == showInMainView) {
        emit requestShowInMainView(uid);
    } else if (selectedAction == deleteSeries) {
        emit requestDeleteSeries(uid);
    }
}





void ThumbnailListWidget::updateThumbnailsSize(Qt::DockWidgetArea area) {
    int itemSize = 128;  // Размер иконки

    // Определяем поток в зависимости от ориентации
    if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
        setFlow(QListView::TopToBottom);  // Вертикальное размещение
        int listWidth = width();
        itemSize = static_cast<int>(listWidth * 0.65);  // Размер иконки пропорционален ширине
    } else if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        setFlow(QListView::LeftToRight);  // Горизонтальное размещение
        int listHeight = height();
        itemSize = static_cast<int>(listHeight * 0.65);  // Размер иконки пропорционален высоте
    }

    setIconSize(QSize(itemSize, itemSize));  // Устанавливаем размер иконки

    // Устанавливаем отступ между иконкой и текстом
    const int padding = 15;  // Отступ между иконкой и текстом

    // Применяем новый размер и отступы
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *item = this->item(i);

        // Новый размер для каждого элемента с учетом отступа
        item->setSizeHint(QSize(itemSize, itemSize + padding));  // Увеличиваем высоту, чтобы добавить отступ
    }

    update();  // Обновляем виджет
}

