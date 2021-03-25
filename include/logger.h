#ifndef BULLSEYE_LOGGER_H
#define BULLSEYE_LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <ctime>

namespace bullseye::logger {
    inline void get_current_time(char* date_time_str) {
        std::time_t time = std::time(nullptr);
        strftime(date_time_str, 23, "[%Y-%m-%d %H:%M:%S] ", std::localtime(&time));
    }

    inline void info(const char* format, ...) {
        char time_formatted[23];
        get_current_time(time_formatted);

        va_list args;
        fprintf(stdout, time_formatted);
        fprintf(stdout, "INFO ");
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fprintf(stdout, "\n");
    }

    inline void debug(const char* format, ...) {
        char time_formatted[23];
        get_current_time(time_formatted);

        va_list args;
        fprintf(stdout, time_formatted);
        fprintf(stdout, "DEBUG ");
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fprintf(stdout, "\n");
    }

    inline void warn(const char* format, ...) {
        char time_formatted[23];
        get_current_time(time_formatted);

        va_list args;
        fprintf(stdout, time_formatted);
        fprintf(stdout, "WARN ");
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fprintf(stdout, "\n");
    }

    inline void error(const char* format, ...) {
        char time_formatted[23];
        get_current_time(time_formatted);

        va_list args;
        fprintf(stdout, time_formatted);
        fprintf(stdout, "ERROR ");
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fprintf(stdout, "\n");
    }
}

#endif