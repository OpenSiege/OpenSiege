from conans import ConanFile, CMake

class OpenSiegeConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "openscenegraph/3.6.5", "spdlog/1.8.5"
    generators = "cmake"
    default_options = "openscenegraph:shared=True"
