#include "VTKPipelineViewer.h"
#include <QVBoxLayout>

// Конструктор
VTKPipelineViewer::VTKPipelineViewer(QWidget* parent)
    : QWidget(parent), vtkWidget(new QVTKOpenGLNativeWidget(this)) {

    qDebug() << "VTKPipelineViewer::VTKPipelineViewer: Создание объекта";

    // Инициализация VTK компонентов
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    vtkWidget->setRenderWindow(renderWindow);

    imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
    imageViewer->SetRenderWindow(renderWindow);

    interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindow->SetInteractor(interactor);

    qDebug() << "VTKPipelineViewer::VTKPipelineViewer: Объект успешно создан";
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

    // Инициализация интерактора и рендера
    interactor->Initialize();
    imageViewer->Render();

    qDebug() << "VTKPipelineViewer::initializePipeline: Пайплайн успешно инициализирован";
}



// Перестройка пайплайна с учётом списка фильтров
void VTKPipelineViewer::rebuildPipeline() {
    qDebug() << "VTKPipelineViewer::rebuildPipeline: Перестройка пайплайна";

    // Если список фильтров пуст, напрямую передаём исходное изображение
    if (filters.empty()) {
        imageViewer->SetInputData(inputImage);
    } else {
        // Используем локальную переменную для работы с пайплайном фильтров
        vtkSmartPointer<vtkImageData> currentImage = inputImage;

        for (const auto &name : filterNames) {
            if (filters.contains(name)) {
                vtkSmartPointer<vtkImageAlgorithm> filter = filters.value(name);
                filter->SetInputData(currentImage);  // Передаем результат предыдущего фильтра
                filter->Update();  // Обновляем фильтр для выполнения операции

                // После применения фильтра, результат становится входом для следующего фильтра
                currentImage = filter->GetOutput();  // Передаем результат текущего фильтра как вход для следующего
            } else {
                qDebug() << "Фильтр с таким названием не найден.";
            }
        }
        // Последний фильтр подключаем к imageViewer
        imageViewer->SetInputData(currentImage);  // Используем SetInputData для vtkImageData
    }
    // Обновление рендера
    imageViewer->Render();
    qDebug() << "VTKPipelineViewer::rebuildPipeline: Перестройка завершена";
}




void VTKPipelineViewer::addFilterToEnd(const QString& filterName, vtkSmartPointer<vtkImageAlgorithm> filter) {
    if (!filters.contains(filterName)) {
        filterNames.append(filterName);  // Добавляем в конец списка
        filters[filterName] = filter;    // Добавляем в хеш
    } else {
        qDebug() << "Фильтр с таким именем уже существует.";
    }
}

void VTKPipelineViewer::addFilterToFront(const QString& filterName, vtkSmartPointer<vtkImageAlgorithm> filter) {
    if (!filters.contains(filterName)) {
        filterNames.prepend(filterName);  // Добавляем в начало списка
        filters[filterName] = filter;     // Добавляем в хеш
    } else {
        qDebug() << "Фильтр с таким именем уже существует.";
    }
}

void VTKPipelineViewer::addFilterBefore(const QString& existingFilterName, const QString& newFilterName, vtkSmartPointer<vtkImageAlgorithm> newFilter) {
    int index = filterNames.indexOf(existingFilterName);
    if (index != -1 && !filters.contains(newFilterName)) {
        filterNames.insert(index, newFilterName);  // Вставляем перед существующим фильтром
        filters[newFilterName] = newFilter;
    } else {
        qDebug() << "Не удалось вставить фильтр перед" << existingFilterName;
    }
}

void VTKPipelineViewer::addFilterAfter(const QString& existingFilterName, const QString& newFilterName, vtkSmartPointer<vtkImageAlgorithm> newFilter) {
    int index = filterNames.indexOf(existingFilterName);
    if (index != -1 && !filters.contains(newFilterName)) {
        filterNames.insert(index + 1, newFilterName);  // Вставляем после существующего фильтра
        filters[newFilterName] = newFilter;
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

