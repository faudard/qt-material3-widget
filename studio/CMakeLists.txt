# Optional developer tool. No runtime/SDK target depends on Studio.
add_library(qtmaterial3_studio_document STATIC studiodocument.cpp studiodocument.h)
target_compile_features(qtmaterial3_studio_document PRIVATE cxx_std_17)
target_link_libraries(qtmaterial3_studio_document PUBLIC Qt${QT_VERSION_MAJOR}::Core)
add_executable(qtmaterial3_visual_studio main.cpp registry.qrc)
target_compile_features(qtmaterial3_visual_studio PRIVATE cxx_std_17)
target_link_libraries(qtmaterial3_visual_studio PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets qtmaterial3_widgets qtmaterial3_studio_document)
target_include_directories(qtmaterial3_visual_studio PRIVATE "${PROJECT_SOURCE_DIR}/include")
set_target_properties(qtmaterial3_visual_studio PROPERTIES AUTOMOC ON AUTORCC ON)
if(QTMATERIAL3_BUILD_TESTS)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
    add_executable(tst_studio_document tst_studiodocument.cpp)
    target_compile_features(tst_studio_document PRIVATE cxx_std_17)
    target_link_libraries(tst_studio_document PRIVATE
        qtmaterial3_studio_document Qt${QT_VERSION_MAJOR}::Test)
    add_test(NAME studio_document_contract COMMAND tst_studio_document)
endif()
