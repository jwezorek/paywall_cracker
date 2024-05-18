#pragma once

#include <Windows.h>
#include <string>

ATOM register_main_window(HINSTANCE hInstance, const std::string& wnd_class);
BOOL init_instance(HINSTANCE hInstance, int nCmdShow, 
    const std::string& wnd_class, const std::string& title);
LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM); 