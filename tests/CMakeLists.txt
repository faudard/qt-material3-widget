find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

function(qtmaterial3_add_test_executable target source)
    if(COMMAND qt_add_executable)
        qt_add_executable(${target} ${source})
    else()
        add_executable(${target} ${source})
    endif()
endfunction()

qtmaterial3_add_test_executable(tst_themebuilder theme/tst_themebuilder.cpp)
target_link_libraries(tst_themebuilder PRIVATE Qt${QT_VERSION_MAJOR}::Test qtmaterial3_theme)
add_test(NAME tst_themebuilder COMMAND tst_themebuilder)

qtmaterial3_add_test_executable(tst_thememanager theme/tst_thememanager.cpp)
target_link_libraries(tst_thememanager PRIVATE Qt${QT_VERSION_MAJOR}::Test qtmaterial3_theme)
add_test(NAME tst_thememanager COMMAND tst_thememanager)

qtmaterial3_add_test_executable(tst_legacyaliasmap legacy/tst_legacyaliasmap.cpp)
target_link_libraries(tst_legacyaliasmap PRIVATE Qt${QT_VERSION_MAJOR}::Test qtmaterial3_theme qtmaterial3_legacy)
add_test(NAME tst_legacyaliasmap COMMAND tst_legacyaliasmap)

qtmaterial3_add_test_executable(tst_shapes theme/tst_shapes.cpp)
target_link_libraries(tst_shapes PRIVATE Qt${QT_VERSION_MAJOR}::Test qtmaterial3_theme)
add_test(NAME tst_shapes COMMAND tst_shapes)

qtmaterial3_add_test_executable(tst_statelayer theme/tst_statelayer.cpp)
target_link_libraries(tst_statelayer PRIVATE Qt${QT_VERSION_MAJOR}::Test qtmaterial3_theme)
add_test(NAME tst_statelayer COMMAND tst_statelayer)