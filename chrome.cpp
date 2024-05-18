#include "chrome.h"
#include <string>
#include <sstream>

namespace {

    bool IsChromeWindow(HWND hwnd) {
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
    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        std::vector<HWND>* pWindows = reinterpret_cast<std::vector<HWND>*>(lParam);
        if (IsChromeWindow(hwnd)) {
            pWindows->push_back(hwnd);
        }
        return TRUE; // Continue enumeration
    }

}

// Function to find all top-level Chrome windows
std::vector<HWND> FindAllChromeWindows() {
    std::vector<HWND> windows;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));

    for (auto wnd : windows) {
        RECT r;
        GetWindowRect(wnd, &r);
        bool visible = IsWindowVisible(wnd);
    }

    return windows;
}
