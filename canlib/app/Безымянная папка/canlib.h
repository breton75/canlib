#ifndef CANLIB_H
#define CANLIB_H

#include <QProcess>
#include <QByteArray>
#include <QDebug>
#include <QCanBusFrame>
#include <QCanBus>
#include <QCanBusDevice>
#include <QVariant>


namespace canlib {

const QString hex64 = "000102030405060708090A0b0c0d0e0f"
                      "101112131415161718191a1b1c1d1e1f"
                      "202122232425262728292a2b2c2d2e2f"
                      "303132333435363738393a3b3c3d3e3f";

enum {
    ERROR = -1,
    OK = 0
};

const int CAN_LEN_64 = 64;
const int CAN_LEN_8 = 8;

typedef char* can_dev;

struct can_err{
    int code = canlib::OK;
    char *msg = nullptr;
};

struct can_params {
    unsigned int error_filter = 0;
    int loopback = -1;       // -1 - unssigned, 0 - false, any other - true
    int receive_own = -1;    // -1 - unssigned, 0 - false, any other - true
    int can_fd = -1;         // -1 - unssigned, 0 - false, any other - true
    unsigned int bitrate = 0;  // 0 - unssigned

    int extended_frame_format = 0; // 0 - false (id is 11 bits number), any other - true (id is 29 bits number)
    int flexible_rate_format = 0;  // 0 - false (max 8 byte per frame), any other - true (max 64 byte per frame)
};

/*
enum FrameType {
    UnknownFrame        = 0x0,
    DataFrame           = 0x1,
    ErrorFrame          = 0x2,
    RemoteRequestFrame  = 0x3,
    InvalidFrame        = 0x4
}; */

struct can_frame {
  char *device;
  unsigned int id;
  int type;
  char data[64];
  unsigned int data_length;
  char *data_str;
  unsigned int microseconds;
};


class SvCanDevice
{
  public:
    SvCanDevice() {}
//    ~SvCanDevice();

    QCanBusDevice* canDevice = nullptr;

    can_params params; // = nullptr;

    QString backendName;
    QString interfaceName;
    QString lastError;
};

can_err *exec_cmd(const char *cmd);
can_err *get_can_list();

can_err *create_device(const char* backend_name, const char* interface_name);
can_err *connect(char *device);
can_err *disconnect(char *device);
can_err *reconnect(char *device);

can_err *write(char *device, can_frame *frame);
can_frame *read(char *device);

SvCanDevice* get_device_by_id(char *device);

can_err *set_params(SvCanDevice *DEVICE, can_params *params);
can_err *set_params(char *device, can_params *params);

void free_device(char *device);
void free_error(can_err* error);
void free_frame(can_frame* frame);

char* makechar(QString str);

}

#endif // CANLIB_H
