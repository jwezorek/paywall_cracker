#pragma once

#define NOMINMAX
#include <Windows.h>
#include <atomic>
#include <thread>

namespace pwc {

    constexpr DWORD  MSG_COMPLETE = WM_APP + 101;

    class state {
    private:
        HWND wnd_;
        HWND preamble_ctrl_;
        HWND interval_ctrl_;
        HWND pulse_mode_ctrl_;
        HWND btn_;
        HWND chrome_wnd_;
        HFONT font_;
        std::atomic<bool> is_running_;
        std::atomic<bool> cancelled_;
        std::thread thread_;
        int preamble_;
        int interval_;
        int count_;

        static void thread_body(state* state);
        void handle_start_of_thread();
        void handle_end_of_thread();

    public:
        state(HWND wnd);
        void set_ctrls(HWND preamble, HWND interval, HWND pulse, HWND btn);
        void toggle_check_state();
        void set_font(HFONT font);
        HFONT font() const;
        bool is_running() const;
        void set_cancelled();
        void wait_for_thread();
        void launch_thread();
        HWND button() const;
        bool cancelled() const;
    };

    void handle_main_button_click(state& state);
    void handle_thread_complete(state& state);
}