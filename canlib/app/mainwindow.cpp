#include "mainwindow.h"
#include "ui_mainwindow.h"

QMutex mutex;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef Q_OS_LINUX
    
    r = canlib::get_can_list();

    if(r->code == canlib::OK) {
//        qDebug() << r->msg ;0
        QStringList l = QString(r->msg).split('\n');
        for(int i = 0; i < l.count(); i++)
            l[i] = l[i].section(':', 1, 1).trimmed();

        ui->cbCanList_0->addItems(l);
        ui->cbCanList_0->setCurrentIndex(ui->cbCanList_0->findText("can0"));

        ui->cbCanList_1->addItems(l);
        ui->cbCanList_1->setCurrentIndex(ui->cbCanList_1->findText("can1"));
    }

    else
        qCritical() << "Error on getting can list: " << r->msg;

    canlib::free_error(r);
    
#else 
    
    ui->cbCanList_0->addItem("can0");
    ui->cbCanList_1->addItem("can1");
    
#endif
    
    foreach (QCheckBox *check, ui->widgetParams_0->findChildren<QCheckBox*>()) {
        check->setCheckState(Qt::PartiallyChecked);
    }

    foreach (QCheckBox *check, ui->widgetParams_1->findChildren<QCheckBox*>()) {
        check->setCheckState(Qt::PartiallyChecked);
    }

}

MainWindow::~MainWindow()
{
    if(_tcan0)
        _tcan0->~SvListenCan(); // stop();

    if(_tcan1)
        _tcan1->~SvListenCan();

    delete ui;
}

void MainWindow::log(QString str)
{
    ui->ptextLog->insertPlainText(str);
}

/** ********* CAN 0 ************* **/

void MainWindow::on_bnInit_0_clicked()
{
    if(ui->bnInit_0->isChecked() && init0())
    {
        ui->bnInit_0->setText("Close");
        ui->bnInit_0->setChecked(true);

        ui->bnWrite_0->setEnabled(true);
        ui->editWrite_0->setEnabled(true);

        ui->widgetParams_0->setEnabled(false);

    }
    else if(!ui->bnInit_0->isChecked())
    {
        ui->bnInit_0->setChecked(false);

        ui->bnWrite_0->setEnabled(false);
        ui->editWrite_0->setEnabled(false);

        ui->widgetParams_0->setEnabled(true);

        QApplication::processEvents();

        close0();

        ui->bnInit_0->setText("Init");
    }
    else
        ui->bnInit_0->setChecked(false);

}

bool MainWindow::init0()
{
    QString backend = "socketcan";
    QString ifName = ui->cbCanList_0->currentText();

    char *backendName = (char*)malloc(strlen(backend.toStdString().c_str()));
    strcpy(backendName, backend.toStdString().c_str());

    char *interfaceName = (char*)malloc(strlen(ifName.toStdString().c_str()));
    strcpy(interfaceName, ifName.toStdString().c_str());

    try
    {
        /* 1 create_device */
        canlib::can_err *err = canlib::create_device(backendName, interfaceName);

        if(err->code != canlib::OK) _excpt.raise(err);

        _device_can0 = err->msg;

        delete err;

        /* 2 set_params */
        canlib::can_params p = check_params(0);

        r = canlib::set_params(_device_can0, &p);
        if(r->code != canlib::OK) _excpt.raise(r);

        canlib::free_error(r);

        /* 3 connect */
        r = canlib::connect_device(_device_can0);
        if(r->code != canlib::OK) _excpt.raise(r);

        canlib::free_error(r);

        /* 4 listen */
        _tcan0 = new SvListenCan(_device_can0, 1000);
//        connect(_tcan0, SIGNAL(finished()), _tcan0, SLOT(deleteLater()));
        connect(_tcan0, SIGNAL(new_data(canlib::can_frame*)), this, SLOT(new_data(canlib::can_frame*)));
        _tcan0->start();

        qInfo() << "started listen/write can0";

        return true;
    }

    catch (SvException &e) {
        if(_device_can0) canlib::free_device(_device_can0);
        qCritical() << "Error on init can0: " << QString(e.err->msg);
        QMessageBox::critical(0, "Error", QString(e.err->msg));
        canlib::free_error(e.err);

        return false;
    }
}

void MainWindow::close0()
{
    if(_tcan0)
    {
        _tcan0->stop();
        delete _tcan0;
        _tcan0 = nullptr;
    }


    r = canlib::disconnect_device(_device_can0);
    if(r->code != canlib::OK) {
        qCritical() << "Eror on closing can0:" << QString(r->msg);
        QMessageBox::critical(0, "Error", QString(r->msg));
    }
    canlib::free_error(r);

    canlib::free_device(_device_can0);

    qInfo() << "can0 closed";

}

void MainWindow::on_bnWrite_0_clicked()
{
    if(!_device_can0)
        return;

    canlib::can_frame frame;

    frame.id = 1;
    frame.type = 1;

    QString s = QString("%1%1").arg(ui->editWrite_0->text());
    QByteArray b = QByteArray::fromHex(QByteArray(s.toLatin1()));
    b.truncate(64);

    memcpy(&(frame.data), b.data(), b.length());
    frame.data_length = b.length();

    r = canlib::write_frame(_device_can0, &frame);

    if(r->code == canlib::OK)
        qInfo() << "written to can0:" << b.toHex();

    else {
        qCritical() << "Error on writing to can0:" << QString(r->msg);
        QMessageBox::critical(0, "Error", QString(r->msg));
    }

    canlib::free_error(r);

}

/** ********* CAN 1 ************* **/

void MainWindow::on_bnInit_1_clicked()
{
    if(ui->bnInit_1->isChecked() && init1())
    {
        ui->bnInit_1->setText("Close");

        ui->bnWrite_1->setEnabled(true);
        ui->editWrite_1->setEnabled(true);

        ui->widgetParams_1->setEnabled(false);

    }
    else if(!ui->bnInit_1->isChecked())
    {
        ui->bnWrite_1->setEnabled(false);
        ui->editWrite_1->setEnabled(false);

        ui->widgetParams_1->setEnabled(true);

        QApplication::processEvents();

        close1();

        ui->bnInit_1->setText("Init");
    }
    else {
        ui->bnInit_1->setChecked(false);
    }
}

bool MainWindow::init1()
{
    QString backend = "socketcan";
    QString ifName = ui->cbCanList_1->currentText();

    char *backendName = (char*)malloc(strlen(backend.toStdString().c_str()));
    strcpy(backendName, backend.toStdString().c_str());

    char *interfaceName = (char*)malloc(strlen(ifName.toStdString().c_str()));
    strcpy(interfaceName, ifName.toStdString().c_str());

    try
    {
        /* 1 create_device */
        canlib::can_err *err = canlib::create_device(backendName, interfaceName);

        if(err->code != canlib::OK) _excpt.raise(err);

        _device_can1 = err->msg;

        delete err;

        /* 2 set_params */
        canlib::can_params p = check_params(1);

        r = canlib::set_params(_device_can1, &p);
        if(r->code != canlib::OK) _excpt.raise(r);

        canlib::free_error(r);

        /* 3 connect */
        r = canlib::connect_device(_device_can1);
        if(r->code != canlib::OK) _excpt.raise(r);

        canlib::free_error(r);

        /* 4 listen */
        _tcan1 = new SvListenCan(_device_can1, 1000);
//        connect(_tcan1, SIGNAL(finished()), _tcan1, SLOT(deleteLater()));
        connect(_tcan1, SIGNAL(new_data(canlib::can_frame*)), this, SLOT(new_data(canlib::can_frame*)));
        _tcan1->start();

        qInfo() << "started listen/write can1";

        return true;
    }

    catch (SvException &e) {
        if(_device_can1) canlib::free_device(_device_can1);
        qCritical() << "Error on init can1: " << QString(e.err->msg);
        QMessageBox::critical(0, "Error", QString(e.err->msg));
        canlib::free_error(e.err);

        return false;
    }
}


void MainWindow::close1()
{
    if(_tcan1)
    {
        _tcan1->stop();
        delete _tcan1;
        _tcan1 = nullptr;
    }


    r = canlib::disconnect_device(_device_can1);
    if(r->code != canlib::OK) {
        qCritical() << "Eror on closing can1:" << QString(r->msg);
        QMessageBox::critical(0, "Error", QString(r->msg));
    }
    canlib::free_error(r);

    canlib::free_device(_device_can1);

    qInfo() << "can1 closed";

}

void MainWindow::on_bnWrite_1_clicked()
{
    if(!_device_can1)
        return;

    canlib::can_frame frame; // = new canlib::can_frame;

    frame.id = 1;
    frame.type = 1;

    QByteArray b = QByteArray::fromHex(QByteArray(ui->editWrite_1->text().toLatin1()));
    memcpy(&(frame.data), b.data(), b.length());

    frame.data_length = b.length();

    r = canlib::write_frame(_device_can1, &frame);

    if(r->code == canlib::OK)
        qInfo() << "written to can1:" << b.toHex();

    else {
        qCritical() << "Error on writing to can1:" << QString(r->msg);
        QMessageBox::critical(0, "Error", QString(r->msg));
    }

    canlib::free_error(r);
//    canlib::free_frame(frame);
}


/** ******************************* **/


canlib::can_params MainWindow::check_params(int can_num)
{
    bool ok;
    canlib::can_params params;

    if(can_num == 0)
    {
        uint bitrate = ui->editBitrate_0->text().toInt(&ok);
        if(ui->checkBitrate_0->checkState() != Qt::Checked && ok)
            params.bitrate = bitrate;

        if(ui->checkCANFD_0->checkState() != Qt::PartiallyChecked)
            params.can_fd = ui->checkCANFD_0->isChecked() ? 1 : 0;

        uint error_filter = ui->editErrorFilter_0->text().toInt(&ok);
        if(ui->checkErrorFilter_0->checkState() != Qt::PartiallyChecked && ok)
            params.error_filter = error_filter;

        if(ui->checkExtendedFrame_0->checkState() != Qt::PartiallyChecked)
            params.extended_frame_format = ui->checkExtendedFrame_0->isChecked() ? 1 : 0;

        if(ui->checkFlexRateFormat_0->checkState() != Qt::PartiallyChecked)
            params.flexible_rate_format = ui->checkFlexRateFormat_0->isChecked() ? 1 : 0;

        if(ui->checkLoopback_0->checkState() != Qt::PartiallyChecked)
            params.loopback = ui->checkLoopback_0->isChecked() ? 1 : 0;

        if(ui->checkReceiveOwn_0->checkState() != Qt::PartiallyChecked)
            params.receive_own = ui->checkReceiveOwn_0->isChecked() ? 1 : 0;
    }
    else if (can_num == 1)
    {
        uint bitrate = ui->editBitrate_1->text().toInt(&ok);
        if(ui->checkBitrate_1->checkState() != Qt::Checked && ok)
            params.bitrate = bitrate;

        if(ui->checkCANFD_1->checkState() != Qt::PartiallyChecked)
            params.can_fd = ui->checkCANFD_1->isChecked() ? 1 : 0;

        uint error_filter = ui->editErrorFilter_1->text().toInt(&ok);
        if(ui->checkErrorFilter_1->checkState() != Qt::PartiallyChecked && ok)
            params.error_filter = error_filter;

        if(ui->checkExtendedFrame_1->checkState() != Qt::PartiallyChecked)
            params.extended_frame_format = ui->checkExtendedFrame_1->isChecked() ? 1 : 0;

        if(ui->checkFlexRateFormat_1->checkState() != Qt::PartiallyChecked)
            params.flexible_rate_format = ui->checkFlexRateFormat_1->isChecked() ? 1 : 0;

        if(ui->checkLoopback_1->checkState() != Qt::PartiallyChecked)
            params.loopback = ui->checkLoopback_1->isChecked() ? 1 : 0;

        if(ui->checkReceiveOwn_1->checkState() != Qt::PartiallyChecked)
            params.receive_own = ui->checkReceiveOwn_1->isChecked() ? 1 : 0;
    }

    return params;
}

void MainWindow::new_data(canlib::can_frame* frame)
{
    mutex.lock();
    if(!frame || !frame->data)
        return;

    QByteArray b = QByteArray::fromRawData(frame->data, frame->data_length);

    QString source;
    if(frame->device == _device_can0)
        source = "can0";
    else if(frame->device == _device_can1)
        source = "can1";
    else
        source = "unknown";


    ui->ptextLog->insertPlainText(QString("source %1: type %2: id %3: data %4: data len %5: can %6: len %7\n")
                                       .arg(source)
                                       .arg(frame->type)
                                       .arg(frame->id)
                                       .arg(QString(b.toHex()))
                                       .arg(frame->data_length)
                                  .arg(QString(frame->data_str))
                                  .arg(strlen(frame->data_str)));
    mutex.unlock();

}


/** ******************************* **/
SvListenCan::~SvListenCan()
{
  stop();
  qDebug() << "stoppped";
  deleteLater();
}

void SvListenCan::run()
{
  _started = true;
  _finished = false;

  while(_started)
  {
    mutex.lock();
    canlib::can_frame *frame = canlib::read_frame(_device);
    mutex.unlock();

    if(frame)
      emit new_data(frame);

    msleep(_timeout);

  }

  _finished = true;

}

void SvListenCan::stop()
{
  _started = false;
  while(!_finished) QApplication::processEvents();
}
