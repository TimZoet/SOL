from conan import ConanFile
from conan.tools.files import copy

class SolConan(ConanFile):
    ############################################################################
    ## Package info.                                                          ##
    ############################################################################
    
    name = "sol"
    
    description = "..."
    
    url = "https://github.com/TimZoet/SOL"
    
    @property
    def user(self):
        return getattr(self, "_user", "timzoet")
    
    @user.setter
    def user(self, value):
        self._user = value
    
    @property
    def channel(self):
        return getattr(self, "_channel", f"v{self.version}")
    
    @channel.setter
    def channel(self, value):
        self._channel = value

    ############################################################################
    ## Settings.                                                              ##
    ############################################################################

    python_requires = "pyreq/1.0.1@timzoet/v1.0.1"
    
    python_requires_extend = "pyreq.BaseConan"
    
    options = {
        "enable_cache_settings": [True, False]
    }
    
    default_options = {
        "enable_cache_settings": True
    }
    
    ############################################################################
    ## Base methods.                                                          ##
    ############################################################################
    
    def set_version(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.set_version(self, "solVersionString.cmake", "SOL_VERSION")
    
    def init(self):
        base = self.python_requires["pyreq"].module.BaseConan
        self.settings = base.settings
        self.options.update(base.options, base.default_options)

    ############################################################################
    ## Building.                                                              ##
    ############################################################################
    
    def export_sources(self):
        copy(self, "CMakeLists.txt", self.recipe_folder, self.export_sources_folder)
        copy(self, "license", self.recipe_folder, self.export_sources_folder)
        copy(self, "readme.md", self.recipe_folder, self.export_sources_folder)
        copy(self, "solVersionString.cmake", self.recipe_folder, self.export_sources_folder)
        copy(self, "buildtools/*", self.recipe_folder, self.export_sources_folder)
        copy(self, "modules/*", self.recipe_folder, self.export_sources_folder)
    
    def config_options(self):
        base = self.python_requires["pyreq"].module.BaseConan
        if self.settings.os == "Windows":
            del self.options.fPIC
    
    def requirements(self):
        base = self.python_requires["pyreq"].module.BaseConan
        base.requirements(self)

        self.requires("common/1.1.0@timzoet/v1.1.0")
        self.requires("glfw/3.3.6")
        self.requires("stduuid/1.0.0@timzoet/stable")
        self.requires("vulkan/1.3.243.0")

        if self.options.build_tests:
            self.requires("bettertest/1.0.1@timzoet/v1.0.1")
    
    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
    
    def package_info(self):
        self.cpp_info.components["generated"].libs = ["sol-generated"]
        self.cpp_info.components["generated"].requires = [
            "cmake-modules::cmake-modules",
            "vulkan::vulkan"
        ]
        
        self.cpp_info.components["error"].libs = ["sol-error"]
        self.cpp_info.components["core"].requires = [
            "generated",
            "vulkan::vulkan"
        ]
        
        self.cpp_info.components["core"].libs = ["sol-core"]
        self.cpp_info.components["core"].requires = [
            "error",
            "generated",
            "common::common",
            "glfw::glfw",
            "stduuid::stduuid",
            "vulkan::vulkan"
        ]
        
        self.cpp_info.components["memory"].libs = ["sol-memory"]
        self.cpp_info.components["memory"].requires = [
            "core"
        ]
        
        self.cpp_info.components["mesh"].libs = ["sol-mesh"]
        self.cpp_info.components["mesh"].requires = [
            "memory"
        ]
        
        self.cpp_info.components["texture"].libs = ["sol-texture"]
        self.cpp_info.components["texture"].requires = [
            "memory"
        ]

    def generate(self):
        base = self.python_requires["pyreq"].module.BaseConan
        
        tc = base.generate_toolchain(self)

        if self.options.enable_cache_settings:
            tc.variables["SOL_CORE_ENABLE_CACHE_SETTINGS"] = True

        tc.generate()
        
        deps = base.generate_deps(self)
        deps.generate()
    
    def configure_cmake(self):
        base = self.python_requires["pyreq"].module.BaseConan
        cmake = base.configure_cmake(self)
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()
