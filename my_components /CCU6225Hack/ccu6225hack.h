#ifndef CCU6225HACK_H
#define CCU6225HACK_H

// \r - 0xD
// \n - 0xA

//#define MY_DEBUG // для отладки протокола

// возможности CCU6225
#define MAX_CONTROLS    23 // максималььное количество контролов
#define MAX_SENSORS      8 // максимальне количество сенсоров
#define MAX_SWITCHS      7 // максмальное количество переключателей (реле+выходы)
#define MAX_LOGIC_LEX   30 // максимальное количество названий состояний реле и датчиков
#define MAX_CONNECTIONS  8 // максимальное количество адресатов(телефонов) - получателей сообщений

// для сборки составных входящих сообщений
#define MAX_SMS_BUFF_SIZE MAX_NUMBER_OCTETS // макимально возможный размер СМС сообщения
#define STOR_SMS_COUNT 20 // максимальное количество буферов входящих СМС
#define PICES_SMS_MAX 14  // максимальное количество возможных кусков
#define REC_MAX_CNT 10    // максимально допустимое количество получателей смс
#define QUE_QUANTITY 80   // глубина очереди исходящих сообщений
#define TIME_CLEAR_STOR_SMS 1000*60*60*4// 3 часа время по прошествию которого не до конца принятое кусочное СМС удаляется из буфера

#define IN_BUFF_SIZE 0x1FF // размеры буферов анализа данных
#define OUT_BUFF_SIZE 0x1FF

// настройка периодов обработки
constexpr uint32_t interchPeriod      = 120000; // период опроса проца при интерчендже между иклами опроса
constexpr uint32_t toProcPeriod       =  15000; // минимальный период отпраки PDU процессору, для опроса датчиков
constexpr uint32_t outSmsPerod        =  40000; // минимальная пауза между исходящими смс
constexpr uint32_t outSmsErrorTimeout =  30000; // таймаут ошибки отпраки исходяих смс 

#define _EXT {.b=nullptr}, ssUndef, 0, 0

union pCntr { 
   esphome::switch_::Switch* w;
   esphome::sensor::Sensor* s;
   esphome::binary_sensor::BinarySensor* b;
   uint32_t raw;
};

enum coType { cNone, cSwitch, cSensor, cBinary, cCall}; // тип объекта

enum swState:uint8_t {ssFree,    // разрешено изменять состояние
                      ssNeedSet, // нужно отправить команду на установку
                      ssWaiteSet,// ждем выполнения команды 
                      ssUndef,   // состояние не установлено
                     };          // флаги обработки данных свитчей от процца 

struct ccu6225cntrl { // структура для обработки контрола
   char* lex;          // лексема 
   pCntr control;    // указатель на объект контрола
   coType type=cNone;        // тип контрола
   pCntr addSens;    // дополнителный логический датчик комплиментарный аналоговому
   swState workstate=ssUndef;// состояние режима обработки
   uint8_t hash=0;           // хеш
   uint8_t frhash=0;         // сл. элемент с таким же хешем
   uint16_t timer=0;         // таймер периода запроса данных в секундах
   uint16_t delay=0;         // период запроса данных в секундах
};

struct ccu6225bool{ // структура для работы с состояниями
   char* lex;        // лексема 
   bool state=false;       // соответствующее значение
   uint8_t hash=0;         // хеш
};

enum  numfrst:uint8_t {nTest, nArm, nDisarm, nAlarm, nBalans, nBatery, nPower, nTtime, nData}; // перечисление особых позиций
enum  numscnd:uint8_t {nsOn, nsOff, nsSms, nsT, nsOK, nsFault, nsOther}; // перечисление особых позиций
uint8_t cntrSize=0;
uint8_t scndSize=0;
char* pass = nullptr;
ccu6225cntrl* cntr=nullptr;
ccu6225bool* scnd=nullptr;
uint8_t sensCnt=0; // счетчик сенсоров для избежания переполнения
uint8_t swCnt=0; // счетчик логических лексем
static char TAG[]="CCU6225_HACKER";
//часы
esphome::time::RealTimeClock* _rtc=nullptr;
// текстовые сенсоры
esphome::text_sensor::TextSensor* _secur=nullptr;   // сенсор кто оперировал постановкой-снятем с охраны 
esphome::text_sensor::TextSensor* _inCont=nullptr;  // текст входящего сообщения
esphome::text_sensor::TextSensor* _outCont=nullptr; // текст исходящего сообщения
esphome::text_sensor::TextSensor* _outAddr=nullptr; // получатель сообщения
esphome::text_sensor::TextSensor* _inAddr=nullptr;  // отправитель сообщения
esphome::text_sensor::TextSensor* _tstamp=nullptr;  // Дата время контролера GSM
esphome::text_sensor::TextSensor* _unkrep=nullptr;  // неизвестные данные протокола
esphome::text_sensor::TextSensor* _timestamp=nullptr;  // Время процессора
esphome::text_sensor::TextSensor* _ttime=nullptr;  // конфигурация ttime 
esphome::text_sensor::TextSensor* _inCall=nullptr;  // входящий звонок от кого
esphome::text_sensor::TextSensor* _outCall=nullptr;  // исходящий звонок кому 
// сенсoры
esphome::sensor::Sensor* _gsmError=nullptr;
esphome::sensor::Sensor* _gsmRssi=nullptr;
esphome::sensor::Sensor* _gsmQual=nullptr;
// сенсoры
esphome::binary_sensor::BinarySensor* _outsmsready_=nullptr;
esphome::binary_sensor::BinarySensor* _senreg=nullptr;
// свичи
esphome::switch_::Switch* _outsms_=nullptr; //разрешение сообщений наружу
esphome::switch_::Switch* _interch_=nullptr; //разрешение опроса проца
bool checkSW=true; // флаг необходимсти проверки изенения состояния свитча
bool clbcSWactive=true; // флаг разрешения работы обратного вызова от свича
uint8_t inter_buff_size=70; // размер буфера обмена с процессором внутренними сообщениями
// буфер данных со стороны SIM300
uint8_t in_buff[IN_BUFF_SIZE+2]; 
uint16_t in_array=0;
uint8_t in_comm_hash=0xA5;
bool inPDU=false; // флаг ожидания пакета PDU
// буфер данных со стороны процессора
uint8_t out_buff[OUT_BUFF_SIZE+2]; 
uint16_t out_array=0;
uint8_t out_comm_hash=0xA5;
// буфер для сообщений внутреннего обмена
char* interBuff=nullptr; // буфер обмена 
uint8_t inter_buff_cnt=0xFF; // ограничитель количества лексем при внутреннем обмене
// БУФЕРА данных для копирования их при перехвате
char cpmsFake[]="\"SM\",0,20,\"SM\",0,20,\"SM\",0,20\n";
//char cpms[]="\"SM\",0,20,\"SM\",0,20,\"SM\",0,20\n____________";
char cregFake[]="0,1\n";
//char creg[]="0,1\n____";
char csq[]= "14,7\n___";
char cclk[]="\"03/01/01,14:50:04+00\"";
char cpinFake[]="READY\n";
//char cpin[]="READY\n__________";
char cband[]="\"EGSM_DCS_MODE\"\n_______________";
char cfun[]="1\n___";
char cmgr[]="0,,0\n______";
char prompt[]="> ";
char cmgs[]="AT+CMGS=000\r\0\0\0\0\0";  
bool cmgsOk=false; // флаг ожидания OK в окончании CMGS 
uint8_t cusd[100]={0};//{0x30,0x2C,0x22,0x04,0x11,0x04,0x30,0x04,0x3B,0x04,0x30,0x04,0x3D,0x04,0x41,0x00,0x3A,0x00,0x94,0x00,0x81,0x00,0x20,0x00,0x20,0x00,0x20,0x04,0x40,0x22,0x2C,0x37,0x32};
uint8_t sizecusd=33;
const char cmgsMask[]=": %02d\0"; // маска формирвания сччетчика смс
const char _ok[]="\r\n\r\nOK\r\n"; // подтверждающий ответ
uint16_t GSMerror=0; // буфер ошибки в работе модуля
uint16_t outSMScounter=0; // счетчик отправленных СМС
bool connectOk=false; // флаг статуса подключения SIM300 к сети 
bool noInSMS=true; // флаг отсутствия входящих смс
bool _hook=false; //флаг включения перехвата обмена данными
bool _hookReady=false; // флаг разрешения включения перехвата (все операции завершены)
bool _hookDelayed=false; // флаг отложенного перехвата
// переменные для обработки отправки сообщения
uint8_t cSMS=0; // счетчик идентификатор мнгосоставных сообщений
bool outSMSneedSend=false; // флаг необходимости отправки сообщения, поднять для запуска процедуры, после установки сообщения и адреса
int16_t PDUerr=0; // размер отправляемого PDU
uint32_t outSMStimer=0; // тамер стадиий отправки смс
uint8_t outSMSerrCnt=0; // счетчик ошибочных отправок, он же индикатор занятости, если больше 0, то занято 
struct piceSMSbuffer { // структура одной записи куска СМС в буфере
   char* pices[PICES_SMS_MAX];
   uint8_t parts=0; // общее количество кусков смс
   uint8_t id=0; // отличтельный номер смс
   uint8_t count=0; // количество принятых кусков
   uint32_t time; // таймштамп приема последенего куска
   char* sender; // отправитель СМС
};
piceSMSbuffer storPieceSMS[STOR_SMS_COUNT]; //массив указателей на куски составного СМС
char def[]=" "; // заглушка текстовых массивов 
enum ciBlock:uint8_t { ciNone=0, // не блокировать сообщения этому адресу во вне 
                       ciInfo, // пропускать только ALARM, ARM, DISARM, блокировать информационные
                       ciInfoState, // пропускать только ALARM 
                       ciArmDisarm, // пропускать только ARM/DISARM
                       ciFull}; // блокировать все сообщения этому адресу во вне 
// список абонентов и их прав на получение смс
struct  sAddrPerm{ // структура получатель смс и его права
   char* addr=nullptr;; // получатель СМС
   ciBlock perm=ciNone; // права получателя
};
sAddrPerm abons[MAX_CONNECTIONS]; // список фильтруемых абонентов с их правами
// переменные для обработки передачи PDU процу
char* _inSMSmsg=def; // буфер входящего сообщения (в сторону проца)
char* _inSMSAddr=def; // буфер адреса отправителя
const char _inSMSHub[]="+70000000000";// адрес СМС центра 79168960438
bool inSMSneedSend=false; // флаг необходимости отправки PDU процу, поднять для запуска процедуры, после установки сообщения и адреса
int16_t PDUsize=0; // размер или ошибка отправляемого процессору PDU
uint32_t inSMSTimer=0; // тамер отправки смс
uint32_t interchangeTimer=0; // таймер формирования запроса внутреннего обмена PDU
uint8_t inSMStryCnt=0; // счетчик попыток отправки PDU процу
// для парсинга лексем
uint8_t globalState=0; // буфер глобальных флагов
#define GL_ALARM  0B00000001
#define GL_TEST   0B00000010
#define GL_ARM    0B00000100
#define GL_DISARM 0B00001000
bool _arm=false; // статус охраны
uint8_t startHash=0;  // стартовый хеш, при расчете хешей
uint8_t toProcHash=0; // хеш запроса отправленного процу (сумма хешей первых лексем)
uint8_t fromProcHash=0; // хеш запроса от проца (сумма хешей первых лексем)
uint8_t testHash=0; // хешь ответа TEST
uint8_t hours=0; // показания времени приходящие от проца
uint8_t mins=0;
bool needTimeSync=true; //флаг необходимсти снхронизаии времени
bool swInited=false; //флаг окончания инициализации свичей

void setDec2(char* buff, uint8_t dec){
    buff[0]=(dec/10)+'0';
    buff[1]=(dec%10)+'0';
}

bool getDec1(char c, uint8_t* dec){
    if(c>='0' && c<='9'){
       *dec=c-'0';
       return true;
    }
    return false;
}

bool getDec2(char* buff, uint8_t* dec){
    uint8_t ret0=0;
    if(getDec1(buff[0], &ret0)){
        uint8_t ret1=0;
        if(getDec1(buff[1], &ret1)){
            *dec=ret0*10+ret1;
            return true;
        }
    }
    return false;
}
// структура одной записи TTIME
struct stTtime{
    uint8_t d=0;
    uint8_t h=0;
    uint8_t m=0;
    uint8_t getStr(char* buff){
       if(d==0) return 0;
       if(d>5) d=5;
       buff[0]=(d%10)+'0';
       buff[1]='.';
       if(h>=24) h=0;
       setDec2(&(buff[2]),h);
       buff[4]=':';
       if(m>=60) m=0;
       setDec2(&(buff[5]),m);
       buff[7]=0;
       return 7;
    };
    uint8_t readStr(char* buff){
       uint8_t _d=0;
       if(getDec1(buff[0],&_d)){
          if(_d>0 && buff[1]=='.'){
             uint8_t _h=0;
             if(getDec2(&buff[2],&_h)){
                if(buff[4]==':'){
                   uint8_t _m=0;
                   if(getDec2(&buff[5],&_m)){  
                      d=_d;
                      h=_h;
                      m=_m;
                      return 7;
                   }                       
                }
             }
          }
       }
       return 0;
    }
};
stTtime dTtime[4]={0}; // массив TTIME

// ОЧЕРЕДЬ ИСХОДЯЩИХ СМС
struct queSMSbuffer { // структура для очереди отправляемых смс
   uint8_t* que_msg={0}; // отправляемый текст
   uint8_t* que_rec={0}; // получатель
   uint8_t que_piece=0; //  количество кусков, номер куска (сообщения попадают в очередь уже нарезанные, при необходимости)
};
uint8_t que_in=0; // указатель на положение для внесения в сообщения в очередь (циклически буфер)
uint8_t que_out=0; // указатель на положение в буфере для отправки сообщения
uint8_t que_cnt=0; // колчество сообщени в очереди
queSMSbuffer queBuff[QUE_QUANTITY]; // очередь сообщений

///////// ОБСЛУЖВАНИЕ ОЧЕРЕДИ ИСХОДЯЩИХ СООБЩЕНИЙ /////////////////////
uint8_t getNext(uint8_t i){
   if(++i>=QUE_QUANTITY) i=0;
   return i;
}

// вносит кусок сообщения в очередь отправки
// отправитель должен быть только один
// size - размер сообщения уже корректирован под одно PDU
// pieace - старшая тетрада - общее количество кусков, младшая - номер куска
bool putNewPieceSMS(uint8_t* msg, uint8_t* rec, uint8_t msg_size, uint8_t rec_size,  uint8_t piece=0){
   if(que_cnt>=QUE_QUANTITY) {
       //ESP_LOGE(TAG,"SMS que busy - ERROR");
       return false; // в очереди нет места
   }
   queBuff[que_in].que_msg=(uint8_t*)malloc(msg_size+1);
   queBuff[que_in].que_rec=(uint8_t*)malloc(rec_size+1);
   if(queBuff[que_in].que_msg==nullptr || queBuff[que_in].que_rec==nullptr){ // не удалось выделить память
      if(queBuff[que_in].que_msg!=nullptr) {
          free(queBuff[que_in].que_msg);
          queBuff[que_in].que_msg=nullptr;
      }
      if(queBuff[que_in].que_rec!=nullptr) {
          free(queBuff[que_in].que_rec);
          queBuff[que_in].que_rec=nullptr;
      }
      ESP_LOGE(TAG,"Mem alloc for SMS que - ERROR");
      return false;
   }
   memcpy(queBuff[que_in].que_rec, rec, rec_size);
   queBuff[que_in].que_rec[rec_size]=0;
   memcpy(queBuff[que_in].que_msg, msg, msg_size);
   queBuff[que_in].que_msg[msg_size]=0;
   queBuff[que_in].que_piece=piece;
   ESP_LOGE(TAG,"SMS add in pos:%d, size:%d, piece:%x, for:%s, msg:%s", que_in, msg_size, queBuff[que_in].que_piece, queBuff[que_in].que_rec, queBuff[que_in].que_msg);
   que_in=getNext(que_in);
   que_cnt++;
   return true; 
}

// удаляет запись из очереди отправки по номеру, ничего не смещает
void delQueSMS(uint8_t que_del){
   if(que_cnt==0) return;
   if(queBuff[que_del].que_msg!=nullptr) free(queBuff[que_del].que_msg); 
   queBuff[que_del].que_msg=nullptr;
   if(queBuff[que_del].que_rec!=nullptr) free(queBuff[que_del].que_rec);
   queBuff[que_del].que_rec=nullptr;
   queBuff[que_del].que_piece=0;
   ESP_LOGE(TAG,"Que SMS deleted from pos: %d",que_del); 
   que_cnt--;
}

// удаляет первое запись из очереди
void delQueSMS(){
    delQueSMS(que_out);
    que_out=getNext(que_out);
}

// удаляет последнее запись из очереди
void delQueSMSend(){
    if(que_cnt==0) return;    
    if(que_in>0){
       que_in--;
    } else {
       que_in=QUE_QUANTITY-1;
    }
    delQueSMS(que_in);
}

void coreSMS(uint8_t* c){ // замена запрещенных символов
   if((*c>='[' && *c<='^') || (*c>='{' && *c<='~')) *c='?';
}

// расчет размера буфера для СМС
uint16_t getFullSize(uint8_t* in){
    uint16_t size=0;
    bool ret=false;
    while(*in!=0){
       if(*in>=0xD0){
          in+=2;
          ret=true; //ЕСТЬ СИМВОЛЫ 16 БИТ
       } else {
          in++;
       }
       size+=2;
    }
    if (ret==false) size/=2;
    return size;
}

// расчет размера куска смс, которое влезет, после обработки в буфер
uint16_t getPiceSize(uint8_t* in, uint16_t destSize){
    bool test=false;
    uint16_t size=0;
    uint16_t store_size=0;
    while(*in!=0){
       if(*in>=0xD0){
          test=true;
          in+=2;
          size+=2;
       } else {
          in++;
          size++;
       }
       if(size<destSize/2){
          store_size=size;
       } else if (test) return store_size;
       if(size>=destSize) return size;
    }
    if (test) return store_size;
    return size;
}

// вносит в очередь исходящее сообщение, при необходимости делит его на части
bool putNewSMS(char* msg_, char* rec_){   
   bool ret=true;
   int16_t size_msg=strlen(msg_);
   int16_t size_rec=strlen(rec_);
   if(size_msg==0 || size_rec<1 || size_msg>1500 || size_rec>100) return false; // ограничим размер смс
   coreSMS((uint8_t*)msg_); // заменяем все не легитимные символы
   for(uint8_t i=1; i<size_msg; i++){
      if(msg_[i-1]<0xD0) coreSMS(((uint8_t*)msg_)+i);
   }
   int16_t needSize = getFullSize((uint8_t*)msg_); // считаемм требуемый размер буфера
   uint8_t* recs[REC_MAX_CNT]={nullptr}; // массив указателей на номера получателей
   uint8_t k=0; // тут будет количество номеров
   uint8_t* msgTest=(uint8_t*)rec_;
   while(msgTest[0]!=0){ // строим список номеров
      uint8_t j=0;
      while(msgTest[j]!=0 && msgTest[j]!=';' && msgTest[j]!='|' && msgTest[j]!=' '){j++;}
      if(j>4){
         recs[k]=(uint8_t*)malloc(j+1);
         if(recs[k]!=nullptr){
            memcpy(recs[k],msgTest,j);
            recs[k][j]=0;
            k++;
         }
      }
      if(msgTest[j]==0) break;
      msgTest+=j+1;
   }
   if(k<1){ // нет получателей
      ret=false;
      goto putNewSMSfin;
   }
   if (needSize<MAX_SMS_BUFF_SIZE){ // сообщение без разбиения на части
      for(uint8_t i=0; i<k; i++){
         if(recs[i]!=nullptr){
            if(putNewPieceSMS((uint8_t*)msg_, recs[i], size_msg, size_rec, 0)==false){
                ret=false;
                goto putNewSMSfin;
            }
            free(recs[i]);
            recs[i]=nullptr;
         }
      }
   } else { // сообщение с разбиением на части
      uint8_t piceSize[PICES_SMS_MAX]={0};
      uint8_t pi_cnt=0;
      msgTest=(uint8_t*)msg_;
      while(pi_cnt<PICES_SMS_MAX){ // количество кусков, тут в сообщении будет заголовок UDH, его размер вычитаем из размера сообщений
          piceSize[pi_cnt]=getPiceSize(msgTest, MAX_SMS_BUFF_SIZE-3);
          if(piceSize[pi_cnt]<=0) break;
          msgTest+=piceSize[pi_cnt++];
      }
      if((pi_cnt*k)>QUE_QUANTITY-que_cnt || pi_cnt>0xF){ // не хватает свободных позиций в очереди или слишком много кусков 
         ret=false;
         goto putNewSMSfin;
      }
      for(uint8_t j=0; j<k; j++){
          if(recs[j]!=nullptr){
             msgTest=(uint8_t*)msg_;
             uint8_t old=que_in;
             for(uint8_t i=1; i<pi_cnt+1; i++){
                uint8_t size=piceSize[i-1];
                if(putNewPieceSMS(msgTest, recs[j], size, size_rec, (pi_cnt<<4)+i)==false){ //в случае неудачи
                   while(old!=que_in) delQueSMSend();  // удалим все , что напихали в очередь для отправки этого сообщения
                   ret=false;
                   goto putNewSMSfin;
                }        
                msgTest+=size;
             }
             free(recs[j]);
             recs[j]=nullptr;
          }
      }
   }
putNewSMSfin:
   for(uint8_t i=0; i<REC_MAX_CNT; i++){ // чистим буфер номеров
      if (recs[i]!=nullptr) free(recs[i]);
   }
   if(_outsmsready_!=nullptr) _outsmsready_->publish_state(ret);
   return ret;
}

bool putNewSMS(std::string msg, std::string rec){
   if(_outsms_!=nullptr  && _outsms_->state==false){
      ESP_LOGE(TAG,"Outbound SMS forbidden");
      if(_outsmsready_!=nullptr) _outsmsready_->publish_state(false);
      return false;
   }
   return putNewSMS(msg.c_str(), rec.c_str());  
}

// выдает очередное запись из очереди
bool getQueSMS(uint8_t** msg, uint8_t** rec, uint8_t* cnt){
   if(que_cnt!=0 && queBuff[que_out].que_msg!=nullptr && queBuff[que_out].que_rec!=nullptr){
      *msg=queBuff[que_out].que_msg;
      *rec=queBuff[que_out].que_rec;
      *cnt=queBuff[que_out].que_piece;
      ESP_LOGE(TAG,"SMS return from pos: %d, piece:%x msg: %s, for: %s", que_out, *cnt, *msg, *rec);
      return true;
   }
   return false;
}
  
// удаляет отправленное сообщение или все куски кусочного
void delQueSMSFull(){
   if(que_cnt==0) return;
   uint8_t* msg;
   uint8_t* rec;
   uint8_t part=queBuff[que_out].que_piece;
   delQueSMS();
   if(part==0) return;
   uint8_t testPart;
   for(uint8_t i=0; i<16 && que_cnt>0; i++){
      testPart=queBuff[que_out].que_piece;
      if(testPart==0 || testPart-part!=1) {
          ESP_LOGE(TAG,"Pices SMS deleted %x, %x", testPart, part );
          return;
      }
      delQueSMS();
      part=testPart;
   }
}

////////////// ОБСЛУЖИВАНИЕ ОЧЕРЕДИ ВХОДЯЩИХ МНОГОКУСОЧНЫХ СООБЩЕНИЙ //////

// очистка буфера сообщенй
void clearPicesBuff(piceSMSbuffer* buff){
    for(uint8_t i=0; i<PICES_SMS_MAX; i++){
       if(buff->pices[i]!=nullptr){
          free(buff->pices[i]);
          buff->pices[i]=nullptr;
       }
    } 
    if(buff->sender!=nullptr) {
       free(buff->sender);
       buff->sender=nullptr;
    }        
    //buff->num=0;
    buff->parts=0;
    buff->count=0;
    buff->time=0;
    buff->id=0;
}

void initPicesBuffer(){
   for(uint8_t i=0; i<STOR_SMS_COUNT; i++){
      clearPicesBuff(&(storPieceSMS[i]));
   }
}

// поиск подходящей записи, при необходимости подготовка
uint8_t findPosPieces(char* sender, uint8_t parts, uint8_t id){
   uint8_t j=0xFF;
   for(uint8_t i=0; i<STOR_SMS_COUNT; i++){
       if(storPieceSMS[i].sender!=nullptr && id==storPieceSMS[i].id && parts==storPieceSMS[i].parts && strcmp(storPieceSMS[i].sender,sender)==0){ // нашли наш буфер
          return i;
       }
   }
   uint32_t time=0;
   uint32_t ml=millis();
   for(uint8_t i=0; i<STOR_SMS_COUNT; i++){
      if(storPieceSMS[i].sender==nullptr) {
         j=i;
         break;
      }
      uint32_t test=ml-storPieceSMS[i].time;
      if(time<test){ // запоминаем буфер с самой старой записью
         j=i;
         time=test;
      }          
   }
   clearPicesBuff(&(storPieceSMS[j]));
   storPieceSMS[j].sender=(char*)malloc(strlen(sender)+1);
   if(storPieceSMS[j].sender==nullptr) return 0xFF;
   strcpy(storPieceSMS[j].sender,sender);
   storPieceSMS[j].parts=parts;
   storPieceSMS[j].id=id;
   return j;
}
 
// очистка замерзших сообщений (многокусочные полученные не полностью
void sanityClearSP(){
    uint32_t time=millis();
    for(uint8_t i=0; i<STOR_SMS_COUNT; i++){
       if(storPieceSMS[i].time != 0 && time-storPieceSMS[i].time > TIME_CLEAR_STOR_SMS){
          ESP_LOGE(TAG,"Time clear pices stor SMS from sender: %s, pos: %d, expected: %d, recieved %d", storPieceSMS[i].sender, i, storPieceSMS[i].parts, storPieceSMS[i].count);
          clearPicesBuff(&(storPieceSMS[i]));
       }
    }
}
 
// весение куска в буфер, возврат try, при наличии всех кyсков
bool setPieces(char* msg, uint8_t i, uint8_t part){
   sanityClearSP();// сначала почистим оередь от зависонов
   part--;
   if(part>=storPieceSMS[i].parts) return false;
   if(storPieceSMS[i].pices[part]==nullptr){
      storPieceSMS[i].pices[part]=(char*)malloc(strlen(msg)+1);
      if(storPieceSMS[i].pices[part]==nullptr) return false;
      strcpy(storPieceSMS[i].pices[part],msg);
      storPieceSMS[i].count++;
   }
   storPieceSMS[i].time=millis();
   return storPieceSMS[i].count==storPieceSMS[i].parts;
}

// восстановление полного смс
char* storePiece(char* msg, char* sender, uint8_t piece, uint8_t parts, uint8_t id){
   uint8_t i=findPosPieces(sender, parts, id);
   if (i==0xFF) return nullptr;
   if (setPieces(msg, i, piece)){ // внесение куска
       uint8_t siz[parts]={0}; // массив размеров кусков
       uint16_t size=0; // общий размер
       for(uint8_t j=0; j<parts; j++){ // заполняем данные о размерах
          siz[j]=strlen(storPieceSMS[i].pices[j]);
          size+=siz[j];
       }
       if(size==0) return nullptr;
       char* ret=(char*)malloc(size+1);
       if(ret==nullptr) return nullptr;
       char* point=ret;
       for(uint8_t j=0; j<parts; j++){
          if(storPieceSMS[i].pices[j]!=nullptr){
             memcpy(point,storPieceSMS[i].pices[j],siz[j]);
             point+=siz[j];          
          }
       }
       *point=0;  
       clearPicesBuff(&(storPieceSMS[i]));
       ESP_LOGD(TAG,"Get full sms (pos:%d) from %s, peaces: %d, %s", i, sender, parts, ret);
       return ret;       
   }
   return nullptr;    
}

////////////////////////// ОБСЛЖИВАНИЕ ХЕШИРОВАНИЯ ЛЕКСЕМ ////////////////
constexpr uint8_t dscrc_table[256]= {
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

// один шаг рассчета CRC
uint8_t crc_update(uint8_t crc, uint8_t in) {
    return dscrc_table[crc ^ in];
}

char getHex(uint8_t d){
   if(d<=9){
      return d+'0';
   } 
   return d-0xA+'A';   
}

bool isNext(char b){
   return (b==' ' || b=='\n' || b=='\r' || b=='%' || b==':');
}

uint8_t getHash( char* str, uint8_t *count){
   if(str[0]==0) return 0;
   uint8_t hash=startHash;
   uint8_t i=0;
   if(startHash==0){
      for(i=i; i<20 && str[i]>' ' && str[i]!=':' ; i++){
         hash +=str[i];
      }
       hash += i;
   } else {
      for(i=i; i<20 && str[i]>' ' && str[i]!=':' ; i++){
         hash = crc_update(hash,str[i]);
      }
       hash = crc_update(hash,i);
   }
   *count=i;
   return hash;
}

uint8_t getHash(char* str){
   uint8_t temp=0;
   return getHash(str, &temp);
}

char* printHexs(uint8_t* buf, uint16_t size){
   static char test[520];
   uint16_t j=0;
   for(uint16_t i=0; i<size; i++){
      test[j++]=getHex(buf[i]>>4);      
      test[j++]=getHex(buf[i]&0xF); 
      test[j++]=';';     
   }
   test[j]=0;
   return test;
}

bool calcHash(){
   uint8_t test[cntrSize+scndSize]={0};
   for(uint8_t i=0; i<cntrSize; i++){
      cntr[i].hash=getHash((char*)cntr[i].lex);
      for(uint8_t j=0; j<i; j++){
         if(cntr[j].hash==cntr[i].hash){ // совпали два хеша
            if(strcmp(cntr[j].lex,cntr[i].lex)!=0){ // а две лексемы не совпали
               ESP_LOGD(TAG,"Coincidence first lexems hash %d=%d, %x",i,j,cntr[i].hash);
               return false; // нужно перестроть таблицу хешей
            } else { // если лексемы совпали свяжем их
               if(cntr[j].frhash==0) cntr[j].frhash=i; // ссылка лексему с таким же хешем
            }
         }
      }
      test[i]=cntr[i].hash; // для дальнейшего аналза хешей вспомогательных лексем      
   }
   for(uint8_t i=0; i<scndSize; i++){ // считаем хеши вторых лексем (параметров)
      scnd[i].hash=getHash((char*)scnd[i].lex);
      test[i+cntrSize]=scnd[i].hash;
      for(uint8_t j=0; j<i+cntrSize; j++){
         if(test[j]==scnd[i].hash){ // свпали два хеша
            ESP_LOGD(TAG,"Coincidence second lexems hash %d=%d, %x",i,j,scnd[i].hash);
            return false; // нужно перестроть таблиу хешей
         }
      }
   }       
   return true;   
}

bool hashSetup(){ // заполняем таблиы хешей
   for(startHash=0; startHash<0xFF ; startHash++){
      if(calcHash()){ return true;}
   }
   return false;
}

bool findLex(const char* l, coType t, uint8_t start=0){
   for(uint8_t i=start; i<cntrSize; i++){
      if(strcmp(cntr[i].lex,l)==0 && cntr[i].type==t) return true;   
   }
   return false;
}

// конффигурировние параметров интерченьджа
void setInterChangeParams(uint8_t size_, uint8_t cnt_=0xFF){
    if(size_<30){
       size_=30;
    } 
    inter_buff_size=size_;
    if(cnt_<1){
       cnt_=1;
    } 
    inter_buff_cnt=cnt_;
    if(interBuff!=nullptr) free(interBuff);
    interBuff = (char*)malloc(inter_buff_size+3);
    if(interBuff==nullptr){
       while(1){
           delay(10000);
           ESP_LOGE(TAG,"Fatal ERROR, insufficient memory!!");
       }
    }
}

// добавление сенсора
bool addSensor(esphome::sensor::Sensor* sen, const char* name, esphome::binary_sensor::BinarySensor* addSen=nullptr, uint16_t delay=0){
    if(sensCnt<MAX_SENSORS && findLex(name,cSensor)==false){ 
       cntr[cntrSize].lex = (char*)malloc(strlen(name)+1); 
       if(cntr[cntrSize].lex!=nullptr){
          strcpy(cntr[cntrSize].lex,(char*)name);
          cntr[cntrSize].control.s=sen;
          cntr[cntrSize].type=cSensor;
          cntr[cntrSize].delay=delay;
          cntr[cntrSize].timer=0;
          if(addSen!=nullptr){
             //addSen->publish_state(addSen->state);
             cntr[cntrSize].addSens.b=addSen;
          }
          cntrSize++;
          sensCnt++;
          ESP_LOGE("","%s - %d", name, cntrSize);
          return true;
       }
    }
    ESP_LOGE(TAG,"Error add sensor %s", name);
    return false;
}
bool addSensor(esphome::sensor::Sensor* sen, const char* name, uint16_t delay=0){
    return addSensor(sen, name, nullptr, delay);
}
// добавление бинарного сенсора
bool addBinarySensor(esphome::binary_sensor::BinarySensor* sen, const char* name, uint16_t delay=0){
    if(sensCnt<MAX_SENSORS && findLex(name,cBinary)==false){ // колчество сенсоров превышено
       cntr[cntrSize].lex = (char*)malloc(strlen(name)+1);  
       if(cntr[cntrSize].lex!=nullptr){
          strcpy(cntr[cntrSize].lex,(char*)name);
          //sen->publish_state(sen->state);
          cntr[cntrSize].control.b=sen;
          cntr[cntrSize].type=cBinary;
          cntr[cntrSize].delay=delay;
          cntr[cntrSize].timer=0;
          cntrSize++;
          sensCnt++;
          return true;
       }
    }
    ESP_LOGE(TAG,"Error add binary sensor %s", name);
    return false;
}
// подключение колбэка на свичи
void setSwClbc(esphome::switch_::Switch* sw){
   sw->add_on_state_callback([sw](bool state){
      if(clbcSWactive){ // если разрешено
         for(uint8_t i=0; i<cntrSize; i++){
             if(cntr[i].type==cSwitch && cntr[i].control.w==sw){ // найти инициатора колбэка
                checkSW=true; // поднять флаг необходимости обработки свитчей
                cntr[i].workstate=ssNeedSet; // поднять флаг необходимости установки
                //ESP_LOGE("","%s",cntr[i].lex);
                return;
             }
         }
      }
  }); 
}
// добавление переключателя
bool addSwitch(esphome::switch_::Switch* sw, const char* name, uint16_t delay=0){
    if(swCnt<MAX_SWITCHS && findLex(name,cSwitch)==false){; // колчество переключателей превышено
       cntr[cntrSize].lex = (char*)malloc(strlen(name)+1);  
       if(cntr[cntrSize].lex!=nullptr){
          strcpy(cntr[cntrSize].lex,(char*)name);
          setSwClbc(sw); // подключить обратный вызов
          cntr[cntrSize].control.w=sw;
          cntr[cntrSize].type=cSwitch;
          cntr[cntrSize].delay=delay;
          cntr[cntrSize].timer=0;
          cntrSize++;
          swCnt++;
          return true;
       }
    }
    ESP_LOGE(TAG,"Error add switch %s", name);
    return false;
}
// установка сенсоров
void setSecur(esphome::text_sensor::TextSensor* sen){_secur=sen;}
void setInCont(esphome::text_sensor::TextSensor* sen){_inCont=sen;}
void setOutCont(esphome::text_sensor::TextSensor* sen){_outCont=sen;}
void setOutAddr(esphome::text_sensor::TextSensor* sen){_outAddr=sen;}
void setInAddr(esphome::text_sensor::TextSensor* sen){_inAddr=sen;}
void setTstamp(esphome::text_sensor::TextSensor* sen){_tstamp=sen;}
void setTime(esphome::text_sensor::TextSensor* sen){_timestamp=sen;}
void setTtime(esphome::text_sensor::TextSensor* sen){_ttime=sen;} // сенсор конфигурационной строки TTIME
void setInCall(esphome::text_sensor::TextSensor* sen){_inCall=sen;} // сенсор входящий звонок от кого
void setOutCall(esphome::text_sensor::TextSensor* sen){_outCall=sen;} // сенсор исходящий звонок кому 
void setUnkrep(esphome::text_sensor::TextSensor* sen){_unkrep=sen;}
void setGsmError(esphome::sensor::Sensor* sen){_gsmError=sen;} //GSM ERRR
void setgsmRssi(esphome::sensor::Sensor* sen){_gsmRssi=sen;} //RSSI
void setgsmQual(esphome::sensor::Sensor* sen){_gsmQual=sen;} //возможные потери
void setOutsmsready(esphome::binary_sensor::BinarySensor* sen){_outsmsready_=sen; sen->publish_state(true);} //GSM ERRR
void setSenreg(esphome::binary_sensor::BinarySensor* sen){_senreg=sen; sen->publish_state(false);} //GSM ERRR
// установка свитчей
void setOutsms(esphome::switch_::Switch* sw){_outsms_=sw;}
void setInterch(esphome::switch_::Switch* sw){_interch_=sw;}
// установка RTC
void setRTC(esphome::time::RealTimeClock* rtc){_rtc=rtc;}

// добавление названия логического состояния
bool addBoolLexema(const char* name, bool bl){
    // поиск повторений
    for(uint8_t i=0; i<scndSize; i++){
       if(strcmp(name,scnd[i].lex)==0){
          return (bl==scnd[i].state);
       }
    }
    if(scndSize>=MAX_LOGIC_LEX-1) return false; // колчество логических лексем превышено
    scnd[scndSize].lex = (char*)malloc(strlen(name)+1);  
    if(scnd[scndSize].lex==nullptr) return false;
    strcpy(scnd[scndSize].lex,(char*)name);
    scnd[scndSize].state=bl;
    scndSize++;
    return false;
}
// установка массива буков копированем
bool setStr(char** dest, const char* sour) {
   if(*dest!=nullptr) free(dest);
   *dest = (char*)malloc(strlen(sour)+1);
   if(*dest==nullptr) return false;
   strcpy(*dest,sour);
   return true;
}

void printL(){
   for(uint8_t i=0; i<cntrSize; i++){
       ESP_LOGE("","%s",cntr[i].lex);
   }
}

void printH(){
   for(uint8_t i=0; i<scndSize; i++){
       ESP_LOGE("","%s",scnd[i].lex);
   }
}

// внесение абонента в список с установкой его прав
uint8_t addAddrPerm(char* addr, ciBlock perm){ // сначала ищем номер в списке
   uint8_t i=0;
   for(i; i<MAX_CONNECTIONS; i++){
      if(abons[i].addr==nullptr) {
         size_t len=strlen(addr);
         abons[i].addr=(char*)malloc(len+1);
         if(abons[i].addr!=nullptr){ // внесли новый
            strcpy(abons[i].addr,addr);
         }
         break;
      }
      if(strcmp(abons[i].addr,addr)==0){ // нашли
         break;         
      }
   }
   if(i<MAX_CONNECTIONS){
      abons[i].perm=perm;
      return i;      
   }
   return 0xFF; // неудача   
}
 
// поиск прав абонента в базе
ciBlock findAddrPerm(char* addr){
   uint8_t i=0;
   for(i; i<MAX_CONNECTIONS; i++){
      if(abons[i].addr==nullptr){
         return ciNone; // не нашли    
      } else if(strcmp(abons[i].addr,addr)==0){ // нашли
         return abons[i].perm;        
      }
   }
   return ciNone; // не нашли   
}    
 
// получить статус блокировки для сообщения для этого номера
bool getPerm(char* addr){
   ciBlock perm=findAddrPerm(addr);
   if(perm==ciFull) {
      ESP_LOGD(TAG,"All SMS for %s blocked", addr);
      return true; // блокируются все исходящие
   }
   if((perm==ciArmDisarm) && (((globalState & (GL_ARM | GL_DISARM))==0) || ((globalState & GL_TEST)!=0))){ // пропускаются только ARM/DISARM
      ESP_LOGE(TAG,"Info+Alarm SMS for %s blocked", addr);
      return true; 
   }
   if((perm==ciInfo) && (!(((globalState & GL_ALARM)!=0) || (((globalState & (GL_ARM | GL_DISARM))!=0) && ((globalState & GL_TEST)==0))))){ // блокируются только информационные (пропускася  ALARM, ARM, DISARM) 
      ESP_LOGE(TAG,"Info SMS for %s blocked", addr);
      return true; // блокируются все информационные
   }
   if((perm==ciInfoState) && ((globalState & GL_ALARM)==0)){ // пропускаются только ALARM
      ESP_LOGE(TAG,"Info+Arm SMS for %s blocked", addr);
      return true; 
   }
   return false; // остальные сообщения для этого номера пропускаем
}

// начало инициализации
// MainSender - номер телефона от имени которго ведется внутренний обмен данными
// pas - пароль для обмена данными с CCU6225
// blockMainSender - блокировать отправку сообщений во вне, идущх на номер MainSender, сiFull - все сообщения, сiInfo - только нормационные
void ccu6225setupStart(const char* MainSender, const char* pas, ciBlock blockMainSender=ciNone){ 
   //иниалзация массивов
   static char c0[]="TEST";
   static char c1[]="ARM";
   static char c2[]="DISARM";
   static char c3[]="ALARM";
   static char c4[]="BALANS";
   static char c5[]="BATTERY";
   static char c6[]="POWER";
   static char c7[]="TTIME";
   static ccu6225cntrl cntrtmp[MAX_CONTROLS+1]={
                     { c0 , {.w=nullptr}   , cCall,  _EXT}, 
                     { c1 , {.w=arm_}      , cSwitch,_EXT}, 
                     { c2 , {.w=nullptr}   , cNone,  _EXT}, 
                     { c3 , {.b=nullptr}   , cBinary,_EXT}, 
                     { c4,  {.s=balans_}   , cSensor,_EXT}, 
                     { c5 , {.s=battery_}  , cSensor,_EXT}, 
                     { c6 , {.b=power_}    , cBinary,_EXT},
                     { c7 , {.w=ttime_}    , cSwitch,_EXT},
   };
   cntr=cntrtmp;
   cntrSize=nData;
   setStr(&pass, pas);
   addAddrPerm((char*)MainSender, blockMainSender); // первый номер в списке разрешений - основной
   if(cntr[nArm].control.w!=nullptr) setSwClbc(cntr[nArm].control.w); // обратный вызов от свичей
   if(cntr[nTtime].control.w!=nullptr) setSwClbc(cntr[nTtime].control.w); // обратный вызов от свичей
   static char l0[]="ON";
   static char l1[]="OFF";
   static char l2[]="SMS";
   static char l3[]="T";
   static char l4[]="OK";
   static char l5[]="FAULT";
   static ccu6225bool scndtmp[MAX_LOGIC_LEX+1] ={
                     { l0 ,  true , 0},
                     { l1 , false , 0},
                     { l2 ,  true , 0},
                     { l3 , false , 0},
                     { l4 , false , 0},
                     { l5 ,  true , 0},
   };
   scnd=scndtmp;
   scndSize=nsOther;
}

void ccu6225setupEnd(){ 
   // баланс  пароль
   if(abons[0].addr==nullptr){
       abons[0].addr=def;
       ESP_LOGE(TAG,"Fatal ERROR, Main Sender not set");
   }
   if(pass==nullptr) {
       pass=def;
       ESP_LOGE(TAG,"Fatal ERROR, Password not set");
   }
   if(cntr[nBalans].lex==nullptr) {
       cntr[nBalans].lex=def;
       ESP_LOGE(TAG,"Fatal ERROR, BALANCE not set");
   }
   // вычсление хешей для поиска
   if(hashSetup()==false){
      ESP_LOGE(TAG,"Fatal ERROR, HASH not unique");
   }
   // инициализвия буфера кусков входящих сообщений
   initPicesBuffer();
   if(_senreg!=nullptr) _senreg->publish_state(false);
   testHash=cntr[nPower].hash+cntr[nBatery].hash+cntr[nBalans].hash+cntr[nTtime].hash;
   // создаем буфер внутреннего обмена
   setInterChangeParams(inter_buff_size, inter_buff_cnt);
}

bool isDigit(char b){
   return (b>='0' && b<='9');
}

uint8_t dig(char b){
   return b-'0';
}
 
// корректировка для вывода в лог, убираем \r \n
char* coreForLog(char* in){
   static char showBuff[generalWorkBuffLength+1];
   for(uint16_t i=0; i<generalWorkBuffLength; i++){ // подготовка строки для публикации в сенсор
      if(in[i]=='\r' || in[i]=='\n'){ // замена служебных символов для вывода в текcтовые сенсоры
         showBuff[i]=' ';
      } else {
         showBuff[i]=in[i];
         if(in[i]==0) break;
      }
   }                 
   return showBuff;
}

// попытка считать цифру float
bool readFloat(char* str, uint8_t* arrow, float* buf){
    uint8_t i=*arrow;
    int8_t mn=1;
    if(str[i]=='-'){
        mn=-1;
        i++;
    } else if(str[i]=='+'){
        i++;
    }
    char b=str[i++];
    if(isDigit(b)){
       bool mant=false;
       float div=10;
       (*buf)=dig(b);
       for(i; i<255; i++){
          b=str[i];
          if(isDigit(b)){
             if(mant==false){
                (*buf)=(*buf)*10+dig(b);
             } else {
                (*buf)+=((float)dig(b))/div;
                div*=10;
             }
          } else if(b=='.' || b==','){
             mant=true;
          } else if(isNext(b)){
             *arrow=i;
             (*buf)*=mn;
             return true;
          }              
       }
    } 
    return false;
}    

// попытка считать время
bool readTime(char* str, uint8_t* arrow, uint8_t* hours, uint8_t* mins){
    uint8_t i=*arrow;
    if(isDigit(str[i])){
       (*hours)=dig(str[i++])*10;
       if(isDigit(str[i])){
          (*hours)+=dig(str[i++]);
          if(str[i++]==':'){
             if(isDigit(str[i])){
                (*mins)=dig(str[i++])*10;
                if(isDigit(str[i])){
                   (*mins)+=dig(str[i++]);
                   if(isNext(str[i])){
                      *arrow=i;
                      return true;                      
                   }
                }
             }
          }
       }
    }
    return false;
}
    
bool readStr(char* str, uint8_t* arrow, char* outStr, uint8_t bu){
    uint8_t j=0;
    outStr[0]=0;
    for(uint8_t i=*arrow; i<255; i++){
       if(isNext(str[i]) || str[i]==0){
          if(j==0) break;
          outStr[j]=0;
          *arrow=i;
          return true;
       } else if(str[i]>' '){
          if(j<bu-1){
             outStr[j++]=str[i]; 
          }
       }
    }
    return false;
}    

bool readFirstToken(char* str, uint8_t* arrow, uint8_t* number){
   uint8_t count=0;
   uint8_t hash=getHash(&(str[*arrow]), &count);
   // поиск хеша в 1 таблице
   for(uint8_t i=0; i<cntrSize; i++){
      if(cntr[i].hash==hash){
         (*arrow)+=count;
         *number=i;
         return true;
      }
   } 
   return false;   
}

bool readSecondToken(char* str, uint8_t* arrow, uint8_t* number){
   uint8_t count=0;
   uint8_t hash=getHash(&(str[*arrow]), &count);
   // поиск хеша в 1 таблице
   for(uint8_t i=0; i< scndSize; i++){
      if(scnd[i].hash==hash){
         (*arrow)+=count;
         *number=i;
         return true;
      }
   } 
   return false;   
}

enum nLex { nlspace, nlline, nlend, nlerr };
nLex nextLex(char* str, uint8_t* arrow){
   nLex ret=nlspace;
   uint8_t i=*arrow;
   for(i; i<255; i++){
      char b=str[i];
      if(b=='\n' || b=='\r'){
         ret=nlline;
      } else if(b==' ' || b=='%' || b==':'){
         //...          
      } else if(b>' '){
         *arrow=i;
         return ret;
      } else if(b==0){
         *arrow=i;
         return nlend; 
      } else {
         break;
      }
   }
   return nlerr;
}    

bool readParam(char* str, uint8_t* arrow, bool* param){
   uint8_t i=*arrow;
   if(!isNext(str[i+1]) && str[i+1]!=0){
      return false;
   }
   if(str[i]=='+'){
       *param=false;
   } else if(str[i]=='-'){
       *param=true;
   } else {
       return false;
   }
   (*arrow)++;
   return true;
}
         
// установка данных в цифровые и комплементарные им, логические сенсоры
bool setValSens(uint8_t fr, float fl, bool alarm, char* ch){
   //ESP_LOGE("","setValSens frst:%d, fl:%f, alarm:%d, ch:%s",frst,fl,alarm,ch);
   uint8_t frst=fr;
   alarm == alarm || ((globalState & GL_ALARM)!=0);   // если сообщение передается как аварийное
   for(uint8_t i=0; i<5; i++){
      if(frst>=cntrSize) goto setValSensErr;
      if(cntr[frst].type==cSensor && cntr[frst].control.s!=nullptr){
         if(cntr[frst].control.s->state!=fl || cntr[frst].workstate==ssUndef) cntr[frst].control.s->publish_state(fl);
         if(cntr[frst].addSens.b!=nullptr){ // есть дополнительный, логический сенсор
            if(cntr[frst].addSens.b->state!=alarm || cntr[frst].workstate==ssUndef) cntr[frst].addSens.b->publish_state(alarm); // установить туда значение
         }
         cntr[frst].workstate=ssFree;
         return true;
      } else {
         if(cntr[frst].frhash!=0){ // есть элементы с таким же хешем
            frst=cntr[frst].frhash; // проверим их
         } else {
            goto setValSensErr;
         }
      }
   }
 setValSensErr:  
   ESP_LOGV(TAG,"Unexpected value sensor %s, ERROR", (fr<scndSize) ? cntr[fr].lex : "NA");
   return false;
}
//компляция строки настрoйки TTIME
uint8_t setTtimeCreate(char* buff, uint8_t size, stTtime* dTtime){
    uint8_t arrow=0;
    for(uint8_t i=0; i<4; i++){
       uint8_t siz=dTtime[i].getStr(&buff[arrow]);
       if(siz==0) break;
       arrow+=siz;
       buff[arrow++]=' ';
    }           
    arrow--;
    buff[arrow]=0;
    return arrow;
}
// публикация строки настроки TTIME в сенсоре
void pubTtime(){
   if(_ttime!=nullptr) { // публковать в сенсоре
      char buff[33]={0};
      setTtimeCreate(buff, sizeof(buff), dTtime);
      if(strcmp(_ttime->state.c_str(), buff)!=0){
         _ttime->publish_state(buff);
      }
   }
}
// удаление одной записи TTIME, по умолчанию из первой ячейки
bool delTtime(uint8_t i=0){
   if(i<4){
      for(uint8_t j=i+1; j<4; j++){
         memcpy((void*)&dTtime[j-1],(void*)&dTtime[j],sizeof(stTtime));
      }  
      dTtime[3].d=0;
      dTtime[3].h=0;
      dTtime[3].m=0;
      if(cntr[nTtime].control.w!=nullptr) cntr[nTtime].control.w->publish_state(dTtime[0].d!=0);
      pubTtime();
      return true;
   }
   return false;
}
// добавление одной записи TTIME в первую ячейку, со сдвигомм вправо остальных
bool addTtime(uint8_t d, uint8_t h, uint8_t m){
   if(d>5 || h>23 || m>59) return false;
   stTtime test={d,h,m};
   for(uint8_t i=0; i<4; i++){
      if(memcmp((void*)&test,(void*)&dTtime[i],sizeof(stTtime))==0) return false;    
   }
   for(uint8_t j=3; j>0; j--){
      memcpy((void*)&dTtime[j],(void*)&dTtime[j-1],sizeof(stTtime));
   }  
   memcpy((void*)&dTtime[0],(void*)&test,sizeof(stTtime));
   if(cntr[nTtime].control.w!=nullptr) cntr[nTtime].control.w->publish_state(true); 
   pubTtime();
   return true;
}
// устанвка переключателя ttme
bool setOtherSw(uint8_t frst, char* ch, uint8_t* arrow){
   //ESP_LOGE("","setOtherSw frst:%d, ch:%s",frst,&ch[*arrow]);
   if(frst==nTtime){// свитч исключение
      //ESP_LOGE("","SW TTME REPLY %s", &ch[*arrow]);
      if(cntr[nTtime].control.w!=nullptr && cntr[nTtime].type==cSwitch){ // если есть свитч 
         stTtime dTimeTest[4]={0};
         uint8_t i;
         uint8_t k=*arrow;
         while(ch[k]>' '){
            if(dTimeTest[i].readStr(&ch[k])!=7) break;
            k+=7;
            i++;
            if(ch[k]!=' ') break;
            k++;
         }
         if(i>0){ // в строке есть правильные данные
            *arrow=k;
            bool state=memcmp(dTimeTest,dTtime,sizeof(dTtime))==0; 
            if(cntr[nTtime].workstate==ssFree || cntr[nTtime].workstate==ssUndef){ // не заблокирован
               if(cntr[nTtime].control.w->state!=true){
                   clbcSWactive=false; // отключаем колбэк
                   cntr[nTtime].control.w->publish_state(true); // установить значение переключателя
                   clbcSWactive=true; // включаем колбэк
               }
               cntr[nTtime].workstate=ssFree;
               if(state==false) memcpy(dTtime,dTimeTest,sizeof(dTtime)); // обновить текущее состояние настройки
               pubTtime();
               //ESP_LOGE("","TTIME NEW SET");
            } else { // заблокирован
               if(cntr[nTtime].workstate==ssWaiteSet){ // это ответ на команду
                  if(state && cntr[nTtime].control.w->state==true) { // данные и состояние переключателя совпадают
                     cntr[nTtime].workstate=ssFree; // просто разрешаем изменения в дальнешем 
                     pubTtime();                     
                  } else { // ответ не правильный
                     cntr[nTtime].workstate=ssNeedSet; // отправим команду еще раз
                     checkSW=true; // поднимаем флаг необходимости проверки и отправки сообщений
                  }
               } else {
                  checkSW=true; // поднимаем флаг необходимости проверки и отправки сообщений
                  //ESP_LOGE("","TTIME NEXT SET");
               }                   
            }
            return true;
         }             
      }
   }
   return false;
}
// установка данных в глобальные сенсоры
bool setGlobSens(uint8_t frst, char* ch){
  // ESP_LOGE("","setGlobSens frst:%d, ch:%s",frst,ch);
   if(frst>=nData) return false;
   if(frst==nAlarm){ 
       globalState = GL_ALARM;
   } else if(frst==nTest){ //TEST просто пропускаем 
       globalState = GL_TEST;
   } else if(frst==nArm || frst==nDisarm){ // arm/disarm
       _arm=(frst==nArm); // состояние этого свитча полученное от проца
       if((globalState & GL_TEST) == 0) { // не тестовое сообщение
          globalState |= _arm ? GL_ARM : GL_DISARM;
          ESP_LOGE("","SET ARM/DISARM");
       }
       if(cntr[nArm].control.w!=nullptr && cntr[nArm].type==cSwitch){ // если есть свитч 
          if(cntr[nArm].workstate==ssUndef || cntr[nArm].workstate==ssFree){ // отображаемый переключатель не инициализирован и нет команды процесса установки
             if(cntr[nArm].control.w->state!=_arm) {
                 clbcSWactive=false; // отключаем обратный вызов, что бы не сформировалась команда установки
                 cntr[nArm].control.w->publish_state(_arm); // установить состояне о котором сказал проц
                 clbcSWactive=true; // вернуть хук на измменение состояние переключателя
                 if(_secur!=nullptr){
                   if(ch!=0){ // кто поставил на охрану
                      _secur->publish_state(ch);
                   } else {
                      char ch[]="Unknown";
                      _secur->publish_state((char*)ch);
                   }
                 }
             }    
             cntr[nArm].workstate=ssFree;
          } else { // установка состояния в процессе обработки
             if(cntr[nArm].workstate==ssWaiteSet){ // это ответ на команду
                if(cntr[nArm].control.w->state==_arm){ // цель достикли переключатель в правильном состооянии
                    cntr[nArm].workstate=ssFree; // просто разрешаем изменения в дальнешем   
                    if(_secur!=nullptr){ // тут оператор - HA
                      char ha[]="From HA";
                      _secur->publish_state(ha);
                    }
                } else { // ответ не правильный 
                    cntr[nArm].workstate=ssNeedSet; // отправим команду еще раз
                    checkSW=true; // поднимаем флаг необходимости проверки и отправки сообщений
                }
             } else { // тут стоит запрос на отправку команды
                checkSW=true; // поднимаем флаг необходимости проверки свичей и отправки сообщений
             }
          }
       }
   } else {
       ESP_LOGV(TAG,"Unexpected global logic sensor %s, ERROR", (frst<scndSize) ? cntr[frst].lex : "NA");
       return false;
   }
   fromProcHash=0;// это глобальные заголовки, они идут в начале сообщения потому их не хешируем, а раз это начало смс, то даже сбрасываем
   return true;
}

// установка данных в переключатели
bool setSwitch(uint8_t fr, uint8_t secn){
    uint8_t frst=fr;
    //ESP_LOGE("","setSwitch frst:%d, secn:%d",frst,secn);
    if(secn==nsOn || secn==nsOff){ // значения второго параметра свойственно переключателям
        //ESP_LOGE(""," frst:%d, secn:%d",frst,secn);
        for(uint8_t i=0; i<5; i++){
           if(frst>=cntrSize) goto setSwErr;
           if(cntr[frst].type==cSwitch && cntr[frst].control.w!=nullptr){ // это переключатель и он есть
              bool procState = (secn==nsOn); // состояние этого свитча полученное от проца
              if(cntr[frst].workstate==ssUndef || cntr[frst].workstate==ssFree){ // отображаемый переключатель не инициализирован и нет команды процесса установки
                 if(cntr[frst].control.w->state!=procState){
                     clbcSWactive=false; // отключаем обратный вызов, что бы не сформировалась команда установки
                     cntr[frst].control.w->publish_state(procState); // установить состояне о котором сказал проц
                     clbcSWactive=true; // вернуть хук на измменение состояние переключателя
                 }
                 cntr[frst].workstate=ssFree;
              } else { // установка состояния в процессе обработки
                  if(cntr[frst].workstate==ssWaiteSet){ // это ответ на команду
                    if(cntr[frst].control.w->state==procState){ // цель достикли переключатель в правильном состооянии
                       cntr[frst].workstate=ssFree; // просто разрешаем изменения в дальнешем   
                    } else { // ответ не правильный 
                       cntr[frst].workstate=ssNeedSet; // отправим команду еще раз
                       checkSW=true; // поднимаем флаг необходимости проверки и отправки сообщений
                    }                        
                 } else { // тут стоит запрос на отправку команды
                     checkSW=true; // поднимаем флаг необходимости проверки и отправки сообщений
                 }
              }
              return true;
           } else {
              if(cntr[frst].frhash!=0){ // есть элементы с таким же хешем
                 frst=cntr[frst].frhash; // проверим их
              } else {
                 goto setSwErr;
              }
           }
        }
    }
  setSwErr:
    ESP_LOGV(TAG,"Unexpected for switch state %s ERROR", (fr<scndSize) ? cntr[fr].lex : "NA");
    return false;
}

// установка данных в логические сенсоры
bool setLogicSens(uint8_t frst, bool alarm){
   //ESP_LOGE("","setLogicSens frst:%d, alarm:%d",frst,(uint8_t)alarm);
   for(uint8_t i=0; i<5; i++){
      if(frst>=cntrSize) goto setBinErr;
      if(cntr[frst].type==cBinary && cntr[frst].control.b!=nullptr){
         if(cntr[frst].control.b->state!=alarm || cntr[frst].workstate==ssUndef) {
             cntr[frst].control.b->publish_state(alarm);
             cntr[frst].workstate=ssFree;
         }
         return true;
      } else {
         if(cntr[frst].frhash!=0){ // есть элементы с таким же хешем
            frst=cntr[frst].frhash; // проверим их
         } else {
            goto setBinErr;
         }
      }
   }
 setBinErr:  
   ESP_LOGV(TAG,"Unexpected logic sensor %s, ERROR", (frst<scndSize) ? cntr[frst].lex : "NA");
   return false;
}
   
// копирование лексемы в буфер
uint8_t addstr(char* dest, uint8_t arrow, char* sour, uint8_t size){
   uint8_t i=0;
   while(arrow<size && sour[i]>' '){
      dest[arrow++]=sour[i++];
   }
   dest[arrow]=' ';
   return i+1;
}

// создание запроса для интерченж режима
// k - стартовый нoмер лексемы
// size - размер буфера
bool requestAdd(uint8_t* k, char* buff, uint8_t size, uint8_t count){
    uint8_t arrow = addstr(buff, arrow, pass, size); // сначала пароль
    uint8_t cnt=0;
    toProcHash = 0;
    uint16_t nowSec=millis()/1000;
    if(*k+nData>=cntrSize) *k=0;
    if(*k==0){ // test, обратно получим power battery balans ttime
       arrow += addstr(buff, arrow, (char*)cntr[nTest].lex, size); // TEST
       toProcHash = testHash;
       *k=1;
    } else {
       uint8_t i=nData-1+*k;
       for(i; i<cntrSize; i++){ // компиляция строки запроса
          // пробел + лексема + пробел + знак вопроса + терминирующий 0
          if((cnt++>=count) || (arrow+strlen(cntr[i].lex)+4>size)) break; // в буфер не лезет
          if(cntr[i].timer==0 || nowSec-cntr[i].timer>=cntr[i].delay){
             arrow += addstr(buff, arrow, (char*)cntr[i].lex, size); // лексемы
             buff[arrow++]='?';
             buff[arrow++]=' ';
             toProcHash += cntr[i].hash; // считаем хешь лексем
             ESP_LOGV(TAG,"Add to request %s", (char*)cntr[i].lex); 
          } else {
             ESP_LOGV(TAG,"Don't add %s, waiting for %d sec.", (char*)cntr[i].lex, cntr[i].delay-(nowSec-cntr[i].timer)); 
          }
          (*k)++;
       }
    }
    buff[arrow-1]=0;
    ESP_LOGE(TAG,"Create request: %s, Size: %d, hash: 0x%02X, next: %d", buff, arrow, toProcHash, *k);
    return true;
}

// компляция команды установки времени 
bool setTimeCreate(char* buff, uint8_t size){
    if(_rtc==nullptr) return false;
    auto now = _rtc->now();
    if (!now.is_valid()) {
       ESP_LOGE(TAG,"Not get valid time, time sync aborted");
       return false;
    } else {
       uint8_t arrow= addstr(buff, 0, pass, size);// пароль
       char comm[]="SETTIME";
       arrow += addstr(buff, arrow, comm, size); // команда
       setDec2(&(buff[arrow++]),now.day_of_month);
       arrow++;
       buff[arrow++]='.';
       setDec2(&(buff[arrow++]),now.month);
       arrow++;
       buff[arrow++]='.';
       setDec2(&(buff[arrow++]),now.year%100);
       arrow++;
       buff[arrow++]='.';
       setDec2(&(buff[arrow++]),now.hour);
       arrow++;
       buff[arrow++]=':';
       setDec2(&(buff[arrow++]),now.minute);
       arrow++;
       buff[arrow++]=' ';
       buff[arrow++]='!';
       buff[arrow]=0;
       toProcHash = testHash;
    }
    return true;
}

// запрос на вкл/выкл TTIME
bool onoffTtime(char* buff, uint8_t* arr, bool state, uint8_t size){
   uint8_t arrow=arr[0]; 
   if(arrow!=0) return false;
   arrow = arrow + addstr(buff, arrow, pass, size); // сначала пароль
   arrow = arrow + addstr(buff, arrow, (char*)cntr[nTtime].lex, size); // нужная лексема
   toProcHash=testHash;
   if(state){
      if(dTtime[0].d==0) { // если сенсор пустой, но свитч включили, отключим его
         if(cntr[nTtime].control.w!=nullptr)  cntr[nTtime].control.w->publish_state(false);
         ESP_LOGE(TAG,"Nothing setting to TTIME !");  
         goto noTtime;               
     }
     arrow = arrow + setTtimeCreate(&buff[arrow], size, dTtime);
     if(_ttime!=nullptr) { // публковать в сенсоре
        _ttime->publish_state(&buff[arrow]);
     }
   } else {
 noTtime:     
     buff[arrow++]='O';
     buff[arrow++]='F';
     buff[arrow++]='F';
   }
   buff[arrow++]=' ';
   arr[0]=arrow;
   return true;
}

// построение запроса на вкл/выключателей
bool onoffAdd(char* buff, uint8_t *arr, uint8_t sw, bool state, uint8_t size){
   uint8_t arrow=arr[0];
   if(arrow==0){
      arrow += addstr(buff, arrow, pass, size); // сначала пароль
      toProcHash=0;
      if(sw==nArm || sw==nDisarm){ // ARM установка arm/disarm , только первым
         if(state){
            sw=nArm; // ARM
         } else {
            sw=nDisarm; // DISARM 
         }
         arrow += addstr(buff, arrow, (char*)cntr[sw].lex, size); // нужная лексема
         arr[0]=arrow;
         return true;
      }
   }
   if(arrow + strlen(cntr[sw].lex)+5>size) return false; // в буфер не лезет
   arrow += addstr(buff, arrow, (char*)cntr[sw].lex, size); // нужная лексема
   toProcHash+=cntr[sw].hash; // считаем хешь лексем
   buff[arrow++]=(state ? '1' : '0');
   buff[arrow++]=' ';
   arr[0]=arrow;
   return true;
} 

// разбор сообщений
bool msgParce(char* str, uint16_t size){
    uint8_t arrow=0;
    uint8_t hider=0xFF;
    bool test;  
    fromProcHash=0;// буфер подсчета хеша сообщения от проца
    globalState=ciNone; // сбрасываем глобальные флаги
    if(!readTime(str, &arrow, &hours, &mins)){//пытаемся считать время 
       if((readFirstToken(str, &arrow, &hider) && hider==nTest)){// если не получилось, то токен TEST
          char ch=0;
          setGlobSens(nTest, &ch);
          if(nextLex(str, &arrow)!=nlspace) return false;
          if(!readTime(str, &arrow, &hours, &mins)) return false; // а потом время
       }           
    } 
    if(_timestamp!=nullptr){ //публикция сенсора текущего времен
       char bu[]="00:00\0";
       memcpy(bu, &str[arrow-5],5);
       _timestamp->publish_state(bu);
    }
    if(needTimeSync){ // проверка процедуры синхронизации времени
        if(_rtc!=nullptr){ 
            auto now = _rtc->now();
            if(now.hour==hours && abs(now.minute-mins)<2){
               needTimeSync=false;
               ESP_LOGE(TAG,"Time synced");
            }
        } else {
            needTimeSync=false;
        }
    } 
    if(nextLex(str, &arrow)!=nlline) return false;
    ESP_LOGV(TAG,"Get time from sim3000: %d:%d",hours,mins);
    // тут уже знаем время формирования сообщения
    uint8_t control=0xFF;
    uint16_t nowSec=millis()/1000;
    while(str[arrow]!=0 && arrow<size && control!=arrow){
       control=arrow;
       uint8_t frst=0xFF;
       uint8_t secn=0xFF;
       float fl=0;
       bool state=true;
       bool alarm=false;
       uint8_t sawArr=0;
       char ch[20]={0};
       nLex test=nlspace;
       if(readFirstToken(str, &arrow, &frst)){
          if(nextLex(str, &arrow)==nlspace){
             sawArr=arrow;
             //ESP_LOGE("","SWADDR=%d",sawArr);
             if(readFloat(str, &arrow, &fl)){
                test=nextLex(str, &arrow);
             }
             if((test==nlspace) && (readSecondToken(str, &arrow, &secn) || readStr(str, &arrow, (char*)&ch, sizeof(ch)))){ //второй токен или string
                test=nextLex(str, &arrow);
             } 
             if(test==nlspace){
                if(!readParam(str, &arrow, &alarm) && ch[0]==0){
                   readStr(str, &arrow, (char*)&ch, sizeof(ch)); // телефон оператора
                }
             }
          }
       }  
       if(frst!=0xFF && secn!=0xFF && frst<cntrSize && secn<scndSize){
          fromProcHash+=cntr[frst].hash; // считаем хеш сообщения
          if(setSwitch(frst,secn) || setLogicSens(frst, scnd[secn].state) || setGlobSens(frst, ch)){
             if(cntr[frst].delay!=0){ //для параметров с установленным периодом опроса
                cntr[frst].timer=nowSec;
             }
             ESP_LOGV(TAG,"Set %s %s:%s %s, state=%d, alarm=%d",(cntr[frst].type!=cSwitch)? "sensor" : "switch", (char*)cntr[frst].lex, (char*)scnd[secn].lex, ch, (uint8_t)state , (uint8_t)alarm);
          } else {
              goto errMsgParce;
          }
       } else if(frst!=0xFF && frst<cntrSize){
          fromProcHash+=cntr[frst].hash; // считаем хеш сообщения
          if(setValSens(frst,fl,alarm,ch) || setGlobSens(frst, ch)){
              if(cntr[frst].delay!=0){ //для параметров с установленным периодом опроса
                cntr[frst].timer=nowSec;
              }
              ESP_LOGV(TAG,"Set value sensor %s, %f, %s, alarm=%d", (char*)cntr[frst].lex, fl, ch,(uint8_t)alarm);
          } else {
              if(setOtherSw(frst, str, &sawArr)){
                 if(cntr[frst].delay!=0){ //для параметров с установленным периодом опроса
                   cntr[frst].timer=nowSec;
                 }
                 ESP_LOGV(TAG,"Set value other switch %s", (char*)cntr[frst].lex);
                 arrow=sawArr; 
              } else {
                 goto errMsgParce;
              }
          }
       } else {
     errMsgParce:
          if(frst<cntrSize){
             if(cntr[frst].type==cSwitch){
                ESP_LOGE(TAG,"State for switch %s UNDEFINED: %s",(char*)cntr[frst].lex, coreForLog(&(str[arrow])));
             } else {
                ESP_LOGE(TAG,"State for sensor %s failed parce: %s",(char*)cntr[frst].lex, coreForLog(&(str[arrow])));
             }
          } else {
             ESP_LOGE(TAG,"Failed parce %s/%s %s, state=%d, alarm=%d, content: %s", (frst<cntrSize) ? (char*)cntr[frst].lex : "???", (secn<scndSize) ? (char*)scnd[secn].lex : "???", ch, (uint8_t)state , (uint8_t)alarm, coreForLog(&(str[arrow])));
             return false;
          }
       }
       nextLex(str, &arrow);
    }
    return true;
 }

// КОНВЕРТАЦИЯ из UNICODE в KOI8 для возможного декодирования USSD 
void uni2utf8(uint8_t* out, uint8_t* in, uint8_t len) {
    uint8_t j=0;
    uint16_t i=0;
    while(i<len){
       uint8_t temp=in[i++];
       if(temp==4){
          uint8_t test=in[i++]+0x80;
          if(test<=0xbf){
             out[j++]=0xd0;
             out[j++]=test;
          } else {
             out[j++]=0xd1;
             out[j++]=test-0x40;
          }
       } else if (temp==0){
          out[j++]=in[i++];
       } else {
          out[j++]=temp;
       }
    }
    out[j]=0;
}

uint16_t getdec16(const char *pc){
   uint16_t ret=0;
   for(uint8_t i=0; i<20; i++){
      if(pc[i]>='0' && pc[i]<='9'){
          ret*=10;
          ret+=pc[i]-'0';
      } else if(pc[i]!=' ') {
         return ret;
      }
   }
   return ret;
}

bool readTime(char* str, uint8_t* hour, uint8_t* min){
   if(str[2]==':'){
      *hour=getdec(&(str[0]));
      *min=getdec(&(str[3]));
      return true;
   }
   return false;
}

// установка/сброс буфера ответа, как будто есть ожидающее СМС, для активации передачи PDU в сторону проца
void setCpms(char n='0'){
    cpmsFake[5]=n;
    cpmsFake[15]=n;
    cpmsFake[25]=n;
}

// загрузка десятичного числа в строку
uint8_t setDec(char* buff, uint16_t dec, bool noFirst0=true){




    char temp[8]={0};


    for(uint8_t i=0; i<6; i++){
        temp[i]=dec%10 + '0';
        dec/=10;
    }
    bool pass=false;
    uint8_t j=0;
    for(int8_t i=5; i>=0; i--){
        if(temp[i]!='0' || i==0){
           pass=true;
        }
        if(pass){
           buff[j++]=temp[i];
        }
    }
    buff[j]=0;
    return j;
}

#ifdef MY_DEBUG

// ДЛЯ ОТЛАДКИ
uint8_t mod[256]={0};
uint8_t arrMod=0;
uint8_t proc[256]={0};
uint8_t arrProc=0;

bool write_byte(uint8_t b, uint8_t* buarr, uint8_t* bu){
   bool ret=false;
   if(b=='\n'){
      bu[(*buarr)++]='\\';
      bu[(*buarr)++]='n';
      ret=true;
   } else if (b=='\r'){
      bu[(*buarr)++]='\\';
      bu[(*buarr)++]='r';
   } else if (b==0){
      bu[(*buarr)++]='\\';
      bu[(*buarr)++]='0';
   } else {
      bu[(*buarr)++]=b;
   }
   bu[*buarr]=0;
   if(ret){
      *buarr=0;
   }
   return ret;
}

bool write_byteP(uint8_t b, uint8_t* buarr, uint8_t* bu){
   bool ret=false;
   if(b=='\n'){
      bu[(*buarr)++]='\\';
      bu[(*buarr)++]='n';
   } else if (b=='\r'){
      bu[(*buarr)++]='\\';
      bu[(*buarr)++]='r';
      ret=true;
   } else if (b==0){
      bu[(*buarr)++]='\\';
      bu[(*buarr)++]='0';
   } else {
      bu[(*buarr)++]=b;
   }
   bu[*buarr]=0;
   if(ret){
      *buarr=0;
   }
   return ret;
}
void wbProc(uint8_t b){
   if(write_byteP(b, &arrProc, proc)) ESP_LOGE("","PROC: %s",(char*) proc);
}
void waProc(uint8_t* bu, uint8_t count){
   for(uint8_t i=0; i<count; i++){
      wbProc(bu[i]);  
   }
}
void wbMod(uint8_t b){
   if(write_byte(b, &arrMod, mod)) ESP_LOGE("","SIM: %s",(char*) mod);
}
void waMod(uint8_t* bu, uint8_t count){
   for(uint8_t i=0; i<count; i++){
      wbMod(bu[i]);  
   }
}

// макросы для отладки 
#define UART_RX_WB(a)     wbMod(a);
#define UART_RX_WA(a,b)   waMod(a,b);

#define UART_RX_HOCK_WB(a)     wbMod(a);
#define UART_RX_HOCK_WA(a,b)   waMod(a,b);

#define UART_TX_WB(a)     wbProc(a);
#define UART_TX_WA(a,b)   waProc(a,b);

#define UART_TX_HOCK_WB(a)     wbProc(a);
#define UART_TX_HOCK_WA(a,b)   waProc(a,b);

#else

// макросы для отладки 
#define UART_RX_WB(a)     id(uart_rx).write_byte(a);    //wbMod(a);
#define UART_RX_WA(a,b)   id(uart_rx).write_array(a,b); //waMod(a,b);

#define UART_RX_HOCK_WB(a)     id(uart_rx).write_byte(a);    //wbMod(a);
#define UART_RX_HOCK_WA(a,b)   id(uart_rx).write_array(a,b); //waMod(a,b);

#define UART_TX_WB(a)     id(uart_tx).write_byte(a);    //wbProc(a);
#define UART_TX_WA(a,b)   id(uart_tx).write_array(a,b); //waProc(a,b);

#define UART_TX_HOCK_WB(a)     id(uart_tx).write_byte(a);    //wbProc(a);
#define UART_TX_HOCK_WA(a,b)   id(uart_tx).write_array(a,b); //waProc(a,b);

#endif

    // РАЗБОР PDU
    bool decode(char* buffpdu, uint16_t sz){
              // РАСШИФРОВКА СМС
              //ESP_LOGE("","PDU SIZE: %d",sz);
              //ESP_LOGE("","%s",printHexs((uint8_t*)bcoreForLoguffpdu,sz));
              
              if(decodePDU((const char*)buffpdu)) {
                 if((tpdu & PDU_SMS_SUBMIT)!=0){
                    generalWorkBuff[generalWorkBuffLength]=0;
                    msgParce(generalWorkBuff, generalWorkBuffLength); // это смс от процессора, поэтому разложить данные по сенсорам 
                    if(cntr[nAlarm].control.b!=nullptr && cntr[nAlarm].type==cBinary){ // если есть сенсор ALARM
                       bool al=((globalState & GL_ALARM)!=0);
                       if(cntr[nAlarm].control.b->state!=al) cntr[nAlarm].control.b->publish_state(al); // установить туда значение
                    }
                    ESP_LOGD(TAG,"Gloal states flags: %x", globalState);
                 }                     
                 //for(uint16_t i=0; i<generalWorkBuffLength && generalWorkBuff[i]!=0; i++){ // подготовка строки для публикации в сенсор
                 //   if(generalWorkBuff[i]=='\r' || generalWorkBuff[i]=='\n'){ // замена служебных символов для вывода в текcтовые сенсоры
                 //      generalWorkBuff[i]='|';
                 //   }
                 //}                 
                 if (overFlow) {
                    ESP_LOGE(TAG,"PDU Buffer Overflow, partial");
                 }
              } else {
                 ESP_LOGE(TAG,"SMS (%s) Decode failed", addressBuff);
                 return false;
              }
              return true;
      }        

    // нераспознанный пакет
    void getInErr(){
        if(_unkrep!=nullptr && strcmp(_unkrep->state.c_str(),(char*)in_buff)!=0){
            _unkrep->publish_state((char*)in_buff);
        }
    }

    void copyrep(char* dest, char* sour, uint8_t len){
       uint8_t i;
       for(i=0;i<(len-1) && sour[i]>=' ';i++){
          dest[i]=sour[i];
       }
       dest[i]=0;
    }

    // сброс отправки смс процу
    void inSMSBreak(){
       if(inSMSneedSend){
          _hook=false;
          inSMSneedSend=false;
          interchangeTimer=millis();
       }
       setCpms('0'); //сбрасываем ожидание
       inSMStryCnt=0;
       inSMSTimer=millis();
    }

    void ccu6225loop(){     
           
           // работа с линией RX - поток данных от SIM300
           if(id(uart_rx).available()){
              uint8_t data; 
              _hookReady=false; // если есть вхоящий поток, отправлять данные нельзя
              id(uart_rx).read_byte(&data);
              if(_hook==false) UART_RX_WB(data); //перехват не включен, пропускаем поток в сторону SIM300 
              if(data =='\n'){
                 if(in_array>0){
                    in_buff[in_array]=0;
                    ESP_LOGV(TAG,"SIM: %s", in_buff);// отладка
                    if(inPDU){ // ожидание входящего PDU
                       inPDU=false;
                       if(decode((char*)in_buff, in_array)){ 
                          ESP_LOGD(TAG,"Inbound SMS from %s : %s", addressBuff, coreForLog(generalWorkBuff));
                          if (concatInfo[0] == 0){ // сообщение одним куском
                             if(_inCont!=nullptr && _inAddr!=nullptr){
                                if(strcmp(_inCont->state.c_str(),generalWorkBuff)!=0){
                                    _inAddr->publish_state(addressBuff);
                                    _inCont->publish_state(generalWorkBuff);
                                }
                             }
                          } else { // кусочные сообщения
                             char* fmsg=storePiece(generalWorkBuff, addressBuff, concatInfo[1], concatInfo[2],concatInfo[0]);
                             if(fmsg!=nullptr){ // накопили кусками полное сообщение 
                                if(_inCont!=nullptr && _inAddr!=nullptr){
                                   if(strcmp(_inCont->state.c_str(),generalWorkBuff)!=0){
                                       _inAddr->publish_state(addressBuff);
                                       _inCont->publish_state(fmsg);
                                   }
                                }
                                free(fmsg);
                             }
                             interchangeTimer=millis(); // продлеваем задержку интерченджа, в ожидании следующего входяего куска
                          }
                       } else {
                          ESP_LOGE(TAG,"Inbount SMS ERROR DECODE");
                       }
                    } else if(in_buff[0]==0x2B){ //'+'
                       if(in_comm_hash==0x80){ //+CBAND ОТВЕТ О ДИАПАЗОНЕ ЧАСТОТ
                          copyrep(cband,(char*)&(in_buff[8]),sizeof(cband));
                          ESP_LOGV(TAG,"CBAND: %s",cband); 
                       } else if(in_comm_hash==0x83){ //+CPMS ОТВЕТ О ОБЛАСТИ ХРАНЕНИЯ СООБЩЕНИЙ
                          //copyrep(cpms,(char*)&(in_buff[7]),sizeof(cpms)); // данные не нужны
                          noInSMS=(in_buff[12]=='0');
                          ESP_LOGV(TAG,"CPMS: %s",(char*)&(in_buff[7]));
                       } else if(in_comm_hash==0x89){ //+CCLK ТЕКУЩАЯ ДАТА ВРЕМЯ
                          copyrep(cclk,(char*)&(in_buff[7]),sizeof(cclk));
                          if(_tstamp!=nullptr) _tstamp->publish_state(cclk);
                          ESP_LOGV(TAG,"CCLK: %s",cclk); 
                       } else if(in_comm_hash==0x8F){ //+CUSD ОТВЕТ на USSD
                          sizecusd=in_array-7;
                          if(sizecusd>sizeof(cusd)-1) sizecusd=sizeof(cusd)-1;
                          memcpy(cusd,(char*)&(in_buff[7]),sizecusd);
                          uint8_t  bu[50]={0};
                          uni2utf8(bu,cusd,sizecusd);
                          ESP_LOGV(TAG,"CUSD DECODE: %s",(char*)bu);
                       } else if(in_comm_hash==0x90){ //+CFUN УСТАНОВКА ФунКцИОНАЛЬНОСТИ МОДЕМА
                          copyrep(cfun,(char*)&(in_buff[7]),sizeof(cfun));
                          ESP_LOGV(TAG,"CFUN: %s",cfun); 
                       } else if(in_comm_hash==0x94){ //+CMGS СКВОЗНАЯ НУМЕРАЦИЯ ИСХОДЯЩИХ СМС
                          uint16_t temp=getdec16((char*)&(in_buff[7])); // запомнить номер 
                          outSMScounter=temp;
                          ESP_LOGV(TAG,"CMGS: %s",(char*)&(in_buff[7]));
                          cmgsOk=true; //помечаем, для правильной обработки команды ОК
                       } else if(in_comm_hash==0x95){ //+CMGR ВХОДЯЩЕЕ СМС
                          inPDU=true;
                          ESP_LOGV(TAG,"CMGR: %s",(char*)&(in_buff[7])); 
                       } else if(in_comm_hash==0x9A){ //+CPIN ОТЧЕТ ОБ АУТЕНТИФИКАЦИИ СИМ КАРТЫ
                          //copyrep(cpin,(char*)&(in_buff[7]),sizeof(cpin));
                          ESP_LOGV(TAG,"CPIN: %s",(char*)&(in_buff[7]));
                       } else if(in_comm_hash==0x9D){ //+CREG СТАТУС РЕГИСТРАЦИИ В СЕТИ
                          //copyrep(creg,(char*)&(in_buff[7]),sizeof(creg));
                          ESP_LOGV(TAG,"CREG: %s",(char*)&(in_buff[7]));
                          connectOk=(in_buff[9]=='1');
                          if(_senreg!=nullptr && _senreg->state!=connectOk){_senreg->publish_state(connectOk);}
                       } else if(in_comm_hash==0xF3 || in_comm_hash==0xE5 || in_comm_hash==0xFD){ //+CMS ERROR: +CME ERROR: ERROR
                          if(in_comm_hash==0xFD){
                              GSMerror=666;
                          } else {
                              GSMerror=getdec16((char*)&(in_buff[12]));
                          }
                          if(_gsmError!=nullptr && _gsmError->state!=GSMerror) _gsmError->publish_state(GSMerror);
                          ESP_LOGE(TAG,"CMS/CME/OTHER ERROR: %d",GSMerror);
                          if(outSMSneedSend){ // выйти из режима захвата
                             _hook=false;
                          }
                       } else if(in_comm_hash==0xF5){ //+CSQ 22,22КАЧЕСТВО СВЯЗИ
                          // КАЧЕСТВО СВЯЗИ
                          copyrep(csq,(char*)&(in_buff[6]),sizeof(csq));
                          ESP_LOGV(TAG,"CSQ %s",csq); 
                          int8_t rssi=getdec16((char*)&in_buff[6]);
                          int8_t qual=getdec16((char*)&in_buff[6+2+(rssi>9 ? 1:0)]);
                          bool rssiOk=true;
                          if(_gsmRssi!=nullptr && rssi<=32 ){
                             if(rssi==0){
                                rssi=-115;
                             } else if(rssi==1){
                                rssi=-111;
                             } else if(rssi<31){
                                rssi=-110+(rssi-2)*2;
                             } else if(rssi==31){
                                rssi=-52;
                             }
                             if( _gsmRssi->state!=rssi) _gsmRssi->publish_state(rssi);
                          }
                          if(_gsmQual!=nullptr && qual<=7){
                             float ret=(0.2*pow(2,qual))/0.75;
                             if(_gsmQual->state!=ret) _gsmQual->publish_state(ret);
                          }
                       } else if(in_comm_hash==0x81){ //+CLCC, входящий, исходящий звонки
                          //ИСХОДЯЩЙ звонок
                          //+CLCC: 1,0,2,0,0,"+7916XXXXXXX",145,"" / 2 - Исходящий вызов в режиме набора.
                          //+CLCC: 1,0,3,0,0,"+7916XXXXXXX",145,"" / 3 - Исходящий вызов в режиме дозвона.
                          //ВХОДЯЩИЙ звонок
                          //+CLCC: 1,1,4,0,0,"+7916XXXXXXX",145,"" / 4 - Входящий вызов в режиме дозвона.
                          //+CLCC: 1,1,0,0,0,"+7916XXXXXXX",145,"" / 0 - Активный вызов.
                          //+CLCC: 1,1,7,0,0,"+7916XXXXXXX",145,"" / 7 - Сброс вызова (разъединение).
                          char* addr=(char*)&in_buff[18];
                          uint8_t i=0;
                          const char* undef="unexpected";
                          for(i; i<15; i++){
                             char c=addr[i];
                             if(c=='"' || c==0 || c=='\''){
                                addr[i]=0;
                                break;
                             }
                          }                              
                          if(addr[i]!=0) addr = (char*)undef;
                          if(in_buff[9]=='0'){ // исходящй
                             if(_outCall!=nullptr && strcmp(addr,_outCall->state.c_str())!=0) _outCall->publish_state(addr);   
                          } else { // входящий
                             if(_inCall!=nullptr && strcmp(addr,_inCall->state.c_str())!=0) _inCall->publish_state(addr);   
                          }
                          ESP_LOGE(TAG,"%s call %s: %s",(in_buff[9]!='0') ? "Inbound" : "Outbound", (in_buff[9]!='0') ? "from" : "to", addr);
                       } else {
                          ESP_LOGE(TAG,"UART_RX_UNKNOW +???? (hash %x): %s, ", in_comm_hash, (char*)in_buff);
                          getInErr();
                       }
                    } else if(in_buff[0]=='>'){ // исходящее PDU
                    } else if(in_buff[0]=='A'){ //ECHO AT
                    } else if(in_buff[0]=='O'){
                       if(in_comm_hash==0xAC){ //OK,
                          //ESP_LOGD(TAG,"GSM_RX_OK: %s",(char*)in_buff);
                          GSMerror=0; // раз ОК, то ошибк нет
                          if(_hook){
                             if(outSMSneedSend){ //  отправляли сообщение наружу- ушло успешно
                                ESP_LOGE(TAG,"OUT SMS send OK");
                                outSMSerrCnt=0; // освободили процедуру
                                outSMSneedSend=false; // отработали отправку
                                delQueSMS(); // удалили этот кусок
                                _hook=false; // снять перехват
                             }
                          } else if(_hookDelayed && cmgsOk){ //ожидаем окончание ответа CMGS от модуля 
                             _hookDelayed=false;// продолжим ждать ответ на на запрос внутреннего обмена
                             if(toProcHash!=0){ // только если знаем хеш ожидаемого соообщения
                                ESP_LOGE(TAG, "Hook re-actvate, next waite reply hash: %x", toProcHash);
                                inSMSTimer = millis(); // откладываем следующую попыку отправки
                                interchangeTimer = millis();
                                _hook=true;  // включаем перехват
                             }
                          } else {
                             //_hookReady=connectOk && noInSMS; // получил подтверждение окончания отправки - OK и модуль в сети, можно инжектировать PDU в сторону SIM300
                             _hookReady = noInSMS; // получил подтверждение окончания отправки - OK и модуль в сети, можно инжектировать PDU в сторону SIM300
                          }
                          cmgsOk=false; 
                       } else {
                          ESP_LOGE(TAG,"UART_RX_OK_ERR: %s, hash %x",(char*)in_buff, in_comm_hash);
                          getInErr();
                       }
                    } else if(in_comm_hash==0xA7){ // "Call Ready"
                    } else if(in_comm_hash==0xA8){ // "\r\n"
                    } else if(in_comm_hash==0x97){ // "35958701288" не знаю че за хрень
                    } else if(in_comm_hash==0xFD){ // "ERROR"
                    } else if(in_comm_hash==0xE7){ // "RDY"
                    } else if(in_comm_hash==0x86){ // "NO CARIER"
                    } else if(in_comm_hash==0x81){ // "RING"
                    } else {
                       ESP_LOGE(TAG,"UART_RX_UNKNOW(hash %x): %s, (%x, %x, %x, %x)", in_comm_hash, (char*)in_buff, in_buff[0], in_buff[1], in_buff[2], in_buff[3]);
                       getInErr();                       
                    }
                    in_array=0;
                    in_comm_hash=0xA5;
                 }
              } else {
                 if(_hook){
                    if(outSMSneedSend && data==' ' && in_array==1 && in_buff[0]=='>'){ // промпт от модуля SIM300 в режиме отправки SMS наружу
                       if(_hook){
                          UART_TX_HOCK_WA((uint8_t*)generalWorkBuff, (PDUerr+1)*2+1); // отправляем ранее подготовленное PDU 
                          ESP_LOGD(TAG,"Outbound inject SMS: %s",coreForLog(generalWorkBuff));
                       }
                    }                       
                 }
                 if(in_array<5){
                    in_comm_hash ^=data; // считаем хеш команды для дальнейшшего разбора
                 }
                 in_buff[in_array++]=data;
                 in_array &= IN_BUFF_SIZE;
              }
           }
           
           // работа с линией ТХ - поток данных в сторну SIM300
           if(id(uart_tx).available()){ 
              uint8_t data;
              _hookReady=false; // есть данные, сей час прерывать обмен данными нельзя
              id(uart_tx).read_byte(&data); // процессор передал байт
              if(_hook){ // обмен перехвачен
                 //...
              } else { //обмен НЕ перехвачен
                 if(data!=0x1a){ // если не конец передачи PDU
                    UART_TX_WB(data); //ПЕРЕДАЕМ ВСЕ МОДУЛЮ GSM
                 }
              } 
              out_buff[out_array++]=data; // копим данные в буфере для анализа
              in_array &= OUT_BUFF_SIZE;
              if(data=='\r' || data==0x1a){ // проц закончил передачу строки или PDU
                  out_buff[out_array]=0;
                  ESP_LOGV(TAG,"PROC: %s", out_buff);// отладка
                  if(_hook==false){ // нет перехвата
                     if(data==0x1a && out_buff[1]!='T'){ // получили PDU от проца без режима перехвата
                        if(decode((char*)out_buff, out_array)){
                           ESP_LOGE(TAG,"Outbund SMS to %s : %s", addressBuff, coreForLog(generalWorkBuff));
                           if(getPerm(addressBuff)){ //если это сообение для этого номера блокированно
                              ESP_LOGE(TAG,"SMS for blocked address and content. Sending aborted.");
                              data=0x1B; // передаем ESC модулю - OТМЕНЯЕМ передачу
                              UART_RX_HOCK_WB(0x1A); // процу говорим, что приняли окончание передачи
                              UART_RX_HOCK_WA((uint8_t*)&_ok[6],2); //"\r\n"
                              outSMScounter=(outSMScounter+5)%98+1; // ответим изменением счетчика смсок
                              sprintf(&(cmgs[7]),cmgsMask,outSMScounter);
                              UART_RX_HOCK_WA((uint8_t*)&cmgs[2],9); // +CMGS: XX
                              UART_RX_HOCK_WA((uint8_t*)_ok,8); //"\r\n\r\nOK\r\n"
                           } else if(_outAddr!=nullptr && _outCont!=nullptr){ // если есть сенсоры отправки 
                              if(strcmp(_outCont->state.c_str(),generalWorkBuff)!=0){ // публикуем исходящие
                                  _outAddr->publish_state(addressBuff);
                                  _outCont->publish_state(generalWorkBuff);
                              }
                           }
                        } else {
                           ESP_LOGE(TAG,"Outbound SMS DECODE ERROR");
                        }
                        UART_TX_WB(data);
                     }
                  } else { // в режиме перехвата
                     if(out_buff[1]=='T'){ // команда АТ от проца в режиме эмуляии
                        UART_RX_HOCK_WA(out_buff,out_array); // ЭХО В РЕЖМЕ ПЕРЕХВАТА
                        UART_RX_HOCK_WA((uint8_t*)&_ok[6],2); //"\r\n" - подтверждаем прием окончания команды 
                        char* rep=0;
                        if(out_comm_hash==0x80){ //AT+CBAND? 80
                           rep=cband;
                        } else if(out_comm_hash==0x81){ //AT+CLCC или AT+CMGF=0  81
                        } else if(out_comm_hash==0x83){ //AT+CPMS? AT+CMGD=001 83
                           if(out_buff[6]=='D'){ //AT+CMGD=001 - проц просит удалить сообщение, значит наше PDU считал нормально
                              if(inSMSneedSend){ 
                                setCpms('0'); // снимаем информацию о наличии PDU в буфере
                                //ESP_LOGE(TAG,"Proc PDU interchange OK");
                              }
                           } else { //AT+CPMS? - проц запросил наличие/отсутствие CМС
                              rep=cpmsFake;
                           }
                        } else if(out_comm_hash==0x89){ //AT+CCLK? 89
                           rep=cclk;
                        } else if(out_comm_hash==0x8f){ //AT+CUSD=1,"*100#" 8f ЗАПРОС USD
                           rep=(char*)cusd;
                        } else if(out_comm_hash==0x90){ //AT+CFUN=1 97
                        } else if(out_comm_hash==0x94){ //AT+CMGS=031 проц собрался передать сообщение и говорит о его размере 
                           rep=prompt; // рисуем ему промпт без ответа ок
                        } else if(out_comm_hash==0x95){ //+CMGR параметры получаемого СМС
                          uint8_t i=getdec16((char*)&(out_buff[8])); // номер опрашиваемой ячейки сообщений
                          if(i==1){ // проц запросил содержимое первого PDU (ЭМУЛИРУЕМ ВХОДЯЯЩЕЕ PDU)
                             PDUsize=encodePDU(_inSMSAddr, _inSMSmsg, _inSMSHub, 0, (char*)&(cclk[1]));
                             ESP_LOGV(TAG,"Crete inter PDU: %s, from %s", _inSMSmsg, _inSMSAddr);
                             uint8_t line;
                             if(PDUsize<0){ // в случае ошибки сообщаем, что PDU нулевого размера
                                inSMSBreak();
                                ESP_LOGE(TAG,"Create interchange PDU ERROR: %d",PDUsize);
                                line=setDec((char*)&cmgr[3], 0); 
                             } else { // или сообщаем размер PDU
                                line=setDec((char*)&cmgr[3], PDUsize);
                             }
                             cmgr[3+line]=0; 
                             rep=cmgr;
                          }
                        } else if(out_comm_hash==0x97){ //AT+CGSN? 97
                        } else if(out_comm_hash==0x9a){ //AT+CPIN  9a
                           rep=cpinFake;
                        } else if(out_comm_hash==0x9d){ //AT+CREG?  9b
                           rep=cregFake;
                        } else if(out_comm_hash==0xcf){ //AT+CSQ  cf
                           rep=csq;
                        } else if(out_buff[2]='&'){ //AT&
                        } else { // если получили незвестную команду на некоторое время отменить перехват управления и транслировать сообщение модулю;)
                           _hook=false;
                           inSMSTimer=millis(); // задежать отправку PDU процу
                           outSMStimer=inSMSTimer; // задержать отправку наружу
                           UART_TX_WA(out_buff,out_array); // транслровать строку модулю, пусть разберется
                           ESP_LOGE(TAG,"UNKNOWN from PROC: %s , hash:%x", out_buff, out_comm_hash);
                        }
                        if(rep==prompt){ // если нужно выставить процу промпт
                            UART_RX_HOCK_WA((uint8_t*)rep,2);// промпт "> "
                        } else if(rep!=0){ // на эту АТ команду есть ответ
                            for(uint8_t i=2; i<8 && out_buff[i]!='?' && out_buff[i]>' ' && out_buff[i]!='='; i++){ // сначала заголовок запроса на который отвечаем "+XXXX"
                                UART_RX_HOCK_WB(out_buff[i]);
                            }
                            UART_RX_HOCK_WA((uint8_t*)cmgsMask,2);// потом разделитель ": "
                            if(rep==(char*)cusd){ // это ответ на запрос CUSD, нет смысла подменять, пусть спросит у SIM300
                               UART_RX_HOCK_WA((uint8_t*)rep,sizecusd);
                            } else {
                                for(uint8_t i=0; i<50 && rep[i]>=' '; i++){ // далее тело ответа на запрос без перевода строки
                                    UART_RX_HOCK_WB(rep[i]);
                                }
                            }
                            UART_RX_HOCK_WA((uint8_t*)&_ok[6],2); // потом перевод строки "\r\n"
                            if(rep==cmgr){ // если есть PDU для передачи процу
                                for(uint8_t i=0; i<255 && generalWorkBuff[i]!=0; i++){ // передаем PDU
                                    UART_RX_HOCK_WB(generalWorkBuff[i]);
                                }
                                UART_RX_HOCK_WA((uint8_t*)&_ok[6],2); // потом перевод строки "\r\n"
                            }
                            UART_RX_HOCK_WA((uint8_t*)&_ok[6],2); // опять перевод строки "\r\n"
                        }
                        if(rep!=prompt) { // промпт выставляется без OK
                           UART_RX_HOCK_WA((uint8_t*)&_ok[4],4); // "OK\r\n"
                        }
                        // конец обработки AT
                     } else if(data==0x1a){ // PDU от проца в режиме перехвата
                        UART_RX_HOCK_WA(out_buff,out_array-1); // ЭХО В РЕЖИМЕ ПЕРЕХВАТА, без 0x1A
                        UART_RX_HOCK_WA((uint8_t*)&_ok[6],2); //"\r\n"
                        if(!decode((char*)out_buff, out_array)){
                            ESP_LOGE(TAG,"PDU from Proc DECODE ERROR");
                        } else {
                            ESP_LOGE(TAG,"Interchange reply: %s", coreForLog(generalWorkBuff));
                        }
                        
                        //ESP_LOGE("","toProcHash=%x, fromProcHash=%x", toProcHash, fromProcHash);// для отладки
                        //toProcHash=fromProcHash; // для отладки

                        // если совпал хеш или нет связи или сообщение для данного получателя заблокировано
                        bool permit=getPerm(addressBuff);
                        if(toProcHash==fromProcHash || connectOk==false || permit){
                            if(toProcHash==fromProcHash){ // ожидаемоое сообщщение обработано
                               ESP_LOGE(TAG, "Interchange SMS proc finale, hash: expected 0x%02X, received 0x%02X", toProcHash, fromProcHash); 
                               toProcHash=0;// признак не сформированного сообщения, можно формировать новое
                               inSMSBreak(); // конец сеанса обработки смс
                               _hook=false; // отключаем перехват
                            } else if (permit){ // запрещенное сообщение
                               ESP_LOGE(TAG, "SMS blocked content and number, hash: expected 0x%02X, received 0x%02X", toProcHash, fromProcHash);
                            } else { // из-за отсутствия связи делаем вид, что сообщение ушло, публикуем его в вебе, но продолжаем ждать нашего сообщения
                               if(_outAddr!=nullptr && _outCont!=nullptr){ // для возможности передать другими каналами связи
                                  if(strcmp(_outCont->state.c_str(),generalWorkBuff)!=0){ 
                                     _outAddr->publish_state(addressBuff);
                                     _outCont->publish_state(generalWorkBuff);
                                  }
                               }
                               if((globalState & GL_ALARM) != 0) { // сообщения с признаком ALARM поставить в очередь на отправку
                                  ESP_LOGE(TAG, "SMS ALARM state put in outbound queue"); 
                                  putNewSMS(generalWorkBuff, addressBuff);
                               }
                               ESP_LOGE(TAG, "Interchange SMS waite next, hash: expected 0x%02X, received 0x%02X", toProcHash, fromProcHash); 
                            }
                            outSMScounter=(outSMScounter+3)%98+1;
                            sprintf(&(cmgs[7]),cmgsMask,outSMScounter);
                            UART_RX_HOCK_WA((uint8_t*)&cmgs[2],9); // +CMGS: XX
                            UART_RX_HOCK_WA((uint8_t*)_ok,8); //"\r\n\r\nOK\r\n"
                        } else { // сообщение не ответ на наш  запрос
                            const char err[]="+CMS ERROR: 515\r\n"; // выставим ошибку отправки, что бы проц повторил отправку модулю
                            UART_RX_HOCK_WA((uint8_t*)err,sizeof(err)-1);
                            ESP_LOGE(TAG, "Interchange SMS paused, hash: expected 0x%02X, received 0x%02X", toProcHash, fromProcHash);
                            _hookDelayed=true; // будем ожидать ответа на наш запрос, но позже
                            _hook=false; // отключаем перехват
                        }
                        inSMSTimer=millis(); // откладываем следующую попыку отправки
                     }
                  } 
                  //out_buff[out_array-1]=0; // удаляем "\r"
                  //ESP_LOGE(TAG,"%s, hash: %x", out_buff, out_comm_hash);
                  out_array=0; // чистим буфер
                  out_comm_hash=0xA5; // новое начало подсчета хеша
              } else { // считаем хеш строки
                 if(out_array<8 && out_array>2){
                   out_comm_hash ^=data; // считаем хеш строки
                 }
              }
           }
   
           // обработка процедуры отправки смс наружу
           if(outSMSneedSend){ // активна
              if(_hook){ // стоит перехват
                  if(millis()-outSMStimer>outSmsErrorTimeout){ // если процедура не завершилась за 30 секунд
                     _hook=false; // освободить перехват обмена данными
                     ESP_LOGE(TAG,"Seend outbund SMS Timeut");
                  }
              } else if( _hookReady && connectOk && millis()-outSMStimer>outSmsPerod){ // есть СМС для отправки, модуль в сети и есть окно для перехвата и управление не перехвачено, таймер блокировки истек 
                  if(outSMSerrCnt++<20){ // количество неудач
                     uint8_t* msg;
                     uint8_t* rec;
                     uint8_t piese;
                     if(getQueSMS(&msg, &rec, &piese)){ // взять подготовленную СМС из очереди
                        if(rec[0]!='+' && rec[1]!='7'){ // номер телефона не Российский
                           PDUerr=-998;
                           //ESP_LOGE(TAG,"Recipient address ERROR: %d", PDUerr);
                           goto SMSBREAK; 
                        }
                        uint8_t partnumber=piese & 0xF; // номер кусочной смс
                        if(partnumber < 2 && outSMSerrCnt==1) cSMS++; // нумерация исходящих смс, только для некусочных и при первом куске кусочных 
                        PDUerr=encodePDU((char*) rec, (char*) msg, 0, 7500, 0, (piese==0) ? 0 : cSMS, piese>>4, partnumber); // формруем ИСХОДЯЕЕ SMS
                        ESP_LOGE(TAG,"Out SMS: %s, for %s, part:%d of %d", msg, rec, partnumber, piese>>4);
                        if(PDUerr>0){ // пду нормально сформровалось
                            _hook=true; // блокируем обмен
                            PDUerr--; // корректировка для правильной отдачи размера сообщения
                            sprintf(&(cmgs[7]),"=%03d\r",PDUerr);
                            UART_TX_HOCK_WA((uint8_t*)cmgs, 12); // команда
                            outSMStimer=millis();// для второго шага - отправки PDU
                            // счетчик попыток, он же индикатор занятости процедуры отправки
                            ESP_LOGE(TAG,"Start inject outbound SMS");
                        } else { // PDU формируется с ошибкой
                            //ESP_LOGE(TAG,"Create outbound PDU ERROR: %d", PDUerr);
                            goto SMSBREAK;
                        }
                     } else { // если из буфера СМС не получили текущее сообщение или номер не лоокальный
                        PDUerr=-999;
                        //ESP_LOGE(TAG,"Unexpected SMS content, ERROR: %d");
                        goto SMSBREAK;
                     }
                  } else {
                     PDUerr=-100;
            SMSBREAK:
                     ESP_LOGE(TAG,"Send SMS ERROR: %d, count: %d", PDUerr, outSMSerrCnt);
                     outSMSerrCnt=0; // освободили процедуру
                     outSMSneedSend=false; // отработали отправку
                     delQueSMSFull();
                  }   
              }
           } else { // проверяем очередь
              if(que_cnt>0 && inSMSneedSend==false && _hook==false) {
                  outSMSneedSend=true; // если там что то есть поднять флаг отправки
                  ESP_LOGE(TAG,"Activate send proc SMS");  
              }
           }
           
           // формирование регулярных сообщений процу для режима интерченьдж
           if( outSMSneedSend==false && (_interch_==nullptr || _interch_->state)){       
             if(inSMSneedSend==false) {
               // УПРАВЛЕНИЕ СВИТЧАМИ через PDU, по необходимости проверка текущего состояния свитчей для формирования управляюего сообщения 
               uint8_t arrow=0;
               uint8_t count=0;
               for(uint8_t i=0; i<cntrSize; i++){
                  if( cntr[i].type==cSwitch && cntr[i].control.w!=nullptr && cntr[i].workstate==ssNeedSet){// выбираем только реальные переключатели  c запросом на отправку
                    if(i==nTtime && arrow==0){ // установка команды TTIME, только первая и только одиночная
                       onoffTtime(interBuff, &arrow, cntr[i].control.w->state, inter_buff_size);
                       cntr[i].workstate=ssWaiteSet; //признак, что сообщение отправлено и ждем реакции на него 
                       break;
                    }
                    if((count++ < inter_buff_cnt) && (onoffAdd(interBuff, &arrow, i, cntr[i].control.w->state, inter_buff_size))){ // добавляем к управляющему сообщению команду
                       cntr[i].workstate=ssWaiteSet; //признак, что сообщение отправлено и ждем реакции на него 
                    } else {
                       break;
                    }
                  }
               }
               if(arrow>0 /*&& swInited*/){ // есть команда для отправки
                   interBuff[arrow++]='!';
                   if(toProcHash==0 && (arrow + strlen(cntr[nBalans].lex)+4<inter_buff_size)){ // значит добавилась только ARM/DISSARM  есть место
                      interBuff[arrow++]=' ';
                      arrow += addstr(interBuff, arrow, (char*)cntr[nBalans].lex, inter_buff_size); // добавим баланс
                      toProcHash+=cntr[nBalans].hash; // считаем хешь лексем
                      interBuff[arrow++]='?';
                   }
                   interBuff[arrow]=0;
                   ESP_LOGE(TAG,"Create switch comm: %s, Size: %d, Hash: %x", interBuff, arrow, toProcHash);
                inSMSproc:
                   _inSMSmsg=interBuff;
                   _inSMSAddr=abons[0].addr;
                   inSMSneedSend=(_interch_==nullptr || _interch_->state); // флаг СМС для запуска обмена данными с процом, только если разрешено
               } else {
                   uint32_t ml=millis();
                   if(ml-interchangeTimer > interchPeriod || swInited==false || needTimeSync){ // опрос раз в 20 сек или ускрорено при первом включении
                      if(swInited==false){//флаг окончания инициализации свитчей и сенсоров
                         swInited=true;
                         for(uint8_t i=0; i<cntrSize; i++){
                            if((cntr[i].type==cSwitch && cntr[i].workstate==ssUndef) || (cntr[i].type==cSensor && cntr[i].control.s!=nullptr &&  isnan(cntr[i].control.s->state))){
                                swInited=false;
                            }
                         }
                      }
                      interchangeTimer=ml;
                      static uint8_t errcnt=0;
                      if (needTimeSync && _rtc!=nullptr) { // нужна снхронизация времени
                         if(errcnt++>3){
                            needTimeSync=false;
                            errcnt=0;
                         }
                         if(setTimeCreate(interBuff, inter_buff_size)){
                            ESP_LOGE(TAG,"Create time sync comm: %s",interBuff);
                            goto inSMSproc;
                         } else {
                            needTimeSync=false;
                         }
                      } else {
                         errcnt=0;
                         ESP_LOGD(TAG,"Start regular interchange");
                         static uint8_t counter=0;
                         requestAdd(&counter, interBuff, inter_buff_size, inter_buff_cnt);
                         goto inSMSproc;
                      }
                   }
               }
             } else if(millis()-interchangeTimer>interchPeriod){ // сброс отправки при долгой задержке
               inSMSBreak();
               ESP_LOGE(TAG,"Break interchange - TIMEOUT");
             }
           }

           // запуск отправки PDU процессору 
           if(inSMSneedSend && outSMSneedSend==false && _hookReady && _hook==false && millis()-inSMSTimer > toProcPeriod){ // можно отправлять PDU в сторону проца но не чаще чем раз в 15 сек
               if(inSMStryCnt++>3){
                  inSMSBreak();
                  PDUsize=-100; // ошибка внутреннего обмена
                  ESP_LOGE(TAG,"Proc PDU interchange FAILED");
               } else {
                  _hook=true; // переключаем обмен на себя
                  setCpms('1'); //указываем, что есть смс для передачи процу
                  ESP_LOGD(TAG,"Start proc PDU interchange");
               }
               inSMSTimer = millis();
           } 
            
    }           
                    
#endif //CCU6225HACK_H
