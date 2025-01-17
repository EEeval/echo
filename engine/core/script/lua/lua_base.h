#pragma once

#include "engine/core/base/variant.h"
#include "engine/core/util/object_pool.h"

extern "C"
{
#include <thirdparty/lua/lua.h>
#include <thirdparty/lua/lualib.h>
#include <thirdparty/lua/lauxlib.h>
}

#define LUA_FUNCTION_MAX_NAME_COUNT 16

namespace Echo
{
	class Object;
	class Node;
    class DataStream;
	struct LuaStackCheck
	{
		int			m_elementNum;
		lua_State*	m_luaState;

		LuaStackCheck(lua_State* state)
		{
			m_luaState = state;
			m_elementNum = lua_gettop(state);
		}

		~LuaStackCheck()
		{
			EchoAssert(m_elementNum == lua_gettop(m_luaState));
		}
	};

#ifdef ECHO_EDITOR_MODE
#define LUA_STACK_CHECK(state) LuaStackCheck stackCheck(state)
#else
#define LUA_STACK_CHECK(state)
#endif 

	extern ObjectPool<Vector3>		LuaVec3Pool;
	extern ObjectPool<Quaternion>	LuaQuaternionPool;
	extern ObjectPool<String>		LuaStrPool;
	extern ObjectPool<StringOption> LuaStrOptionPool;
	extern ObjectPool<ResourcePath>	LuaResourcePathPool;
	extern ObjectPool<RealVector>	LuaRealVectorPool;
	extern ObjectPool<Matrix>		LuaMatrixPool;

	// log messages
	void lua_binder_warning(const char* msg);
	void lua_binder_error(const char* msg);
	void lua_get_obj_name(Object* obj, char* buffer, int len);
    void lua_register_object(Object* obj);

	// lua stack to value
	template<typename T> INLINE T lua_getvalue(lua_State* L, int index)			
	{ 
		lua_binder_error("lua stack get value error, unknow c type"); 
		static T st = variant_cast<T>(Variant()); 
		return st; 
	}

	template<typename T> INLINE void lua_freevalue(T value)
	{
	}

	template<> INLINE const char* lua_getvalue<const char*>(lua_State* L, int index)
	{ 
		return lua_isnil(L, index) ? "nil" : (lua_istable(L, index) ? "table" : lua_tostring(L, index));
	}

	template<> INLINE bool lua_getvalue<bool>(lua_State* L, int index)
	{
		return (bool)lua_toboolean(L, index);
	}

	template<> INLINE i32 lua_getvalue<i32>(lua_State* L, int index)
	{ 
		return (i32)lua_tonumber(L, index);
	}

	template<> INLINE ui32 lua_getvalue<ui32>(lua_State* L, int index)
	{ 
		return (ui32)lua_tonumber(L, index);
	}
	
	template<> INLINE float lua_getvalue<float>(lua_State* L, int index)
	{
		return (float)lua_tonumber(L, index);
	}

	template<> INLINE double lua_getvalue<double>(lua_State* L, int index)
	{
		return lua_tonumber(L, index);
	}

	template<> INLINE String lua_getvalue<String>(lua_State* L, int index)
	{
		return lua_tostring(L, index);
	}
	
	template<> INLINE const String& lua_getvalue<const String&>(lua_State* L, int index)
	{
		String* result = LuaStrPool.newObj();
		*result = lua_tostring(L, index);	// this is wrong, how to modify this?
		return *result;
	}

	template<> INLINE void lua_freevalue<const String&>(const String& value)
	{
		String* ptr = (String*)&value;
		LuaStrPool.deleteObj(ptr);
	}

	template<> INLINE const StringOption& lua_getvalue<const StringOption&>(lua_State* L, int index)
	{
		StringOption* result = LuaStrOptionPool.newObj();
		*result = lua_tostring(L, index);
		return *result;
	}

	template<> INLINE void lua_freevalue<const StringOption&>(const StringOption& value)
	{
		StringOption* ptr = (StringOption*)&value;
		LuaStrOptionPool.deleteObj(ptr);
	}

	template<> INLINE const ResourcePath& lua_getvalue<const ResourcePath&>(lua_State* L, int index)
	{
		ResourcePath* result = LuaResourcePathPool.newObj();
		(*result).setPath(lua_tostring(L, index), true);
		return *result;
	}

	template<> INLINE void lua_freevalue<const ResourcePath&>(const ResourcePath& value)
	{
		ResourcePath* ptr = (ResourcePath*)&value;
		LuaResourcePathPool.deleteObj(ptr);
	}

	template<> INLINE const Vector3& lua_getvalue<const Vector3&>(lua_State* state, int idx) 
	{
		Vector3& result = *LuaVec3Pool.newObj();
		lua_getfield(state, idx, "x");
		lua_getfield(state, idx, "y");
		lua_getfield(state, idx, "z");
		result.x = (float)lua_tonumber(state, idx+1);
		result.y = (float)lua_tonumber(state, idx+2);
		result.z = (float)lua_tonumber(state, idx+3);
		lua_pop(state, 3);
		return result; 
	}

	template<> INLINE void lua_freevalue<const Vector3&>(const Vector3& value)
	{
		Vector3* ptr = (Vector3*)&value;
		LuaVec3Pool.deleteObj(ptr);
	}

	template<> INLINE const Quaternion& lua_getvalue<const Quaternion&>(lua_State* state, int idx)
	{
		Quaternion& result = *LuaQuaternionPool.newObj();
		lua_getfield(state, idx, "x");
		lua_getfield(state, idx, "y");
		lua_getfield(state, idx, "z");
		lua_getfield(state, idx, "w");
		result.x = (float)lua_tonumber(state, idx + 1);
		result.y = (float)lua_tonumber(state, idx + 2);
		result.z = (float)lua_tonumber(state, idx + 3);
		result.w = (float)lua_tonumber(state, idx + 4);
		lua_pop(state, 4);
		return result;
	}

	template<> INLINE void lua_freevalue<const Quaternion&>(const Quaternion& value)
	{
		Quaternion* ptr = (Quaternion*)&value;
		LuaQuaternionPool.deleteObj(ptr);
	}

	template<> INLINE const RealVector& lua_getvalue<const RealVector&>(lua_State* state, int idx)
	{
		RealVector* result = LuaRealVectorPool.newObj();
		result->clear();
		lua_pushnil(state);
		while (lua_next(state, idx) != 0)
		{
			result->emplace_back((float)lua_tonumber(state, -1));
			lua_pop(state, 1);
		}

		return *result;
	}

	template<> INLINE void lua_freevalue<const RealVector&>(const RealVector& value)
	{
		// we will use object pool replace new delete
		RealVector* ptr = (RealVector*)&value;
		LuaRealVectorPool.deleteObj(ptr);
	}

	template<> INLINE const Matrix& lua_getvalue<const Matrix&>(lua_State* state, int idx)
	{
		Matrix* result = LuaMatrixPool.newObj();
		result->reset();

		lua_pushnil(state);
		while (lua_next(state, idx) != 0)
		{
			int row = lua_gettop(state);
			RealVector rowElement;

			lua_pushnil(state);
			while (lua_next(state, row) != 0)
			{
				rowElement.emplace_back(lua_tonumber(state, -1));
				lua_pop(state, 1);
			}

			result->addRow(rowElement);
			lua_pop(state, 1);
		}

		return *result;
	}

	template<> INLINE void lua_freevalue<const Matrix&>(const Matrix& value)
	{
		// we will use object pool replace new delete
		Matrix* ptr = (Matrix*)&value;
		LuaMatrixPool.deleteObj(ptr);
	}

	template<> INLINE Object* lua_getvalue<Object*>(lua_State* state, int idx)
	{
		LUA_STACK_CHECK(state);

		lua_getfield(state, idx, "this");
		Object* ptr = static_cast<Object*>(lua_touserdata(state, -1));
		lua_pop(state, 1);

		return ptr;
	}

	template<> INLINE Node* lua_getvalue<Node*>(lua_State* state, int idx)
	{
		LUA_STACK_CHECK(state);

		lua_getfield(state, idx, "this");
		Node* nodeptr = static_cast<Node*>(lua_touserdata(state, -1));
		lua_pop(state, 1);

		return nodeptr;
	}

	// lua operate
	int lua_get_tables(lua_State* luaState, const StringArray& objectNames, const int count);
	int lua_get_upper_tables(lua_State* luaState, const String& objectName, String& currentLayerName);

	// lua push vlaue to stack
	template<typename T> INLINE void lua_pushvalue(lua_State* L, T value) 
	{
		lua_binder_error("lua stack push value error, unknow c type"); 
	}

	template<> INLINE void lua_pushvalue<const char*>(lua_State* state, const char* value) 
	{ 
		lua_pushfstring(state, value); 
	}

    template<> INLINE void lua_pushvalue<String>(lua_State* state, String value)
    {
        lua_pushfstring(state, value.c_str());
    }

    template<> INLINE void lua_pushvalue<const String&>(lua_State* state, const String& value)
    {
        lua_pushfstring(state, value.c_str());
    }

	template<> INLINE void lua_pushvalue<bool>(lua_State* state, bool value) 
	{ 
		lua_pushboolean(state, value); 
	}

	template<> INLINE void lua_pushvalue<i32>(lua_State* state, i32 value) 
	{ 
		lua_pushinteger(state, value); 
	}

	template<> INLINE void lua_pushvalue<ui32>(lua_State* state, ui32 value) 
	{ 
		lua_pushinteger(state, value); 
	}

	template<> INLINE void lua_pushvalue<float>(lua_State* state, float value)
	{
		lua_pushnumber(state, value);
	}

	template<> INLINE void lua_pushvalue<double>(lua_State* state, double value)
	{
		lua_pushnumber(state, value);
	}

	template<> INLINE void lua_pushvalue<const Vector2&>(lua_State* state, const Vector2& value)
	{
		lua_newtable(state);
        int idx = lua_gettop(state);
		lua_pushnumber(state, value.x);
		lua_setfield(state, idx, "x");
		lua_pushnumber(state, value.y);
		lua_setfield(state, idx, "y");
        
        lua_getglobal(state, "vec2");
        lua_setmetatable(state, idx);
	}

	template<> INLINE void lua_pushvalue<const Vector2>(lua_State* state, const Vector2 value)
	{
		lua_pushvalue<const Vector2&>(state, value);
	}

	template<> INLINE void lua_pushvalue<const Vector3&>(lua_State* state, const Vector3& value)
	{
		lua_newtable(state);
		int idx = lua_gettop(state);
		lua_pushnumber(state, value.x);
		lua_setfield(state, idx, "x");
		lua_pushnumber(state, value.y);
		lua_setfield(state, idx, "y");
		lua_pushnumber(state, value.z);
		lua_setfield(state, idx, "z");

		lua_getglobal(state, "vec3");
		lua_setmetatable(state, idx);
	}

	template<> INLINE void lua_pushvalue<const Vector3>(lua_State* state, const Vector3 value)
	{
		lua_pushvalue<const Vector3&>(state, value);
	}

	template<> INLINE void lua_pushvalue<const Quaternion&>(lua_State* state, const Quaternion& value)
	{
		lua_newtable(state);
		int idx = lua_gettop(state);
		lua_pushnumber(state, value.x);
		lua_setfield(state, idx, "x");
		lua_pushnumber(state, value.y);
		lua_setfield(state, idx, "y");
		lua_pushnumber(state, value.z);
		lua_setfield(state, idx, "z");
		lua_pushnumber(state, value.w);
		lua_setfield(state, idx, "w");

		lua_getglobal(state, "quaternion");
		lua_setmetatable(state, idx);
	}

	template<> INLINE void lua_pushvalue<Quaternion>(lua_State* state, Quaternion value)
	{
		lua_pushvalue<const Quaternion&>(state, value);
	}

	template<> INLINE void lua_pushvalue<const Quaternion>(lua_State* state, const Quaternion value)
	{
		lua_pushvalue<const Quaternion&>(state, value);
	}

	template<> INLINE void lua_pushvalue<const RealVector&>(lua_State* state, const RealVector& value)
	{
		lua_newtable(state);
		for (ui32 i=0; i<value.size(); i++)
		{
			lua_pushnumber(state, i);			// key   -2
			lua_pushnumber(state, value[i]);	// value -1	
			lua_settable(state, -3);			// t[key] = value && pop key and value
		}
	}

	template<> INLINE void lua_pushvalue<RealVector>(lua_State* state, RealVector value)
	{
		lua_pushvalue<const RealVector&>(state, value);
	}

	template<> INLINE void lua_pushvalue<const Matrix&>(lua_State* state, const Matrix& value)
	{
		lua_newtable(state);
		for (int i = 0; i < value.getHeight(); i++)
		{
			lua_pushnumber(state, i);
			lua_newtable(state);
			for (int j = 0; j < value.getWidth(); j++)
			{
				lua_pushnumber(state, j);			// key   -2
				lua_pushnumber(state, value[i][j]);	// value -1	
				lua_settable(state, -3);			// t[key] = value && pop key and value
			}
			lua_settable(state, -3);
		}
	}

	template<> INLINE void lua_pushvalue<Matrix>(lua_State* state, Matrix value)
	{
		lua_pushvalue<const Matrix&>(state, value);
	}

	template<> INLINE void lua_pushvalue<Object*>(lua_State* state, Object* value) 
	{
		if (value)
		{
            lua_register_object(value);

			char obj_name[128] = "_";
			lua_get_obj_name(value, obj_name + 1, 127);

			lua_getglobal(state, "objs");
			lua_getfield(state, -1, obj_name);
			lua_remove(state, -2);
		}
		else
		{
			lua_pushnil(state);
		}
	}

	template<> INLINE void lua_pushvalue<const Variant*>(lua_State* state, const Variant* value)
	{
		switch (value->getType())
		{
		case Variant::Type::Bool:	lua_pushvalue<bool>(state, value->toBool()); break;
		case Variant::Type::Int:	lua_pushvalue<i32>(state, value->toI32()); break;
		case Variant::Type::UInt:	lua_pushvalue<ui32>(state, value->toUI32()); break;
		case Variant::Type::Real:	lua_pushvalue<double>(state, value->toDouble()); break;
		default:					lua_pushnil(state); lua_binder_error("lua stack push value error, unknow c type"); break;
		}
	}

	template<> INLINE void lua_pushvalue<Variant>(lua_State* state, Variant value)
	{
		lua_pushvalue<const Variant*>(state, &value);
	}

#define LUA_PUSH_VALUE(TYPE) \
	template<> INLINE void lua_pushvalue<TYPE*>(lua_State* state, TYPE* value) \
	{ \
		Object* obj = (Object*)value; \
		lua_pushvalue<Object*>(state, obj); \
	}
}
