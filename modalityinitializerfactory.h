#pragma once

#include <QString>

#include "IModalityInitializer.h"
#include "mrinitializer.h"
#include "ctinitializer.h"


class ModalityInitializerFactory {
public:
    static IModalityInitializer* createInitializer(const QString& modality);
};
