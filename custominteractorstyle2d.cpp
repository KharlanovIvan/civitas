#include "custominteractorstyle2d.h"

// Макрос VTK для создания объекта через метод New()
vtkStandardNewMacro(CustomInteractorStyle2D);

// Устанавливаем ImageViewer, инициализируем границы серии
void CustomInteractorStyle2D::SetImageViewer(vtkSmartPointer<vtkImageViewer2> viewer) {
    ImageViewer = viewer;

    if (ImageViewer) {
        minSlice = ImageViewer->GetSliceMin();  // Получаем минимальный индекс среза
        maxSlice = ImageViewer->GetSliceMax();  // Получаем максимальный индекс среза
        currentSlice = ImageViewer->GetSlice(); // Устанавливаем текущий срез
        qDebug() << "Инициализация срезов: min =" << minSlice << ", max =" << maxSlice
                 << ", currentSlice =" << currentSlice;  // Добавлено логирование для отладки

        // Получаем размер изображения
        auto size = ImageViewer->GetSize();
        qDebug() << "Image Size:" << size[0] << "x" << size[1] << "x" << size[2];

    }
}

// Включение или отключение прокрутки серий
void CustomInteractorStyle2D::EnableScrolling(bool enable) {
    scrollingEnabled = enable;
}

// Обновление текущего среза
void CustomInteractorStyle2D::UpdateSlice(int sliceDelta) {
    if (!scrollingEnabled || !ImageViewer) return; // Проверяем, включена ли прокрутка и есть ли ImageViewer

    int newSlice = currentSlice + sliceDelta;

    // Логирование для отладки
    qDebug() << "Текущий срез: " << currentSlice << ", Новый срез: " << newSlice
             << ", minSlice: " << minSlice << ", maxSlice: " << maxSlice;

    // Проверяем, не выходит ли новый срез за границы
    if (newSlice >= minSlice && newSlice <= maxSlice) {
        currentSlice = newSlice;
        ImageViewer->SetSlice(newSlice);  // Устанавливаем новый срез
        ImageViewer->Render();  // Перерисовываем изображение
        qDebug() << "Текущий срез: " << newSlice;
    } else {
        qDebug() << "Срез выходит за допустимые границы!";
    }
}

// Обработчик события: колесико мыши вверх
void CustomInteractorStyle2D::OnMouseWheelForward() {
    UpdateSlice(1);  // Переключаем на следующий срез
    if (!scrollingEnabled || !ImageViewer) { // Проверяем, включена ли прокрутка и есть ли ImageViewer
        vtkInteractorStyleImage::OnMouseWheelForward();  // Вызываем базовый метод для стандартной логики колесика
    }
}

// Обработчик события: колесико мыши вниз
void CustomInteractorStyle2D::OnMouseWheelBackward() {
    UpdateSlice(-1);  // Переключаем на предыдущий срез
   if (!scrollingEnabled || !ImageViewer) { // Проверяем, включена ли прокрутка и есть ли ImageViewer
        vtkInteractorStyleImage::OnMouseWheelBackward();  // Вызываем базовый метод для стандартной логики колесика
   }
}

// Обработчик события: нажатие левой кнопки мыши
void CustomInteractorStyle2D::OnLeftButtonDown() {
    qDebug() << "Левая кнопка мыши нажата!";
    vtkInteractorStyleImage::OnLeftButtonDown();  // Вызываем базовый метод
}

// Обработчик события: нажатие правой кнопки мыши
void CustomInteractorStyle2D::OnRightButtonDown() {
    qDebug() << "Правая кнопка мыши нажата!";
    vtkInteractorStyleImage::OnRightButtonDown();  // Вызываем базовый метод
}

// Обработчик события: нажатие клавиши на клавиатуре
void CustomInteractorStyle2D::OnKeyPress() {
    vtkRenderWindowInteractor* interactor = this->GetInteractor();
    std::string key = interactor->GetKeySym();  // Получаем символ нажатой клавиши
    qDebug() << "Нажата клавиша: " << QString::fromStdString(key);

    if (key == "r") {  // Если нажата клавиша "r"
        qDebug() << "Сброс камеры!";
        if (this->GetCurrentRenderer()) {
            this->GetCurrentRenderer()->ResetCamera();  // Сбрасываем положение камеры
        }
    }

    vtkInteractorStyleImage::OnKeyPress();  // Вызываем базовый метод
}


void CustomInteractorStyle2D::Reset() {
    // Сбрасываем текущий срез и границы срезов
    if (ImageViewer) {
        minSlice = ImageViewer->GetSliceMin();  // Получаем минимальный индекс среза
        maxSlice = ImageViewer->GetSliceMax();  // Получаем максимальный индекс среза
        currentSlice = ImageViewer->GetSlice(); // Устанавливаем текущий срез
    } else {
        minSlice = 0;
        maxSlice = 0;
        currentSlice = 0;
    }

    // Сбрасываем флаг прокрутки
    scrollingEnabled = false;

    // Логирование для отладки
    qDebug() << "Сброс настроек интерактора: minSlice =" << minSlice
             << ", maxSlice =" << maxSlice
             << ", currentSlice =" << currentSlice;
}
