#include <string>
#include <stdio.h>
#include <thread>
#include "anyoption.h"
#include "pc_uart.h"
#include "fileXfer.h"

#define TIME_STR_LEN                10
#define UART_RX_BUFFER_MAX          255

using namespace std;

UartHandler *uartHandler = nullptr;

void proc_func() {
    while (true) {
        fxfer_parser();
    }
}

int main(int argc, char *argv[]) {
    AnyOption *opt = new AnyOption();

    opt->setVerbose();
    opt->autoUsagePrint(true);

    opt->addUsage("Usage: ");
    opt->addUsage("");
    opt->addUsage(" -h --help                  prints this help ");
    opt->addUsage(" --uart_path <uart_path>    path to uart device (i.e. /dev/ttyS9)");
    opt->addUsage(" --baud_rate <val>          uart baudrate (i.e. 115200)");
    opt->addUsage(" --cmd <command>            commands: get_files_list,");
    opt->addUsage("                                      make_handshake,");
    opt->addUsage("                                      send_file,");
    opt->addUsage("                                      receive_file,");
    opt->addUsage("                                      get_file_hash,");
    opt->addUsage("                                      listen");

    opt->setFlag("help", 'h');
    opt->setOption("uart_path");
    opt->setOption("baud_rate");
    opt->setOption("cmd");

    opt->processCommandArgs(argc, argv);

    if (!opt->hasOptions()) { /* print usage if no options */
        opt->printUsage();
        delete opt;
        return 0;
    }

    if (opt->getFlag("help") || opt->getFlag('h')){
        opt->printUsage();
        return 0;
    }
    if (opt->getValue("uart_path") == NULL){
        cout << "   --uart_path parameter error" << endl;
        return 0;
    }
    if (opt->getValue("baud_rate") == NULL){
        cout << "   --uart_path parameter error" << endl;
        return 0;
    }
    if (opt->getValue("cmd") == NULL){
        cout << "   --cmd parameter error" << endl;
        return 0;
    }

    /* Get parameters values */
    string uart_path = opt->getValue("uart_path");
    int baud_rate = stoi(opt->getValue("baud_rate"));
    string cmd = opt->getValue("cmd");

    /* Open UART */
    uartHandler = new UartHandler();
    if (uartHandler->openComPort(const_cast<char*>(uart_path.c_str()), baud_rate) < 0) {
        cout << "Can't open UART with path: " << uart_path.c_str() << endl;
        return 0;
    }
    cout << "UART opened" << endl;


    thread fxfer_proc_thread(proc_func);
    fxfer_proc_thread.detach();

    /* Run some command */
    if (cmd == "make_handshake") {
        make_handshake(1024);
    } else if (cmd == "get_files_list") {
        make_handshake(1024);
        request_files_list();
    } else if (cmd == "send_file") {
        string filename;
        cout << "Enter filename:" << endl;
        cin >> filename;
        make_handshake(1024);
        send_file(filename.c_str());
    } else if (cmd == "receive_file") {
        cout << "Not supported" << endl;
    } else if (cmd == "get_file_hash") {
        string filename;
        cout << "Enter filename:" << endl;
        cin >> filename;
        make_handshake(1024);
        request_file_hash(filename.c_str());
    } else if (cmd == "listen") {
        while (1) {
            usleep(1000000);
        }
    }

    return 0;
}
