solution "VeRender"
    -- BUILD_DIR           = "../bin"
    -- PROJECT_DIR         = "../" .. _ACTION .. "/"
    -- LIB_DIR             = "../../lib/"
    --LIB_PROJECT_DIR     = PROJECT_DIR .. "lib"
    --LIB_BUILD_DIR       = PROJECT_DIR .. "lib/bin"
    -- SRC_DIR             = "../../src/"

    configuration "Debug"
        targetdir "../bin/Debug"

    configuration "Release"
        targetdir "../bin/Release"

    configurations { "Debug", "Release" }
    location "../build"
    
    platforms{"x64"}

    language "c++"

project "MyProject"
    kind "ConsoleApp"

    debugdir "../WorkSpace"
    flags {
		"Symbols"-- for debug pdb file
	}
    linkoptions { "/NODEFAULTLIB:library;"}
    
    includedirs{
        "../ThirdParty/glfw-3.2.1.bin.WIN64/include",
        "../ThirdParty/nlohmann",
        "../ThirdParty/rttr-0.9.6-src/src",
        --"../ThirdParty/DirectX/Include",
        "../Source/Render/",
        "../Source/imgui/",
    }
    files{
       "../Source/*.h",
       "../Source/*.cpp",
       "../Source/Render/*.cpp",
       "../Source/Render/*.h",
       "../Source/imgui/*.h",
       "../Source/imgui/*.cpp",
    }

    configuration {"vs2017"}
        buildoptions_cpp{}
        libdirs {
            "../ThirdParty/glfw-3.2.1.bin.WIN64/lib-vc2015",
           -- "../ThirdParty/DirectX/lib",
        }
        links {"glfw3","d3d11","d3dcompiler"}