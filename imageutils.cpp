#include "imageutils.h"



QImage ImageUtils::loadDicomFileToQImageUseDCMTK(const QString &filePath) {
    QImage image;

    if (filePath.isEmpty()) {
        qDebug() << "ImageUtils::generateThumbnail: filePaths is empty!";
        return image;
    }

    unsigned short bitsAllocated = 0; // Обычно 16 бит в DICOM

    try {

        // Загружаем DICOM-файл
        QScopedPointer<DcmFileFormat> fileFormat(loadDcmFileFormat(filePath));
        if (!fileFormat) {
            qWarning() << "ImageUtils::loadDicomFileToQImage: Ошибка загрузки DCM-файла!";

        }

        DcmDataset* dataset = fileFormat->getDataset();
        // Читаем значение тега (0028,0100)
        if (dataset->findAndGetUint16(DCM_BitsAllocated, bitsAllocated).good()) {
            qWarning() << "ImageUtils::loadDicomFileToQImage: BitsAllocated: " << bitsAllocated;
        } else {
            qWarning() << "ImageUtils::loadDicomFileToQImage: Ошибка: не удалось получить BitsAllocated";
        }
        delete(dataset);

        QScopedPointer<DicomImage> dicomImage(getDicomImageFromDcmFileFormat(fileFormat.data()));
        if (!dicomImage || dicomImage->getStatus() != EIS_Normal) {
            qWarning() << "ImageUtils::loadDicomFileToQImage: Ошибка обработки DICOM-изображения!";
        }

        // Конвертация в QImage
        image = ImageUtils::dicomImageToQImage(dicomImage.data(), static_cast<Uint16>(bitsAllocated));

    } catch (const std::exception& e) {
        qCritical() << "ImageUtils::generateThumbnail: Exception occurred:" << e.what();
    } catch (...) {
        qCritical() << "ImageUtils::generateThumbnail: Unknown exception occurred!";
    }

    return image;
}


QImage ImageUtils::dicomImageToQImage(DicomImage* dicomImage, Uint16 bitsAllocated) {
    qDebug() << "ImageUtils::dicomImageToQImage: Начало конвертации DicomImage в QImage...";

    if (!dicomImage) {
        qWarning() << "ImageUtils::dicomImageToQImage: Ошибка - передан нулевой указатель!";
        return QImage();
    }

    if (dicomImage->getStatus() != EIS_Normal) {
        qWarning() << "ImageUtils::dicomImageToQImage: Ошибка загрузки DICOM-изображения. Код статуса:"
                   << dicomImage->getStatus();
        return QImage();
    }

    if (bitsAllocated != 8 && bitsAllocated != 16) {
        qWarning() << "ImageUtils::dicomImageToQImage: Неподдерживаемый bitsAllocated =" << bitsAllocated
                   << ". Попытка автоподбора значений...";
        bitsAllocated = 16;  // Пробуем сначала 16 бит
    }

    unsigned long width = dicomImage->getWidth();
    unsigned long height = dicomImage->getHeight();

    const Uint8* pixelData = (const Uint8*)dicomImage->getOutputData(bitsAllocated);

    if (!pixelData) {
        qWarning() << "ImageUtils::dicomImageToQImage: Ошибка получения данных с bitsAllocated =" << bitsAllocated;

        // Попробуем альтернативную глубину цвета
        bitsAllocated = (bitsAllocated == 16) ? 8 : 16;
        qDebug() << "ImageUtils::dicomImageToQImage: Пробуем использовать bitsAllocated =" << bitsAllocated;

        pixelData = (const Uint8*)dicomImage->getOutputData(bitsAllocated);

        if (!pixelData) {
            qCritical() << "ImageUtils::dicomImageToQImage: Не удалось получить данные изображения ни с 16, ни с 8 битами!";
            return QImage();
        }
    }

    // Вычисляем размер данных
    int bytesPerPixel = (bitsAllocated == 8) ? 1 : 2;
    int imageSize = width * height * bytesPerPixel;

    // Используем QByteArray с резервированием памяти (уменьшаем вероятность повторных аллокаций)
    QByteArray pixelDataCopy;
    pixelDataCopy.reserve(imageSize);
    pixelDataCopy.append((const char*)pixelData, imageSize);

    // Создание QImage с независимой копией данных
    QImage image;
    if (bitsAllocated == 8) {
        image = QImage((const uchar*)pixelDataCopy.constData(), width, height, QImage::Format_Grayscale8).copy();
    } else if (bitsAllocated == 16) {
        image = QImage((const uchar*)pixelDataCopy.constData(), width, height, QImage::Format_Grayscale16).copy();
    }

    if (image.isNull()) {
        qCritical() << "ImageUtils::dicomImageToQImage: Ошибка создания QImage!";
    } else {
        qDebug() << "ImageUtils::dicomImageToQImage: Успешно создано изображение" << width << "x" << height;
    }

    return image;
}



DcmFileFormat* ImageUtils::loadDcmFileFormat(const QString &filePath) {
    qDebug() << "ImageUtils::loadDcmFileFormat: Попытка загрузки файла:" << filePath;

    // Проверка существования файла
    if (!QFile::exists(filePath)) {
        qWarning() << "ImageUtils::loadDcmFileFormat: Ошибка - файл не существует:" << filePath;
        return nullptr;
    }

    // Используем unique_ptr для автоматического управления памятью
    std::unique_ptr<DcmFileFormat> fileFormat = std::make_unique<DcmFileFormat>();

    // Загружаем файл
    OFCondition status = fileFormat->loadFile(filePath.toLocal8Bit().constData());

    if (!status.good()) {
        qWarning() << "ImageUtils::loadDcmFileFormat: Ошибка загрузки DICOM-файла:" << QString(status.text());
        return nullptr;  // unique_ptr автоматически освободит память
    }

    qDebug() << "ImageUtils::loadDcmFileFormat: Файл успешно загружен:" << filePath;

    // Передаём владение объектом наружу (не освобождая память)
    return fileFormat.release();
}





DicomImage* ImageUtils::getDicomImageFromDcmFileFormat(DcmFileFormat* dcmFileFormat) {
    if (!dcmFileFormat || !dcmFileFormat->getDataset()) {
        qWarning() << "ImageUtils::getDicomImageFromDcmFileFormat: Некорректный DcmFileFormat (nullptr или поврежденный dataset).";
        return nullptr;
    }

    if (!dcmFileFormat->getDataset()->tagExists(DCM_PixelData)) {
        qWarning() << "ImageUtils::getDicomImageFromDcmFileFormat: Пиксельные данные не найдены в DICOM-файле.";
        return nullptr;
    }

    // Используем unique_ptr для управления памятью
    std::unique_ptr<DicomImage> dicomImage = std::make_unique<DicomImage>(
        dcmFileFormat->getDataset(),
        dcmFileFormat->getDataset()->getOriginalXfer()
        );

    EI_Status status = dicomImage->getStatus();
    if (status == EIS_Normal) {
        qDebug() << "ImageUtils::getDicomImageFromDcmFileFormat: DICOM-изображение загружено. Размеры: "
                 << dicomImage->getWidth() << "x" << dicomImage->getHeight();
        return dicomImage.release(); // Передаем владение наружу
    }

    // Логирование ошибок в зависимости от статуса
    QString errorMsg;
    switch (status) {
    case EIS_InvalidDocument:
        errorMsg = "Файл DICOM не является корректным DICOM-документом.";
        break;
    case EIS_InvalidImage:
        errorMsg = "Невозможно прочитать изображение из DICOM-документа.";
        break;
    case EIS_NoDataDictionary:
        errorMsg = "Отсутствует словарь данных DICOM.";
        break;
    case EIS_NotSupportedValue:
        errorMsg = "Некорректное или неподдерживаемое значение в DICOM.";
        break;
    case EIS_MemoryFailure:
        errorMsg = "Ошибка управления памятью при загрузке DICOM.";
        break;
    case EIS_InvalidValue:
        errorMsg = "Ошибка из-за некорректных данных DICOM.";
        break;
    case EIS_MissingAttribute:
        errorMsg = "Отсутствуют обязательные атрибуты DICOM.";
        break;
    case EIS_OtherError:
    default:
        errorMsg = "Неизвестная ошибка загрузки DICOM-изображения.";
        break;
    }

    qWarning() << "ImageUtils::getDicomImageFromDcmFileFormat: Ошибка загрузки изображения DICOM: " << errorMsg;
    return nullptr;
}


QString ImageUtils::getImageOrientation(const itk::MetaDataDictionary &metaDataDict) {
    std::string imageOrientation;
    if (itk::ExposeMetaData<std::string>(metaDataDict, "0020|0037", imageOrientation)) {
        qDebug() << "Image Orientation (Patient): " << QString::fromStdString(imageOrientation);

        if (imageOrientation == "1\\0\\0\\0\\1\\0") {
            qDebug() << "Ориентация: Аксиальная (стандартная)";
            return "Аксиальная (стандартная)";
        } else if (imageOrientation == "1\\0\\0\\0\\0\\1") {
            qDebug() << "Ориентация: Корональная";
            return "Корональная";
        } else if (imageOrientation == "0\\1\\0\\0\\0\\1") {
            qDebug() << "Ориентация: Сагиттальная";
            return "Сагиттальная";
        } else if (imageOrientation == "0\\1\\0\\1\\0\\0") {
            qDebug() << "Ориентация: Нестандартная (горизонтально повернутая топограмма)";
            return "Нестандартная (горизонтально повернутая топограмма)";
        } else {
            qDebug() << "Ориентация: НЕОПРЕДЕЛЕННАЯ → " << QString::fromStdString(imageOrientation);
            return "Неопределенная ориентация";
        }
    } else {
        qDebug() << "Ошибка: Не удалось получить тег 0020|0037 (Image Orientation)";
        return "Ошибка при получении ориентации";
    }
}

#include <cmath> // Для std::abs

bool isApproximatelyEqual(const itk::Matrix<double, 3, 3>& mat1, const itk::Matrix<double, 3, 3>& mat2, double epsilon = 1e-5) {
    for (unsigned int i = 0; i < 3; ++i) {
        for (unsigned int j = 0; j < 3; ++j) {
            if (std::abs(mat1[i][j] - mat2[i][j]) > epsilon) {
                return false;
            }
        }
    }
    return true;
}


QString ImageUtils::getOrientationFromDirection(const itk::Matrix<double, 3, 3>& direction) {
    itk::Matrix<double, 3, 3> axial, coronal, sagittal;
    axial.SetIdentity();

    coronal[0][0] = 1; coronal[0][1] = 0; coronal[0][2] = 0;
    coronal[1][0] = 0; coronal[1][1] = 0; coronal[1][2] = 1;
    coronal[2][0] = 0; coronal[2][1] = -1; coronal[2][2] = 0;

    sagittal[0][0] = 0; sagittal[0][1] = 1; sagittal[0][2] = 0;
    sagittal[1][0] = 0; sagittal[1][1] = 0; sagittal[1][2] = 1;
    sagittal[2][0] = -1; sagittal[2][1] = 0; sagittal[2][2] = 0;

    if (isApproximatelyEqual(direction, axial)) return "АКСИАЛЬНАЯ";
    if (isApproximatelyEqual(direction, coronal)) return "КОРОНАЛЬНАЯ";
    if (isApproximatelyEqual(direction, sagittal)) return "САГИТТАЛЬНАЯ";

    return "НЕСТАНДАРТНАЯ";
}



itk::Image<float, 3>::Pointer ImageUtils::correctImageOrientation(itk::Image<float, 3>::Pointer image) {
    using FlipFilterType = itk::FlipImageFilter<itk::Image<float, 3>>;
    auto flipFilter = FlipFilterType::New();

    FlipFilterType::FlipAxesArrayType flipAxes;
    flipAxes[0] = true;
    flipAxes[1] = false;
    flipAxes[2] = false;

    flipFilter->SetInput(image);
    flipFilter->SetFlipAxes(flipAxes);
    flipFilter->Update();

    return flipFilter->GetOutput();
}


#include <stdexcept>

itk::Image<float, 3>::Pointer ImageUtils::ConvertQImageToITKImage(const QImage &qImage) {
    // Проверяем, что изображение не пустое
    if (qImage.isNull()) {
        throw std::runtime_error("QImage is null!");
    }

    // Преобразуем QImage в формат, который можно использовать (например, Format_ARGB32)
    QImage image = qImage.convertToFormat(QImage::Format_ARGB32);

    // Получаем размеры изображения
    int width = image.width();
    int height = image.height();
    int depth = 1; // Третье измерение (глубина) равно 1, так как это 2D-изображение

    // Проверяем, что размеры изображения корректны
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid image dimensions!");
    }

    // Создаем массив для хранения данных изображения
    float *data = new float[width * height * depth];

    // Копируем данные из QImage в массив
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            QRgb pixel = image.pixel(x, y);
            float grayValue = qGray(pixel); // Преобразуем цвет в оттенок серого
            data[y * width + x] = grayValue; // Записываем значение в массив
        }
    }

    // Используем ImportImageFilter для создания ITK-изображения
    using ImportFilterType = itk::ImportImageFilter<float, 3>;
    ImportFilterType::Pointer importFilter = ImportFilterType::New();

    // Устанавливаем размер изображения
    ImportFilterType::SizeType size;
    size[0] = width;  // Размер по X
    size[1] = height; // Размер по Y
    size[2] = depth;  // Размер по Z

    // Устанавливаем начало координат
    ImportFilterType::IndexType start;
    start.Fill(0);

    // Устанавливаем регион
    ImportFilterType::RegionType region;
    region.SetIndex(start);
    region.SetSize(size);

    importFilter->SetRegion(region);

    // Устанавливаем spacing (расстояние между пикселями)
    double spacing[3];
    spacing[0] = 1.0; // По X
    spacing[1] = 1.0; // По Y
    spacing[2] = 1.0; // По Z
    importFilter->SetSpacing(spacing);

    // Устанавливаем начало координат в пространстве
    double origin[3];
    origin[0] = 0.0; // По X
    origin[1] = 0.0; // По Y
    origin[2] = 0.0; // По Z
    importFilter->SetOrigin(origin);

    // Устанавливаем данные изображения
    const bool importImageFilterWillOwnTheBuffer = true;
    importFilter->SetImportPointer(data, width * height * depth, importImageFilterWillOwnTheBuffer);

    try {
        // Запускаем фильтр
        importFilter->Update();
    } catch (itk::ExceptionObject &error) {
        // Ловим исключения ITK
        delete[] data; // Освобождаем память в случае ошибки
        throw std::runtime_error("ITK error: " + std::string(error.what()));
    }

    // Возвращаем результат
    return importFilter->GetOutput();
}
