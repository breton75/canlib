#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QMessageBox>
#include <QException>

#include "../canlib/canlib.h"

namespace Ui {
class MainWindow;
}

class SvListenCan;
//class SvException;

class SvException: public QException
{
public:
    void raise(canlib::can_err *err) /*const*/ { this->err = err; throw *this; }
    SvException *clone() const { return new SvException(*this); }

    canlib::can_err *err;

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void log(QString str);

private slots:
    void on_bnInit_0_clicked();
    void on_bnWrite_0_clicked();

    void on_bnInit_1_clicked();
    void on_bnWrite_1_clicked();

private:
    Ui::MainWindow *ui;

    SvException _excpt;
    canlib::can_err* r;

    bool init0();
    canlib::can_params check_params(int can_num);
    void close0();


    bool init1();
    void close1();

    char *_device_can0 = nullptr;
    char *_device_can1 = nullptr;

//    unsigned long int _devid = 0;

    SvListenCan *_tcan0 = nullptr;
    SvListenCan *_tcan1 = nullptr;

public slots:
    void new_data(canlib::can_frame* frame);

};


class SvListenCan: public QThread
{
    Q_OBJECT

public:
  explicit SvListenCan(char *device, quint32 timeout)
  {
    _device = device;
    _timeout = timeout;
  }

  ~SvListenCan();

  bool finished();
  void stop();

  canlib::can_frame can_data;


private:
  void run() Q_DECL_OVERRIDE;

  char *_device;
  quint32 _timeout;

  bool _started;
  bool _finished;

signals:
  void new_data(canlib::can_frame *frame);

};

#endif // MAINWINDOW_H
