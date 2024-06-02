#include "chrome.h"
#include <string>
#include <sstream>
#include <psapi.h>
#include <filesystem>

namespace {

    std::string get_exe_name_from_wnd(HWND hWnd) {
        DWORD processID;
        GetWindowThreadProcessId(hWnd, &processID);  

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
        if (!hProcess) {
            return {};
        }

        char buffer[MAX_PATH];
        if (GetModuleFileNameEx(hProcess, NULL, buffer, MAX_PATH) == 0) {
            CloseHandle(hProcess);
            return {};
        }

        CloseHandle(hProcess); 
        std::string full_path(buffer);

        return std::filesystem::path(full_path).filename().string();
    }

    bool is_chrome_window(HWND hwnd) {
        const int length = 256;
        char class_name[length] = { 0 };
        GetClassName(hwnd, class_name, length);

        DWORD style = GetWindowLong(hwnd, GWL_STYLE);

        bool has_chrome_class = std::strcmp(class_name, "Chrome_WidgetWin_1") == 0;
        bool is_overlapped_window = (style & WS_OVERLAPPEDWINDOW);
        bool is_visible = IsWindowVisible(hwnd);
        bool is_chrome_exe = get_exe_name_from_wnd(hwnd) == "chrome.exe";

        return is_chrome_exe && 
            has_chrome_class && 
            is_overlapped_window && 
            is_visible;
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
