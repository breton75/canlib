/*
 * Описание библиотеки canlib для работы с устройствами через интерфейс CAN. 
 * Написано в Qt 5.8.0
 * 
 * автор ведущий инженер Свиридов С. А.
 * 
 * описание стандарта CAN [https://ru.wikipedia.org/wiki/Controller_Area_Network]
 * 
 * /

/* все структуры, функции и переменные находятся в пространстве имен canlib */
namespace canlib {

/* код ошибки */
enum {
    ERROR = -1,
    OK = 0
};

/* структура для описания результата функций. 
 * при code == canlib::OK функция завершилась успешно */
struct can_err {
    int code = OK;
    char *msg = nullptr;
};

/* структура для описания параметров работы контроллера CAN */
struct can_params {
    unsigned int error_filter = 0;
    int loopback = -1;          // -1 - unssigned, 0 - false, any other - true
    int receive_own = -1;       // -1 - unssigned, 0 - false, any other - true
    int can_fd = -1;            // -1 - unssigned, 0 - false, any other - true
    unsigned int bitrate = 0;   // 0 - unssigned

    int extended_frame_format = 0; // 0 - false (id is 11 bits number), any other - true (id is 29 bits number)
    int flexible_rate_format = 0;  // 0 - false (max 8 byte per frame), any other - true (max 64 byte per frame)
};

/* тип фрейма */
enum {
  UnknownFrame        = 0x0,
  DataFrame           = 0x1,
  ErrorFrame          = 0x2,
  RemoteRequestFrame  = 0x3,
  InvalidFrame        = 0x4
};

/* структура для описания исходящего или входящего фрейма */
struct can_frame {
  char *device;              // указатель на устройство получатель (только для входящего фрейма)
  unsigned int id;           // идентификатор фрейма
  int type;                  // тип фрейма (data frame, remote frame, error frame)
  char data[64];             // данные
  unsigned int data_length;  // длина данных
  char *data_str;            // данные в текстовом виде (только для входящего фрейма)
  unsigned int microseconds; // время в микросекундах (только для входящего фрейма)
};


/* получить список доступных сетевых интерфейсов (только в Linux).
 * если код ошибки (code) == canlib::OK, то msg содержит список, разделенный символом \n
 * иначе, msg содержит текст описания ошибки */
can_err *get_can_list();


/* создать устройство. создает объект для работы с заданным устройством CAN и возвращает указатель на него
 * передаваемые параметры: backend_name (в обычном случае 'socketcan'), interface_name (в обычном случае 'can0', 'can1')
 * если код ошибки (code) == canlib::OK, то msg содержит указатель на созданный объект
 * иначе, msg содержит текст описания ошибки */
/** примечание. все текстовые параметры типа char* должны заканчиваться символом '\0' **/
can_err *create_device(const char* backend_name, const char* interface_name);

/* установить параметры для устройства по указателю device.
 * если код ошибки (code) == canlib::OK, то параметры установлены успешно.
 * иначе, msg содержит текст описания ошибки */
can_err *set_params(char *device, can_params *params);

/* подключиться к устройству по указателю device 
 * если код ошибки (code) == canlib::OK, то соединение успешно
 * иначе, msg содержит текст описания ошибки */
can_err *connect(char *device);

/* отключиться от устройства по указателю device 
 * если код ошибки (code) == canlib::OK, то успешно отключено
 * иначе, msg содержит текст описания ошибки */
can_err *disconnect(char *device);

/* переподключиться к устройству по указателю device. требуется после изменения параметров.
 * если код ошибки (code) == canlib::OK, то успешно отключено
 * иначе, msg содержит текст описания ошибки */
can_err *reconnect(char *device);

/* записать данные в CAN шину по указателю device.
 * если код ошибки (code) == canlib::OK, то запись успешна
 * иначе, msg содержит текст описания ошибки */
can_err *write(char *device, can_frame *frame);

/* прочитать данные из CAN шины по указателю device.
 * возвращает указатель на полученный фрейм.
 * если указатель равен NULL, значит в буфере данных нет (либо во время выполнения возникли ошибки) */
 can_frame *read(char *device);

/* функции для высвобождения ресурсов */
void free_device(char *device);
void free_error(can_err* error);
void free_frame(can_frame* frame);

}


/* примерная реализация работы с библиотекой */

char *MyDevice = NULL;

void do_init()
{
  err = new canlib::can_err;
  
  char *backend_name = "socketcan";
  char *interface_name = "can0";

  /* создаем устройство */
  canlib::can_err *err = canlib::create_device(backend_name, interface_name);
  if(err->code == canlib::OK) {
    
    MyDevice = err->msg;
    delete err; // здесь освобождаем именно через delete, а не через free_error!
    
    // do something
    
  }
  else {
    ShowMessage("Ошибка создания устройства: " & err->msg);
    canlib::free_error(err);
  }
  
  free(backend_name);
  free(interface_name);
  
}

void do_set_params()
{
  /* устанавливаем параметры */
  canlib::can_params params;
  params.receive_own = 1; // для примера
  
  canlib::can_err *err = canlib::set_params(MyDevice, &params);
  if(err->code == canlib::OK) {
    
    // do something
    
  }
  else
    ShowMessage("Ошибка задания параметров: " & err->msg);
  
  canlib::free_error(err);
  
}

void do_connect()
{
  /* подключаемся */
  canlib::can_err *err = canlib::connect(MyDevice);
  if(err->code == canlib::OK) {
    
    // do something
    
  }
  else
    ShowMessage("Ошибка подключения: " & err->msg);
  
  canlib::free_error(err);
  
}

void do_write()
{
  /* передаем данные */
  canlib::can_frame frame;
  frame.id = 1;
  frame.type = canlib::DataFrame;
  frame.data = {0x1, 0x2, 0x3};
  frame.data_length = 3;
  
  canlib::can_err *err = canlib::write(MyDevice, &frame);
  if(err->code == canlib::OK) {
    
    // do something  
    
  }
  else
    ShowMessage("Ошибка передачи данных: " & err->msg);
      
  canlib::free_error(err);
  
}

void do_read()
{
  /* читаем данные */
  canlib::can_frame *frame = canlib::read(MyDevice);
  if(frame)
  {
    ShowMessage("Получены данные: \n"
              "id: %1\n" 
              "тип: %2\n"
              "длина: %3\n"
              "устройство: %4\n"
              "время: %5\n" 
              "текстовый вид: %6",
              {frame->id, frame->type, frame->data_length, frame->device, frame->microseconds, frame->data_str});
  
    /* обработка данных */
    // frame->data;
  
  }
  else  
    ShowMessage("Ошибка чтения данных");
    
  canlib::free_frame(frame);
  
}

void do_disconnect()
{
  canlib::can_err *err = canlib::disconnect(MyDevice);
  if(err->code == canlib::OK) {
    
    // do something  
    
  }
  else
    ShowMessage("Error" & err->msg);
  
  canlib::free_error(err);

}
 
void do_free_device()
{
  canlib::free_device(MyDevice);
  MyDevice = NULL;
}
