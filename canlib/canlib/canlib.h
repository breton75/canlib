#ifndef CANLIB_H
#define CANLIB_H
#include <QProcess>
#include <QByteArray>
#include <QDebug>
#include <QCanBusFrame>
#include <QCanBus>
#include <QCanBusDevice>
#include <QVariant>
#include <QString>
#include <QDir>
#include <QFile>

#include <QtCore/qglobal.h>

#if defined(CANLIB_LIBRARY)
#  define CANLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CANLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

//#include "canlib_global.h"


namespace canlib {

extern "C" {

    enum {
        ERROR = -1,
        OK = 0
    };



    struct CANLIBSHARED_EXPORT can_err{
        int code = canlib::OK;
        char *msg = nullptr;
    };

    struct CANLIBSHARED_EXPORT can_params {
        unsigned int error_filter = 0;
        int loopback = -1;       // -1 - unssigned, 0 - false, any other - true
        int receive_own = -1;    // -1 - unssigned, 0 - false, any other - true
        int can_fd = -1;         // -1 - unssigned, 0 - false, any other - true
        unsigned int bitrate = 0;  // 0 - unssigned

        int extended_frame_format = 0; // 0 - false (id is 11 bits number), any other - true (id is 29 bits number)
        int flexible_rate_format = 0;  // 0 - false (max 8 byte per frame), any other - true (max 64 byte per frame)
    };

    struct CANLIBSHARED_EXPORT can_frame {
      char *device;
      unsigned int id;
      int type;
      char *data;
      unsigned int data_length;
      char *data_str;
      unsigned int microseconds;
    };

    CANLIBSHARED_EXPORT can_err *get_can_list();

    CANLIBSHARED_EXPORT can_err *create_device(const char* backend_name, const char* interface_name);
    CANLIBSHARED_EXPORT can_err *connect_device(char *device);
    CANLIBSHARED_EXPORT can_err *disconnect_device(char *device);
    CANLIBSHARED_EXPORT can_err *reconnect_device(char *device);

    CANLIBSHARED_EXPORT can_err *write_frame(char *device, can_frame *frame);
    CANLIBSHARED_EXPORT can_frame *read_frame(char *device);

    CANLIBSHARED_EXPORT can_err *set_params(char *device, can_params *params);

    CANLIBSHARED_EXPORT void free_device(char *device);
    CANLIBSHARED_EXPORT void free_error(can_err* error);
    CANLIBSHARED_EXPORT void free_frame(can_frame* frame);

}

//void gte();
char* makechar(QString str);

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

SvCanDevice* get_device_by_id(char *device);

can_err *exec_cmd(const char *cmd);
}

#endif // CANLIB_H
