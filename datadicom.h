#pragma once

#include <QObject>
#include <QImage>

#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>

#include "imageutils.h"


class DataDICOM
{
public:

    QString seriesUID;
    QImage thumbnail;  // Теперь QImage хранится непосредственно в структуре
    itk::MetaDataDictionary metaData;
    itk::Image<float, 3>::Pointer image;
    QStringList filePaths; // Список путей всех файлов серии.

    DataDICOM();

    void generateThumbnail();

};

