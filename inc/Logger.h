#ifndef __logger_h__
#define __logger_h__

#include <stdio.h>

enum log_level {
    l_info, l_warn, l_error, l_fatal
};

static inline const char* log_level_name(log_level level) {
    switch(level) {
        case l_info: return "INFO";
        case l_warn: return "WARN";
        case l_error: return "ERROR";
        case l_fatal: return "FATAL";
    }
    /* Should be unreachable */
    return NULL;
}

static inline const char* log_level_color(log_level level) {
    switch(level) {
        case l_info: return "\x1b[00;36m";
        case l_warn: return "\x1b[00;33m";
        case l_error: return "\x1b[00;31m";
        case l_fatal: return "\x1b[37;41m";
    }
    /* Should be unreachable */
    return NULL;
}

#ifndef DEBUG
#define logger(LEVEL, fmt...)
#else
#define logger(LEVEL, fmt...) \
    printf("%s%-5s | %s:%d:%s() ", log_level_color(LEVEL), log_level_name(LEVEL), __FILE__, __LINE__, __func__); \
    printf(fmt);
#endif

#endif
