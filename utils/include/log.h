#ifndef LOG_H
#define LOG_H

#include <stdio.h>

// Turning it all into one symbol
#define FILE_LINE __FILE__ ":" SYM_TO_STR(__LINE__)
// Extra step to expand symbol
#define SYM_TO_STR(symbol) STRINGIFY(symbol)
// Turn symbol into string literal
#define STRINGIFY(symbol) #symbol

#ifndef LOG_LEVEL
#error You need to specify a LOG_LEVEL
#endif

#define LOG(level, msg, ...) do { \
    printf("%s > %s > ", level, FILE_LINE); \
    printf(msg, ##__VA_ARGS__); \
    printf("\n"); \
} while(0)

// error logs
#if LOG_LEVEL > 0
#define LOGE(msg, ...) LOG("ERROR", (msg), ##__VA_ARGS__)
#else
#define LOGE(msg, ...) do {} while (0)
#endif

// warning logs
#if LOG_LEVEL > 1
#define LOGW(msg, ...) LOG("WARNING", (msg), ##__VA_ARGS__)
#else
#define LOGW(msg, ...) do {} while (0)
#endif

// debug logs
#if LOG_LEVEL > 2
#define LOGD(msg, ...) LOG("DEBUG", (msg), ##__VA_ARGS__)
#else
#define LOGD(msg, ...) do {} while (0)
#endif

// info logs
#if LOG_LEVEL > 3
#define LOGI(msg, ...) LOG("INFO", (msg), ##__VA_ARGS__)
#else
#define LOGI(msg, ...) do {} while (0)
#endif

#endif // LOG_H
