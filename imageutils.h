#pragma once

#include <QImage>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QSharedPointer>

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include <itkImageRegionConstIterator.h>
#include <itkMetaDataObject.h>
#include <itkImage.h>
#include <itkGDCMImageIO.h>
#include <itkImageFileReader.h>
#include <itkMatrix.h>
#include <itkFlipImageFilter.h>
#include <itkImportImageFilter.h>
#include <itkImageToVTKImageFilter.h>
#include <itkImage.h>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkNew.h>




class ImageUtils {
public:
    // Запрещаем создание экземпляров класса
    ImageUtils() = delete;

    static QImage dicomImageToQImage(DicomImage* dicomImage, Uint16 bitsAllocated);

    static DcmFileFormat* loadDcmFileFormat(const QString &filePath);

    static DicomImage* getDicomImageFromDcmFileFormat(DcmFileFormat* dcmFileFormat);

    static QImage loadDicomFileToQImageUseDCMTK(const QString &filePath);

    static QString getImageOrientation(const itk::MetaDataDictionary &metaDataDict);

    static QString getOrientationFromDirection(const itk::Matrix<double, 3, 3> &direction);

    static itk::Image<float, 3>::Pointer correctImageOrientation(itk::Image<float, 3>::Pointer image);

    static itk::Image<float, 3>::Pointer ConvertQImageToITKImage(const QImage &qImage);


    static itk::Image<float, 3>::Pointer ApplyWindowLevelITK(itk::Image<float, 3>::Pointer image, double window, double level);

    static DicomImage* ConvertITKSliceToDicomImage(itk::Image<float, 3>::Pointer itkImageSlice);

    static vtkSmartPointer<vtkImageData> convertITKtoVTK(itk::Image<float, 3>::Pointer ItkImage);

};
