#pragma once
#include "Core/Core.h"


//simple definitions given the name of the class and its super class
#define OBJECT_CLASS_DEF(class, superclass) using ThisClass = class; \
using Super = superclass; \
ClassType GetClassType() const override { return typeid(this); }; \
using superclass::superclass;  

//get static properties of a class by constructing an instance of it
//#define OBJECT_STATIC_PROPERTIES(ObjectClass) [&]() -> std::vector<Property> { ObjectClass obj; obj.DefineProperties(); return obj.GetProperties(); }();

//get static ClassType of a class by constructing an instance of it
#define OBJECT_STATIC_CLASSTYPE(ObjectClass) [&]() -> ClassType { ObjectClass obj; return obj.GetClassType(); }();

//get static class of an object
#define OBJECT_STATIC_CLASS(ObjectClass) ObjectClass::_GetStaticClass<ObjectClass>()

#define _PROP_MEMBER_NAME m_Properties

//Simple prop additions macro
#define PROPDEF_FLAGS(x, flags) { int _flags = flags; \
if(typeid(x) == typeid(bool)) {_PROP_MEMBER_NAME.push_back(Property(#x, _Category, PropType::BOOL,						 &x, sizeof(bool),  _flags)); } else \
if(typeid(x) == typeid(int)) {_PROP_MEMBER_NAME.push_back(Property(#x, _Category, PropType::INT,						 &x, sizeof(int),   _flags)); } else \
if(typeid(x) == typeid(float)) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category,PropType::FLOAT,					 &x, sizeof(float), _flags)); } else \
if(typeid(x) == typeid(std::string)) {_PROP_MEMBER_NAME.push_back(Property(#x, _Category, PropType::STRING,				 &x, ((std::string*)&x)->length(), _flags)); } else \
if(typeid(x) == typeid(vec2d)) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category,PropType::VEC2D,					 &x, sizeof(vec2d), _flags)); } else \
if(typeid(x) == typeid(vec3d)) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category,PropType::VEC3D,					 &x, sizeof(vec3d), _flags)); } else \
if(typeid(x) == typeid(color3)) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category,PropType::COLOR3,					 &x, sizeof(color3), _flags)); } else \
if(typeid(x) == typeid(color4)) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category,PropType::COLOR4,					 &x, sizeof(color4), _flags)); } else \
if(typeid(x) == typeid(Transform)) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category,PropType::TRANSFORM,		     &x, sizeof(Transform), _flags)); } else \
if(std::is_base_of<StructBase, decltype(x)>::value) {_PROP_MEMBER_NAME.push_back(Property(#x, _Category, PropType::STRUCT,&x, sizeof(x), _flags)); } \
}

//TODO give all "DStructs" a function that returns their serializable size based on their properties

#define PROPDEF(x, flags) PROPDEF_FLAGS(x, flags)

#define OBJECT_PROPS_BEGIN() void DefineProperties() override { Super::DefineProperties(); std::string _Category = "";
#define PROPS_CATEGORY(category) _Category = #category; 
#define OBJECT_PROPS_END() }

//struct macros

#define STRUCT_CLASS_DEF(class, superclass) using ThisClass = class; \
using Super = superclass; \
using Super::Super; \
ClassType GetClassType() const override { return typeid(this); }; \
ThisClass() : Super(false) { DefineProperties(); }

#define STRUCT_PROPS_BEGIN() void DefineProperties() override { Super::DefineProperties(); std::string _Category = "";
#define STRUCT_PROPS_END() }

//get static class of an object
#define STRUCT_STATIC_CLASS(StructClass) StructClass::_GetStaticClass<StructClass>()