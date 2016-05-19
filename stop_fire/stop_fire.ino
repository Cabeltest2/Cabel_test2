/*
  Система пожаротушения. Светлодарск... Донецкой обл.

  Применяется Arduino Nano

  
  Применяются  4  датчика газа MQ2, драйвер L298N два коллекторных двигателя с редукторами (на валах привода щелевые IR датчики FC03), реле, светодиод, пьезопищалка.
  По срабатыванию одного из четырех MQ2 должна выполняется следующая программа:
1) закончен опрос датчиков;
2) включаем светодиод ;
3) включаем пьезопищалку – сигнал тревоги;
4) двигатель А начинает вращение «вперед» до определенного количества оборотов вала привода, например  17 оборотов (импульсов с IR датчика  FC03 ) – двигатель стоп;
5) двигатель В начинает вращение «влево» до определенного количества оборотов вала привода, например  8 оборотов (импульсов с IR датчика  FC03 ) – двигатель стоп;
6) включаем реле на 10-20 секунд;
7) выключаем реле;
8) выключаем пьезопщалку;
9) выключаем светодиод;
10) двигатель В начинает вращение «в право» до 8 оборотов вала привода (импульсов с IR   датчика  FC03 ) – двигатель стоп;
11) двигатель А начинает вращение «назад» до 17 оборотов вала привода (импульсов с IR датчика  FC03 ) – двигатель стоп;
12) снова, переходим к опросу всех четырех датчиков газа MQ2.
    Суть проекта заключается в создании макета простой, автоматической противопожарной системы.
	Четыре датчика газа MQ2 опрашиваются до появлений высокого уровня на одном из них. 
	Как только один сработал, прекращается опрос датчиков и программа переходит к выполнению одного из четырех вариантов цикла.
    В зависимости от того какой из четырех датчиков газа MQ2 срабатывает - изменяется количество оборотов вала двигателя А 
	и исходное  направление вращения двигателя В – куда двигается в начале (влево или вправо). 
	Если с начала влево то возвращаясь в исходное состояние двигается в право  . 
	Количество оборотов вала двигателя В не изменяется и постоянно = 8, количество оборотов вала двигателя А может быть или 17 или 34. 
    Сработал датчик, включается светодиод, включается пьезопищалка. Каретка с  противопожарным раструбом  доставляется 
	по адресу сработавшего датчика, на 10-20 секунд включается реле насоса . Происходит «тушение пожара». Выключается светодиод ,
	выключается пьезопищалка. Каретка с раструбом возвращается в исходное положение.
     Датчики газа подключал к аналоговым входам, порог устанавливал 200. Работу счетчика щелевых датчиков FC03 проверял при поданном 
	 питании 3.3В ( при 5 В наблюдался дребезг и т.д.) . Сигнал подавал на 2 и 3 цифровые ввода ардуины, считал при помощи buttonPushCounter. 
  */

#define sensorMQ2_1   A0                       // Назначение входа датчика газаMQ2 №1 пин Nano A0
#define sensorMQ2_2   A1                       // Назначение входа датчика газаMQ2 №2 пин Nano A1
#define sensorMQ2_3   A2                       // Назначение входа датчика газаMQ2 №3 пин Nano A2
#define sensorMQ2_4   A3                       // Назначение входа датчика газаMQ2 №4 пин Nano A3

#define sensorFC03_1  2                        // Назначение входа датчика FC03 №1 пин Nano D2
#define sensorFC03_2  3                        // Назначение входа датчика FC03 №2 пин Nano D3

#define motorA_en     10                       // Подключение пин Nano D10 к пину 7 на L298N (предварительно убрав перемычку)
#define motorA_in1     9                       // Подключение пин Nano D9 к пину 8 на L298N    
#define motorA_in2     8                       // Подключение пин Nano D8 к пину 9 на L298N 

#define motorB_en      5                       // Подключение пин Nano D5 к пину 12 на L298N (предварительно убрав перемычку)
#define motorB_in3     7                       // Подключение пин Nano D7 к пину 10 на L298N   
#define motorB_in4     6                       // Подключение пин Nano D6 к пину 11 на L298N   

#define led12         12                       // Подключение пин Nano D12 к светодиоду
#define led13         13                       // Применение встроенного светодиода Nano D13

#define rele1         11                       // Подключение пин Nano D11 к драйверу (усилителю) реле. Высокий уровень - включить.
#define buzzer         4                       // Подключение пин Nano D4 к драйверу (усилителю) зуммера. Высокий уровень - включить.                

volatile unsigned int pulsesA = 0;             // Счетчик количества импульсов мотора А
volatile unsigned int pulsesB = 0;             // Счетчик количества импульсов мотора B

int int_motorA     = 17;                       // Переменная количества импульсов мотора А
int int_motorB     =  8;                       // Переменная количества импульсов мотора B

int porog_sensorMQ2_1 = 200;                   // Переменная количества порога датчика газа MQ2_1
int porog_sensorMQ2_2 = 200;                   // Переменная количества порога датчика газа MQ2_2

void counterA()
{
 pulsesA++;
}

void counterB()
{
 pulsesB++;
}


void setup() 
{
 Serial.begin(9600);                  // инициализация порта

pinMode(sensorFC03_1, INPUT);         // Настраиваем вход датчика FC03 №1 пин Nano D2 на ввод
pinMode(sensorFC03_2, INPUT);         // Настраиваем вход датчика FC03 №2 пин Nano D3 на ввод

 
 // инициализируем все пины для управления двигателями как outputs
pinMode(motorA_en, OUTPUT);
pinMode(motorB_en, OUTPUT);
pinMode(motorA_in1, OUTPUT);
pinMode(motorA_in2, OUTPUT);
pinMode(motorB_in3, OUTPUT);
pinMode(motorB_in4, OUTPUT);

// выключаем двигатели
digitalWrite(motorA_in1, LOW);
digitalWrite(motorA_in2, LOW);
digitalWrite(motorB_in3, LOW);
digitalWrite(motorB_in4, LOW);

// Настраиваем остальные выводы как outputs
pinMode(led12, OUTPUT);
pinMode(led13, OUTPUT);
pinMode(rele1, OUTPUT);
pinMode(buzzer, OUTPUT);

digitalWrite(led12, LOW);               // выключить светодиод
digitalWrite(led13, LOW);               // выключить светодиод
digitalWrite(rele1, LOW);               // выключить реле
digitalWrite(buzzer,LOW);               // выключить зуммер

attachInterrupt(0, counterA, FALLING);  // Включить прерывания по импульсу от датчика sensorFC03_1
attachInterrupt(1, counterB, FALLING);  // Включить прерывания по импульсу от датчика sensorFC03_2
}

void loop() 
{
 

}

/*
Направление вращения ротора двигателя управляется сигналами HIGH или LOW на каждый привод (или канал). 
Например, для первого мотора, HIGH на IN1 и LOW на IN2 обеспечит вращение в одном направлении,
а LOW и HIGH заставит вращаться в противоположную сторону.
При этом двигатели не будут вращаться, пока не будет сигнала HIGH на пине 7 для первого двигателя
или на 12 пине для второго. Остановить их вращение можно подачей сигнала LOW на те же указанные выше пины.
Для управления скоростью вращения используется ШИМ-сигнал.



 * #define PIN_DO 2 // Установка контакта используемого в Arduino
volatile unsigned int pulses;
float rpm;
unsigned long timeOld;
#define HOLES_DISC 15
 
void counter()
{
 pulses++;
}
 
void setup()
{
 Serial.begin(9600);
 pinMode(PIN_DO, INPUT);
 pulses = 0;
 timeOld = 0;
 attachInterrupt(digitalPinToInterrupt(PIN_DO), counter, FALLING);
}
 
void loop()
{
 if (millis() - timeOld >= 1000)
 {
 detachInterrupt(digitalPinToInterrupt(PIN_DO));
 rpm = (pulses * 60) / (HOLES_DISC);
 Serial.println(rpm); 
 
 timeOld = millis();
 pulses = 0;
 attachInterrupt(digitalPinToInterrupt(PIN_DO), counter, FALLING);
 }
}








// подключите пины контроллера к цифровым пинам Arduino

// первый двигатель

int enA = 10;

int in1 = 9;

int in2 = 8;

// второй двигатель

int enB = 5;

int in3 = 7;

int in4 = 6;

void setup()

{

// инициализируем все пины для управления двигателями как outputs

pinMode(enA, OUTPUT);

pinMode(enB, OUTPUT);

pinMode(in1, OUTPUT);

pinMode(in2, OUTPUT);

pinMode(in3, OUTPUT);

pinMode(in4, OUTPUT);

}

void demoOne()

{

// эта функция обеспечит вращение двигателей в двух направлениях на установленной скорости

// запуск двигателя A

digitalWrite(in1, HIGH);

digitalWrite(in2, LOW);

// устанавливаем скорость 200 из доступного диапазона 0~255

analogWrite(enA, 200);

// запуск двигателя B

digitalWrite(in3, HIGH);

digitalWrite(in4, LOW);

// устанавливаем скорость 200 из доступного диапазона 0~255

analogWrite(enB, 200);

delay(2000);

// меняем направление вращения двигателей

digitalWrite(in1, LOW);

digitalWrite(in2, HIGH);

digitalWrite(in3, LOW);

digitalWrite(in4, HIGH);

delay(2000);

// выключаем двигатели

digitalWrite(in1, LOW);

digitalWrite(in2, LOW);

digitalWrite(in3, LOW);

digitalWrite(in4, LOW);

}

void demoTwo()

{

// эта функция обеспечивает работу двигателей во всем диапазоне возможных скоростей

// обратите внимание, что максимальная скорость определяется самим двигателем и напряжением питания

// ШИМ-значения генерируются функцией analogWrite()

// и зависят от вашей платы управления

// запускают двигатели

digitalWrite(in1, LOW);

digitalWrite(in2, HIGH);

digitalWrite(in3, LOW);

digitalWrite(in4, HIGH);

// ускорение от нуля до максимального значения

for (int i = 0; i < 256; i++)

{

analogWrite(enA, i);

analogWrite(enB, i);

delay(20);

}

// торможение от максимального значения к минимальному

for (int i = 255; i >= 0; --i)

{

analogWrite(enA, i);

analogWrite(enB, i);

delay(20);

}

// теперь отключаем моторы

digitalWrite(in1, LOW);

digitalWrite(in2, LOW);

digitalWrite(in3, LOW);

digitalWrite(in4, LOW);

}

void loop()

{

demoOne();

delay(1000);

demoTwo();

delay(1000);

}
*/