#include "clogger.h"

#ifdef WIN32
#define EPOCH_FILETIME          116444736000000000ULL // FILETIME of Jan 1 1970 00:00:00
#endif

#define MS_PART_BUFFER_SIZE     6  // "sss] "
#define TIME_BUFFER_SIZE        27 // "[2020-01-01 11:11:11.111] "
#define DATE_BUFFER_SIZE        11 // "2020-01-01"

#ifdef WIN32
#define NEWLINE_CHAR            "\r\n"
#else
#define NEWLINE_CHAR            "\n"
#endif

namespace bullseye {
    static bool log_file_write_enabled = false;
    static char current_log_file_path[255];
    static FILE* current_log_file_handle;
    static unsigned int current_day;

    static inline void internal_gettimeofday(struct timeval* tp) {
    #ifdef WIN32
        FILETIME file_time;
        SYSTEMTIME system_time;
        ULARGE_INTEGER ularge;

        GetSystemTime(&system_time);
        SystemTimeToFileTime(&system_time, &file_time);
        ularge.LowPart = file_time.dwLowDateTime;
        ularge.HighPart = file_time.dwHighDateTime;

        tp->tv_sec = (long) ((ularge.QuadPart - EPOCH_FILETIME) / 10000000L);
        tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    #else
        gettimeofday(tp, 0);
    #endif
    }

    static inline void internal_get_current_time_str(char* date_time_str) {
        time_t now_time = time(NULL);

        struct timeval tp;
        internal_gettimeofday(&tp);

        char ms_str[MS_PART_BUFFER_SIZE];
        memset(ms_str, '\0', MS_PART_BUFFER_SIZE * sizeof(char));

        snprintf(ms_str, MS_PART_BUFFER_SIZE, "%03ld] ", tp.tv_usec / 1000L);
        strftime(date_time_str, TIME_BUFFER_SIZE, "[%Y-%m-%d %H:%M:%S.", localtime(&now_time));
        strncat(date_time_str, ms_str, MS_PART_BUFFER_SIZE);
    }

    static inline void internal_get_current_date_str(char* date_time_str) {
        time_t now_time = time(NULL);

        struct timeval tp;
        internal_gettimeofday(&tp);

        strftime(date_time_str, TIME_BUFFER_SIZE, "%Y-%m-%d", localtime(&now_time));
    }

    static inline void update_current_day(const char* date_formatted) {
        char day_str[3];
        memset(day_str, '\0', 3 * sizeof(char));

        strncpy(day_str, date_formatted + DATE_BUFFER_SIZE - 3, 2);
        current_day = atoi(day_str);
    }

    static inline bool open_file() {
        current_log_file_handle = fopen(current_log_file_path, "a");
        if (current_log_file_handle == NULL) {
            CLOG_ERROR("Failed opening log file %s!", current_log_file_path);
            perror("Failed opening log file!");

            return false;
        }

        return true;
    }

    // Returns true if logger was rolled over to next file
    static inline bool rollover_if_needed(const char* date_time_formatted) {
        const unsigned int old_current_day = current_day;

        char date_formatted[DATE_BUFFER_SIZE];
        memset(date_formatted, '\0', DATE_BUFFER_SIZE * sizeof(char));
        strncpy(date_formatted, date_time_formatted + 1, DATE_BUFFER_SIZE - 1);

        update_current_day(date_formatted);

        if (current_day > old_current_day) {
            clogger_cleanup_file();

            strncpy(current_log_file_path + strlen(current_log_file_path) - strlen(CLOGGER_FILE_EXTENSION) - DATE_BUFFER_SIZE + 1,
                date_formatted, strlen(date_formatted));
            strncpy(current_log_file_path + strlen(current_log_file_path) - strlen(CLOGGER_FILE_EXTENSION), CLOGGER_FILE_EXTENSION,
                strlen(CLOGGER_FILE_EXTENSION) + 1);

            if (!open_file()) {
                return false;
            }

            return true;
        }

        return false;
    }

    void clogger_init_file(const char* path) {
        memset(current_log_file_path, '\0', 255 * sizeof(char));

        char date_formatted[DATE_BUFFER_SIZE];
        memset(date_formatted, '\0', DATE_BUFFER_SIZE * sizeof(char));
        internal_get_current_date_str(date_formatted);

        strncat(current_log_file_path, path, strlen(path));
        strncat(current_log_file_path, date_formatted, strlen(date_formatted));
        strcat(current_log_file_path, CLOGGER_FILE_EXTENSION);

        current_log_file_handle = fopen(current_log_file_path, "a");
        if (current_log_file_handle == NULL) {
            CLOG_ERROR("Failed opening log file %s!", current_log_file_path);
            perror("Failed opening log file!");
        } else {
            log_file_write_enabled = true;
            update_current_day(date_formatted);
        }
    }

    void clogger_cleanup_file() {
        if (current_log_file_handle != NULL) {
            int close_result = fclose(current_log_file_handle);

            if (close_result < 0) {
                perror("Failed closing log file!");
            }
        }
    }

    void debug(const char* format, ...) {
        char time_formatted[TIME_BUFFER_SIZE];
        memset(time_formatted, '\0', TIME_BUFFER_SIZE * sizeof(char));
        internal_get_current_time_str(time_formatted);

        if (log_file_write_enabled && rollover_if_needed(time_formatted)) {
            CLOG_DEBUG("Rolling over log file to %s", current_log_file_path);
        }

        va_list args;

        fputs(time_formatted, stdout);
        if (log_file_write_enabled) fwrite(time_formatted, 1, sizeof(time_formatted) - 1, current_log_file_handle);

        fputs(CLOGGER_DEBUG_TAG, stdout);
        if (log_file_write_enabled) fwrite(CLOGGER_PLAIN_DEBUG_TAG, 1, sizeof(CLOGGER_PLAIN_DEBUG_TAG) - 1, current_log_file_handle);

        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);

        if (log_file_write_enabled) {
            va_start(args, format);
            vfprintf(current_log_file_handle, format, args);
            va_end(args);
        }

        fputs(NEWLINE_CHAR, stdout);
        if (log_file_write_enabled) fwrite(NEWLINE_CHAR, 1, sizeof(char), current_log_file_handle);
    }

    void info(const char* format, ...) {
        char time_formatted[TIME_BUFFER_SIZE];
        memset(time_formatted, '\0', TIME_BUFFER_SIZE * sizeof(char));
        internal_get_current_time_str(time_formatted);

        if (log_file_write_enabled && rollover_if_needed(time_formatted)) {
            CLOG_DEBUG("Rolling over log file to %s", current_log_file_path);
        }

        va_list args;

        fputs(time_formatted, stdout);
        if (log_file_write_enabled) fwrite(time_formatted, 1, sizeof(time_formatted) - 1, current_log_file_handle);

        fputs(CLOGGER_INFO_TAG, stdout);
        if (log_file_write_enabled) fwrite(CLOGGER_PLAIN_INFO_TAG, 1, sizeof(CLOGGER_PLAIN_INFO_TAG) - 1, current_log_file_handle);

        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);

        if (log_file_write_enabled) {
            va_start(args, format);
            vfprintf(current_log_file_handle, format, args);
            va_end(args);
        }

        fputs(NEWLINE_CHAR, stdout);
        if (log_file_write_enabled) fwrite(NEWLINE_CHAR, 1, sizeof(char), current_log_file_handle);
    }

    void warn(const char* format, ...) {
        char time_formatted[TIME_BUFFER_SIZE];
        memset(time_formatted, '\0', TIME_BUFFER_SIZE * sizeof(char));
        internal_get_current_time_str(time_formatted);

        if (log_file_write_enabled && rollover_if_needed(time_formatted)) {
            CLOG_DEBUG("Rolling over log file to %s", current_log_file_path);
        }

        va_list args;

        fputs(time_formatted, stdout);
        if (log_file_write_enabled) fwrite(time_formatted, 1, sizeof(time_formatted) - 1, current_log_file_handle);

        fputs(CLOGGER_WARN_TAG, stdout);
        if (log_file_write_enabled) fwrite(CLOGGER_PLAIN_WARN_TAG, 1, sizeof(CLOGGER_PLAIN_WARN_TAG) - 1, current_log_file_handle);

        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);

        if (log_file_write_enabled) {
            va_start(args, format);
            vfprintf(current_log_file_handle, format, args);
            va_end(args);
        }

        fputs(NEWLINE_CHAR, stdout);
        if (log_file_write_enabled) fwrite(NEWLINE_CHAR, 1, sizeof(char), current_log_file_handle);
    }

    void error(const char* format, ...) {
        char time_formatted[TIME_BUFFER_SIZE];
        memset(time_formatted, '\0', TIME_BUFFER_SIZE * sizeof(char));
        internal_get_current_time_str(time_formatted);

        if (log_file_write_enabled && rollover_if_needed(time_formatted)) {
            CLOG_DEBUG("Rolling over log file to %s", current_log_file_path);
        }

        va_list args;

        fputs(time_formatted, stdout);
        if (log_file_write_enabled) fwrite(time_formatted, 1, sizeof(time_formatted) - 1, current_log_file_handle);

        fputs(CLOGGER_ERROR_TAG, stdout);
        if (log_file_write_enabled) fwrite(CLOGGER_PLAIN_ERROR_TAG, 1, sizeof(CLOGGER_PLAIN_ERROR_TAG) - 1, current_log_file_handle);

        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);

        if (log_file_write_enabled) {
            va_start(args, format);
            vfprintf(current_log_file_handle, format, args);
            va_end(args);
        }

        fputs(NEWLINE_CHAR, stdout);
        if (log_file_write_enabled) fwrite(NEWLINE_CHAR, 1, sizeof(char), current_log_file_handle);
    }
}