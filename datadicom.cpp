#include "datadicom.h"

DataDICOM::DataDICOM() {}

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
