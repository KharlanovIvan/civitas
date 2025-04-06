#include "datadicom.h"
#include <itkImageToVTKImageFilter.h>
#include <QDebug>

DataDICOM::DataDICOM(QObject* parent) : QObject(parent) {
    qDebug() << "DataDICOM: Создан новый объект";
}

bool DataDICOM::setITKImage(itk::Image<float, 3>::Pointer image) {
    QMutexLocker locker(&vtkImageMutex); // Блокируем мьютекс на время работы

    if (!image) {
        qDebug() << "DataDICOM::setITKImage: Передано пустое ITK-изображение!";
        return false;
    }

    ITKImage = image; // Сохраняем ITK-изображение
    qDebug() << "DataDICOM::setITKImage: ITK изображение установлено.";

    VTKImage = ImageUtils::convertITKtoVTK(ITKImage);

    return true;
}


/*
vtkSmartPointer<vtkImageData> DataDICOM::convertITKtoVTK(itk::Image<float, 3>::Pointer ItkImage) {

    if (!ItkImage) {
        qDebug() << "DataDICOM::convertITKtoVTK: Ошибка! ITK-изображение отсутствует.";
        return nullptr;
    }

    qDebug() << "DataDICOM::convertITKtoVTK: Начинаем преобразование ITK->VTK...";

    try {
        using ITKToVTKFilterType = itk::ImageToVTKImageFilter<ImageType>;
        ITKToVTKFilterType::Pointer itkToVtkFilter = ITKToVTKFilterType::New();
        itkToVtkFilter->SetInput(ItkImage);
        itkToVtkFilter->Update();

        vtkSmartPointer<vtkImageData> VtkImage = vtkSmartPointer<vtkImageData>::New();
        VtkImage = itkToVtkFilter->GetOutput();

        if (!VtkImage) {
            qDebug() << "DataDICOM::convertITKtoVTK: Конвертация ITK->VTK не успела завершится!.";
            return nullptr;
        }

        qDebug() << "DataDICOM::convertITKtoVTK: Конвертация ITK->VTK завершена успешно.";

    } catch (const std::exception& e) {
        qDebug() << "DataDICOM::convertITKtoVTK: Исключение во время конвертации ->" << e.what();
        return nullptr;
    }

    return VtkImage;
}
*/
vtkSmartPointer<vtkImageData> DataDICOM::getVTKImage() {
    QMutexLocker locker(&vtkImageMutex);

    if (!VTKImage) {
        qDebug() << "DataDICOM::getVTKImage: VTK-изображение пустое!";
        return nullptr;
    }

    qDebug() << "DataDICOM::getVTKImage: Отдаём VTK-изображение.";
    return VTKImage;
}

vtkSmartPointer<vtkImageData> DataDICOM::getDeepCopyVTKImage() {
    QMutexLocker locker(&vtkImageMutex);

    if (!VTKImage) {
        qDebug() << "DataDICOM::getDeepCopyVTKImage: VTK-изображение ещё отсутствует!";
        return nullptr;
    }

    // Создаём новую копию изображения
    vtkSmartPointer<vtkImageData> deepCopy = vtkSmartPointer<vtkImageData>::New();
    deepCopy->DeepCopy(VTKImage);

    qDebug() << "DataDICOM::getDeepCopyVTKImage: Создана глубокая копия VTK-изображения.";
    return deepCopy;
}



// ======================== ГЕТТЕРЫ И СЕТТЕРЫ ========================
itk::Image<float, 3>::Pointer DataDICOM::getITKImage() const {
    return ITKImage;
}

QString DataDICOM::getSeriesUID() const {
    return seriesUID;
}

void DataDICOM::setSeriesUID(const QString& uid) {
    seriesUID = uid;
}

QString DataDICOM::getModality() const {
    return modality;
}

void DataDICOM::setModality(const QString& modality) {
    this->modality = modality;
}

QStringList DataDICOM::getFilePaths() const {
    return filePaths;
}

void DataDICOM::addFilePath(const QString &filePath) {
    filePaths.append(filePath);
}

double DataDICOM::getWindowWidth() const {
    return windowWidth;
}

void DataDICOM::setWindowWidth(double width) {
    windowWidth = width;
}

double DataDICOM::getWindowCenter() const {
    return windowCenter;
}

void DataDICOM::setWindowCenter(double center) {
    windowCenter = center;
}

const QImage& DataDICOM::getThumbnail() const {
    return thumbnail;
}
void DataDICOM::setThumbnail(QImage thumbnail){
    this->thumbnail = thumbnail;
}

itk::MetaDataDictionary DataDICOM::getMetaData() const {
    return metaData;
}

void DataDICOM::setMetadata(itk::MetaDataDictionary metaData) {
    this->metaData = metaData;
}




void DataDICOM::generateThumbnail() {
    if (filePaths.empty()) {
        qDebug() << "DataDICOM::generateThumbnail: filePaths is empty!";
        return;
    }

    int thumbnailIndex = filePaths.size() / 2;
    unsigned short bitsAllocated = 16; // Обычно 16 бит в DICOM

    using ImageType = itk::Image<float, 3>;

    auto dicomIO = itk::GDCMImageIO::New();
    auto reader = itk::ImageFileReader<ImageType>::New();
    reader->SetFileName(filePaths[thumbnailIndex].toStdString());
    reader->SetImageIO(dicomIO);

    try {
        reader->Update();
    } catch (itk::ExceptionObject &error) {
        qDebug() << "DataDICOM::generateThumbnail: Ошибка при чтении DICOM-файла:" << error.what();
        return;
    }

    std::string bitsAllocatedStr;
    if (itk::ExposeMetaData<std::string>(metaData, "0028|0100", bitsAllocatedStr)) {
        qDebug() << "DataDICOM::generateThumbnail: Bits Allocated - " << QString::fromStdString(bitsAllocatedStr);

        bitsAllocated = QString::fromStdString(bitsAllocatedStr).toUShort();
    }

    ImageType::Pointer originalITKImage = reader->GetOutput();
    itk::ImageRegionConstIterator<ImageType> origIt(
        originalITKImage,
        originalITKImage->GetLargestPossibleRegion()
        );

    float origMin = std::numeric_limits<float>::max();
    float origMax = std::numeric_limits<float>::lowest();
    for (origIt.GoToBegin(); !origIt.IsAtEnd(); ++origIt) {
        const float val = origIt.Get();
        origMin = std::min(origMin, val);
        origMax = std::max(origMax, val);
    }
    qDebug() << "DataDICOM::generateThumbnail: Original data range in ITKImage:" << origMin << "-" << origMax; // 0 - 0 означает, что все пиксели изображения имеют одинаковую интенсивность (значение 0)

    const float epsilon = 1e-6f; // Допустимая погрешность

    if (std::abs(origMin - 0.0f) < epsilon && std::abs(origMax - 0.0f) < epsilon) {
        // Все пиксели близки к нулю в пределах погрешности
        qDebug() << "DataDICOM::generateThumbnail: Все пиксели в ImageType::Pointer originalITKImage нулевые (или почти нулевые)";

        try {
            // Загружаем DICOM-файл
            QScopedPointer<DcmFileFormat> fileFormat(ImageUtils::loadDcmFileFormat(filePaths.at(thumbnailIndex)));
            if (!fileFormat) {
                qWarning() << "Ошибка загрузки DCM-файла!";
                return;
            }

            QScopedPointer<DicomImage> dicomImage(ImageUtils::getDicomImageFromDcmFileFormat(fileFormat.data()));
            if (!dicomImage || dicomImage->getStatus() != EIS_Normal) {
                qWarning() << "Ошибка обработки DICOM-изображения!";
                return;
            }

            // Конвертация в QImage
            thumbnail = ImageUtils::dicomImageToQImage(dicomImage.data(), static_cast<Uint16>(bitsAllocated));


        } catch (const std::exception& e) {
            qCritical() << "DataDICOM::generateThumbnail: Exception occurred:" << e.what();
        } catch (...) {
            qCritical() << "DataDICOM::generateThumbnail: Unknown exception occurred!";
        }
    } else {

        ImageType::Pointer ITKImageWindowFilter = ImageUtils::ApplyWindowLevelITK(originalITKImage, windowWidth, windowCenter);

        QScopedPointer<DicomImage> dcmImageThumbnail(
            ImageUtils::ConvertITKSliceToDicomImage(ITKImageWindowFilter)
            );

        thumbnail = ImageUtils::dicomImageToQImage(dcmImageThumbnail.data(), static_cast<Uint16>(bitsAllocated));
    }
}
