from conans import ConanFile, CMake

class OpenSiegeConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "openscenegraph/3.6.5", "re2c/2.0.3", "spdlog/1.8.5"
    generators = "cmake"
    default_options = "openscenegraph:shared=True"
    
    def configure(self):
        if self.settings.os == "Windows":
            self.requires("winflexbison/2.5.24")
        if self.settings.os == "Linux":
            self.requires("bison/3.7.1")
    
    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
