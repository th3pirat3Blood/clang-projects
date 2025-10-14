#ifndef __LOGINJECTOR_CUSTOM_LOGGER_H__
#define __LOGINJECTOR_CUSTOM_LOGGER_H__

#include <stdio.h>
#include <time.h>

#define LOG_START(FUNC_NAME) do { \
    time_t t = time(NULL); \
    struct tm *tm_info = localtime(&t); \
    char buffer[26]; \
    strftime(buffer, 26, "%H:%M:%S", tm_info); \
    printf("[LOG %s] Entering function: %s\n", buffer, FUNC_NAME); \
} while(0)	

#define LOG_END(FUNC_NAME) do { \
    time_t t = time(NULL); \
    struct tm *tm_info = localtime(&t); \
    char buffer[26]; \
    strftime(buffer, 26, "%H:%M:%S", tm_info); \
    printf("[LOG %s] Exiting function: %s\n", buffer, FUNC_NAME); \
} while(0)

#endif



