// Librerias I2C para controlar el mpu6050
// la libreria MPU6050.h necesita I2Cdev.h, I2Cdev.h necesita Wire.h
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// La dirección del MPU6050 puede ser 0x68 o 0x69, dependiendo 
// del estado de AD0. Si no se especifica, 0x68 estará implicito
MPU6050 sensor;

// Valores RAW (sin procesar) del acelerometro y giroscopio en los ejes x,y,z
int ax, ay, az;
int gx = 0; int gy = 0; int gz = 0;

float num_readings = 20;
float gx_array[20] = {0};
float gx_rms = 0;
int gx_i = 0;
float gy_array[20] = {0};
float gy_rms = 0;
int gy_i = 0;
float gz_array[20] = {0};
float gz_rms = 0;
int gz_i = 0;
float rms_total = 0;
float prev_rms_total;


bool is_agitando = false;
int minima_duracion_agitacion = 3000; //el minimo tiempo que debe estar agitando para ser considerado buena
int umbral_agitacion = 20;
long prev_time_agitacion = 0;
long current_time = 0;
long duracion_agitacion = 0;
bool led_encendido = false;
long prev_time_led = 0;
int duracion_encendido_led = 2000;
bool send_pulse = false;
int pulse_duration = 1000;
long prev_time_pulse = 0;
int pulso_agitacion=0;


float rms(float array[]){
  float suma = 0;
  for (int i=0;i<num_readings;i++){
    suma += pow(array[i],2);
  }
  return sqrt(suma/num_readings);
}

String message;

void setup() {
  Serial.begin(9600);    //Iniciando puerto serial
  Wire.begin();           //Iniciando I2C  
  sensor.initialize();    //Iniciando el sensor
  // pinMode(4, OUTPUT);

  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, LOW);

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  if (sensor.testConnection()) Serial.println("Sensor iniciado correctamente");
  else Serial.println("Error al iniciar el sensor");
}

void loop() {

  // Leer las aceleraciones y velocidades angulares
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);

  gx_array[gx_i] = gx;
  gx_array[gy_i] = gy;
  gx_array[gz_i] = gz;

  gx_rms = rms(gx_array);
  gy_rms = rms(gy_array);
  gz_rms = rms(gz_array);

  gx_i ++;
  gy_i ++;
  gz_i ++;

  if (gx_i == num_readings){
    gx_i = 0;
  }
  if (gy_i == num_readings){
    gy_i = 0;
  }
  if (gz_i == num_readings){
    gz_i = 0;
  }


  //Mostrar las lecturas separadas por un [tab]

  // message += String(ax);
  // message += " ";
  // message += String(ay);
  // message += " ";
  // message += String(az);
  // message += " ";
  // message += String(gx);
  // message += " ";
  // message += String(gy);
  // message += " ";
  // message += String(gz);
  // message += "\n";

  // Serial.println(message);

  // Serial.print("ax:");
  // Serial.print(ax);
  // Serial.print(" ");
  // Serial.print("ay:");
  // Serial.print(ay);
  // Serial.print(" ");
  // Serial.print("az:");
  // Serial.print(az);
  // Serial.print(" ");
  // Serial.print("gx:");
  // Serial.print(gx);
  // Serial.print(" ");
  // Serial.print("gy:");
  // Serial.print(gy);
  // Serial.print(" ");
  // Serial.print("gz:");
  // Serial.println(gz);

  prev_rms_total = rms_total;

  rms_total = (gx_rms + gy_rms + gz_rms)/1000;
  Serial.print(30);
  Serial.print(" ");
  Serial.print("umbral_agitacion:");
  Serial.print(umbral_agitacion);
  Serial.print(" ");
  Serial.print("rms_total:");
  Serial.println(rms_total);
  Serial.print(" ");
  Serial.print("pulso_agitacion:");
  Serial.println(pulso_agitacion);


  current_time = millis();
  if (!is_agitando && !send_pulse){
    if (rms_total >= umbral_agitacion && prev_rms_total < umbral_agitacion){
      prev_time_agitacion = current_time;
      is_agitando = true;
    }
  }
  else{
    if(rms_total >= umbral_agitacion){
      duracion_agitacion = current_time - prev_time_agitacion;
    }
    else{
      is_agitando = false;
    }
  }

  if (duracion_agitacion >= minima_duracion_agitacion){
    digitalWrite(4, HIGH);
    prev_time_pulse = current_time;
    send_pulse = true;
    is_agitando = false;
    pulso_agitacion = 20;
    duracion_agitacion = 0;
  }
  
  if (send_pulse){
    if (current_time - prev_time_pulse >= pulse_duration){
      digitalWrite(4, LOW);
      pulso_agitacion = 0;
      send_pulse = false;
    }
  }

  // ESTO ES PARA DEBUGEAR EL CODIGO SOLO USANDO EL MEGA. CUANDO EL ACELEROMETRO SUPERA EL UMBRAL PRENDE EL LED DEL MEGA POR 2 SEGUNDOS Y DESPUES LO APAGA
  // 
  // if (duracion_agitacion >= minima_duracion_agitacion){
  //   digitalWrite(LED_BUILTIN, HIGH);
  //   led_encendido = true;
  //   prev_time_led = current_time;
  //   is_agitando = false;
  //   duracion_agitacion = 0;
  // }

  // if (led_encendido){
  //   if (current_time - prev_time_led >= duracion_encendido_led){
  //     digitalWrite(LED_BUILTIN, LOW);
  //     led_encendido = false;
  //   }
  // }


  delay(10);
}