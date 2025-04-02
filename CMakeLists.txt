cmake_minimum_required(VERSION 3.16)

project(CIVITAS VERSION 0.1 LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

find_package(ITK CONFIG REQUIRED)
find_package(VTK REQUIRED)
find_package(DCMTK CONFIG REQUIRED)
find_package(OpenCV REQUIRED)




set(TS_FILES
    translations/CIVITAS_ru_RU.ts
    translations/CIVITAS_en_US.ts
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CIVITAS
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        custominteractorstyle2d.h custominteractorstyle2d.cpp
        gallery.h gallery.cpp
        LanguagePacksQt.qrc
        LanguagePacksApp.qrc
        imageutils.h imageutils.cpp
        datadicom.h datadicom.cpp
        vtkpipelineviewer.h vtkpipelineviewer.cpp
        IModalityInitializer.h
        ctinitializer.h ctinitializer.cpp
        mrinitializer.h mrinitializer.cpp
        modalityinitializerfactory.h modalityinitializerfactory.cpp









    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CIVITAS APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(CIVITAS SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CIVITAS
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()


target_link_libraries(CIVITAS PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${VTK_LIBRARIES}
    DCMTK::DCMTK
    ${OpenCV_LIBS}
    ITKFFT
    ITKVTK
    ITKznz
    itksys
    ITKIOGDCM
    ITKIOImageBase
    ITKCommon





)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CIVITAS)
endif()
set_target_properties(CIVITAS PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CIVITAS
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CIVITAS)
endif()
