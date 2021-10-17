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
		IncludeDir["Json"],
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
		"Modules/%{prj.name}/Source/OpenVR/include/",
		IncludeDir["GLFW"],
		IncludeDir["GLAD"],
		IncludeDir["ImGui"],
		IncludeDir["GLM"],
		IncludeDir["Objloader"],
		IncludeDir["STB_IMAGE"],
		IncludeDir["ASSIMP"],
		IncludeDir["Json"],
		IncludeDir["PhysX"]
	}

	links
	{
		"Engine",
		"CustomClasses",
		"Modules/%{prj.name}/Source/OpenVR/libs/openvr_api.lib"
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



project "Project"
	location "Modules/Project"
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
		"Modules/VRModule/Source",
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
		IncludeDir["Json"],
		IncludeDir["PhysX"]
	}

	links
	{
		"Engine",
		"VRModule",
		"CustomClasses"
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