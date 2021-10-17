#pragma once

//Dependencies
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "json/json.h"

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
#include "Event/AssetEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/PhysicsActorEvent.h"

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
#include "Framework/InputManager.h"
#include "Framework/Registry.h"
#include "Framework/Module.h"
#include "Framework/Property.h"
#include "Framework/StructBase.h"
#include "Framework/FrameworkMacros.h"
#include "Framework/Method.h"
#include "Framework/StaticClass.h"
#include "Framework/Array.h"
#include "Framework/EnumBase.h"

//Framework Classes
#include "Framework/Classes/Components/TransformComponent.h"
#include "Framework/Classes/Components/ColliderComponent.h"
#include "Framework/Classes/Components/BoxColliderComponent.h"
#include "Framework/Classes/Components/MeshColliderComponent.h"
#include "Framework/Classes/Components/StaticMeshComponent.h"
#include "Framework/Classes/SceneObjects/DirectionalLightObject.h"
#include "Framework/Classes/SceneObjects/SkyLightObject.h"
#include "Framework/Classes/SceneObjects/StaticMeshObject.h"

//physics
#include "Physics/PhysicsCore.h"

//Assets
#include "Assets/Asset.h"
#include "Assets/AssetHandle.h"
#include "Assets/AssetManager.h"
#include "Assets/AssetRef.h"
#include "Assets/Classes/SceneAsset.h"

//Asset Classes
#include "Assets/Classes/TextureAsset.h"
#include "Assets/Classes/PrefabAsset.h"
#include "Assets/Classes/SceneAsset.h"
#include "Assets/Classes/ObjectAsset.h"
#include "Assets/Classes/MaterialAsset.h"
#include "Assets/Classes/CubemapAsset.h"
#include "Assets/Classes/MeshAsset.h"

//Rendering 
#include "Rendering/RenderCore.h"
#include "Rendering/Classes/Renderers/QuadRenderer.h"
#include "Rendering/Classes/Renderers/CubemapRenderer.h"
#include "Rendering/Classes/Renderers/MeshRenderer.h"
#include "Rendering/Classes/Renderers/DebugRenderer.h"
#include "Rendering/Classes/Pipelines/DefaultPipeline.h"

//Default Materials
#include "Rendering/Classes/Materials/PBR_Material.h"

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
#include "Utils/SceneUtils.h"
#include "Utils/RenderUtils.h"
#include "Utils/ModelLoader.h"
#include "Utils/InputUtils.h"
#include "Utils/PhysicsUtils.h"