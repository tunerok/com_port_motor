#define PACK_LENGTH 5
#define ERROR_MESSAGE "$error&"
#define OK_MESSAGE "$ready&"
 
char incoming_pack[PACK_LENGTH]  = {0,0,0,0,0};   // переменная для хранения полученного байта,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
char inc_byte = ' ';
int num_of_current_bytes = 0;
bool is_reading = false; //переменная для определения считывает ли контроллер информацию сейчас или нет
bool is_ready = false; //переменная для определения готовности посл-ти для парсера

//константы выводов к драйверу мотора
const int in3 = 6;
const int in4 = 7;

const int in1 = 4;
const int in2 = 3;

const int en1 = 10;
const int en2 = 11;

void port_listener(){
  
  while(is_ready == false){
       if (Serial.available() > 0) {
          inc_byte = Serial.read();
          switch (inc_byte){
          case '$': //стартовый бит 
            if (num_of_current_bytes == 0){ //если ничего нет, то начинаем запись 
              is_reading = true;
              Serial.println("reading start");
              is_ready = false;
              num_of_current_bytes++;
              break;
            }
            else { 
              error();
              break;
            }
            
          case '&' : //бит конца пакета
            if (num_of_current_bytes == PACK_LENGTH){
              is_reading = false;
              
              
              port_send();
              is_ready = true;
              break;
            }
            else
            {
              Serial.println(num_of_current_bytes, DEC);
              error();
              break;
            }
          default:
          {Serial.println("default ");
            if (is_reading){
            Serial.println("reading ");
            incoming_pack[num_of_current_bytes] = inc_byte - 48;//считывание пакета and ascii to dec
            num_of_current_bytes++;
            break;
            }
            else
            break;
          }
          }
              
            
      }
  }
}


void port_send(){
  Serial.print(OK_MESSAGE);
  Serial.println("sss");
 // reset();
  
}

void error(){
  Serial.print(ERROR_MESSAGE);
  Serial.println("eee");
  reset();
}

void reset(){
  for(int i = 0; i<PACK_LENGTH; i++)
    incoming_pack[i]=0;
  inc_byte = ' ';
  num_of_current_bytes = 0;
  is_reading = false; //переменная для определения считывает ли контроллер информацию сейчас или нет
  is_ready = false; //переменная для определения готовности посл-ти для парсера
}

void soft_start(int incomingByte){
   for (int i =0; i<incomingByte; i++){
              if (i>100){
                break;
                }
              analogWrite(en1, i);
              delay(1);
            }
}


void setup() {
    Serial.begin(9600); // устанавливаем последовательное соединение
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(en2, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(en1, OUTPUT);
}
 
 
void loop() {
  //  while(1){
      Serial.println("loop ");
    port_listener();
    is_ready = false; //выполнить после выхода из считывается порта, т.к. ее нужно занулять
    Serial.println();
    if(incoming_pack[1] == 1){//передний мотор
      digitalWrite(in3, LOW);//вправо
      digitalWrite(in4, HIGH);
    }
    else{
      digitalWrite(in3, HIGH);//влево
      digitalWrite(in4, LOW);
    }
    if(incoming_pack[3] == 1){//мотор задний
    digitalWrite(in1, LOW);//вперед
    digitalWrite(in2, HIGH);
    }
    else{
    digitalWrite(in1, HIGH);//назад
    digitalWrite(in2, LOW);
    }

    incoming_pack[2] = (incoming_pack[2])*25.5; //первод в шкалу 255
    incoming_pack[4] = (incoming_pack[4])*25.5;

    analogWrite(en2, incoming_pack[2]);
    soft_start(incoming_pack[4]);
    analogWrite(en1, incoming_pack[4]);
   delay(1000);
   analogWrite(en1,0);
   analogWrite(en2,0);
    reset();
    
  
 }

