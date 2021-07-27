/*
clogger v0.6 - Very simple logging utility for C/C++
2021 (c) Michał 'mhwcat' Gumny

This is lightweight, cross-platform utility that can be used for logging needs in C/C++.
It should compile and run on Linux, Windows and macOS. It outputs to stdout and supports parameters.
Four log levels are available: DEBUG, INFO, WARN, ERROR.
Example usage:
CLOG_INFO("Hello number %d from %s!", i, name);
Example output:
[2021-07-19 16:12:46.748] INFO Hello number 3 from clogger!
If you want to use log files, provide log directory path at start of your app:
    CLOG_INIT_FILE("logs/");
and cleanup open log files when your app stops:
    CLOG_CLEANUP_FILE();    

Colored terminal output can be enabled by passing CLOGGER_USE_COLORED_OUTPUT option during compilation.
Log level can be selected by passing CLOGGER_LOG_LEVEL option with possible values:
CLOG_LVL_DEBUG (default)
CLOG_LVL_INFO
CLOG_LVL_WARN
CLOG_LVL_ERROR

To do:
- Use WinAPI calls when writing to log files on Win32 (stdlib is not reliable there)

Changelog:
- v0.6 (2021-07-21)  
    Added log files support
- v0.5 (2021-07-19)  
    Initial version

License:
Copying and distribution of this software, with or without modification,
are permitted in any medium without royalty, provided the copyright notice
and this notice are preserved.
This software is offered as-is, without any warranty.

*/

#ifndef CLOGGER_H
#define CLOGGER_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#ifdef WIN32
#include <stdint.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

#define CLOG_LVL_DEBUG              0
#define CLOG_LVL_INFO               1
#define CLOG_LVL_WARN               2
#define CLOG_LVL_ERROR              3

#ifndef CLOGGER_LOG_LEVEL
#define CLOGGER_LOG_LEVEL           CLOG_LVL_DEBUG
#endif

#define CLOGGER_FILE_EXTENSION      ".clog"

#if CLOGGER_LOG_LEVEL == CLOG_LVL_INFO
#define CLOG_DEBUG(format, ...)     ;
#define CLOG_INFO(format, ...)      info(format, ##__VA_ARGS__)
#define CLOG_WARN(format, ...)      warn(format, ##__VA_ARGS__)
#define CLOG_ERROR(format, ...)     error(format, ##__VA_ARGS__)
#elif CLOGGER_LOG_LEVEL == CLOG_LVL_WARN
#define CLOG_DEBUG(format, ...)     ;
#define CLOG_INFO(format, ...)      ;
#define CLOG_WARN(format, ...)      warn(format, ##__VA_ARGS__)
#define CLOG_ERROR(format, ...)     error(format, ##__VA_ARGS__)
#elif CLOGGER_LOG_LEVEL == CLOG_LVL_ERROR
#define CLOG_DEBUG(format, ...)     ;
#define CLOG_INFO(format, ...)      ;
#define CLOG_WARN(format, ...)      ;
#define CLOG_ERROR(format, ...)     error(format, ##__VA_ARGS__)
#else
#define CLOG_DEBUG(format, ...)     debug(format, ##__VA_ARGS__)
#define CLOG_INFO(format, ...)      info(format, ##__VA_ARGS__)
#define CLOG_WARN(format, ...)      warn(format, ##__VA_ARGS__)
#define CLOG_ERROR(format, ...)     error(format, ##__VA_ARGS__)
#endif
#define CLOG_INIT_FILE(path)        clogger_init_file(path)
#define CLOG_CLEANUP_FILE()         clogger_cleanup_file()

#define CLOGGER_COLOR_DEBUG_TAG     "\033[36mDEBUG\033[39m "
#define CLOGGER_COLOR_INFO_TAG      "\033[92mINFO\033[39m  "
#define CLOGGER_COLOR_WARN_TAG      "\033[33mWARN\033[39m  "
#define CLOGGER_COLOR_ERROR_TAG     "\033[91mERROR\033[39m "
#define CLOGGER_PLAIN_DEBUG_TAG     "DEBUG "
#define CLOGGER_PLAIN_INFO_TAG      "INFO  "
#define CLOGGER_PLAIN_WARN_TAG      "WARN  "
#define CLOGGER_PLAIN_ERROR_TAG     "ERROR "

#ifdef CLOGGER_USE_COLORED_OUTPUT
#define CLOGGER_DEBUG_TAG           CLOGGER_COLOR_DEBUG_TAG
#define CLOGGER_INFO_TAG            CLOGGER_COLOR_INFO_TAG
#define CLOGGER_WARN_TAG            CLOGGER_COLOR_WARN_TAG
#define CLOGGER_ERROR_TAG           CLOGGER_COLOR_ERROR_TAG
#else
#define CLOGGER_DEBUG_TAG           CLOGGER_PLAIN_DEBUG_TAG
#define CLOGGER_INFO_TAG            CLOGGER_PLAIN_INFO_TAG
#define CLOGGER_WARN_TAG            CLOGGER_PLAIN_WARN_TAG
#define CLOGGER_ERROR_TAG           CLOGGER_PLAIN_ERROR_TAG
#endif

namespace bullseye {
    void clogger_init_file(const char* path);
    void clogger_cleanup_file();

    void debug(const char* format, ...);
    void info(const char* format, ...);
    void warn(const char* format, ...);
    void error(const char* format, ...);
}

#endif