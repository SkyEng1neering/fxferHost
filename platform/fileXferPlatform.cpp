#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>
#include <sys/select.h>
#include "fileXferPlatform.h"
#include "pc_uart.h"

#define READ_TIMEOUT_MS                                  60000
#define DEBUG_IO_BUFFER_LEN                              256

static char debug_io_buf[DEBUG_IO_BUFFER_LEN] = {0};

extern UartHandler *uartHandler;

void platform_send(uint8_t* data, uint16_t len) {
    uartHandler->writeToPort(data, len);
}

uint16_t platform_read(uint8_t* data, uint16_t len) {
    return (uint16_t)uartHandler->readFromPortBlocking(data, len, READ_TIMEOUT_MS);
}

void platform_sleep(uint32_t ms) {
    usleep(ms*1000);
}

uint32_t platform_get_tick() {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    return current_time.tv_sec*1000 + current_time.tv_usec/1000;
}

void log_info(const char* str, ...) {
    va_list list;
    va_start(list, str);
    vsnprintf(debug_io_buf, DEBUG_IO_BUFFER_LEN - 1, str, list);
    printf("%s", debug_io_buf);
    va_end(list);
}

void log_debug(const char* str, ...) {
    va_list list;
    va_start(list, str);
    vsnprintf(debug_io_buf, DEBUG_IO_BUFFER_LEN - 1, str, list);
    printf("%s", debug_io_buf);
    va_end(list);
}

void log_error(const char* str, ...) {
    va_list list;
    va_start(list, str);
    vsnprintf(debug_io_buf, DEBUG_IO_BUFFER_LEN - 1, str, list);
    printf("%s", debug_io_buf);
    va_end(list);
}
