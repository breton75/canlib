#include "canlib.h"

using namespace canlib;

canlib::can_err *canlib::exec_cmd(const char *cmd)
{
    canlib::can_err* result = new canlib::can_err;

    QProcess* p = new QProcess();
    p->start(QString(cmd));

    if(p->waitForStarted() & p->waitForFinished())
    {
        QByteArray b  = p->readAllStandardError();
        result->code = canlib::ERROR;

        if(b.isEmpty()) {
            b = p->readAllStandardOutput();
            result->code = canlib::OK;
        }

        result->msg = (char*)malloc(b.size());
        strcpy(result->msg, b.data());

    }
    else
    {
        QByteArray b = p->readAllStandardError();

        result->msg = (char*)malloc(b.size());
        strcpy(result->msg, b.data());

        result->code = canlib::ERROR;

    }

    p->close();
    delete p;

    return result;
}

canlib::can_err *canlib::get_can_list()
{
    char *c = canlib::makechar("ip -o link show");
    canlib::can_err* result = canlib::exec_cmd(c);

    free(c);

    return result;
}

//void canlib::gte()
//{
//    char *c = canlib::makechar("ldd /home/user/proj/canlib/app_build/app");
//    can_err* result = canlib::exec_cmd(c);
//    free(c);

//    if(result->code == 0) {
////        qDebug() << r->msg ;0
//        QStringList l = QString(result->msg).split('\n');

//        free_error(result);

//        for(int i = 0; i < l.count(); i++)
//        {
//            QString ss = l[i];
//            ss = ss.section("=>", 1, 1).trimmed();
//            ss.truncate(ss.indexOf("("));
//            ss = ss.trimmed();
//            qDebug() << ss;
//            QFileInfo f = QFileInfo(ss);
//            if(f.exists()){
//                QString cmd = QString("cp %1 /home/user/proj/qtlibs/%2")
//                        .arg(ss).arg(f.fileName());

//                result = exec_cmd(makechar(cmd));
//                if(result->code != canlib::OK)
//                    qDebug() << result->msg;
//                free_error(result);
//            }

//        }

//    }
//    else
//        qDebug() << result->msg;


//}


canlib::can_err *canlib::create_device(const char* backend_name, const char* interface_name)
{
    canlib::can_err *result = new canlib::can_err;

    SvCanDevice *DEVICE = new SvCanDevice();

    DEVICE->backendName = QString(backend_name);
    DEVICE->interfaceName = QString(interface_name);

    /* create can device */
    DEVICE->canDevice = QCanBus::instance()->createDevice(DEVICE->backendName, DEVICE->interfaceName, &DEVICE->lastError);

    if (!DEVICE->canDevice) {
        result->code = canlib::ERROR;
        result->msg = canlib::makechar(QString("Error creating device '%1', reason: '%2'\0")
                                .arg(DEVICE->backendName).arg(DEVICE->lastError));
        return result;
    }

    result->msg = reinterpret_cast<char*>(DEVICE);

    return result;
}

canlib::can_err *canlib::connect_device(char *device)
{
    canlib::can_err *result = new canlib::can_err;

    SvCanDevice *DEVICE = get_device_by_id(device);

    if(!DEVICE){
        result->code = canlib::ERROR;
        result->msg = canlib::makechar("Wrong device pointer\0");
        return result;
    }

    /* connecting */
    if (!DEVICE->canDevice->connectDevice()) {
        result->code = canlib::ERROR;
        result->msg = canlib::makechar(QString("Connection error: %1\0").arg(DEVICE->canDevice->errorString()));
        return result;
    }

    return result;
}

char *canlib::makechar(QString str)
{
    char* result = (char*)malloc(str.toStdString().length() + 1);
//    qDebug() << str.toStdString().c_str() << str.toStdString().length();
    memset((void*)(result + str.toStdString().length()), 0, 1);
    str.toStdString().copy(result, str.toStdString().length());

    return result;
}

canlib::can_err *canlib::set_params(char *device, can_params *params)
{
    canlib::can_err *result = new canlib::can_err;

    SvCanDevice *DEVICE = canlib::get_device_by_id(device);

    if(!DEVICE){
        result->code = canlib::ERROR;
        result->msg = canlib::makechar("Wrong device pointer\0");
        return result;
    }

    DEVICE->params.error_filter = params->error_filter;
    DEVICE->params.loopback = params->loopback;
    DEVICE->params.receive_own = params->receive_own;
    DEVICE->params.can_fd = params->can_fd;
    DEVICE->params.bitrate = params->bitrate;
    DEVICE->params.flexible_rate_format = params->flexible_rate_format;
    DEVICE->params.extended_frame_format = params->extended_frame_format;

    /* applying params to can device */
    /* Note: In most cases, configuration changes only take effect after a reconnect. */
    QVariant loopback = DEVICE->params.loopback == -1 ? QVariant() : QVariant(DEVICE->params.loopback == 0 ? false : true);
    QVariant receive_own = DEVICE->params.receive_own == -1 ? QVariant() : QVariant(DEVICE->params.receive_own == 0 ? false : true);
    QVariant can_fd = DEVICE->params.can_fd == -1 ? QVariant() : QVariant(DEVICE->params.can_fd == 0 ? false : true);
    QVariant bitrate = DEVICE->params.bitrate == 0 ? QVariant() : QVariant(DEVICE->params.bitrate);

    DEVICE->canDevice->setConfigurationParameter(QCanBusDevice::ErrorFilterKey, QVariant(DEVICE->params.error_filter));
    DEVICE->canDevice->setConfigurationParameter(QCanBusDevice::LoopbackKey, loopback);
    DEVICE->canDevice->setConfigurationParameter(QCanBusDevice::ReceiveOwnKey, receive_own);
    DEVICE->canDevice->setConfigurationParameter(QCanBusDevice::BitRateKey, bitrate);
    DEVICE->canDevice->setConfigurationParameter(QCanBusDevice::CanFdKey, can_fd);

    return  result;
}

canlib::can_err *canlib::disconnect_device(char *device)
{
  canlib::can_err *result = new canlib::can_err;

  SvCanDevice *DEVICE = canlib::get_device_by_id(device);
  if(!DEVICE){
      result->code = canlib::ERROR;
      result->msg = canlib::makechar("Wrong device pointer");
      return result;
  }

  if(QCanBusDevice::ConnectedState == DEVICE->canDevice->state())
    DEVICE->canDevice->disconnectDevice();


  return result;

}

canlib::can_err *reconnect_device(char *device)
{
    /* disconnect. on error return */
    canlib::can_err *result = canlib::disconnect_device(device);
    
    if(result->code != canlib::OK)
        return result;

    canlib::free_error(result);

    /* connect */
    return canlib::connect_device(device);

}

canlib::can_err *canlib::write_frame(char *device, can_frame *frame)
{
    canlib::can_err *result = new canlib::can_err;

    SvCanDevice *DEVICE = canlib::get_device_by_id(device);

    if(!DEVICE){
        result->code = canlib::ERROR;
        result->msg = canlib::makechar("Wrong device pointer");
        return result;
    }

    bool eff = DEVICE->params.extended_frame_format == 0;
    // qDebug() <<"qq" << QString::number(int(frame->data[0]), 16) << int(frame->data[1]) << frame->data[2] ;
    if((!eff && (frame->id > 0x7FF)) ||     //11 bits
        (eff && (frame->id > 0x3FFFFFFF)))  // 29 bits
    {
        result->code = canlib::ERROR;
        result->msg = canlib::makechar(QString("Frame id (%1) is out of range %2 bits").arg(frame->id).arg(eff ? 29 : 11));
        return result;
    }

    QByteArray buf = QByteArray::fromRawData(frame->data, frame->data_length);
    buf.truncate(DEVICE->params.flexible_rate_format ? 64 : 8);

    QCanBusFrame frm;
    frm.setPayload(buf);
    frm.setFrameId(frame->id);
    frm.setFrameType(static_cast<QCanBusFrame::FrameType>(frame->type));

    frm.setExtendedFrameFormat(DEVICE->params.extended_frame_format);
    frm.setFlexibleDataRateFormat(DEVICE->params.flexible_rate_format);

//    // qDebug() << "frame" << frm.payload().toHex();

    if(!DEVICE->canDevice->writeFrame(frm))
    {
        result->code = canlib::ERROR;
        result->msg = canlib::makechar(DEVICE->canDevice->errorString());
        return result;
    }

    return result;
}

canlib::can_frame *canlib::read_frame(char *device)
{
    SvCanDevice *DEVICE = canlib::get_device_by_id(device);

    if(!DEVICE)
        return NULL;

    QCanBusFrame frm = DEVICE->canDevice->readFrame();
    if(!frm.isValid())
        return NULL;

    canlib::can_frame *result = new canlib::can_frame;
    result->device = device;
    result->id = frm.frameId();
    result->type = frm.frameType();

//    // qDebug() << "ee" << frm.payload();
    result->data_length = frm.payload().length();
    
    result->data = (char*)malloc(result->data_length);
    memcpy(result->data, frm.payload().data(), frm.payload().length());
    
    result->data_str = canlib::makechar(frm.toString()); // (char*)malloc(b.length());

    result->microseconds = frm.timeStamp().seconds() * 1000000 +
            frm.timeStamp().microSeconds();

    return result;
}

SvCanDevice* canlib::get_device_by_id(char *device)
{
  if(!device)
      return NULL;

  /* небезопасное пребразование. но другого ничего не придумал.
   * dynamic_cast в qt не работает. bad_cast не перехватывается */
  SvCanDevice* d = reinterpret_cast<SvCanDevice*>(device);

  return d;
}

void canlib::free_device(char *device)
{
    SvCanDevice *DEVICE = canlib::get_device_by_id(device);

    if(!DEVICE)
        return;

    delete DEVICE->canDevice;
//    delete DEVICE->params;
    delete DEVICE;
}

void canlib::free_frame(can_frame *frame)
{
    if(!frame)
        return;

    free(frame->data);
    free(frame->data_str);
    delete frame;
}

void canlib::free_error(can_err *error)
{
    if(!error)
        return;

    free(error->msg);
    delete error;
}
