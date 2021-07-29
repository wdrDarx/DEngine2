#pragma once
#include "Core/Core.h"


//simple definitions given the name of the class and its super class
#define OBJECT_CLASS_DEF(class, superclass) using ThisClass = class; \
using Super = superclass; \
ClassType GetClassType() const override { return typeid(this); }; \
using superclass::superclass;  

//get static class of an object
#define OBJECT_STATIC_CLASS(ObjectClass) ObjectClass::_GetStaticClass<ObjectClass>()

#define _PROP_MEMBER_NAME m_Properties

//returns a PropType from a cpp value input
#define TO_PROP_TYPE(cppType) [&]() -> PropType { PropType out = PropType::NONE; \
if(typeid(cppType) == typeid(bool)) { out = PropType::BOOL; } else \
if(typeid(cppType) == typeid(int)) { out = PropType::INT; } else \
if(typeid(cppType) == typeid(float)) { out = PropType::FLOAT; } else \
if(typeid(cppType) == typeid(std::string)) { out =  PropType::STRING; } else \
if(typeid(cppType) == typeid(vec2d)) {out = PropType::VEC2D; } else \
if(typeid(cppType) == typeid(vec3d)) {out = PropType::VEC3D; } else \
if(typeid(cppType) == typeid(color3)) {out = PropType::COLOR3; } else \
if(typeid(cppType) == typeid(color4)) {out = PropType::COLOR4; } else \
if(typeid(cppType) == typeid(Transform)) {out = PropType::TRANSFORM; } else \
if(std::is_enum<decltype(cppType)>::value) {out = PropType::ENUM; } else \
if(is_base_of_template<AssetRef, decltype(cppType)>::value) {out = PropType::ASSETREF;} else \
if(std::is_base_of<_ArrayInternal, decltype(cppType)>::value) {out = PropType::ARRAY;} else \
if(std::is_base_of<StructBase, decltype(cppType)>::value) {out = PropType::STRUCT; } \
 return out; }();

//return std::type_info from a PropType input
#define FROM_PROP_TYPE(propType)  [&]() ->  std::type_index {  std::type_index out = typeid(bool); \
if(propType == PropType::BOOL) {out = typeid(bool);} else \
if(propType == PropType::INT) {out = typeid(int);} else \
if(propType == PropType::FLOAT) {out = typeid(float);} else \
if(propType == PropType::STRING) {out = typeid(std::string);} else \
if(propType == PropType::VEC2D) {out = typeid(vec2d);} else \
if(propType == PropType::VEC3D) {out = typeid(vec3d);} else \
if(propType == PropType::COLOR3) {out = typeid(color3);} else \
if(propType == PropType::COLOR4) {out = typeid(color4);} else \
if(propType == PropType::TRANSFORM) {out = typeid(Transform);} else \
if(propType == PropType::ENUM) {out = typeid(EnumBase);} else \
if(propType == PropType::ASSETREF) {out = typeid(AssetRef<Asset>);} else \
if(propType == PropType::ARRAY) {out = typeid(_ArrayInternal);} else \
if(propType == PropType::STRUCT) {out = typeid(StructBase);} else \
return out; }();

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
if(std::is_enum<decltype(x)>::value) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category, PropType::ENUM,					     &x, sizeof(x), _flags)); } else \
if(is_base_of_template<AssetRef, decltype(x)>::value) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category,PropType::ASSETREF, &x, sizeof(x), _flags)); } else \
if(std::is_base_of<_ArrayInternal, decltype(x)>::value) {_PROP_MEMBER_NAME.push_back(Property(#x,  _Category,PropType::ARRAY, &x, sizeof(x), _flags)); } else \
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