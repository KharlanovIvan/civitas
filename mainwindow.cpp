#include "mainwindow.h"
#include "IModalityInitializer.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    screenSaverCentralWidget = new QVTKOpenGLNativeWidget;

    gridLayoutCentralWidget = new QGridLayout(this);
    gridLayoutCentralWidget->addWidget(screenSaverCentralWidget);

    centralWidget = new QWidget(this);
    centralWidget->setLayout(gridLayoutCentralWidget);

    setCentralWidget(centralWidget);

    // Создание интерфейса
    setupUI();

}


MainWindow::~MainWindow() {}



void MainWindow::setupUI() {

    qDebug() << "Установка элементов интерфейса: ";

    // Создание меню "Файл"
    qDebug() << "Создание меню 'Файл'...";
    initFileMenu();

    qDebug() << "Создание меню 'Правка'...";
    // Создание меню "Правка"
    initEditMenu();

     qDebug() << "Создание меню 'Вид'...";
    // Создание меню "Вид"
    initViewMenu();

     qDebug() << "Создание меню 'Настройки'...";
    // Создание меню "Настройки"
    initSettingsMenu();

     qDebug() << "Создание меню 'Справка'...";
    // Создание меню "Справка"
    initHelpMenu();

    qDebug() << "Добавление всех меню в меню-бар...";
    // Добавление всех меню в меню-бар
    setupMenuBar();

    qDebug() << "Инициализация галереи...";
    // Инициализация галереи
    initGallery();

}


void MainWindow::initCTBasicToolbar() {
    // Создаем галерею и добавляем её в док-область справа
    CTBasicToolbarPanel = QSharedPointer<CTBasicToolbar>::create(this);

    CTBasicToolbarPanel->setWindowTitle(tr("Базовые инструменты KT"));

    addDockWidget(Qt::LeftDockWidgetArea, CTBasicToolbarPanel.data());
    CTBasicToolbarPanel->applySizeConstraints();




    // Настройка видимости галереи через меню "Вид"
    pActCTBasicToolbarPanel = new QAction(tr("&Базовые инструменты KT"));
    pActCTBasicToolbarPanel->setCheckable(true);
    pActCTBasicToolbarPanel->setChecked(CTBasicToolbarPanel->isVisible());

    pSetPanel->addAction(pActCTBasicToolbarPanel); // Добавляем действие для галереи

    // Подключаем сигналы для управления видимостью галереи
    connect(pActCTBasicToolbarPanel, &QAction::toggled, this, [this](bool checked) {
        if (checked) {
            CTBasicToolbarPanel->show();
        } else {
            CTBasicToolbarPanel->hide();
        }
    });

    // Подключаем сигнал для отслеживания видимости панели
    connect(CTBasicToolbarPanel.data(), &QDockWidget::visibilityChanged, this, [this](bool visible) {
        if (!visible) {
            pActCTBasicToolbarPanel->setChecked(false);  // Когда панель скрыта, снимаем галочку
        } else {
            pActCTBasicToolbarPanel->setChecked(true);  // Когда панель видна, устанавливаем галочку
        }
    });

}


void MainWindow::initGallery() {
    // Создаем галерею и добавляем её в док-область справа
    gallery = new Gallery(this);
    addDockWidget(Qt::RightDockWidgetArea, gallery);

    // Настройка видимости галереи через меню "Вид"
    pActGalleryPanel = new QAction(tr("&Галерея"));
    pActGalleryPanel->setCheckable(true);
    pActGalleryPanel->setChecked(gallery->isVisible());

    pSetPanel->addAction(pActGalleryPanel); // Добавляем действие для галереи

    // Подключаем сигналы для управления видимостью галереи
    connect(pActGalleryPanel, &QAction::toggled, this, [this](bool checked) {
        checked ? gallery->show() : gallery->hide();
    });

    connect(gallery, &QDockWidget::visibilityChanged, this, [this](bool visible) {
        if (!visible) {
            pActGalleryPanel->setChecked(false);
        } else {
            pActGalleryPanel->setChecked(true);
        }
    });

    connect(gallery, &Gallery::thumbnailClicked, this, &MainWindow::onThumbnailClicked);
    connect(gallery, &Gallery::thumbnailDoubleClicked, this, &MainWindow::onThumbnailDoubleClicked);

    connect(gallery, &Gallery::seriesDropped, this, &MainWindow::onSeriesDropped);

}


void MainWindow::onSeriesDropped(const QString &seriesUID, const QPoint &dropPosition) {
    qDebug() << "MainWindow: Перетащена серия с UID:" << seriesUID;
    qDebug() << "Позиция дропа: " << dropPosition;

    // Получаем виджет, на который был осуществлен дроп
    QVTKOpenGLNativeWidget* widgetUnderDrop = qobject_cast<QVTKOpenGLNativeWidget*>(childAt(dropPosition));

    if (!widgetUnderDrop) {
        qDebug() << "Не удалось определить виджет под дропом.";
        return;
    }

    if (!widgetUnderDrop) {
        qDebug() << "Дроп не на VTKPipelineViewer.";
        return;
    }

    // Логируем, что дроп был на виджет VTKPipelineViewer
    qDebug() << "Дроп осуществлен на VTKPipelineViewer.";

    QSharedPointer<VTKPipelineViewer> pipelineViewer = QSharedPointer<VTKPipelineViewer>::create();
    // Загрузим данные и создадим новый viewer для этой серии
    viewerManager->addViewer(pipelineViewer);
    pipelineViewer->initializePipeline(DICOMSeries[seriesUID]);
}



void MainWindow::initFileMenu() {
    // Создаем меню "Файл"
    pFile = new QMenu(tr("&Файл"));

    // Добавляем действия в меню "Файл"
    pActOpenFile = new QAction(tr("Открыть файл"));
    pActOpenFiles = new QAction(tr("Открыть серию"));
    pActExit = new QAction(tr("&Выход"));

    pFile->addAction(pActOpenFile);
    pFile->addAction(pActOpenFiles);
    pFile->addAction(pActExit);

    // Подключаем сигналы к слотам
    connect(pActOpenFiles, &QAction::triggered, this, &MainWindow::openFolder);
    connect(pActOpenFile, &QAction::triggered, this, &MainWindow::openFile);
    connect(pActExit, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWindow::initEditMenu() {
    // Создаем меню "Правка"
    pEdit = new QMenu(tr("&Правка"));
    // Здесь можно добавить действия для меню "Правка"
}

void MainWindow::initViewMenu() {
    // Создаем меню "Вид"
    pView = new QMenu(tr("&Вид"));

    // Создаем подменю "Панели инструментов"
    pSetPanel = new QMenu(tr("&Панели инструментов"));
    pView->addMenu(pSetPanel);
}

void MainWindow::initSettingsMenu() {
    // Создаем меню "Настройки"
    pSettings = new QMenu(tr("&Настройки"));

    // Создаем подменю "Язык"
    pLanguageMenu = new QMenu(tr("&Язык"));
    pSettings->addMenu(pLanguageMenu);

    // Добавляем доступные языки
    QStringList availableLanguages = { "English", "Русский" };
    languageGroup = new QActionGroup(this);

    for (const QString &lang : availableLanguages) {
        QAction *langAction = new QAction(lang, this);
        langAction->setCheckable(true);
        langAction->setData(lang);
        pLanguageMenu->addAction(langAction);
        languageGroup->addAction(langAction);

        // Подключаем слот для изменения языка
        connect(langAction, &QAction::triggered, this, [this, lang]() {
            changeLanguage(lang);
        });
    }
}

void MainWindow::initHelpMenu() {
    // Создаем меню "Справка"
    pHelp = new QMenu(tr("&Справка"));
    // Здесь можно добавить действия для меню "Справка"
}

void MainWindow::setupMenuBar() {
    // Получаем указатель на меню-бар
    pMenuBar = QMainWindow::menuBar();

    // Добавляем все меню в меню-бар
    pMenuBar->addMenu(pFile);
    pMenuBar->addMenu(pEdit);
    pMenuBar->addMenu(pView);
    pMenuBar->addMenu(pSettings);
    pMenuBar->addMenu(pHelp);
}


void MainWindow::updateUI() {
    // Обновление всех элементов UI
    qDebug() << "Обновление всех элементов UI...";

    pFile->setTitle(tr("&Файл"));
    pEdit->setTitle(tr("&Правка"));
    pView->setTitle(tr("&Вид"));
    pSettings->setTitle(tr("&Настройки"));
    pSetPanel->setTitle(tr("Панели инструментов"));
    pHelp->setTitle(tr("&Справка"));
    pLanguageMenu->setTitle(tr("Язык"));

    pActOpenFile->setText(tr("Открыть файл"));
    pActOpenFiles->setText(tr("Открыть серию"));
    pActExit->setText(tr("Выход"));
    pActGalleryPanel->setText(tr("&Галерея"));

    gallery->setWindowTitle(tr("Галерея"));

    if (!CTBasicToolbarPanel.isNull()) {
        CTBasicToolbarPanel->setWindowTitle(tr("Базовые инструменты KT"));
    } else {
        qDebug() << "CTBasicToolbarPanel не инициализирован!";
    }


    // Устанавливаем галочку на текущий язык
    for (QAction *action : languageGroup->actions()) {
        action->setChecked(action->data().toString() == currentLanguage);
    }
}


void MainWindow::onThumbnailDoubleClicked(const QString &seriesUID) {
    qDebug() << "Двойной щелчок на серии с UID:" << seriesUID;

    if(gridLayoutCentralWidget) {
        // Очистка лейаута перед добавлением новых виджетов
        QLayoutItem* item;
        while ((item = gridLayoutCentralWidget->takeAt(0)) != nullptr) {
            // Удаляем виджет из элемента лейаута
            if (QWidget* widget = item->widget()) {
                widget->deleteLater(); // Безопасное удаление виджета
            }
            // Удаляем сам элемент лейаута
            delete item;
        }

        delete gridLayoutCentralWidget;
        gridLayoutCentralWidget = nullptr; // Обнуляем указатель
        vtkViewers.clear();
    }

    setCentralWidget(nullptr);

    if (DICOMSeries.contains(seriesUID)) {
        currentData = DICOMSeries[seriesUID];

        displayCurrentImage();
    }

}


void MainWindow::displayCurrentImage() {
    QString modality = currentData->getModality();
    qDebug() << "MainWindow::openFile: Модальность: " << modality;

    // Инициализация ViewerManager, если ещё не создан
    if (!viewerManager) {
        viewerManager = new VTKViewerManager(this);
        setCentralWidget(viewerManager);
    }

    // Создаём Viewer
    QSharedPointer<VTKPipelineViewer> viewer(new VTKPipelineViewer(this));
    viewerManager->addViewer(viewer);

    // Создаём инициализатор через фабрику
    QSharedPointer<IModalityInitializer> initializer(ModalityInitializerFactory::createInitializer(modality));
    if (initializer) {
        QTimer::singleShot(0, this, [=]() {
            initializer->initializePipeline(viewer, currentData);
            initializer->initializeUI(this);
        });
    } else {
        qDebug() << "MainWindow::openFile: Неизвестная модальность. Используем стандартную инициализацию.";
        QTimer::singleShot(0, this, [=]() {
            viewer->initializePipeline(currentData);
        });
    }
}

/*
void MainWindow::displayCurrentImage() {
    QString modality = currentData->getModality();
    qDebug() << "MainWindow::openFile: Модальность: " << modality;

    // Создаём объект VTKPipelineViewer
    QSharedPointer<VTKPipelineViewer> viewer(new VTKPipelineViewer(this));
    vtkViewers.append(viewer);

    gridLayoutCentralWidget = new QGridLayout(this);

    int columns = 2;
    for (int i = 0; i < vtkViewers.size(); ++i) {
        auto widget = vtkViewers[i]->getVTKWidget();
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        int row = i / columns;
        int col = i % columns;
        gridLayoutCentralWidget->addWidget(widget, row, col);
    }

    centralWidget = new QWidget(this);
    centralWidget->setLayout(gridLayoutCentralWidget);

    setCentralWidget(centralWidget);

    // Создаём инициализатор через фабрику
    QSharedPointer<IModalityInitializer>initializer(ModalityInitializerFactory::createInitializer(modality));
    if (initializer) {
        // Инициализируем пайплайн и UI для данной модальности
        QTimer::singleShot(0, this, [=]() {
            initializer->initializePipeline(viewer, currentData);
            initializer->initializeUI(this);
        });
    } else {
        qDebug() << "MainWindow::openFile: Неизвестная модальность. Используем стандартную инициализацию.";
        QTimer::singleShot(0, this, [=]() {
            viewer->initializePipeline(currentData);
        });
    }
}
*/

void MainWindow::onThumbnailClicked(const QString &seriesUID) {
    qDebug() << "Щелчок на серии с UID:" << seriesUID;
}


void MainWindow::openFile() {
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "Выберите файл",
        QDir::homePath(),
        "DICOM Files (*.dcm *.DCM *);;All Files (*)"
        );

    if (!filePath.isEmpty()) {
        qDebug() << "MainWindow::openFile: Выбран файл:" << filePath;
        if (!DICOMSeries.isEmpty()) {
            //DICOMSeries.clear();
        }

        if (!vtkViewers.empty()) {
            //vtkViewers.clear();
        }

        try {
            if (!loadDicomFromFile(filePath)) {
                qDebug() << "MainWindow::openFile: не удалость открыть файл: " << filePath;
                QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть файл: \n%1").arg(filePath));
                return;
            }

            displayCurrentImage();

        } catch (const itk::ExceptionObject &e) {
            qDebug() << "MainWindow::openFile: Ошибка ITK: " << e.what();
            QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть файл: \n%1 \nОшибка ITK:%2").arg(filePath).arg(e.what()));
        } catch (const std::exception &e) {
            qDebug() << "MainWindow::openFile: Неизвестная ошибка: " << e.what();
            QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть файл: \n%1 \n Ошибка:%2").arg(filePath).arg(e.what()));
        }
    } else {
        qDebug() << "MainWindow::openFile: Выбор файла отменен.";
        return;
    }
}


void MainWindow::openFolder() {
    QString folderPath = QFileDialog::getExistingDirectory(
        nullptr,
        "Выберите папку",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly
        );

    if (folderPath.isEmpty()) {
        qDebug() << "MainWindow::openFolder: Выбор папки отменен.";
        return;
    }

    qDebug() << "MainWindow::openFolder: Выбрана папка:" << folderPath;

    if (!DICOMSeries.isEmpty()) {
        DICOMSeries.clear();
    }

    if (viewerManager) {
        viewerManager->clearAllViewers();
    }

    try {
        if (!loadDicomFromDirectory(folderPath)) {
            qDebug() << "MainWindow::openFolder: В указанном каталоге нет DICOM-файлов: " << folderPath;
            QMessageBox::critical(this, "Ошибка", QString("В указанном каталоге нет DICOM-файлов: \n%1").arg(folderPath));
            return;
        }

        displayCurrentImage();

    } catch (const itk::ExceptionObject &e) {
        QMessageBox::critical(this, "Ошибка", QString("Ошибка ITK:\n%1").arg(e.what()));
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Ошибка", QString("Неизвестная ошибка:\n%1").arg(e.what()));
    }
}

/*

void MainWindow::openFolder() {

    QString folderPath = QFileDialog::getExistingDirectory(
        nullptr,
        "Выберите папку",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly
        );

    if (!folderPath.isEmpty()) {
        qDebug() << "MainWindow::openFolder: Выбрана папка:" << folderPath;

        if (!DICOMSeries.isEmpty()) {
            DICOMSeries.clear();
        }

        if (!vtkViewers.empty()) {
            vtkViewers.clear();
        }

        try {
            if (!loadDicomFromDirectory(folderPath)) {
                qDebug() << "MainWindow::openFolder: не удалость открыть файлы в каталоге: " << folderPath;
                QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть файлы в каталоге: \n%1").arg(folderPath));
                return;
            }

            displayCurrentImage();

        } catch (const itk::ExceptionObject &e) {
            qDebug() << "MainWindow::openFolder: Ошибка ITK: " << e.what();
            QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть файлы в каталоге: \n%1 \nОшибка ITK:%2").arg(folderPath).arg(e.what()));
        } catch (const std::exception &e) {
            qDebug() << "MainWindow::openFolder: Неизвестная ошибка: " << e.what();
            QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть файлы в каталоге: \n%1 \n Ошибка:%2").arg(folderPath).arg(e.what()));
        }
    } else {
        qDebug() << "MainWindow::openFolder: Выбор папки отменен.";
        return;
    }
}
*/


bool MainWindow::loadDicomFromDirectory(const QString &folderPath) {

    using ImageType = itk::Image<float, 3>;

    auto namesGenerator = itk::GDCMSeriesFileNames::New();
    namesGenerator->SetDirectory(folderPath.toStdString());

    const auto seriesUIDs = namesGenerator->GetSeriesUIDs();
    if (seriesUIDs.empty()) {
        qDebug() << "MainWindow::loadDicomFromDirectory: Ошибка: В указанной папке нет DICOM-файлов.";
        return false;
    }

    auto dicomIO = itk::GDCMImageIO::New();

    gallery->clearThumbnails();

    for (const auto &seriesUID : seriesUIDs) {
        std::vector<std::string> files = namesGenerator->GetFileNames(seriesUID);
        if (files.empty()) continue;

        auto reader = itk::ImageSeriesReader<ImageType>::New();
        reader->SetImageIO(dicomIO);
        reader->SetFileNames(files);

        reader->Update();

        QSharedPointer<DataDICOM> data(new DataDICOM);

        data->setMetadata(dicomIO->GetMetaDataDictionary());

        std::string modality;
        if (itk::ExposeMetaData<std::string>(dicomIO->GetMetaDataDictionary(), "0008|0060", modality)) {
            qDebug() << "Modality of DICOM series: " << QString::fromStdString(modality);
            data->setModality(QString::fromStdString(modality));
        }

        std::string photometric;
        if (itk::ExposeMetaData<std::string>(dicomIO->GetMetaDataDictionary(), "0028|0004", photometric)) {
            qDebug() << "Photometric Interpretation: " << QString::fromStdString(photometric);
        }

        std::string bitsAllocated;
        if (itk::ExposeMetaData<std::string>(dicomIO->GetMetaDataDictionary(), "0028|0100", bitsAllocated)) {
            qDebug() << "Bits Allocated: " << QString::fromStdString(bitsAllocated);
        }

        std::string samplesPerPixel;
        if (itk::ExposeMetaData<std::string>(dicomIO->GetMetaDataDictionary(), "0028|0002", samplesPerPixel)) {
            qDebug() << "Samples Per Pixel: " << QString::fromStdString(samplesPerPixel);
        }

        // Извлекаем WindowCenter
        std::string windowCenterStr;
        if (itk::ExposeMetaData<std::string>(data->getMetaData(), "0028|1050", windowCenterStr)) {
            data->setWindowCenter(std::stod(windowCenterStr));
            qDebug() << "WindowCenter: " << data->getWindowCenter();
        } else {
            qDebug() << "Ошибка: не удалось извлечь WindowCenter из метаданных!";
        }

        // Извлекаем WindowWidth
        std::string windowWidthStr;
        if (itk::ExposeMetaData<std::string>(data->getMetaData(), "0028|1051", windowWidthStr)) {
            data->setWindowWidth(std::stod(windowWidthStr));
            qDebug() << "WindowWidth: " << data->getWindowWidth();
        } else {
            qDebug() << "Ошибка: не удалось извлечь WindowWidth из метаданных!";
        }

        std::string descriptionStr;
        if (!itk::ExposeMetaData<std::string>(data->getMetaData(), "0008|103e", descriptionStr)) {    // Получаем Series Description
            qDebug() << "Ошибка: не удалось извлечь Series Description из метаданных.";
        }

        QString qDescription = QString::fromStdString(descriptionStr);
        if (qDescription.isEmpty()) {
            qDescription = data->getSeriesUID();
        } else {
            qDebug() << "Series Description: " << qDescription;
        }

        if(qDescription.contains("Topogram") || qDescription.contains("Patient Protocol")) {
            qDebug() << "Получено служебное изображения";
        } else {
            data->setITKImage(reader->GetOutput());
            qDebug() << "DICOM файл загружен, размер: "
                     << data->getITKImage()->GetLargestPossibleRegion().GetSize()[0] << " x "
                     << data->getITKImage()->GetLargestPossibleRegion().GetSize()[1] << " x "
                     << data->getITKImage()->GetLargestPossibleRegion().GetSize()[2];
        }

        // Сохраняем список файлов
        for (const auto &file : files) {
            data->addFilePath(QString::fromStdString(file));
        }

        data->generateThumbnail();

        // Если не удалось получить itkImage, то получаем его из QImage thumbnail.
        if (data->getITKImage().IsNull()) {
            itk::Image<float, 3>::Pointer itkImage = ImageUtils::ConvertQImageToITKImage(data->getThumbnail());
            data->setITKImage(itkImage);
        }

        data->setSeriesUID(QString::fromStdString(seriesUID));

        DICOMSeries[QString::fromStdString(seriesUID)] = data;


        // Инициализация currentData
        auto it = DICOMSeries.find(QString::fromStdString(seriesUID));
        if (it != DICOMSeries.end()) {
            currentData = it.value();
        } else {
            qDebug() << "Ошибка: seriesUID не найден в DICOMSeries.";
            return false;
        }

        qDebug() << "Серия " << QString::fromStdString(seriesUID) << " загружена.";

        gallery->addThumbnail(qDescription, &currentData->getThumbnail(), data->getSeriesUID());
    }

    gallery->sortThumbnails();
    gallery->show();

    return true;
}


bool MainWindow::loadDicomFromFile(const QString &filePath) {


    using ImageType = itk::Image<float, 3>;

    gallery->clearThumbnails();

    auto dicomIO = itk::GDCMImageIO::New();
    auto reader = itk::ImageFileReader<ImageType>::New();
    reader->SetFileName(filePath.toStdString());
    reader->SetImageIO(dicomIO);


    try {
        reader->Update();
    } catch (itk::ExceptionObject &error) {
        qDebug() << "MainWindow::loadDicomFromFile: Ошибка при чтении DICOM-файла:" << error.what();
        return false;
    }

    QSharedPointer<DataDICOM> data(new DataDICOM);

    data->setMetadata(dicomIO->GetMetaDataDictionary());

    std::string modality;
    if (itk::ExposeMetaData<std::string>(dicomIO->GetMetaDataDictionary(), "0008|0060", modality)) {
        qDebug() << "DICOM file modality: " << QString::fromStdString(modality);
        data->setModality(QString::fromStdString(modality));
    }

    std::string photometric;
    if (itk::ExposeMetaData<std::string>(dicomIO->GetMetaDataDictionary(), "0028|0004", photometric)) {
        qDebug() << "MainWindow::loadDicomFromFile: Photometric Interpretation: " << QString::fromStdString(photometric);
    }

    std::string bitsAllocated;
    if (itk::ExposeMetaData<std::string>(dicomIO->GetMetaDataDictionary(), "0028|0100", bitsAllocated)) {
        qDebug() << "MainWindow::loadDicomFromFile: Bits Allocated: " << QString::fromStdString(bitsAllocated);
    }

    std::string samplesPerPixel;
    if (itk::ExposeMetaData<std::string>(dicomIO->GetMetaDataDictionary(), "0028|0002", samplesPerPixel)) {
        qDebug() << "MainWindow::loadDicomFromFile: Samples Per Pixel: " << QString::fromStdString(samplesPerPixel);
    }

    std::string seriesUID;
    if (!itk::ExposeMetaData<std::string>(data->getMetaData(), "0008|0016", seriesUID)) {
        qDebug() << "MainWindow::loadDicomFromFile: Ошибка: не удалось извлечь Series UID из метаданных!";
        return false;
    }

    // Извлекаем WindowCenter
    std::string windowCenterStr;
    if (itk::ExposeMetaData<std::string>(data->getMetaData(), "0028|1050", windowCenterStr)) {
        data->setWindowCenter(std::stod(windowCenterStr));
        qDebug() << "MainWindow::loadDicomFromFile: WindowCenter: " << data->getWindowCenter();
    } else {
         qDebug() << "MainWindow::loadDicomFromFile: Ошибка: не удалось извлечь WindowCenter из метаданных!";
    }

    // Извлекаем WindowWidth
    std::string windowWidthStr;
    if (itk::ExposeMetaData<std::string>(data->getMetaData(), "0028|1051", windowWidthStr)) {
        data->setWindowWidth(std::stod(windowWidthStr));
        qDebug() << "MainWindow::loadDicomFromFile: WindowWidth: " << data->getWindowWidth();
    } else {
        qDebug() << "MainWindow::loadDicomFromFile: Ошибка: не удалось извлечь WindowWidth из метаданных!";
    }


    std::string descriptionStr;
    if (!itk::ExposeMetaData<std::string>(data->getMetaData(), "0008|103e", descriptionStr)) {    // Получаем Series Description
        qDebug() << "MainWindow::loadDicomFromFile: Ошибка: не удалось извлечь Series Description из метаданных.";
    }

    QString qDescription = QString::fromStdString(descriptionStr);
    if (qDescription.isEmpty()) {
        qDescription = data->getSeriesUID();
    }

    if(qDescription.contains("Topogram") || qDescription.contains("Patient Protocol")) {
        qDebug() << "MainWindow::loadDicomFromFile: Получено служебное изображения";
    } else {
        data->setITKImage(reader->GetOutput());
        qDebug() << "MainWindow::loadDicomFromFile: DICOM файл загружен, размер: "
                 << data->getITKImage()->GetLargestPossibleRegion().GetSize()[0] << " x "
                 << data->getITKImage()->GetLargestPossibleRegion().GetSize()[1] << " x "
                 << data->getITKImage()->GetLargestPossibleRegion().GetSize()[2];
    }

    // Сохраняем путь к файлу в DataDICOM
    data->addFilePath(filePath);

    data->generateThumbnail();

    // Если не удалось получить itkImage, то получаем его из QImage thumbnail.
    if (data->getITKImage().IsNull()) {
        itk::Image<float, 3>::Pointer itkImage = ImageUtils::ConvertQImageToITKImage(data->getThumbnail());
        data->setITKImage(itkImage);
    }

    data->setSeriesUID(QString::fromStdString(seriesUID));


    DICOMSeries[QString::fromStdString(seriesUID)] = data;

    // Инициализация currentData
    auto it = DICOMSeries.find(QString::fromStdString(seriesUID));
    if (it != DICOMSeries.end()) {
        currentData = it.value();
    } else {
        qDebug() << "MainWindow::loadDicomFromFile: Ошибка: seriesUID не найден в DICOMSeries.";
        return false;
    }

    qDebug() << ":MainWindow::loadDicomFromFile UID: " << QString::fromStdString(seriesUID) << " загружено.";
    qDebug() << "MainWindow::loadDicomFromFile: Файл сохранён по пути: " << data->getFilePaths()[0];


    gallery->addThumbnail(qDescription, &currentData->getThumbnail(), data->getSeriesUID());
    gallery->show();



    return true;
}







void MainWindow::changeLanguage(const QString &nameLanguage) {


    QString newLang;
    if (nameLanguage == "Русский") {
        newLang = "ru_RU";
    } else if (nameLanguage == "English") {
        newLang = "en_US";
    }

    if (nameLanguage == currentLanguage) return;

    currentLanguage = nameLanguage;

    // Загружаем стандартный перевод Qt
    static QTranslator qtTranslator;
    qApp->removeTranslator(&qtTranslator);
    QString qtTranslationFile = QString(":/resources/translations/qt_%1.qm").arg(newLang);
    if (qtTranslator.load(qtTranslationFile)) {
        qApp->installTranslator(&qtTranslator);
    }

    // Загружаем перевод приложения
    static QTranslator appTranslator;
    qApp->removeTranslator(&appTranslator);
    QString appTranslationFile = QString(":/translations/CIVITAS_%1.qm").arg(newLang);
    if (appTranslator.load(appTranslationFile)) {
        qApp->installTranslator(&appTranslator);
    } else {
        QString a = QString(":/translations/CIVITAS_%1.qm").arg(newLang);
        qDebug() << "Ошибка при загрузке перевода!" << a;
    }

    // Обновляем UI
    updateUI();

    //В терминале (из корневой папки проекта) выполни:
    //lupdate . -recursive -no-obsolete -ts translations/CIVITAS_ru_RU.ts translations/CIVITAS_en_US.ts
    //Это добавит все строки, помеченные tr("..."), в файлы .ts.

}




void MainWindow::closeEvent(QCloseEvent* event) {
    // Спрашиваем подтверждение у пользователя
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Подтверждение"), tr("Вы уверены, что хотите выйти?"),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Сохраняем данные перед закрытием
        saveSettings();

        // Принимаем событие закрытия
        event->accept();
    } else {
        // Игнорируем событие закрытия
        event->ignore();
    }
}

void MainWindow::saveSettings() {
    QSettings settings("CIVITAS MedTech", "CIVITAS");


    // Сохраняем размер и положение окна
    settings.setValue("windowSize", size());
    qDebug() << "void MainWindow::saveSettings: сохраненные размеры окна: " << settings.value("windowSize").toSize();
    settings.setValue("windowPosition", pos());

    settings.setValue("language", currentLanguage);

    // Сохраняем состояние док-виджетов
    settings.setValue("galleryVisible", pActGalleryPanel->isChecked());

    settings.sync();

}

void MainWindow::restoreSettings() {
    QSettings settings("CIVITAS MedTech", "CIVITAS");

    // Если первый запуск — задаем дефолтные настройки
    if (settings.allKeys().isEmpty()) {
        qDebug() << "MainWindow::restoreSettings: Сохраненных настроек не обнаружено.";
        setDefaultSettings();
    }

    // Теперь загружаем настройки
    QString savedLang = settings.value("language").toString();
    changeLanguage(savedLang);

    qDebug() << "MainWindow::restoreSettings: сохраненные размеры окна: " << settings.value("windowSize").toSize();

    resize(settings.value("windowSize").toSize());
    move(settings.value("windowPosition").toPoint());

    bool isGalleryVisible = settings.value("galleryVisible").toBool();
    pActGalleryPanel->setChecked(isGalleryVisible);
    emit pActGalleryPanel->toggled(isGalleryVisible);
}


void MainWindow::setDefaultSettings() {
    QSettings settings("CIVITAS MedTech", "CIVITAS");

    settings.setValue("language", QLocale::system().name().left(2));
    settings.setValue("windowSize", QSize(1024, 768));
    settings.setValue("windowPosition", QPoint(100, 100));
    settings.setValue("galleryVisible", false);
    settings.sync();  // Сохранить настройки
}


void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);  // Вызов базовой реализации

    qDebug() << "MainWindow::showEvent: Загрузка сохраненных настроек...";
    static bool firstShow = true;
    if (firstShow) {  // Гарантируем, что restoreSettings() вызывается только один раз
        firstShow = false;
        restoreSettings();
    }
}
void MainWindow::resizeEvent(QResizeEvent *event) {
    // Вызов стандартного поведения
    QMainWindow::resizeEvent(event);

    if(!CTBasicToolbarPanel.isNull()) {
        // Получаем новые размеры главного окна
        int newWidth = event->size().width();
        int newHeight = event->size().height();

        // Проверяем, если панель прикреплена (не в режиме "topLevel")
        if (!CTBasicToolbarPanel->isFloating()) {
            // Получаем позицию панели в главном окне
            Qt::DockWidgetArea area = this->dockWidgetArea(CTBasicToolbarPanel.data());

            if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea) {
                // Устанавливаем ограничения для ширины
                int newWidthLimit = qMax(200, static_cast<int>(newWidth * 0.10));
                CTBasicToolbarPanel->setMinimumWidth(newWidthLimit);
                CTBasicToolbarPanel->setMaximumWidth(newWidthLimit);
                // Ограничиваем только ширину, оставляем высоту гибкой
                CTBasicToolbarPanel->setMinimumHeight(0);
                CTBasicToolbarPanel->setMaximumHeight(QWIDGETSIZE_MAX);
            } else if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
                // Устанавливаем ограничения для высоты
                int newHeightLimit = qMax(200, static_cast<int>(newHeight * 0.10));
                CTBasicToolbarPanel->setMinimumHeight(newHeightLimit);
                CTBasicToolbarPanel->setMaximumHeight(newHeightLimit);
                // Ограничиваем только высоту, оставляем ширину гибкой
                CTBasicToolbarPanel->setMinimumWidth(0);
                CTBasicToolbarPanel->setMaximumWidth(QWIDGETSIZE_MAX);
            }
        }
    }

}

