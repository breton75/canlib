#ifndef CANLIB_H
#define CANLIB_H

#include <QProcess>
#include <QByteArray>
#include <QDebug>
#include <QCanBusFrame>
#include <QCanBus>
#include <QCanBusDevice>
#include <QVariant>


//#ifdef __cplusplus
//extern "C" {
//#endif

namespace canlib {

enum {
    ERROR = -1,
    OK = 0
};

struct can_err{
    int code = OK;
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

can_err *set_params(char *device, can_params *params);

void free_device(char *device);
void free_error(can_err* error);
void free_frame(can_frame* frame);

char* makechar(QString str);

//#ifdef __cplusplus
//}
//#endif

}

#endif // CANLIB_H
