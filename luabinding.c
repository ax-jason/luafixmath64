
#if __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lauxlib.h>

#define R128_STDC_ONLY
#define R128_IMPLEMENTATION
#include "r128.h"

#define fix_t R128
#define __METATABLE_NAME "___FIX_MATATABLE___"

static void create_meta(lua_State *L);

static const fix_t* lua_tofix(lua_State *L, int idx, fix_t* pv)
{
	fix_t* p = luaL_testudata(L, idx, __METATABLE_NAME);
	if(p)
	{
		return p;
	}
	else
	{
		r128FromFloat(pv, luaL_checknumber(L, idx));
		return pv;
	}
}

static fix_t* pre_push_fix(lua_State *L)
{
	fix_t* p = lua_newuserdata(L, sizeof(fix_t));
	create_meta(L);
	lua_setmetatable(L, -2);
	return p;
}

static int push_fix(lua_State *L, const fix_t * pv)
{
	if(!pv) {return 0;}
	fix_t* p = lua_newuserdata(L, sizeof(fix_t));
	r128Copy(p, pv);
	create_meta(L);
	lua_setmetatable(L, -2);
	return 1;
}

static int fix_tostring(lua_State *L)
{
	fix_t v;
	lua_pushnumber(L, r128ToFloat(lua_tofix(L, 1, &v)));
	return 1;
}

static int fix_rawvalue(lua_State *L)
{
	fix_t v; 
	int* pv = (int*)lua_tofix(L, 1, &v);
      	char buff[34];
	snprintf(buff, 34, "0x%08X%08X%08X%08X", *pv, *(pv+1), *(pv+2), *(pv+3));
	lua_pushlstring(L, buff, 34);
	return 1;
}

static int fix_add(lua_State *L)
{
	fix_t v1, v2;
	r128Add(pre_push_fix(L), lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2));
	return 1;
}

static int fix_sub(lua_State *L)
{
	fix_t v1, v2;
	r128Sub(pre_push_fix(L), lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2));
	return 1;
}

static int fix_mul(lua_State *L)
{
	fix_t v1, v2;
	r128Mul(pre_push_fix(L), lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2));
	return 1;
}

static int fix_div(lua_State *L)
{
	fix_t v1, v2;
	r128Div(pre_push_fix(L), lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2));
	return 1;
}

static int fix_mod(lua_State *L)
{
	fix_t v1, v2;
	r128Mod(pre_push_fix(L), lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2));
	return 1;
}

static int fix_pow(lua_State *L)
{
	return luaL_error(L, "pow ( ^ ) operator is not valid for fix128 type value");
}

static int fix_unm(lua_State *L)
{
	fix_t v;
	r128Neg(pre_push_fix(L), lua_tofix(L, 1, &v));
	return 1;
}

static int fix_idiv(lua_State *L)
{
	fix_t v1, v2;
	fix_t* pv = pre_push_fix(L);
	r128Div(pv, lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2));
	r128Floor(pv, pv);
	return 1;
}

static int fix_lt(lua_State *L)
{
	fix_t v1, v2;
	lua_pushboolean(L, r128Cmp(lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2)) < 0);
	return 1;
}

static int fix_le(lua_State *L)
{
	fix_t v1, v2;
	lua_pushboolean(L, r128Cmp(lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2)) <= 0);
	return 1;
}
static int fix_eq(lua_State *L)
{
	fix_t v1, v2;
	const fix_t* lhs = lua_tofix(L, 1, &v1);
	const fix_t* rhs = lua_tofix(L, 2, &v2);
	lua_pushboolean(L, lhs->lo == rhs->lo && lhs->hi == rhs->hi);
	return 1;
}

static int l_tofix(lua_State *L)
{
	if(lua_isnumber(L, 1))
	{
		fix_t* p = pre_push_fix(L);
		r128FromFloat(p, lua_tonumber(L, 1));
		return 1;
	}
	else
	{
		return 0;
	}
}

static int fix_abs(lua_State *L)
{
	fix_t v;
	const fix_t* pv = lua_tofix(L, 1, &v);
	fix_t* p = pre_push_fix(L);
	if(r128IsNeg(pv))
	{
		r128Neg(p, pv);
	}
	else
	{
		r128Copy(p, pv);
	}
	return 1;
}
static int fix_floor(lua_State *L)
{
	fix_t v;
	r128Floor(pre_push_fix(L), lua_tofix(L, 1, &v));
	return 1;
}
static int fix_ceil(lua_State *L)
{
	fix_t v;
	r128Ceil(pre_push_fix(L), lua_tofix(L, 1, &v));
	return 1;
}
static int fix_min(lua_State *L)
{
	fix_t v1, v2;
	r128Min(pre_push_fix(L), lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2));
	return 1;	
}
static int fix_max(lua_State *L)
{
	fix_t v1, v2;
	r128Max(pre_push_fix(L), lua_tofix(L, 1, &v1), lua_tofix(L, 2, &v2));
	return 1;
}

static int fix_sqrt(lua_State *L)
{
	fix_t v;
	r128Sqrt(pre_push_fix(L), lua_tofix(L, 1, &v));
	return 1;
}

#ifdef WIN32
#define LUA_LIB_API __declspec(dllexport)
#else
#define LUA_LIB_API
#endif

const luaL_Reg lua_fixmath_meta_methods[] = {
	{"__add",   fix_add},
	{"__sub",   fix_sub},
	{"__mul",   fix_mul},
	{"__div",   fix_div},
	{"__mod",   fix_mod},
	{"__pow",   fix_pow},
	{"__unm",   fix_unm},
	{"__idiv",   fix_idiv},
	{"__lt",   fix_lt},
	{"__le",   fix_le},
	{"__eq",   fix_eq},
	{"__tostring",   fix_tostring},
	{NULL, NULL}
};

const luaL_Reg lua_fixmath_modules[] = {
	{"tofix",   l_tofix},
	{"tostring",   fix_tostring},
	{"tonumber",   fix_tostring},
	{"rawvalue",   fix_rawvalue},
	{"abs",   fix_abs},
	{"floor",   fix_floor},
	{"ceil",   fix_ceil},
	{"min",   fix_min},
	{"max",   fix_max},
	{"sqrt",   fix_sqrt},
	{NULL, NULL}
};

static void fill_meta(lua_State *L)
{
	luaL_setfuncs(L, lua_fixmath_meta_methods, 0);
	luaL_newlib(L, lua_fixmath_modules);
  	lua_setfield(L, -2, "__index");
}

static void create_meta(lua_State *L)
{
	if(luaL_newmetatable (L, __METATABLE_NAME) != 0)
	{
		fill_meta(L);
	}
}

LUA_LIB_API int luaopen_fixmath64(lua_State* L)
{
    	luaL_newlib(L, lua_fixmath_modules);
      	push_fix(L, &R128_max);
  	lua_setfield(L, -2, "maxvalue");
      	push_fix(L, &R128_min);
  	lua_setfield(L, -2, "minvalue");
	return 1;
}

#if __cplusplus
}
#endif