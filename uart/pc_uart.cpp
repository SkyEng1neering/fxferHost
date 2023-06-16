#include <sys/time.h>
#include "pc_uart.h"

using namespace std;

struct BaudratePair {
    uint32_t value;
    speed_t termios_val;
};

BaudratePair baudrates[] = {
        { 115200, B115200 },
        { 230400, B230400 },
        { 460800, B460800 },
        { 230400, B500000 },
        { 500000, B576000 },
        { 921600, B921600 },
        { 1000000, B1000000 },
        { 1500000, B1500000 },
        { 2000000, B2000000 },
        { 2500000, B2500000 },
        { 3000000, B3000000 },
        { 3500000, B3500000 },
        { 4000000, B4000000 }
};

int	UartHandler::openPort(char *comPort, uint32_t baudrate) {
    int fd; /* File descriptor for the port */
    fd = open(comPort, O_RDWR);
    if (fd == -1){
        cout << "Error while opening port " << comPort << endl;
        return fd;
    }

    /* Find baudrate corresponded to given value */
    speed_t br = B115200;
    for (int i = 0; i < sizeof(baudrates); i++) {
        if (baudrates[i].value == baudrate) {
            br = baudrates[i].termios_val;
        }
    }
    setupSettings(fd, br);
    return fd;
}

void UartHandler::setupSettings(int fd, speed_t baudrate) {
    struct termios options;
    memset(&options, 0, sizeof(options));
    /*
     * Get the current options for the port...
     */
    if (tcgetattr(fd, &options) != 0)   {
        cout << "tcgetattr() == " << strerror(errno) << endl;
    }

    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    /*
     * Enable the receiver and set local mode...
     */
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;

    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF | IXANY);

    options.c_lflag &= ~ICANON;
    options.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    options.c_lflag &= ~ECHO; // Disable echo

    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    // options.c_iflag &= ~(INLCR | ICRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_oflag &= ~(OPOST);

    options.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    options.c_cc[VMIN] = 0;

    /*
     * Set the new options for the port...
     */
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        cout << "tcsetattr() == " << strerror(errno) << endl;
    }
}

void UartHandler::writeToPort(uint8_t *buffer, int size) {
    int n = write(fd, buffer, size);
    if (n < 0)
        cout << "Error while writing to port" << endl;
}

uint32_t UartHandler::getTickMs() {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    return current_time.tv_sec*1000 + current_time.tv_usec/1000;
}

int UartHandler::readFromPortBlocking(uint8_t *buffer, int num, uint32_t to_ms) {
    int dataRead = 0;
    uint32_t startTick = getTickMs();
    while ((dataRead != num) && (getTickMs() - startTick <= to_ms)) {
        dataRead += readFromPort(&buffer[dataRead], num - dataRead);
    }
    return dataRead;
}

int UartHandler::readFromPort(uint8_t *buffer, int num) {
    fd_set rd;
    struct timeval tv;
    int err;

    FD_ZERO(&rd);
    FD_SET(fd, &rd);

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    err = select(fd + 1, &rd, NULL, NULL, &tv);
    return read(fd, buffer, num);
}

void UartHandler::closePort() {
    close(fd);
}

int UartHandler::getFd() {
    return fd;
}
