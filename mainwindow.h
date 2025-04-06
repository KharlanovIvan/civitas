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
#include <QTimer>

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
#include "ctbasictoolbar.h"
#include "imageutils.h"
#include "datadicom.h"
#include "vtkpipelineviewer.h"
#include "IModalityInitializer.h"
#include "modalityinitializerfactory.h"
#include "ctinitializer.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

    bool MPR = true;
    QVector<QSharedPointer<VTKPipelineViewer>> vtkViewers;

    QGridLayout *gridLayoutCentralWidget;

    QSharedPointer<DataDICOM> currentData;

    QMap<QString, QSharedPointer<DataDICOM>>DICOMSeries;

    QVector<QString> seriesList;

    QString dirFolder;
    QString dirFile;
    QString currentLanguage;

    QWidget *centralWidget;
    QVTKOpenGLNativeWidget* screenSaverCentralWidget;

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
    QAction *pActCTBasicToolbarPanel;

     // Меню Настройки
    QMenu *pSettings;

    QMenu* pLanguageMenu;

    QActionGroup *languageGroup;

     // Меню Помощь
    QMenu *pHelp;

    Gallery* gallery;
    QSharedPointer<CTBasicToolbar> CTBasicToolbarPanel;

private slots:
    void openFolder();
    void openFile();
    bool loadDicomFromFile(const QString &filePath);
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

public slots:
    void initGallery();
    void initCTBasicToolbar();

    void setDefaultSettings();

    void onThumbnailClicked(const QString &seriesUID);
    void onThumbnailDoubleClicked(const QString &seriesUID);

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

};

