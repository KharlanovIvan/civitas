#include "ctbasictoolbar.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QFormLayout>
#include <QStackedLayout>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QRadioButton>
#include <QRadioButton>
#include <QRadioButton>


CTBasicToolbar::CTBasicToolbar(QWidget *parent) : QDockWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Центральный контейнер для панели
    QWidget *centralWidget = new QWidget(this);
    setWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // ==== Группа 1: Навигация по срезам ====
    QGroupBox *sliceGroup = new QGroupBox("Срезы");
    QVBoxLayout *sliceLayout = new QVBoxLayout();

    sliceSlider = new QSlider(Qt::Horizontal);
    sliceSlider->setMinimum(0);
    sliceSlider->setMaximum(100); // будет адаптироваться
    sliceLayout->addWidget(sliceSlider);

    QLabel *sliceLabel = new QLabel("Срез: 0");
    sliceLayout->addWidget(sliceLabel);

    connect(sliceSlider, &QSlider::valueChanged, this, [=](int value){
        sliceLabel->setText(QString("Срез: %1").arg(value));
        emit sliceChanged(value);
    });

    sliceGroup->setLayout(sliceLayout);
    mainLayout->addWidget(sliceGroup);

    // ==== Группа 2: Окно/уровень (window/level) ====
    QGroupBox *windowGroup = new QGroupBox("Окно/Уровень");
    QGridLayout *wlLayout = new QGridLayout();

    windowSpin = new QSpinBox();
    levelSpin = new QSpinBox();
    windowSpin->setRange(1, 5000);
    levelSpin->setRange(-1000, 1000);

    wlLayout->addWidget(new QLabel("Окно:"), 0, 0);
    wlLayout->addWidget(windowSpin, 0, 1);
    wlLayout->addWidget(new QLabel("Уровень:"), 1, 0);
    wlLayout->addWidget(levelSpin, 1, 1);

    connect(windowSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CTBasicToolbar::windowChanged);
    connect(levelSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &CTBasicToolbar::levelChanged);

    windowGroup->setLayout(wlLayout);
    mainLayout->addWidget(windowGroup);

    // ==== Группа 3: Режим отображения ====
    QGroupBox *viewGroup = new QGroupBox("Режим отображения");
    QVBoxLayout *viewLayout = new QVBoxLayout();

    viewCombo = new QComboBox();
    viewCombo->addItems({"Axial", "Sagittal", "Coronal", "3D View"});
    viewLayout->addWidget(viewCombo);

    connect(viewCombo, &QComboBox::currentTextChanged,
            this, &CTBasicToolbar::viewModeChanged);

    viewGroup->setLayout(viewLayout);
    mainLayout->addWidget(viewGroup);

    // ==== Группа 4: Фильтрация / обработка ====
    QGroupBox *filterGroup = new QGroupBox("Обработка");
    QVBoxLayout *filterLayout = new QVBoxLayout();

    QPushButton *denoiseButton = new QPushButton("Шумоподавление");
    QPushButton *sharpenButton = new QPushButton("Резкость");
    QPushButton *thresholdButton = new QPushButton("Порог");

    filterLayout->addWidget(denoiseButton);
    filterLayout->addWidget(sharpenButton);
    filterLayout->addWidget(thresholdButton);

    connect(denoiseButton, &QPushButton::clicked, this, &CTBasicToolbar::applyDenoise);
    connect(sharpenButton, &QPushButton::clicked, this, &CTBasicToolbar::applySharpen);
    connect(thresholdButton, &QPushButton::clicked, this, &CTBasicToolbar::applyThreshold);

    filterGroup->setLayout(filterLayout);
    mainLayout->addWidget(filterGroup);

    mainLayout->addStretch();

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
