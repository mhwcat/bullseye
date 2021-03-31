#ifndef BULLSEYE_LOGGER_H
#define BULLSEYE_LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <cstdint>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace bullseye::logger {
    /* FILETIME of Jan 1 1970 00:00:00. */
    static const uint64_t epoch = ((uint64_t) 116444736000000000ULL);
    static const unsigned int MS_PART_BUFFER_SIZE = 6; // "sss] "
    static const unsigned int TIME_BUFFER_SIZE = 32;

    inline void internal_gettimeofday(struct timeval* tp) {
#ifdef WIN32
        FILETIME file_time;
        SYSTEMTIME system_time;
        ULARGE_INTEGER ularge;

        GetSystemTime(&system_time);
        SystemTimeToFileTime(&system_time, &file_time);
        ularge.LowPart = file_time.dwLowDateTime;
        ularge.HighPart = file_time.dwHighDateTime;

        tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
        tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
#else
        gettimeofday(tp, 0);
#endif
    }

    inline void get_current_time(char* date_time_str) {
        std::time_t time = std::time(nullptr);

        timeval tp;
        internal_gettimeofday(&tp);
        char ms_str[MS_PART_BUFFER_SIZE] = { '\0' };

        snprintf(ms_str, MS_PART_BUFFER_SIZE, "%03ld] ", tp.tv_usec / 1000);
        strftime(date_time_str, TIME_BUFFER_SIZE, "[%Y-%m-%d %H:%M:%S.", std::localtime(&time));
        strncat(date_time_str, ms_str, MS_PART_BUFFER_SIZE);
    }

    inline void info(const char* format, ...) {
        char time_formatted[TIME_BUFFER_SIZE] = { '\0' };
        get_current_time(time_formatted);

        va_list args;
        fputs(time_formatted, stdout);
        fputs("\033[92mINFO\033[39m  ", stdout);
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fputs("\n", stdout);
    }

    inline void debug(const char* format, ...) {
        char time_formatted[TIME_BUFFER_SIZE] = { '\0' };
        get_current_time(time_formatted);

        va_list args;
        fputs(time_formatted, stdout);
        fputs("\033[36mDEBUG\033[39m ", stdout);
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fputs("\n", stdout);
    }

    inline void warn(const char* format, ...) {
        char time_formatted[TIME_BUFFER_SIZE] = { '\0' };
        get_current_time(time_formatted);

        va_list args;
        fputs(time_formatted, stdout);
        fputs("\033[33mWARN\033[39m  ", stdout);
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fputs("\n", stdout);
    }

    inline void error(const char* format, ...) {
        char time_formatted[TIME_BUFFER_SIZE] = { '\0' };
        get_current_time(time_formatted);

        va_list args;
        fputs(time_formatted, stdout);
        fputs("\033[91mERROR\033[39m ", stdout);
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fputs("\n", stdout);
    }
}

#endif