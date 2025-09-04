# CIVITAS — Qt/VTK DICOM Viewer (WIP)

Мини‑просмотрщик медицинских изображений DICOM на **Qt (Widgets)** с визуализацией через **VTK**, обработкой и метаданными из **ITK** и **DCMTK**, а также заготовками для фильтров **OpenCV**. Проект находится на ранней стадии и предназначен как демонстрация моих навыков C++/Qt и работы со стеком медицинской визуализации.

> Цель: показать архитектурный подход (модули, пайплайн фильтров, док‑панели, локализация, работа с vcpkg/CMake) и базовый функционал просмотра серий DICOM.

---

## 🔧 Технологический стек
- **C++17**, **CMake ≥ 3.16**
- **Qt 6 / Qt 5 (Widgets, LinguistTools)**
- **VTK** (QVTKOpenGLNativeWidget, ImageViewer2, фильтры)
- **ITK** (чтение серий, обработка, конвертация ITK→VTK)
- **DCMTK** (чтение DICOM, извлечение метаданных, рендер миниатюр)
- **OpenCV** (зарезервировано для фильтров)
- Пакетный менеджер: **vcpkg** (triplet `x64-windows`)

---

## ✨ Что уже работает
- **Открытие DICOM**: одиночный файл или целая серия (папка).
- **Извлечение метаданных** (Modality, WindowCenter/Width, Series Description и др.).
- **Преобразование ITK→VTK** через `itk::ImageToVTKImageFilter`.
- **Галерея серий (Dock)** с миниатюрами, сортировкой служебных серий (Topogram/Protocol) и drag & drop.
- **Просмотрщик 2D** на базе `vtkImageViewer2` с интерактором для прокрутки срезов колесом.
- **Пайплайн фильтров** в `VTKPipelineViewer` с управлением порядком (добавление/удаление/замена/перемещение). Включены:
  - `Flip` (осевой флип через `vtkImageFlip`)
  - `Window/Level` (`vtkImageMapToWindowLevelColors`), параметры берутся из DICOM
- **Панель инструментов CT (Dock)**: слайдер срезов, Window/Level, выбор проекции, кнопки «Шумоподавление/Резкость/Порог» (сигналы готовы, подключение в пайплайн — далее по роадмапу).
- **Многооконный режим** через `VTKViewerManager` (до 4 виджетов на `QSplitter`, выделение активного, быстрый **Ctrl+Click** для сравнения).
- **Локализация (Qt Linguist)**: `ru_RU` и `en_US`, обновление строк через `lupdate`.
- **Сохранение настроек** в `QSettings` (размер/позиция окна, язык, видимость панелей).
- **Потокобезопасный доступ** к VTK‑изображению (мьютекс в `DataDICOM`).

---

## 🧭 Архитектура (высокоуровнево)

```
MainWindow
 ├─ Меню/настройки/доки (Gallery, CTBasicToolbar)
 ├─ VTKViewerManager (размещение нескольких VTKPipelineViewer)
 │    └─ VTKPipelineViewer (рендер, пайплайн фильтров, интерактор)
 ├─ DataDICOM (ITK/VTK данные, метаданные, тумбнейлы)
 ├─ ImageUtils (конверсия ITK↔VTK, DCMTK→QImage, Window/Level ITK)
 ├─ ModalityInitializerFactory
 │    ├─ CTInitializer (flip, window/level, интерактор со скроллом)
 │    └─ MRInitializer (заготовка)
 ├─ CustomInteractorStyle2D (скролл срезов, hotkeys)
 └─ Gallery + ThumbnailListWidget (миниатюры, контекстное меню, DnD)
```

Ключевая идея: **VTKPipelineViewer** хранит именованные фильтры и их порядок (с приоритетами). Это делает добавление/замену/перестановку фильтров безопасным и декларативным.

---

## 📂 Структура проекта (основные файлы)

```
CMakeLists.txt
src/
  main.cpp
  mainwindow.{h,cpp}
  datadicom.{h,cpp}
  imageutils.{h,cpp}
  vtkpipelineviewer.{h,cpp}
  vtkviewermanager.{h,cpp}
  gallery.{h,cpp}
  thumbnaillistwidget.{h,cpp}
  custominteractorstyle2d.{h,cpp}
  IModalityInitializer.h
  ctinitializer.{h,cpp}
  mrinitializer.{h,cpp}
  modalityinitializerfactory.{h,cpp}
resources/
  LanguagePacksQt.qrc
  LanguagePacksApp.qrc
translations/
  CIVITAS_ru_RU.ts
  CIVITAS_en_US.ts
```

> Примечание: на этапе активной разработки возможны несовпадения имён/расположения файлов; CMakeLists.txt — актуальный источник истины.

---

## ▶️ Быстрый старт (Windows 11)

### Предустановки
- **Visual Studio 2019/2022** (C++ Desktop)
- **Qt 6.x** (или Qt 5.x) с компонентом **Qt Widgets**
- **vcpkg** в `C:\vcpkg` (желательно `VCPKG_ROOT=C:\vcpkg`)

### Установка зависимостей (vcpkg)
```powershell
# Пример установки базовых библиотек
vcpkg install vtk itk dcmtk opencv --triplet x64-windows
```
> Если VTK собирается без Qt‑модуля, убедитесь, что в билде есть `GUISupport/Qt` (в vcpkg это управляется фичами порта VTK). В Qt Creator обычно достаточно подключить QVTKOpenGLNativeWidget из установленного VTK.

### CMake (из консоли)
```powershell
cmake -S . -B build -G "Ninja" ^
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
  -DVCPKG_TARGET_TRIPLET=x64-windows ^
  -DCMAKE_PREFIX_PATH="C:/vcpkg/installed/x64-windows"  # + путь к Qt при необходимости

cmake --build build --config Release
```

### Qt Creator
1. Открыть проект `CMakeLists.txt`.
2. Выбрать **Kit** c Qt 6 (или Qt 5) и MSVC x64.
3. Указать `CMake Toolchain` vcpkg и (при необходимости) добавить пути Qt в `CMAKE_PREFIX_PATH`.
4. Сборка/запуск из IDE.

### Локализация
Обновить `.ts`:
```bash
lupdate . -recursive -no-obsolete -ts translations/CIVITAS_ru_RU.ts translations/CIVITAS_en_US.ts
```
Собрать `.qm` (через `lrelease` или из Qt Creator при сборке ресурсов).

---

## 🖱️ Как пользоваться
- **Файл → Открыть файл** — открыть одиночный DICOM (*.dcm).
- **Файл → Открыть серию** — выбрать папку с серией (UID определяется автоматически).
- В **Галерее** доступен выбор серии, двойной клик — показать в центральной области.
- Колесо мыши — прокрутка срезов (включено в `CustomInteractorStyle2D`).
- **Ctrl+Click** по просмотровому окну — сравнение с активным (режим 2 окон).
- Панель **CT Basic Toolbar** — задания Window/Level, режимов вида и кнопки обработки (hook’и готовы).

---

## 🧪 Пример добавления фильтра в пайплайн
```cpp
// Где-то в инициализации модальности:
auto gaussian = vtkSmartPointer<vtkImageGaussianSmooth>::New();
viewer->addFilter("GaussianBlur", gaussian);
// Фильтр автоматически встанет согласно приоритетам и перестроит пайплайн.
```
Доступны операции `addFilterToEnd/addFilterToFront/addFilterBefore/addFilterAfter/replaceFilter/removeFilter/moveFilterBefore/moveFilterAfter` и перечисление `iterateFilters()`.

---

## 🗺️ Роадмап (ближайшее)
- Связать **CTBasicToolbar** с пайплайном (изменение WL, включение фильтров из UI).
- **MPR** (аксиальная/сагиттальная/корональная) с синхронизацией курсора и WL.
- **3D Volume Rendering** (VTK), предустановки для разных органов/задач.
- Библиотека базовых фильтров (шумоподавление, резкость, порог, LUT, морфология) на VTK/ITK/OpenCV.
- Улучшение drag & drop серий (замена/вставка во ViewerManager), галерея с группировкой.
- Тесты (GoogleTest), **CI** (GitHub Actions) и **CPack** для сборки дистрибутива.
- Режим плагинов/модулей (платные расширения — архитектурные крючки уже заложены).

---

## ⚠️ Статус
WIP: часть кнопок и модальностей — заглушки; интерфейсы и точки расширения готовы.

---

## 📝 Лицензия
MIT.

---

## 👤 Автор
Иван — C++/Qt разработчик, врач‑хирург (опыт с КТ), интерес к визуализации и прикладному ИИ.

