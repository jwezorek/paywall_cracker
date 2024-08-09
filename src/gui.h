#pragma once

#include <Windows.h>
#include <string>

namespace pwc {

    ATOM register_main_window(HINSTANCE hInstance, const std::string& wnd_class);
    BOOL init_instance(HINSTANCE hInstance, int nCmdShow, HFONT font,
        const std::string& wnd_class, const std::string& title);

}