#ifndef BULLSEYE_LOGGER_H
#define BULLSEYE_LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <ctime>

namespace logger {
    inline void get_current_time(char* date_time_str) {
        std::time_t time = std::time(nullptr);
        strftime(date_time_str, 23, "[%Y-%m-%d %H:%M:%S] ", std::localtime(&time));
    }

    void info(const char* format, ...) {
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

    void debug(const char* format, ...) {
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

    void error(const char* format, ...) {
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