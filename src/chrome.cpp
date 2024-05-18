#include "chrome.h"
#include <string>
#include <sstream>

namespace {

    bool is_chrome_window(HWND hwnd) {
        const int length = 256;
        char class_name[length] = { 0 };
        GetClassName(hwnd, class_name, length);

        DWORD style = GetWindowLong(hwnd, GWL_STYLE);

        bool has_chrome_class = std::strcmp(class_name, "Chrome_WidgetWin_1") == 0;
        bool is_overlapped_window = (style & WS_OVERLAPPEDWINDOW);
        bool is_visible = IsWindowVisible(hwnd);

        return has_chrome_class && is_overlapped_window && is_visible;
    }

    // Callback function to be called for each top-level window
    BOOL CALLBACK enum_wnd_proc(HWND hwnd, LPARAM lParam) {
        std::vector<HWND>* pWindows = reinterpret_cast<std::vector<HWND>*>(lParam);
        if (is_chrome_window(hwnd)) {
            pWindows->push_back(hwnd);
        }
        return TRUE; // Continue enumeration
    }

}

// Function to find all top-level Chrome windows
std::vector<HWND> pwc::find_all_toplevel_chrome_windows() {
    std::vector<HWND> windows;
    EnumWindows(enum_wnd_proc, reinterpret_cast<LPARAM>(&windows));
    return windows;
}
