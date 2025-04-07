#include "VTKPipelineViewer.h"


// Конструктор
VTKPipelineViewer::VTKPipelineViewer(QWidget* parent)
    : QWidget(parent), vtkWidget(new QVTKOpenGLNativeWidget(this)) {

    qDebug() << "VTKPipelineViewer::VTKPipelineViewer: Объект успешно создан";
}


void VTKPipelineViewer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(this, event->modifiers() & Qt::ControlModifier);
    }
    QWidget::mousePressEvent(event); // Не забудьте вызвать базовую реализацию
}


// Деструктор
VTKPipelineViewer::~VTKPipelineViewer() {
    qDebug() << "VTKPipelineViewer::~VTKPipelineViewer: Удаление объекта";
    // Qt и vtkSmartPointer сами очистят память.
}

void VTKPipelineViewer::setInteractorStyle(vtkSmartPointer<vtkInteractorStyleImage> interactorStyle) {
    this->interactorStyle = interactorStyle;
    interactor->SetInteractorStyle(interactorStyle);
}

// Инициализация пайплайна на основе данных DICOM
void VTKPipelineViewer::initializePipeline(const QSharedPointer<DataDICOM>& data) {


    qDebug() << "VTKPipelineViewer::initializePipeline: Инициализация пайплайна";

    // Инициализация VTK компонентов
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    vtkWidget->setRenderWindow(renderWindow);

    // Очистка предыдущего рендера
    if (imageViewer) {
        imageViewer->GetRenderWindow()->RemoveRenderer(imageViewer->GetRenderer());
        renderWindow->Finalize();
    }
    imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
    imageViewer->SetRenderWindow(renderWindow);

    // Получаем исходное VTK изображение (предполагается потокобезопасность внутри DataDICOM)
    inputImage = data->getVTKImage();
    if (!inputImage) {
        qDebug() << "VTKPipelineViewer::initializePipeline: Ошибка - исходное изображение не получено";
        return;
    }

    // Очистка ранее добавленных фильтров (если необходимо)
    clearFilters();

    // Перестраиваем пайплайн с учетом списка фильтров
    rebuildPipeline();

    interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindow->SetInteractor(interactor);

    // Инициализация интерактора и рендера
    interactor->Initialize();
    imageViewer->Render();

    qDebug() << "VTKPipelineViewer::initializePipeline: Пайплайн успешно инициализирован";
}



// Перестройка пайплайна с учётом списка фильтров
void VTKPipelineViewer::rebuildPipeline() {
    qDebug() << "VTKPipelineViewer::rebuildPipeline: Перестройка пайплайна";

    if (!inputImage) {
        qDebug() << "VTKPipelineViewer::rebuildPipeline: Входное изображение не задано!";
        return;
    }

    // Если список фильтров пуст, напрямую передаём исходное изображение
    if (filters.empty()) {
        imageViewer->SetInputData(inputImage);
    } else {
        // Используем локальную переменную для работы с пайплайном фильтров
        vtkSmartPointer<vtkImageData> currentImage = inputImage;

        for (const auto &name : filterNames) {
            if (filters.contains(name)) {
                qDebug() << "VTKPipelineViewer::rebuildPipeline: Найден фильтр: " << name;
                vtkSmartPointer<vtkImageAlgorithm> filter = filters.value(name);
                filter->SetInputData(currentImage);  // Передаем результат предыдущего фильтра
                filter->Update();  // Обновляем фильтр для выполнения операции

                // После применения фильтра, результат становится входом для следующего фильтра
                currentImage = filter->GetOutput();  // Передаем результат текущего фильтра как вход для следующего
                if(!currentImage) {
                    qDebug() << "VTKPipelineViewer::rebuildPipeline: Не удалось передать результат текущего фильтра как вход для следующего!";
                }
            } else {
                qDebug() << "VTKPipelineViewer::rebuildPipeline: Фильтр с таким названием не найден.";
            }
        }

        // Последний фильтр подключаем к imageViewer
        imageViewer->SetInputData(currentImage);  // Используем SetInputData для vtkImageData
    }
    // Обновление рендера
    imageViewer->Render();
    qDebug() << "VTKPipelineViewer::rebuildPipeline: Перестройка завершена";
}



void VTKPipelineViewer::setFlipFilter(int flip) {
    // Проверяем, создан ли фильтр, и если нет – создаем его
    qDebug() << "VTKPipelineViewer::setFlipFilter: Проверка наличия фильра Flip...";
    if (!filters.contains("Flip")) {
        qDebug() << "VTKPipelineViewer::setFlipFilter: Фильтр Flip не найден.";
        vtkSmartPointer<vtkImageFlip> flipFilter = vtkSmartPointer<vtkImageFlip>::New();
        addFilter("Flip", flipFilter);
    } else {
        qDebug() << "VTKPipelineViewer::setFlipFilter: Фильтр Flip найден.";
    }

     qDebug() << "VTKPipelineViewer::setFlipFilter: Настрока фильтра Flip:" << flip;
    // Настраиваем фильтр
    auto filter = vtkImageFlip::SafeDownCast(filters["Flip"]);
    if (filter) {
        filter->SetFilteredAxis(flip); // По умолчанию переворачиваем по оси Y
        filter->Update();
        imageViewer->Render();
    }
}

void VTKPipelineViewer::setWindowLevelFilter(double window, double level) {
    // Проверяем, создан ли фильтр, и если нет – создаем его
    qDebug() << "VTKPipelineViewer::setWindowLevelFilter: Проверка наличия фильтра WindowLevel...";
    if (!filters.contains("WindowLevel")) {
        qDebug() << "VTKPipelineViewer::setWindowLevelFilter: Фильтр WindowLevel не найден.";
        vtkSmartPointer<vtkImageMapToWindowLevelColors> windowLevelFilter = vtkSmartPointer<vtkImageMapToWindowLevelColors>::New();
        addFilter("WindowLevel", windowLevelFilter);
    } else {
        qDebug() << "VTKPipelineViewer::setWindowLevelFilter: Фильтр WindowLevel найден.";
    }

    // Настраиваем фильтр
    auto filter = vtkImageMapToWindowLevelColors::SafeDownCast(filters["WindowLevel"]);
    if (filter) {
        filter->SetWindow(window);
        filter->SetLevel(level);
        filter->Update();
        qDebug() << "VTKPipelineViewer::setWindowLevelFilter: Новые праметры окна: window =" << window << ", level =" << level;
        imageViewer->Render();
    }
}


void VTKPipelineViewer::addFilter(const QString& name, vtkSmartPointer<vtkImageAlgorithm> filter) {
    qDebug() << "VTKPipelineViewer::addFilter: Добавление нового фильтра: " << name;
    if(!filter) {
         qDebug() << "VTKPipelineViewer::addFilter: Получен пустой фильтр: " << name;
        return;
    }
    static QMap<QString, int> filterPriority = {
        {"Denoise", 1}, {"Normalize", 2}, {"Rescale", 3}, {"Flip", 4},
        {"Rotate", 5}, {"GaussianBlur", 6}, {"EdgeDetection", 7},
        {"Thresholding", 8}, {"Segmentation", 9}, {"Morphology", 10},
        {"BoneEnhancement", 11}, {"SoftTissueEnhancement", 12}, {"VesselEnhancement", 13},
        {"WindowLevel", 14}, {"LUT", 15}
    };

    // Находим место для вставки
    auto it = std::lower_bound(filterNames.begin(), filterNames.end(), name,
                               [&](const QString &existingFilter, const QString &newFilter) {
                                   return filterPriority.value(existingFilter, 100) < filterPriority.value(newFilter, 100);
                               }
                               );

    // Вставляем фильтр в правильное место
    filterNames.insert(it, name);
    filters.insert(name, filter);

    // Перестроение пайплайна
    rebuildPipeline();
}



void VTKPipelineViewer::addFilterToEnd(const QString& filterName, vtkSmartPointer<vtkImageAlgorithm> filter) {
    if (!filters.contains(filterName)) {
        filterNames.append(filterName);  // Добавляем в конец списка
        filters[filterName] = filter;    // Добавляем в хеш
        // Перестроение пайплайна
        rebuildPipeline();
    } else {
        qDebug() << "Фильтр с таким именем уже существует.";
    }
}

void VTKPipelineViewer::addFilterToFront(const QString& filterName, vtkSmartPointer<vtkImageAlgorithm> filter) {
    if (!filters.contains(filterName)) {
        filterNames.prepend(filterName);  // Добавляем в начало списка
        filters[filterName] = filter;     // Добавляем в хеш
        // Перестроение пайплайна
        rebuildPipeline();
    } else {
        qDebug() << "Фильтр с таким именем уже существует.";
    }
}

void VTKPipelineViewer::addFilterBefore(const QString& existingFilterName, const QString& newFilterName, vtkSmartPointer<vtkImageAlgorithm> newFilter) {
    int index = filterNames.indexOf(existingFilterName);
    if (index != -1 && !filters.contains(newFilterName)) {
        filterNames.insert(index, newFilterName);  // Вставляем перед существующим фильтром
        filters[newFilterName] = newFilter;
        // Перестроение пайплайна
        rebuildPipeline();
    } else {
        qDebug() << "Не удалось вставить фильтр перед" << existingFilterName;
    }
}

void VTKPipelineViewer::addFilterAfter(const QString& existingFilterName, const QString& newFilterName, vtkSmartPointer<vtkImageAlgorithm> newFilter) {
    int index = filterNames.indexOf(existingFilterName);
    if (index != -1 && !filters.contains(newFilterName)) {
        filterNames.insert(index + 1, newFilterName);  // Вставляем после существующего фильтра
        filters[newFilterName] = newFilter;
        // Перестроение пайплайна
        rebuildPipeline();
    } else {
        qDebug() << "Не удалось вставить фильтр после" << existingFilterName;
    }
}

void VTKPipelineViewer::removeFilter(const QString& filterName) {
    if (filters.contains(filterName)) {
        filters.remove(filterName);         // Удаляем фильтр из хеша
        filterNames.removeAll(filterName);  // Удаляем имя из списка

    } else {
        qDebug() << "Фильтр не найден для удаления.";
    }
}

void VTKPipelineViewer::replaceFilter(const QString& filterName, vtkSmartPointer<vtkImageAlgorithm> newFilter) {
    if (filters.contains(filterName)) {
        filters[filterName] = newFilter;  // Заменяем фильтр в хеше
    } else {
        qDebug() << "Фильтр не найден для замены.";
    }
}

void VTKPipelineViewer::moveFilterBefore(const QString& existingFilterName, const QString& filterName) {
    int currentIndex = filterNames.indexOf(filterName);
    int targetIndex = filterNames.indexOf(existingFilterName);

    if (currentIndex != -1 && targetIndex != -1) {
        filterNames.removeAt(currentIndex);       // Удаляем элемент из текущей позиции
        filterNames.insert(targetIndex, filterName);  // Вставляем перед
    } else {
        qDebug() << "Не удалось переместить фильтр.";
    }
}

void VTKPipelineViewer::moveFilterAfter(const QString& existingFilterName, const QString& filterName) {
    int currentIndex = filterNames.indexOf(filterName);
    int targetIndex = filterNames.indexOf(existingFilterName);

    if (currentIndex != -1 && targetIndex != -1) {
        filterNames.removeAt(currentIndex);       // Удаляем элемент из текущей позиции
        filterNames.insert(targetIndex + 1, filterName);  // Вставляем после
    } else {
        qDebug() << "Не удалось переместить фильтр.";
    }
}

vtkSmartPointer<vtkImageAlgorithm> VTKPipelineViewer::getFilter(const QString& filterName) const {
    if (filters.contains(filterName)) {
        return filters[filterName];
    } else {
        qDebug() << "Фильтр не найден.";
        return nullptr;
    }
}

QList<vtkSmartPointer<vtkImageAlgorithm>> VTKPipelineViewer::getFilters() const {
    QList<vtkSmartPointer<vtkImageAlgorithm>> filterList;
    for (const QString& filterName : filterNames) {
        filterList.append(filters[filterName]);
    }
    return filterList;
}

QString VTKPipelineViewer::getFilterName(vtkSmartPointer<vtkImageAlgorithm> filter) const {
    for (const QString& filterName : filterNames) {
        if (filters[filterName] == filter) {
            return filterName;
        }
    }
    return QString();  // Возвращаем пустую строку, если фильтр не найден
}

void VTKPipelineViewer::iterateFilters() const {
    for (const QString& filterName : filterNames) {
        vtkSmartPointer<vtkImageAlgorithm> filter = filters[filterName];
        qDebug() << "Фильтр: " << filterName;
        // Можно добавить дополнительную логику для работы с filter
    }
}

void VTKPipelineViewer::clearFilters() {
    filterNames.clear();  // Очищаем список имен фильтров
    filters.clear();      // Очищаем хеш с фильтрами
    qDebug() << "Все фильтры были очищены.";
}

void VTKPipelineViewer::setSliceOrientation(int sliceOrientation) {

}

