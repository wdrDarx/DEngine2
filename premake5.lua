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

	startproject "ModuleSystem"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
IncludeDir = {}
IncludeDir["GLFW"] = "Vendor/GLFW/include"
IncludeDir["GLAD"] = "Vendor/GLAD/include"
IncludeDir["ImGui"] = "Vendor/ImGui"

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
		"%{prj.name}/Source"
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
		"%{prj.name}/Source",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}"
	}

	links
	{
		"opengl32.lib",
		"imGui",
		"GLFW",
		"GLAD"
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
		("{COPY} %{cfg.buildtarget.relpath} ../../bin/" .. outputdir .. "/Engine")
	}

	includedirs
	{
		"Engine/Source",
		"%{prj.name}/Source",
		"../Engine/"
	}

	links
	{
		"RegistryModule"
	}

	filter "configurations:Debug"
      symbols "On"

	filter "configurations:Release"
      optimize "On"

group "" --Modules