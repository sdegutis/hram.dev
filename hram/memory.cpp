#include "memory.h"

#include <cstdint>
#include <memory>

static int newmemory(lua_State* L) {
	auto n = luaL_checkinteger(L, 1);
	auto d = static_cast<uint8_t*>(lua_newuserdatauv(L, n, 1));
	memset(d, 0, n);
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

static int resizemem(lua_State* L) {
	lua_pushnil(L);
	return 1;

	//auto d = static_cast<uint8_t*>(luaL_checkudata(L, 1, "core.memory"));
	//lua_getiuservalue(L, 1, 1);
	//return 1;
}

static const struct luaL_Reg memorylib_f[] = {
	{"alloc", newmemory},
	{NULL, NULL}
};

static const struct luaL_Reg memorylib_m[] = {
	{"__newindex", setmemory},
	{"__index", getmemory},
	{"__len", lenmemory},
	{"__mod", resizemem},
	{NULL, NULL}
};

int luaopen_memory(lua_State* L) {

	auto a = static_cast<uint8_t*>(malloc(100));
	auto b = static_cast<uint32_t*>(malloc(100));

	printf("a %x\n", a);
	printf("a %x\n", &a[0]);
	printf("a %x\n", &a[1]);
	printf("a %x\n", &a[2]);

	printf("b %x\n", b);
	printf("b %x\n", &b[0]);
	printf("b %x\n", &b[1]);
	printf("b %x\n", &b[2]);

	luaL_newmetatable(L, "core.memory");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, memorylib_m, 0);
	luaL_newlib(L, memorylib_f);
	return 1;
}
