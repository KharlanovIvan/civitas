#include "ctbasictoolbar.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFormLayout>
#include <QStackedLayout>

CTBasicToolbar::CTBasicToolbar(QWidget *parent) : QDockWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Создаем контейнер для группировки элементов
    QWidget *centralWidget = new QWidget(this);
    setWidget(centralWidget); // Устанавливаем центральный виджет для QDockWidget

    // Создаем группу и макет для первой группы
    QGroupBox *group1 = new QGroupBox("Группа 1");
    QVBoxLayout *groupLayout1 = new QVBoxLayout();
    group1->setLayout(groupLayout1);

    // Добавляем кнопки в группу
    QPushButton *button1 = new QPushButton("Button 1");
    QPushButton *button2 = new QPushButton("Button 2");

    groupLayout1->addWidget(button1);
    groupLayout1->addWidget(button2);

    // Изменение фона области с кнопками (dark theme)
    group1->setStyleSheet(
        "QGroupBox {"
        "   border: 2px solid #555555;"   // Темная рамка
        "   border-radius: 5px;"           // Скругление углов рамки
        "   background-color: #2e2e2e;"    // Темный фон для группы
        "   padding: 10px;"                // Отступы внутри группы
        "}"
        "QGroupBox:title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top center;"
        "   padding: 0 10px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   color: #9a9a9a;"               // Цвет текста в заголовке
        "}"
        "QVBoxLayout {"
        "   background-color: #3a3a3a;"    // Темный фон для области с кнопками
        "}"
        "QPushButton {"
        "   background-color: #444444;"     // Темный фон для кнопок
        "   color: #ffffff;"                // Белый текст на кнопках
        "   border: 1px solid #888888;"     // Светлая рамка вокруг кнопок
        "   border-radius: 5px;"            // Скругление углов кнопок
        "   padding: 5px;"                  // Отступы внутри кнопки
        "}"
        );



    // Помещаем группу в основной макет центрального виджета
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(group1);

    // Следим за тем, откреплена ли панель
    connect(this, &QDockWidget::topLevelChanged, this, &CTBasicToolbar::handleTopLevelChanged);
}






void CTBasicToolbar::resizeEvent(QResizeEvent *event) {

    QDockWidget::resizeEvent(event);

}

void CTBasicToolbar::handleTopLevelChanged(bool topLevel) {
    qDebug() << "CTBasicToolbar::handleTopLevelChanged: topLevel =" << topLevel;

    if (topLevel) {
        // Откреплена — можно свободно растягивать
        setMinimumSize(200, 200);
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    } else {
        // Прикреплена — пересчитаем размеры согласно области
        applySizeConstraints();
    }
}

void CTBasicToolbar::applySizeConstraints() {

    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget())) {
        Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
        if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
            int newWidth = qMax(200, static_cast<int>(mainWindow->width() * 0.10));
            setMinimumWidth(newWidth);
            setMaximumWidth(newWidth);
            setMinimumHeight(newWidth);
            setMaximumHeight(QWIDGETSIZE_MAX);
        } else if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
            int newHeight = qMax(200, static_cast<int>(mainWindow->height() * 0.10));
            setMinimumHeight(newHeight);
            setMaximumHeight(newHeight);
            setMinimumWidth(newHeight);
            setMaximumWidth(QWIDGETSIZE_MAX);
        }
    }

}
