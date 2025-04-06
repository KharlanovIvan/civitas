#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QDockWidget>
#include <QDebug>
#include <QTimer>

class CTBasicToolbar : public QDockWidget
{
    Q_OBJECT
public:
    explicit CTBasicToolbar(QWidget *parent = nullptr);

    void applySizeConstraints();

signals:

private:

    void resizeEvent(QResizeEvent *event) override;

    void handleTopLevelChanged(bool topLevel);
};


