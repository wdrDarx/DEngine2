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
IncludeDir["STB_IMAGE"] = "Vendor/stb_image"

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
		"%{IncludeDir.STB_IMAGE}"
	}

	links
	{
		"opengl32.lib",
		"imGui",
		"GLFW",
		"GLAD"
	}

	defines
	{
		"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS",
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "configurations:Debug"
      symbols "On"

	filter "configurations:Release"
      optimize "On"

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
		"%{IncludeDir.STB_IMAGE}"
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
      optimize "On"


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
		("{COPY} %{cfg.buildtarget.relpath} ../../bin/" .. outputdir .. "/Editor/Modules")
	}

	includedirs
	{
		"Engine/Source",
		"Modules/%{prj.name}/Source",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.STB_IMAGE}"
	}

	links
	{
		"Engine"
	}

	filter "configurations:Debug"
      symbols "On"

	filter "configurations:Release"
      optimize "On"

group "" --Modules