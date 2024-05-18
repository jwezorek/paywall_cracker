#pragma once

#include <vector>
#include <Windows.h>

namespace pwc {

    std::vector<HWND> find_all_toplevel_chrome_windows();

}