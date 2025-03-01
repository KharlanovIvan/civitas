#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "CIVITAS_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    //MainWindow w;
    //w.show();

    // Создание главного окна
    QMainWindow mainWindow;
    QVTKOpenGLNativeWidget *vtkWidget = new QVTKOpenGLNativeWidget(&mainWindow);
    mainWindow.setCentralWidget(vtkWidget);
    mainWindow.resize(800, 600);

    // Создание VTK конуса
    vtkNew<vtkConeSource> coneSource;
    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(coneSource->GetOutputPort());

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(coneActor);

    vtkWidget->renderWindow()->AddRenderer(renderer);

    // Отображение окна
    mainWindow.show();

    return a.exec();
}
