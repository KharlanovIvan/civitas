#include "modalityinitializerfactory.h"


IModalityInitializer* ModalityInitializerFactory::createInitializer(const QString& modality) {
    if (modality.compare("CT", Qt::CaseInsensitive) == 0) {
        return new CTInitializer();
    } else if (modality.compare("MR", Qt::CaseInsensitive) == 0) {
        return new MRInitializer();
    }
    // Можно добавить дополнительные модальности или возвращать базовый вариант
    return nullptr;
}
