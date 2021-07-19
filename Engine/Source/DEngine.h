#pragma once

//Dependencies
#include "glad/glad.h"
#include "GLFW/glfw3.h"

//Core
#include "Core/Core.h"

//Events
#include "Event/Callback.h"
#include "Event/Event.h"
#include "Event/EventDispatcher.h"
#include "Event/WindowEvent.h"

//Framework
#include "Framework/Tick.h"
#include "Framework/Application.h"
#include "Framework/Engine.h"
#include "Framework/AppObject.h"
#include "Framework/ObjectBase.h"
#include "Framework/ModuleManager.h"
#include "Framework/Registry.h"
#include "Framework/Module.h"
#include "Framework/Property.h"

//Rendering 
#include "Rendering/Window.h"

//Serialization
#include "Serialization/Buffer.h"

//Utils
#include "Utils/DataCache.h"
#include "Utils/FileDialog.h"
#include "Utils/ImageLoader.h"
#include "Utils/Paths.h"
#include "Utils/Rand.h"
#include "Utils/Task.h"
#include "Utils/Timer.h"
#include "Utils/World.h"