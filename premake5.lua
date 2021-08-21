workspace "DEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

	startproject "Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
IncludeDir = {}
IncludeDir["GLFW"] = "Vendor/GLFW/include"
IncludeDir["GLAD"] = "Vendor/GLAD/include"
IncludeDir["ImGui"] = "Vendor/ImGui"
IncludeDir["GLM"] = "Vendor/GLM"
IncludeDir["Objloader"] = "Vendor/ObjLoader"
IncludeDir["STB_IMAGE"] = "Vendor/stb_image"
IncludeDir["ASSIMP"] = "Vendor/Assimp/include"

--assimp stuff
AssimpLibs = {}
AssimpLibs["ASSIMP_RELEASE"] = "Vendor/Assimp/lib/assimp-Release.lib"
AssimpLibs["ASSIMP_DEBUG"] = "Vendor/Assimp/lib/assimp-Debug.lib"
AssimpLibs["ZLIB"] = "Vendor/zlib/lib/zlibstatic.lib"
AssimpLibs["IRRXLM"] = "Vendor/irrXML/lib/IrrXML.lib" 		

group "Dependecies"
include "Vendor/GLFW"
include "Vendor/ImGui"
include "Vendor/GLAD"
group "" --Dependecies

project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Source",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.STB_IMAGE}",
		"%{IncludeDir.Objloader}",
		"%{IncludeDir.ASSIMP}"
	}

	links
	{			
		AssimpLibs["ZLIB"],
		AssimpLibs["IRRXLM"],

		"opengl32.lib",
		"imGui",
		"GLFW",
		"GLAD"
	}

	filter "configurations:Release"
		links 
		{
			AssimpLibs["ASSIMP_RELEASE"]
		}

	filter "configurations:Debug"
		links 
		{
			AssimpLibs["ASSIMP_DEBUG"]
		}

	defines
	{
		"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS",
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "configurations:Debug"
      symbols "On"

	filter "configurations:Release"
      optimize "Speed"

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"Engine/Source",
		"%{prj.name}/Source",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.STB_IMAGE}",
		"%{IncludeDir.Objloader}",
		"%{IncludeDir.ASSIMP}"
	}

	defines
	{
		"STB_IMAGE_IMPLEMENTATION"
	}

	links
	{
		"Engine"
	}

	postbuildcommands
	{
		"{MKDIR} %{cfg.buildtarget.relpath}/../Modules",
		"{MKDIR} %{cfg.buildtarget.relpath}/../Content",
		"{MKDIR} %{cfg.buildtarget.relpath}/../Engine",
		("{COPY} ../EngineContent/  %{cfg.buildtarget.relpath}/../Engine")
	}

	filter "configurations:Debug"
      symbols "On"

	filter "configurations:Release"
      optimize "Speed"


group "Modules"

project "CustomClasses"
	location "Modules/CustomClasses"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Modules/%{prj.name}/Source/**.h",
		"Modules/%{prj.name}/Source/**.cpp"
	}

	postbuildcommands
	{
		("{MKDIR} %{cfg.buildtarget.relpath}/../../Editor/Modules/%{prj.name}"),
		("{COPY} %{cfg.buildtarget.relpath} ../../bin/" .. outputdir .. "/Editor/Modules/%{prj.name}" )		
	}

	includedirs
	{
		"Engine/Source",
		"Modules/%{prj.name}/Source",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.STB_IMAGE}",
		"%{IncludeDir.Objloader}",
		"%{IncludeDir.ASSIMP}"
	}

	links
	{
		"Engine"
	}

	filter "configurations:Debug"
      symbols "On"

	filter "configurations:Release"
      optimize "Speed"

group "" --Modules