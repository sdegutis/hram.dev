#include "thread.h"

#include <cstdint>
#include <thread>

#include "app.h"

static int newthread(lua_State* L) {
	auto s = luaL_checkstring(L, 1);

	auto t = new std::jthread([s]() {
		auto L = app::newvm();
		luaL_dostring(L, s);
		});

	lua_pushinteger(L, reinterpret_cast<uint64_t>(t));
	return 1;
}

static const luaL_Reg threadlib[] = {
	{"spawn", newthread},
	{NULL,NULL}
};

int luaopen_thread(lua_State* L) {
	luaL_newlib(L, threadlib);
	return 1;
}
