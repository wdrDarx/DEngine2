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
#include "Event/ModuleEvent.h"
#include "Event/SceneEvent.h"
#include "Event/EditorCallback.h"
#include "Event/ApplicationEvent.h"

//Framework
#include "Framework/Tick.h"
#include "Framework/Application.h"
#include "Framework/Engine.h"
#include "Framework/AppObject.h"
#include "Framework/Scene.h"
#include "Framework/SceneObject.h"
#include "Framework/ObjectBase.h"
#include "Framework/ObjectComponent.h"
#include "Framework/ModuleManager.h"
#include "Framework/Registry.h"
#include "Framework/Module.h"
#include "Framework/Property.h"
#include "Framework/StructBase.h"
#include "Framework/FrameworkMacros.h"
#include "Framework/Method.h"
#include "Framework/StaticClass.h"
#include "Framework/Array.h"
#include "Framework/EnumBase.h"

//Assets
#include "Assets/Asset.h"
#include "Assets/AssetHandle.h"
#include "Assets/AssetManager.h"
#include "Assets/AssetRef.h"

//Rendering 
#include "Rendering/Window.h"
#include "Rendering/RenderAPI.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/Indexbuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture.h"
#include "Rendering/VertexArray.h"
#include "Rendering/Vertexbuffer.h"
#include "Rendering/VertexBufferLayout.h"
#include "Rendering/Camera.h"

//Serialization
#include "Serialization/Buffer.h"

//Utils
#include "Utils/DataCache.h"
#include "Utils/FileSystem.h"
#include "Utils/ImageLoader.h"
#include "Utils/Paths.h"
#include "Utils/Rand.h"
#include "Utils/Task.h"
#include "Utils/Timer.h"
#include "Utils/World.h"
#include "Utils/ObjectUtils.h"