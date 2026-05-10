set(QTMATERIAL3_THEME_SOURCES
    qtmaterialcolorscheme.cpp
    qtmaterialthemeoptions.cpp
    qtmaterialtheme.cpp
    qtmaterialthemebuilder.cpp
    qtmaterialcolorbackend.cpp
    qtmaterialmcuadapter.cpp
    qtmaterialthememanager.cpp
    qtmaterialthemeserializer.cpp
    qtmaterialthemevalidation.cpp
    qtmaterialthemeobserver.cpp
    qtmaterialtypographyscale.cpp
    qtmaterialshapescale.cpp
    qtmaterialelevationscale.cpp
    qtmaterialmotiontokens.cpp
    qtmaterialstatelayer.cpp
    qtmaterialdensitytokens.cpp
    qtmaterialiconsizetokens.cpp
    qtmaterialcomponenttokens.cpp
    qtmaterialxmlthemeadapter.cpp
    qtmaterialsystemtheme.cpp
    qtmaterialaccessibilitytokens.cpp
)

set(QTMATERIAL3_THEME_HEADERS
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/qtmaterialglobal.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialcolortoken.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialcolorbackend.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialcolorscheme.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialthemeoptions.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialtheme.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialthemebuilder.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialthememanager.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialthemeserializer.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialthemevalidation.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialthemeobserver.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialtypographyscale.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialshapescale.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialelevationscale.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialmotiontokens.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialstatelayer.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialdensitytokens.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialiconsizetokens.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialcomponenttokens.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialxmlthemeadapter.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialsystemtheme.h
    ${PROJECT_SOURCE_DIR}/include/qtmaterial/theme/qtmaterialaccessibilitytokens.h
)

qtm3_add_module(
    qtmaterial3_theme
    Theme
    QtMaterial3Theme
    QTMATERIAL3_THEME_LIBRARY
    ${QTMATERIAL3_THEME_SOURCES}
    ${QTMATERIAL3_THEME_HEADERS}
)

# QTMATERIAL3_USE_MCU expresses the user request.
# QTMATERIAL3_HAS_MCU expresses whether an actual MCU adapter is compiled into this target.
set(QTMATERIAL3_MCU_ROOT
    "${PROJECT_SOURCE_DIR}/third_party/material-color-utilities"
    CACHE PATH "Path to the Material Color Utilities checkout containing the cpp/ directory"
)

set(QTMATERIAL3_HAS_MCU OFF)

if(QTMATERIAL3_USE_MCU)
    set(QTMATERIAL3_MCU_REQUIRED_HEADERS
        "${QTMATERIAL3_MCU_ROOT}/cpp/cam/hct.h"
        "${QTMATERIAL3_MCU_ROOT}/cpp/dynamiccolor/material_dynamic_colors.h"
        "${QTMATERIAL3_MCU_ROOT}/cpp/scheme/scheme_tonal_spot.h"
        "${QTMATERIAL3_MCU_ROOT}/cpp/scheme/scheme_variant.h"
        "${QTMATERIAL3_MCU_ROOT}/cpp/utils/utils.h"
    )

    set(QTMATERIAL3_MCU_HEADERS_PRESENT ON)

    foreach(_qtm3_mcu_header IN LISTS QTMATERIAL3_MCU_REQUIRED_HEADERS)
        if(NOT EXISTS "${_qtm3_mcu_header}")
            set(QTMATERIAL3_MCU_HEADERS_PRESENT OFF)
        endif()
    endforeach()

    if(QTMATERIAL3_MCU_HEADERS_PRESENT)
        file(GLOB_RECURSE QTMATERIAL3_MCU_SOURCES CONFIGURE_DEPENDS
            "${QTMATERIAL3_MCU_ROOT}/cpp/*.cc"
            "${QTMATERIAL3_MCU_ROOT}/cpp/*.cpp"
        )
        list(FILTER QTMATERIAL3_MCU_SOURCES EXCLUDE REGEX ".*(_test|test)\\.(cc|cpp)$")

        if(QTMATERIAL3_MCU_SOURCES)
            target_sources(
                qtmaterial3_theme
                PRIVATE ${QTMATERIAL3_MCU_SOURCES}
            )
            target_include_directories(
                qtmaterial3_theme
                PRIVATE "${QTMATERIAL3_MCU_ROOT}"
            )
            set(QTMATERIAL3_HAS_MCU ON)
        else()
            message(WARNING
                "QTMATERIAL3_USE_MCU=ON but no MCU C++ sources were found under "
                "${QTMATERIAL3_MCU_ROOT}/cpp; using fallback backend."
            )
        endif()
    else()
        message(WARNING
            "QTMATERIAL3_USE_MCU=ON but Material Color Utilities headers were not found at "
            "${QTMATERIAL3_MCU_ROOT}; using fallback backend."
        )
    endif()
endif()

target_compile_definitions(qtmaterial3_theme
    PUBLIC
        QTMATERIAL3_USE_MCU=$<BOOL:${QTMATERIAL3_USE_MCU}>
        QTMATERIAL3_HAS_MCU=$<BOOL:${QTMATERIAL3_HAS_MCU}>
)

target_link_libraries(qtmaterial3_theme
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
)
