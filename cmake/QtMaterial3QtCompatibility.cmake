include_guard(GLOBAL)

get_filename_component(
    _qtm3_qt_support_manifest
    "${CMAKE_CURRENT_LIST_DIR}/../docs/compatibility/qt-support.json"
    ABSOLUTE
)
if(NOT EXISTS "${_qtm3_qt_support_manifest}")
    message(FATAL_ERROR
        "QtMaterial3 Qt support manifest not found: "
        "${_qtm3_qt_support_manifest}"
    )
endif()

file(READ "${_qtm3_qt_support_manifest}" _qtm3_qt_support_json)

string(
    JSON QTMATERIAL3_QT5_MIN_VERSION
    ERROR_VARIABLE _qtm3_qt5_json_error
    GET "${_qtm3_qt_support_json}" qt 5 minimum
)
if(_qtm3_qt5_json_error)
    message(FATAL_ERROR
        "Cannot read Qt5 minimum from qt-support.json: "
        "${_qtm3_qt5_json_error}"
    )
endif()

string(
    JSON QTMATERIAL3_QT6_MIN_VERSION
    ERROR_VARIABLE _qtm3_qt6_json_error
    GET "${_qtm3_qt_support_json}" qt 6 minimum
)
if(_qtm3_qt6_json_error)
    message(FATAL_ERROR
        "Cannot read Qt6 minimum from qt-support.json: "
        "${_qtm3_qt6_json_error}"
    )
endif()

set(
    QTMATERIAL3_EXPECT_QT_VERSION
    ""
    CACHE STRING
    "If non-empty, require the detected Qt Core version to match exactly"
)

function(qtmaterial3_detect_qt_version out_var)
    if(QT_VERSION_MAJOR EQUAL 5)
        if(DEFINED Qt5Core_VERSION)
            set(_qtm3_version "${Qt5Core_VERSION}")
        elseif(DEFINED Qt5Core_VERSION_STRING)
            set(_qtm3_version "${Qt5Core_VERSION_STRING}")
        else()
            message(FATAL_ERROR "Unable to determine Qt5Core version")
        endif()
    elseif(QT_VERSION_MAJOR EQUAL 6)
        if(DEFINED Qt6Core_VERSION)
            set(_qtm3_version "${Qt6Core_VERSION}")
        elseif(DEFINED Qt6Core_VERSION_STRING)
            set(_qtm3_version "${Qt6Core_VERSION_STRING}")
        else()
            message(FATAL_ERROR "Unable to determine Qt6Core version")
        endif()
    else()
        message(FATAL_ERROR
            "QtMaterial3 supports Qt 5 and Qt 6 only; "
            "detected major ${QT_VERSION_MAJOR}"
        )
    endif()
    set(${out_var} "${_qtm3_version}" PARENT_SCOPE)
endfunction()

function(qtmaterial3_validate_qt_compatibility)
    qtmaterial3_detect_qt_version(_qtm3_qt_version)

    if(QT_VERSION_MAJOR EQUAL 5
       AND _qtm3_qt_version VERSION_LESS QTMATERIAL3_QT5_MIN_VERSION)
        message(FATAL_ERROR
            "QtMaterial3 requires Qt >= ${QTMATERIAL3_QT5_MIN_VERSION}; "
            "detected ${_qtm3_qt_version}"
        )
    elseif(QT_VERSION_MAJOR EQUAL 6
           AND _qtm3_qt_version VERSION_LESS QTMATERIAL3_QT6_MIN_VERSION)
        message(FATAL_ERROR
            "QtMaterial3 requires Qt >= ${QTMATERIAL3_QT6_MIN_VERSION}; "
            "detected ${_qtm3_qt_version}"
        )
    endif()

    if(QTMATERIAL3_EXPECT_QT_VERSION
       AND NOT _qtm3_qt_version VERSION_EQUAL QTMATERIAL3_EXPECT_QT_VERSION)
        message(FATAL_ERROR
            "Exact Qt gate expected ${QTMATERIAL3_EXPECT_QT_VERSION}; "
            "detected ${_qtm3_qt_version}"
        )
    endif()

    set(
        QTMATERIAL3_DETECTED_QT_VERSION
        "${_qtm3_qt_version}"
        CACHE INTERNAL "Detected Qt version" FORCE
    )
    message(
        STATUS
        "QtMaterial3 Qt compatibility: Qt ${_qtm3_qt_version} "
        "(manifest: ${_qtm3_qt_support_manifest})"
    )
endfunction()
