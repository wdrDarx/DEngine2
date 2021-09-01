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
		IncludeDir["GLFW"],
		IncludeDir["GLAD"],
		IncludeDir["ImGui"],
		IncludeDir["GLM"],
		IncludeDir["Objloader"],
		IncludeDir["STB_IMAGE"],
		IncludeDir["ASSIMP"],
		IncludeDir["PhysX"]
	}

	links
	{
		"Engine"
	}

	filter "configurations:Debug"
      symbols "On"

	filter "configurations:Release"
      optimize "On"
	  symbols "On"
	  defines
	  {
			"NDEBUG" --for physx
	  }

	filter "configurations:Runtime"
      optimize "Speed"
	  defines
	  {
			"NDEBUG" --for physx
	  }

project "VRModule"
	location "Modules/VRModule"
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
		"Modules/CustomClasses/Source",
		"Modules/%{prj.name}/Source",
		IncludeDir["GLFW"],
		IncludeDir["GLAD"],
		IncludeDir["ImGui"],
		IncludeDir["GLM"],
		IncludeDir["Objloader"],
		IncludeDir["STB_IMAGE"],
		IncludeDir["ASSIMP"],
		IncludeDir["OpenVR"],
		IncludeDir["PhysX"]
	}

	links
	{
		"Engine",
		"CustomClasses",
		Library["OpenVR"]
	}

	filter "configurations:Debug"
      symbols "On"

	filter "configurations:Release"
      optimize "On"
	  symbols "On"
	  defines
	  {
			"NDEBUG" --for physx
	  }

	filter "configurations:Runtime"
      optimize "Speed"
	  defines
	  {
			"NDEBUG" --for physx
	  }