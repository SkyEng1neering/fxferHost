#ifndef BCP_PC_UART_H_
#define BCP_PC_UART_H_

#include <iostream>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdint.h>

using namespace std;

class UartHandler {
    int fd;
    int openPort(char *comPort, uint32_t baudrate);
    void setupSettings(int fd, speed_t baudrate);

public:
    UartHandler(){}

    int openComPort(char *comPort, uint32_t baudrate){
        fd = openPort(comPort, baudrate);
        if (fd > 0) {
            cout << "[ UART ] port opened" << endl;
            return 0;
        }
        cout << "[ UART ] port open error" << endl;
        return -1;
    }
    void closePort();
    void writeToPort(uint8_t *buffer, int size);
    int readFromPortBlocking(uint8_t *buffer, int num, uint32_t to_ms);
    int readFromPort(uint8_t *buffer, int num);
    int getFd();
    uint32_t getTickMs();
};

#endif /* PC_UART_H_ */
