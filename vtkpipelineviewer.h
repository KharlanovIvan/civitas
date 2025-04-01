#pragma once

#include <QWidget>
#include <QDebug>


// VTK-заголовки
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkImageAlgorithm.h>

// Необходимые фильтры (пример: window/level, flip)
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageFlip.h>

#include "CustomInteractorStyle2D.h"
#include "DataDICOM.h"



class VTKPipelineViewer : public QWidget {
    Q_OBJECT
public:
    // Конструктор/Деструктор
    explicit VTKPipelineViewer(QWidget* parent = nullptr);
    virtual ~VTKPipelineViewer();

    // Инициализация пайплайна с данными DICOM
    void initializePipeline(const QSharedPointer<DataDICOM>& data);

    // Получение виджета VTK для размещения в layout
    QVTKOpenGLNativeWidget* getVTKWidget() const { return vtkWidget; }

    vtkSmartPointer<vtkInteractorStyleImage> getInteractorStyle() const { return interactorStyle; }
    void setInteractorStyle(vtkSmartPointer<vtkInteractorStyleImage> interactorStyle);


    // Методы управления фильтрами
    // Добавление фильтра
    void addFilterToEnd(const QString& filterName, vtkSmartPointer<vtkImageAlgorithm> filter);
    void addFilterToFront(const QString& filterName, vtkSmartPointer<vtkImageAlgorithm> filter);
    void addFilterBefore(const QString& existingFilterName, const QString& newFilterName, vtkSmartPointer<vtkImageAlgorithm> newFilter);
    void addFilterAfter(const QString& existingFilterName, const QString& newFilterName, vtkSmartPointer<vtkImageAlgorithm> newFilter);

    // Удаление фильтра
    void removeFilter(const QString& filterName);

    // Замена фильтра
    void replaceFilter(const QString& filterName, vtkSmartPointer<vtkImageAlgorithm> newFilter);

    // Перемещение фильтра
    void moveFilterBefore(const QString& existingFilterName, const QString& filterName);
    void moveFilterAfter(const QString& existingFilterName, const QString& filterName);

    // Получить фильтр по имени
    vtkSmartPointer<vtkImageAlgorithm> getFilter(const QString& filterName) const;

    // Получить список фильтров
    QList<vtkSmartPointer<vtkImageAlgorithm>> getFilters() const;

    // Получить название фильтра по объекту фильтра
    QString getFilterName(vtkSmartPointer<vtkImageAlgorithm> filter) const;

    // Итерация по фильтрам
    void iterateFilters() const;

    void clearFilters();

    void setSliceOrientation(int sliceOrientation);
    int getSliceOrientation() { return sliceOrientation; }


private:

    int sliceOrientation = vtkImageViewer2::SLICE_ORIENTATION_XY;

    // Основные компоненты VTK-пайплайна
    QVTKOpenGLNativeWidget* vtkWidget; // ВИДЖЕТ для отображения VTK
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkImageViewer2> imageViewer;
    vtkSmartPointer<vtkRenderWindowInteractor> interactor;
    vtkSmartPointer<vtkInteractorStyleImage> interactorStyle;

    vtkSmartPointer<vtkImageData> inputImage;

    // Список фильтров (порядок важен)
    QList<QString> filterNames; // Хранит названия и порядок фильтров
    QHash<QString, vtkSmartPointer<vtkImageAlgorithm>> filters; // Хранит сами фильтры

    // Метод для перестроения пайплайна с учётом списка фильтров
    void rebuildPipeline();

};


