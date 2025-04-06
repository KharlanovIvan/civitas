#pragma once
#ifndef DATADICOM_H
#define DATADICOM_H

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QSharedPointer>

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkNew.h>

#include "imageutils.h"




class DataDICOM : public QObject {
    Q_OBJECT

public:
    explicit DataDICOM(QObject* parent = nullptr);

    // Получение независимой копии VTK изображения (потокобезопасно)
    vtkSmartPointer<vtkImageData> getVTKImage();

    // Генерация уменьшенного изображения (миниатюры)
    void generateThumbnail();

    // Геттеры и сеттеры
    itk::Image<float, 3>::Pointer getITKImage() const;
    bool setITKImage(itk::Image<float, 3>::Pointer image);

    QString getSeriesUID() const;
    void setSeriesUID(const QString& uid);

    QString getModality() const;
    void setModality(const QString& modality);

    QStringList getFilePaths() const;
    void addFilePath(const QString &filePath);

    double getWindowWidth() const;
    void setWindowWidth(double width);

    double getWindowCenter() const;
    void setWindowCenter(double center);

    const QImage& getThumbnail() const;
    void setThumbnail(QImage thumbnail);

    itk::MetaDataDictionary getMetaData() const;
    void setMetadata(itk::MetaDataDictionary metaData);

   // bool convertITKtoVTK();

    vtkSmartPointer<vtkImageData> getDeepCopyVTKImage();


private:
    mutable QMutex vtkImageMutex;  // Мьютекс для защиты данных

    QString seriesUID;    // Уникальный идентификатор серии
    QStringList filePaths;  // Пути к DICOM-файлам

    double windowWidth = 400;
    double windowCenter = 50;

    QString modality;

    QImage thumbnail;  // Миниатюра серии

    itk::MetaDataDictionary metaData; // Метаданные ITK

    // ITK и VTK изображения
    using ImageType = itk::Image<float, 3>;
    ImageType::Pointer ITKImage;
    vtkSmartPointer<vtkImageData> VTKImage;

};

#endif // DATADICOM_H
