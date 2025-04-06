#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QDockWidget>
#include <QDebug>
#include <QTimer>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class CTBasicToolbar : public QDockWidget
{
    Q_OBJECT
public:
    explicit CTBasicToolbar(QWidget *parent = nullptr);

    void applySizeConstraints();

    // Методы доступа (по желанию)
    void setSliceRange(int min, int max);
    void setWindowLevel(int window, int level);
    QPushButton* getDenoiseButton() const;
    QPushButton* getSharpenButton() const;
    QPushButton* getThresholdButton() const;

signals:
    // Сигналы от панели
    void sliceChanged(int);
    void windowChanged(int);
    void levelChanged(int);
    void viewModeChanged(const QString &);
    void applyDenoise();
    void applySharpen();
    void applyThreshold();

private slots:
    void handleTopLevelChanged(bool topLevel);

private:
    // Навигация по срезам
    QSlider *sliceSlider;
    QLabel *sliceLabel;

    // Window / Level
    QSpinBox *windowSpin;
    QSpinBox *levelSpin;

    // Режим просмотра
    QComboBox *viewCombo;

    // Кнопки фильтрации
    QPushButton *denoiseButton;
    QPushButton *sharpenButton;
    QPushButton *thresholdButton;

    void resizeEvent(QResizeEvent *event) override;


};


