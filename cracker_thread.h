#pragma once

#include <Windows.h>
#include <atomic>
#include <thread>

#define MSG_COMPLETE WM_APP + 101

class state {
    HWND wnd_;
    HWND preamble_ctrl_;
    HWND interval_ctrl_;
    HWND count_ctrl_;
    HWND btn_;
    HWND chrome_wnd_;
    std::atomic<bool> is_running_;
    std::atomic<bool> cancelled_;
    std::thread thread_;
    int preamble_;
    int interval_;
    int count_;
public:
    state(HWND wnd);
    void set_ctrls(HWND preamble, HWND interval, HWND count, HWND btn);
    void set_chrome_wnd(HWND wnd);
    bool is_running() const;
    void set_cancelled();
    void handle_start_of_thread();
    void handle_end_of_thread();
    void wait_for_thread();
    void launch_thread();
    HWND button() const;
    bool cancelled() const;
    int preamble() const;
    int interval() const;
    int count() const;
};

void handle_button_click(state& state);
void handle_thread_complete(state& state);