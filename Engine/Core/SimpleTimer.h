#pragma once
//This file is heavily vibe coded be careful.
#ifndef SIMPLE_TIMER_HPP
#define SIMPLE_TIMER_HPP

#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

#define STIMER_START(name) Stopwatch name; name.start()
#define STIMER_STOP(name)  name.stop()
#define STIMER_DURATION_MS(name) (name).durationMs()
#define STIMER_PRINT(name) \
    std::cout << #name << ": " << format_duration((name).durationMs()) << std::endl

// Scoped timer (auto prints when scope ends)
#define SCOPE_TIMER(label) ScopedTimer timer_##__LINE__(label)
#define ENABLE_SCOPE_TIMER_PRINT ScopedTimer::print_ = true;
#define DISABLE_SCOPE_TIMER_PRINT ScopedTimer::print_ = false;


    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    // Helper: format durations nicely
    inline std::string format_duration(double ms) {
        std::ostringstream ss;
        if (ms < 1.0)
            ss << std::fixed << std::setprecision(3) << (ms * 1000.0) << " us";
        else if (ms < 1000.0)
            ss << std::fixed << std::setprecision(3) << ms << " ms";
        else
            ss << std::fixed << std::setprecision(3) << (ms / 1000.0) << " s";
        return ss.str();
    }

    // ---------------- Stopwatch ----------------
    class Stopwatch {
    public:
        Stopwatch() : running_(false) {}

        void start() {
            if (!running_) {
                start_time_ = Clock::now();
                running_ = true;
            }
        }

        void stop() {
            if (running_) {
                end_time_ = Clock::now();
                running_ = false;
            }
        }

        void reset() {
            running_ = false;
            laps_.clear();
        }

        // Milliseconds elapsed since start or between start/stop
        double durationMs() const {
            if (running_) {
                auto now = Clock::now();
                return std::chrono::duration<double, std::milli>(now - start_time_).count();
            }
            else {
                return std::chrono::duration<double, std::milli>(end_time_ - start_time_).count();
            }
        }

        // Record a lap and return its duration
        double lap() {
            auto now = Clock::now();
            double ms = std::chrono::duration<double, std::milli>(now - last_lap_time_).count();
            last_lap_time_ = now;
            laps_.push_back(ms);
            return ms;
        }

        const std::vector<double>& laps() const { return laps_; }

    private:
        TimePoint start_time_{};
        TimePoint end_time_{};
        TimePoint last_lap_time_{};
        bool running_;
        std::vector<double> laps_;
    };


    // ---------------- ScopedTimer ----------------
    // Automatically measures a scope duration and prints on destruction
    class ScopedTimer {
    public:
        explicit ScopedTimer(const std::string& label = "")
            : label_(label), start_(Clock::now()) {
        }

        ~ScopedTimer() {
            if (print_) {
                auto end = Clock::now();
                double ms = std::chrono::duration<double, std::milli>(end - start_).count();
                std::cout << (label_.empty() ? "ScopedTimer" : label_)
                    << ": " << format_duration(ms) << std::endl;
            }
        }

        static bool print_;

    private:
        std::string label_;

        TimePoint start_;
    };

#endif 
