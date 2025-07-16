#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <shlwapi.h>
#include <shlobj_core.h>
#include <stdint.h>
#include <atlstr.h>

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>

#include <lua/lua.hpp>

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
