#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {


    resize(1024, 768);

    vtkWidget = new QVTKOpenGLNativeWidget(this);
    setCentralWidget(vtkWidget);



    // Создание интерфейса
    setupUI();




    // Инициализируем объекты VTK и ITK
    imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    vtkWidget->setRenderWindow(renderWindow);

    // Загрузка настроек
    restoreSettings();


}


MainWindow::~MainWindow() {}



void MainWindow::setupUI() {

    // Инициализация галереи
    initGallery();

    // Создание меню "Файл"
    initFileMenu();

    // Создание меню "Правка"
    initEditMenu();

    // Создание меню "Вид"
    initViewMenu();

    // Создание меню "Настройки"
    initSettingsMenu();

    // Создание меню "Справка"
    initHelpMenu();

    // Добавление всех меню в меню-бар
    setupMenuBar();
}


void MainWindow::initGallery() {
    // Создаем галерею и добавляем её в док-область справа
    gallery = new Gallery(this);
    addDockWidget(Qt::RightDockWidgetArea, gallery);

    // Настройка видимости галереи через меню "Вид"
    pActGalleryPanel = new QAction(tr("&Галерея"));
    pActGalleryPanel->setCheckable(true);
    pActGalleryPanel->setChecked(gallery->isVisible());

    // Подключаем сигналы для управления видимостью галереи
    connect(pActGalleryPanel, &QAction::toggled, this, [this](bool checked) {
        checked ? gallery->show() : gallery->hide();
    });

    connect(gallery, &QDockWidget::visibilityChanged, this, [this](bool visible) {
        if (!visible) {
            pActGalleryPanel->setChecked(false);
        }
    });

    connect(gallery, &Gallery::thumbnailClicked, this, &MainWindow::onThumbnailClicked);
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
    pSetPanel->addAction(pActGalleryPanel); // Добавляем действие для галереи
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

    // Устанавливаем галочку на текущий язык
    for (QAction *action : languageGroup->actions()) {
        action->setChecked(action->data().toString() == currentLanguage);
    }
}

void MainWindow::onThumbnailClicked(const QString &seriesUID) {
    if (DICOMSeries.contains(seriesUID)) {
        currentData = DICOMSeries[seriesUID];
        if (currentData->image.IsNull()) {
            itk::Image<float, 3>::Pointer itkImage = ImageUtils::ConvertQImageToITKImage(currentData->thumbnail);
            initializeVTKImageViewer(itkImage);
        } else {
            initializeVTKImageViewer(currentData->image);
        }
    }
}


void MainWindow::openFile() {
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "Выберите файл",
        QDir::homePath(),
        "DICOM Files (*.dcm *.DCM *);;All Files (*)"  // Фильтр для файлов, например, DICOM
        );

    if (!filePath.isEmpty()) {
        qDebug() << "Выбран файл:" << filePath;

        if (!DICOMSeries.isEmpty()) {
            DICOMSeries.clear();
        }

        try {
            loadDicomFromFile(filePath);
        } catch (const itk::ExceptionObject &e) {
            qDebug() << "Ошибка ITK: " << e.what();
        } catch (const std::exception &e) {
            qDebug() << "Неожиданная ошибка: " << e.what();
        }
    } else {
        qDebug() << "Выбор файла отменен.";
        return;
    }



    if (currentData->image.IsNull()) {
        itk::Image<float, 3>::Pointer itkImage = ImageUtils::ConvertQImageToITKImage(currentData->thumbnail);
        initializeVTKImageViewer(itkImage);
    } else {
        initializeVTKImageViewer(currentData->image);
    }

}


void MainWindow::openFolder() {

    QString folderPath = QFileDialog::getExistingDirectory(
        nullptr,
        "Выберите папку",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly
        );

    if (!folderPath.isEmpty()) {
        qDebug() << "Выбрана папка:" << folderPath;

        if (!DICOMSeries.isEmpty()) {
            DICOMSeries.clear();
        }


        try {
            loadDicomFromDirectory(folderPath);


        } catch (const itk::ExceptionObject &e) {
            qDebug() << "Ошибка ITK: " << e.what();
        } catch (const std::exception &e) {
            qDebug() << "Неожиданная ошибка: " << e.what();
        }
    } else {
        qDebug() << "Выбор папки отменен.";
        return;
    }


    if (currentData->image.IsNull()) {
        itk::Image<float, 3>::Pointer itkImage = ImageUtils::ConvertQImageToITKImage(currentData->thumbnail);
        initializeVTKImageViewer(itkImage);
    } else {
        initializeVTKImageViewer(currentData->image);
    }
}


void MainWindow::loadDicomFromDirectory(const QString &folderPath) {


    using ImageType = itk::Image<float, 3>;

    auto namesGenerator = itk::GDCMSeriesFileNames::New();
    namesGenerator->SetDirectory(folderPath.toStdString());

    const auto seriesUIDs = namesGenerator->GetSeriesUIDs();
    if (seriesUIDs.empty()) {
        qDebug() << "Ошибка: В указанной папке нет DICOM-файлов.";
        return;
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

        data->metaData = dicomIO->GetMetaDataDictionary();

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

        std::string descriptionStr;
        if (!itk::ExposeMetaData<std::string>(data->metaData, "0008|103e", descriptionStr)) {    // Получаем Series Description
            qDebug() << "Ошибка: не удалось извлечь Series Description из метаданных.";
        }

        QString qDescription = QString::fromStdString(descriptionStr);
        if (qDescription.isEmpty()) {
            qDescription = data->seriesUID;
        }

        if(qDescription.contains("Topogram") || qDescription.contains("Patient Protocol")) {
            qDebug() << "Получено служебное изображения";
        } else {
            data->image = reader->GetOutput();
            qDebug() << "DICOM файл загружен, размер: "
                     << data->image->GetLargestPossibleRegion().GetSize()[0] << " x "
                     << data->image->GetLargestPossibleRegion().GetSize()[1] << " x "
                     << data->image->GetLargestPossibleRegion().GetSize()[2];
        }

        // Сохраняем список файлов
        for (const auto &file : files) {
            data->filePaths.append(QString::fromStdString(file));
        }

        data->generateThumbnail();
        data->seriesUID = QString::fromStdString(seriesUID);

        DICOMSeries[QString::fromStdString(seriesUID)] = data;

        currentData = DICOMSeries.value(QString::fromStdString(seriesUID));

        qDebug() << "Серия " << QString::fromStdString(seriesUID) << " загружена.";

        gallery->addThumbnail(qDescription, &currentData->thumbnail, data->seriesUID);
    }
    gallery->sortThumbnails();
    gallery->show();

}


void MainWindow::loadDicomFromFile(const QString &filePath) {


    using ImageType = itk::Image<float, 3>;

    auto dicomIO = itk::GDCMImageIO::New();
    auto reader = itk::ImageFileReader<ImageType>::New();
    reader->SetFileName(filePath.toStdString());
    reader->SetImageIO(dicomIO);


    try {
        reader->Update();
    } catch (itk::ExceptionObject &error) {
        qDebug() << "Ошибка при чтении DICOM-файла:" << error.what();
        return;
    }

    QSharedPointer<DataDICOM> data(new DataDICOM);

    data->metaData = dicomIO->GetMetaDataDictionary();

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

    std::string seriesUID;
    if (!itk::ExposeMetaData<std::string>(data->metaData, "0008|0016", seriesUID)) {
        qDebug() << "Ошибка: не удалось извлечь Series UID из метаданных.";
        return;
    }

    std::string descriptionStr;
    if (!itk::ExposeMetaData<std::string>(data->metaData, "0008|103e", descriptionStr)) {    // Получаем Series Description
        qDebug() << "Ошибка: не удалось извлечь Series Description из метаданных.";
    }

    QString qDescription = QString::fromStdString(descriptionStr);
    if (qDescription.isEmpty()) {
        qDescription = data->seriesUID;
    }

    if(qDescription.contains("Topogram") || qDescription.contains("Patient Protocol")) {
        qDebug() << "Получено служебное изображения";
    } else {
        data->image = reader->GetOutput();
        qDebug() << "DICOM файл загружен, размер: "
                 << data->image->GetLargestPossibleRegion().GetSize()[0] << " x "
                 << data->image->GetLargestPossibleRegion().GetSize()[1] << " x "
                 << data->image->GetLargestPossibleRegion().GetSize()[2];
    }

    // Сохраняем путь к файлу в DataDICOM
    data->filePaths.append(filePath);
    data->generateThumbnail();
    data->seriesUID = QString::fromStdString(seriesUID);

    DICOMSeries[QString::fromStdString(seriesUID)] = data;

    // Инициализация currentData
    auto it = DICOMSeries.find(QString::fromStdString(seriesUID));
    if (it != DICOMSeries.end()) {
        currentData = it.value();
    } else {
        qDebug() << "Ошибка: seriesUID не найден в DICOMSeries.";
        return;
    }

    qDebug() << "UID: " << QString::fromStdString(seriesUID) << " загружено.";
    qDebug() << "Файл сохранён по пути: " << data->filePaths[0];

    gallery->clearThumbnails();
    gallery->addThumbnail(qDescription, &currentData->thumbnail, data->seriesUID);
    gallery->show();
}






void MainWindow::initializeVTKImageViewer(const itk::Image<float, 3>::Pointer &image) {


    // Явно очищаем старый imageViewer, если он существует
    if (imageViewer) {
        imageViewer->GetRenderWindow()->RemoveRenderer(imageViewer->GetRenderer());
        imageViewer->GetRenderWindow()->Finalize();
        imageViewer = nullptr;  // Уничтожаем старый объект
    }

    // Создаём новый imageViewer
    imageViewer = vtkSmartPointer<vtkImageViewer2>::New();

    using ImageType = itk::Image<float, 3>;
    using ConverterType = itk::ImageToVTKImageFilter<ImageType>;

    // Конвертируем ITK-изображение в VTK-изображение
    ConverterType::Pointer converter = ConverterType::New();
    converter->SetInput(image);
    converter->Update();

    vtkSmartPointer<vtkImageData> vtkImage = converter->GetOutput();



    // Создаем фильтр для переворота изображения
    vtkSmartPointer<vtkImageFlip> flipFilter = vtkSmartPointer<vtkImageFlip>::New();
    flipFilter->SetInputData(vtkImage); // Устанавливаем входное изображение

    // Указываем ось для переворота:
    // 0 - ось X (горизонтальный переворот)
    // 1 - ось Y (вертикальный переворот)
    // 2 - ось Z (переворот по глубине)
    flipFilter->SetFilteredAxis(1); // Переворачиваем по оси Y (вертикальный переворот)

    // Запускаем фильтр
    flipFilter->Update();

    // Получаем перевернутое изображение
    vtkSmartPointer<vtkImageData> flippedImage = flipFilter->GetOutput();

    // Устанавливаем перевернутое изображение для отображения в imageViewer
    imageViewer->SetInputData(flippedImage);
    imageViewer->SetRenderWindow(renderWindow);  // Устанавливаем рендер-окно для imageViewer


    // Устанавливаем ориентацию среза
    //imageViewer->SetSliceOrientationToXY();  // Если нужно отображать срезы в плоскости XY
    //imageViewer->SetSliceOrientationToXZ();  // Если нужно отображать срезы в плоскости XZ
    //imageViewer->SetSliceOrientationToYZ();  // Если нужно отображать срезы в плоскости YZ

    // Устанавливаем срезы в imageViewer, если они не были установлены
    //imageViewer->SetSlice(0);  // Начальный срез

    // Создаем и настраиваем интерактор
    auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);  // Привязываем интерактор к рендер-окну

    // Создаем кастомный стиль интерактора
    auto interactorStyle = vtkSmartPointer<CustomInteractorStyle2D>::New();
    interactorStyle->SetImageViewer(imageViewer);  // Привязываем imageViewer к интерактору
    interactor->SetInteractorStyle(interactorStyle);  // Устанавливаем стиль интерактора
    interactorStyle->EnableScrolling(true);  // Включаем листание срезов

    // Инициализируем интерактор
    interactor->Initialize();

    qDebug() << "imageViewer->GetSliceMin();" << imageViewer->GetSliceMin();
    qDebug() << "imageViewer->GetSliceMax();" << imageViewer->GetSliceMax();

    // Рендерим изображение
    imageViewer->Render();
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

    // Сохраняем язык в настройки
    saveSettings();

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
        //saveSettings();

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
    settings.setValue("windowPosition", pos());
    settings.setValue("language", currentLanguage);

    // Сохраняем состояние док-виджетов
    settings.setValue("galleryVisible", pActGalleryPanel->isChecked());
}

void MainWindow::restoreSettings() {
    QSettings settings("CIVITAS MedTech", "CIVITAS");

    // Если первый запуск — задаем дефолтные настройки
    if (settings.allKeys().isEmpty()) {
        setDefaultSettings();
    }

    // Теперь загружаем настройки
    QString savedLang = settings.value("language").toString();
    changeLanguage(savedLang);

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
    settings.setValue("galleryVisible", true);
    settings.sync();  // Сохранить настройки
}

