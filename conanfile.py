import os

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

    python_requires = "pyreq/1.1.0@timzoet/v1.1.0"
    
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
        self.requires("math/1.0.0@timzoet/v1.0.0")
        self.requires("stduuid/1.0.0@timzoet/stable")

        if self.options.build_examples:
            self.requires("parsertongue/1.3.1@timzoet/v1.3.1")

        if self.options.build_tests:
            self.requires("bettertest/1.0.1@timzoet/v1.0.1")
    
    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
    
    def package_info(self):
        self.cpp_info.components["command"].libs = ["sol-command"]
        self.cpp_info.components["command"].requires = [
            "core",
            "material",
            "mesh",
            "render",
            "scenegraph",
            "texture",
            "common::common"
        ]

        self.cpp_info.components["core"].libs = ["sol-core"]
        self.cpp_info.components["core"].requires = [
            "error",
            "generated",
            "common::common"
        ]

        self.cpp_info.components["error"].libs = ["sol-error"]
        self.cpp_info.components["error"].requires = [
            "generated"
        ]

        self.cpp_info.components["generated"].libs = ["sol-generated"]
        self.cpp_info.components["generated"].requires = [
            "cmake-modules::cmake-modules"
        ]

        self.cpp_info.components["material"].libs = ["sol-material"]
        self.cpp_info.components["material"].requires = [
            "core",
            "memory",
            "mesh",
            "texture",
            "stduuid::stduuid"
        ]
        
        self.cpp_info.components["memory"].libs = ["sol-memory"]
        self.cpp_info.components["memory"].requires = [
            "core"
        ]
        
        self.cpp_info.components["mesh"].libs = ["sol-mesh"]
        self.cpp_info.components["mesh"].requires = [
            "memory",
            "stduuid::stduuid"
        ]

        self.cpp_info.components["render"].libs = ["sol-render"]
        self.cpp_info.components["render"].requires = [
            "core",
            "material",
            "mesh",
            "scenegraph"
        ]

        self.cpp_info.components["scenegraph"].libs = ["sol-scenegraph"]
        self.cpp_info.components["scenegraph"].requires = [
            "core",
            "material"
        ]
        
        self.cpp_info.components["texture"].libs = ["sol-texture"]
        self.cpp_info.components["texture"].requires = [
            "core",
            "memory"
        ]

        self.cpp_info.components["window"].libs = ["sol-window"]
        self.cpp_info.components["window"].requires = [
            "core",
            "glfw::glfw"
        ]

    def generate(self):
        base = self.python_requires["pyreq"].module.BaseConan
        
        tc = base.generate_toolchain(self)

        if self.options.enable_cache_settings:
            tc.variables["SOL_CORE_ENABLE_CACHE_SETTINGS"] = True

        tc.generate()
        
        deps = base.generate_deps(self)
        deps.generate()

        # Copy dependencies into build/bin folder.
        # TODO: This can perhaps be moved into the pyreq package.
        for dep in self.dependencies.values():
            for bindir in dep.cpp_info.bindirs:
                if os.path.isabs(bindir):
                    copy(self, "*.dll", bindir, os.path.join(self.build_folder, "bin"))
                # For some reason, package_folder can be None.
                elif dep.package_folder:
                    copy(self, "*.dll", os.path.join(dep.package_folder, bindir), os.path.join(self.build_folder, "bin"))

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
