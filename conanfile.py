from __future__ import annotations

import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout


class QtMaterial3WidgetConan(ConanFile):
    name = "qt-material3-widget"
    version = "1.0.0"
    package_type = "library"
    license = "LGPL-3.0-only"
    url = "https://github.com/faudard/qt-material3-widget"
    homepage = "https://github.com/faudard/qt-material3-widget"
    description = "Material 3 widgets for Qt Widgets"
    topics = ("qt", "widgets", "material-design", "material3")

    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "qt_major": ["5", "6"],
        "with_designer": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
        "qt_major": "6",
        "with_designer": False,
    }

    exports_sources = (
        "CMakeLists.txt",
        "cmake/*",
        "src/*",
        "include/*",
        "designer/*",
        "packaging/QtMaterial3WidgetsConfig.cmake.in",
        "docs/compatibility/*",
        "docs/designer-plugin.md",
        "LICENSE",
        "README.md",
        "CHANGELOG.md",
    )

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.variables["BUILD_SHARED_LIBS"] = bool(self.options.shared)
        toolchain.variables["QTMATERIAL3_BUILD_TESTS"] = False
        toolchain.variables["QTMATERIAL3_BUILD_EXAMPLES"] = False
        toolchain.variables["QTMATERIAL3_BUILD_BENCHMARKS"] = False
        toolchain.variables["QTMATERIAL3_BUILD_DESIGNER_PLUGIN"] = bool(self.options.with_designer)
        toolchain.variables["QTMATERIAL3_INSTALL"] = True
        toolchain.variables["QTMATERIAL3_USE_MCU"] = False
        toolchain.variables["QTMATERIAL3_EXPECT_QT_MAJOR"] = str(self.options.qt_major)
        if self.options.get_safe("fPIC") is not None:
            toolchain.variables["CMAKE_POSITION_INDEPENDENT_CODE"] = bool(self.options.fPIC)
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        CMake(self).install()

    def package_info(self):
        # Reuse the project's authoritative installed CMake target graph.
        self.cpp_info.set_property("cmake_file_name", "QtMaterial3Widgets")
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs.append(
            os.path.join("lib", "cmake", "QtMaterial3Widgets")
        )
