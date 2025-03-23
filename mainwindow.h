#pragma once

#include <QSharedPointer>
#include <QDir>
#include <QString>
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QThread>
#include <QDockWidget>
#include <QCloseEvent>
#include <QSettings>
#include <QTranslator>
#include <QPixmap>
#include <QListWidget>
#include <QListWidgetItem>
#include <QImage>
#include <QColor>
#include <QLabel>

#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkImageToVTKImageFilter.h>
#include <itkMinimumMaximumImageFilter.h>
#include <itkImage.h>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageViewer2.h>
#include <vtkImageReslice.h>
#include <vtkImageResize.h>
#include <vtkImageData.h>
#include <vtkImageCast.h>
#include <vtkImageActor.h>
#include <vtkImageMapper.h>
#include <vtkImageFlip.h>
#include <vtkRendererCollection.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
#include <vtkRenderWindow.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageMapToWindowLevelColors.h>











#include "custominteractorstyle2d.h"
#include "gallery.h"
#include "imageutils.h"
#include "datadicom.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

    QSharedPointer<DataDICOM> currentData;

    QMap<QString, QSharedPointer<DataDICOM>>DICOMSeries;

    QVector<QString> seriesList;

    QString dirFolder;
    QString dirFile;
    QString currentLanguage;

    QWidget *centralWidget;
    QWidget * ScreenSaverCentralWidget;

    QMenuBar* pMenuBar;

    // Меню Файл
    QMenu *pFile;

    QAction *pActOpenFile;
    QAction *pActOpenFiles;
    QAction *pActExit;

     // Меню Правка
    QMenu *pEdit;

    // Меню Вид
    QMenu *pView;

    QMenu* pSetPanel;

    QAction *pActGalleryPanel;

     // Меню Настройки
    QMenu *pSettings;

    QMenu* pLanguageMenu;

    QActionGroup *languageGroup;

     // Меню Помощь
    QMenu *pHelp;

    Gallery* gallery;

    QVTKOpenGLNativeWidget *vtkWidget;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkImageViewer2> imageViewer;
    vtkSmartPointer<vtkRenderWindowInteractor> interactor;

    // Фильтры
    vtkSmartPointer<vtkImageMapToWindowLevelColors> windowLevelFilter;
    vtkSmartPointer<vtkImageFlip> flipFilter;

private slots:
    void openFolder();
    void openFile();
    void initializeVTKImageViewer(const QSharedPointer<DataDICOM> &data);
    void loadDicomFromFile(const QString &filePath);
    void loadDicomFromDirectory(const QString &folderPath);
    void saveSettings();
    void restoreSettings();
    void changeLanguage(const QString &newLang);
    void updateUI();

    void setupUI();

    void initFileMenu();
    void initEditMenu();
    void initViewMenu();
    void initSettingsMenu();
    void initHelpMenu();

    void setupMenuBar();

    void initGallery();

    void setDefaultSettings();

    void onThumbnailClicked(const QString &seriesUID);

    void setWindowLevel(double window, double level);

    void flipImage(int axis);







protected:
    void closeEvent(QCloseEvent* event) override;


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();





};

