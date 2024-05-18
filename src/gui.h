#pragma once

#include <Windows.h>

ATOM register_main_window(HINSTANCE hInstance, CHAR szWindowClass[]);
BOOL init_instance(HINSTANCE hInstance, int nCmdShow, CHAR szWindowClass[], CHAR szTitle[]);
LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM); 