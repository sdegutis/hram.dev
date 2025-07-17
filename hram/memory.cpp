#include "memory.h"

#include <cstdint>

static int newmemory(lua_State* L) {
	auto n = luaL_checkinteger(L, 1);
	lua_newuserdatauv(L, n, 1);
	lua_pushinteger(L, n);
	lua_setiuservalue(L, -2, 1);
	luaL_getmetatable(L, "core.memory");
	lua_setmetatable(L, -2);
	return 1;
}

static int getmemory(lua_State* L) {
	auto d = static_cast<uint8_t*>(luaL_checkudata(L, 1, "core.memory"));
	auto n = luaL_checkinteger(L, 2);
	lua_pushinteger(L, d[n]);
	return 1;
}

static int setmemory(lua_State* L) {
	auto d = static_cast<uint8_t*>(luaL_checkudata(L, 1, "core.memory"));
	auto n = luaL_checkinteger(L, 2);
	auto v = luaL_checkinteger(L, 3);
	d[n] = v;
	return 0;
}

static int lenmemory(lua_State* L) {
	auto d = static_cast<uint8_t*>(luaL_checkudata(L, 1, "core.memory"));
	lua_getiuservalue(L, 1, 1);
	return 1;
}

static const struct luaL_Reg memorylib[] = {
	{"new", newmemory},
	{"set", setmemory},
	{"get", getmemory},
	{"len", lenmemory},
	{NULL, NULL}
};

int luaopen_memory(lua_State* L) {
	luaL_newmetatable(L, "core.memory");
	luaL_newlib(L, memorylib);
	return 1;
}
