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

    return true;
}

bool DataDICOM::convertITKtoVTK() {
    QMutexLocker locker(&vtkImageMutex);

    if (!ITKImage) {
        qDebug() << "DataDICOM::convertITKtoVTK: Ошибка! ITK-изображение отсутствует.";
        return false;
    }

    qDebug() << "DataDICOM::convertITKtoVTK: Начинаем преобразование ITK->VTK...";

    try {
        using ITKToVTKFilterType = itk::ImageToVTKImageFilter<ImageType>;
        ITKToVTKFilterType::Pointer itkToVtkFilter = ITKToVTKFilterType::New();
        itkToVtkFilter->SetInput(ITKImage);
        itkToVtkFilter->Update();

        VTKImage = vtkSmartPointer<vtkImageData>::New();
        VTKImage = itkToVtkFilter->GetOutput();

        if (!VTKImage) {
            vtkImageReady = false;
            qDebug() << "DataDICOM::convertITKtoVTK: Конвертация ITK->VTK не успела завершится!.";
            return false;
        }

        vtkImageReady = true;
        qDebug() << "DataDICOM::convertITKtoVTK: Конвертация ITK->VTK завершена успешно.";
        return true;
    } catch (const std::exception& e) {
        qDebug() << "DataDICOM::convertITKtoVTK: Исключение во время конвертации ->" << e.what();
        return false;
    }
}

vtkSmartPointer<vtkImageData> DataDICOM::getVTKImage() {
    QMutexLocker locker(&vtkImageMutex);

    if (!vtkImageReady) {
        qDebug() << "DataDICOM::getVTKImage: VTK-изображение ещё не готово!";
        return nullptr;
    }

    qDebug() << "DataDICOM::getVTKImage: Отдаём VTK-изображение.";
    return VTKImage;
}

vtkSmartPointer<vtkImageData> DataDICOM::getDeepCopyVTKImage() {
    QMutexLocker locker(&vtkImageMutex);

    if (!vtkImageReady || !VTKImage) {
        qDebug() << "DataDICOM::getDeepCopyVTKImage: VTK-изображение ещё не готово или отсутствует!";
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

bool DataDICOM::isVTKImageReady() const {
    return vtkImageReady;
}


void DataDICOM::generateThumbnail() {
    if (filePaths.empty()) {
        qDebug() << "DataDICOM::generateThumbnail: filePaths is empty!";
        return;
    }

    int thumbnailIndex = filePaths.size() / 2;
    unsigned short bitsAllocated = 0; // Обычно 16 бит в DICOM

    try {
        // Извлекаем BitsAllocated
        if (metaData.HasKey("0028|0100")) {
            std::string bitsAllocatedStr;
            itk::ExposeMetaData(metaData, "0028|0100", bitsAllocatedStr);
            qDebug() << "BitsAllocated (str): " << bitsAllocatedStr;

            bitsAllocated = static_cast<unsigned short>(std::stoi(bitsAllocatedStr));
        } else {
            qWarning() << "DataDICOM::generateThumbnail: Key '0028|0100' not found in metadata!";
            return;
        }

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
}
