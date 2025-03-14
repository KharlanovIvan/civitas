#pragma once  // Защита от повторного включения

#include <QString>
#include <QDebug>
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageViewer2.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkObjectFactory.h>
#include <vtkImageData.h>       // Заголовочный файл для vtkImageData
#include <vtkImageViewer2.h>     // Заголовочный файл для vtkImageViewer2

// Пользовательский интерактор для работы с 2D изображениями
class CustomInteractorStyle2D : public vtkInteractorStyleImage {
public:
    // Статический метод создания объекта (использует VTK-специфический макрос)
    static CustomInteractorStyle2D* New();

    // Макрос VTK для управления временем жизни объекта и полиморфизма
    vtkTypeMacro(CustomInteractorStyle2D, vtkInteractorStyleImage);

    // Установка объекта ImageViewer для работы со срезами
    void SetImageViewer(vtkSmartPointer<vtkImageViewer2> viewer);

    // Включение или отключение возможности прокрутки срезов
    void EnableScrolling(bool enable);

    // Метод для сброса настроек интерактора
    void Reset();

    // Обработчики событий
    void OnMouseWheelForward() override;  // Прокрутка колесика вверх
    void OnMouseWheelBackward() override; // Прокрутка колесика вниз
    void OnLeftButtonDown() override;     // Нажатие левой кнопки мыши
    void OnRightButtonDown() override;    // Нажатие правой кнопки мыши
    void OnKeyPress() override;           // Нажатие клавиши

private:
    vtkSmartPointer<vtkImageViewer2> ImageViewer;  // Объект для работы с 2D изображениями
    int currentSlice = 0;  // Текущий срез
    int minSlice = 0;      // Минимальный индекс среза
    int maxSlice = 0;      // Максимальный индекс среза
    bool scrollingEnabled = false;  // Флаг включения/выключения прокрутки

    // Метод для обновления текущего среза
    void UpdateSlice(int sliceDelta);
};
