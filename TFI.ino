#include <RIC3D.h>
#include <RIC3DMODEM.h>

#define SerialMon Serial

#define SerialAT Serial3

// Module baud rate
uint32_t rate = 115200; 

// Select SIM Card (0 = right, 1 = left)
bool sim_selected = 1; 

const char apn[]      = "grupotesacom.claro.com.ar";
const char gprsUser[] = "";
const char gprsPass[] = "";

const char client_id[]   = "VKfdAfI18N29ANY21fpW"; // aca se debe poner el id del device de tdata

int sensorValue = 0;
float temperature = 0;
float voltage = 0;
char str1[50];
char str2[50];
char str3[50];
char str4[50];
char str5[50];
char str6[50];
char str7[50];
char str8[50];


long current_time = 0;
int graph_time = 10;
long prev_graph = 0;
long publish_time = 5000;
long prev_time = 0;
int i_espirometria = 0;
int t_pulsador = 5000;

float pif = 0;
float volumen_espirometria = 0;
float velocidad_inhalatorio = 0;
long pif_time = 0;
float inicio_pulso = 0;

int prev_pulsador;
int pulsador = 0;

int contador = 0;

int is_pulsado = 0;
int is_inhalacion_buena = 0;
int is_inhalacion_mala = 1;
int error_ambos = 0;
int error_perfil = 0;
int error_acelerometro = 0;
bool periodo_refractario = false;

int capacidad_cartucho = 200;
int dosis_restante = capacidad_cartucho;

int pulso_acelerometro = 0;
int prev_pulso_acelerometro;
bool hay_agitacion = false;
long tiempo_agitacion = 0;
long vencimiento_agitacion = 5000; // Esta variable se usa para indicar el periodo dentro del cual se deberia detectar el pulsador del acelerometro para considerar que hubo una secuencia agitacion-pulsacion
                                   // En este caso la pulsacion se tiene que dar dentro de los 5 segundos tras la agitacion del inhalador.

bool error_led_on = false;
long error_led_duration = 5000;
long error_led_blink_time = 500;
bool error_blink = false;
long error_led_on_start_time = 0;
long error_blink_start_time = 0;

int random_espirometria = 0;

float espirometria_buena[] = {0.87062, 1.2787, 1.7087, 2.1387, 2.5512, 2.9544, 3.3625, 3.77, 4.1569, 4.4887, 4.7444, 4.9462, 5.1637, 5.4087, 5.6369, 5.8112, 5.9031, 5.9631, 6.0231, 6.0937, 6.1481, 6.1912, 6.2675, 6.3331, 6.3387, 6.2731, 6.235, 6.2512, 6.2787, 6.295, 6.3225, 6.3712, 6.3981, 6.3762, 6.3169, 6.2681, 6.2837, 6.3062, 6.29, 6.23, 6.1375, 6.0612, 6.045, 6.0881, 6.1425, 6.1319, 6.045, 5.9362, 5.8437, 5.7675, 5.7287, 5.6912, 5.6206, 5.56, 5.5387, 5.5175, 5.4625, 5.3756, 5.2825, 5.1737, 5.0712, 5.0112, 4.9562, 4.87, 4.755, 4.6412, 4.565, 4.4994, 4.44, 4.3906, 4.3256, 4.2225, 4.075, 3.945, 3.8575, 3.7869, 3.6937, 3.5912, 3.4875, 3.39, 3.2975, 3.21, 3.1175, 2.9925, 2.8675, 2.7637, 2.6662, 2.5844, 2.5025, 2.4, 2.29, 2.1812, 2.095, 2.035, 1.9912, 1.9425, 1.8712, 1.785, 1.6862, 1.6, 1.5075, 1.415, 1.3169, 1.23, 1.1425, 1.0662, 0.97937, 0.88625, 0.7775, 0.66875, 0.56625, 0.4625, 0.34812, 0.23375, 0.13062,0.06,0.00625};
float espirometria_mala[] = {0.348248, 0.51148, 0.68348, 0.85548, 1.02048, 1.18176, 1.345, 1.508, 1.66276, 1.79548, 1.89776, 1.97848, 2.06548, 2.16348, 2.25476, 2.32448, 2.36124, 2.38524, 2.40924, 2.43748, 2.45924, 2.47648, 2.507, 2.53324, 2.53548, 2.50924, 2.494, 2.50048, 2.51148, 2.518, 2.529, 2.54848, 2.55924, 2.55048, 2.52676, 2.50724, 2.51348, 2.52248, 2.516, 2.492, 2.455, 2.42448, 2.418, 2.43524, 2.457, 2.45276, 2.418, 2.37448, 2.33748, 2.307, 2.29148, 2.27648, 2.24824, 2.224, 2.21548, 2.207, 2.185, 2.15024, 2.113, 2.06948, 2.02848, 2.00448, 1.98248, 1.948, 1.902, 1.85648, 1.826, 1.79976, 1.776, 1.75624, 1.73024, 1.689, 1.63, 1.578, 1.543, 1.51476, 1.47748, 1.43648, 1.395, 1.356, 1.319, 1.284, 1.247, 1.197, 1.147, 1.10548, 1.06648, 1.03376, 1.001, 0.96, 0.916, 0.87248, 0.838, 0.814, 0.79648, 0.777, 0.74848, 0.714, 0.67448, 0.64, 0.603, 0.566, 0.52676, 0.492, 0.457, 0.42648, 0.391748, 0.3545, 0.311, 0.2675, 0.2265, 0.185, 0.139248, 0.0935, 0.052248, 0.024, 0.0025};
float senal_nula[117] = {0};

void publicacion(){
  int array_size = 8;
  char* keys_array[array_size];
  char* values_array[array_size];

  dtostrf(1, 4, 2, str1);
  keys_array[0] = "triggers";
  values_array[0] = str1;
  // PublishData("triggers", str);
  SerialMon.println("triggers");
  SerialMon.println(values_array[0]);

  dtostrf(dosis_restante, 4, 2, str2);
  keys_array[1] = "dosis_restante";
  values_array[1] = str2;
  SerialMon.println("dosis_restante");
  SerialMon.println(values_array[1]);

  dtostrf(is_inhalacion_buena, 4, 2, str3);
  keys_array[2] = "inhalacion_buena";
  values_array[2] = str3;
  SerialMon.println("inhalacion_buena");
  SerialMon.println(values_array[2]);


  dtostrf(is_inhalacion_mala, 4, 2, str4);
  keys_array[3] = "inhalacion_mala";
  values_array[3] = str4;
  SerialMon.println("inhalacion_mala");
  SerialMon.println(values_array[3]);


  dtostrf(pif, 4, 2, str5);
  keys_array[4] = "PIF";
  values_array[4] = str5;
  SerialMon.println("PIF");
  SerialMon.println(values_array[4]);


  dtostrf(volumen_espirometria, 4, 2, str6);
  keys_array[5] = "volumen";
  values_array[5] = str6;
  SerialMon.println("volumen");
  SerialMon.println(values_array[5]);


  dtostrf(velocidad_inhalatorio*1000, 4, 2, str7);
  keys_array[6] = "velocidad_inhalatorio";
  values_array[6] = str7;
  SerialMon.println("velocidad_inhalatorio");
  SerialMon.println(values_array[6]);


  if (error_ambos){
    dtostrf(1, 4, 2, str8);
    keys_array[7] = "error_ambos";
    values_array[7] = str8;
    SerialMon.println("error_ambos");
    SerialMon.println(values_array[7]);

  }
  else{
    if (error_perfil){
      dtostrf(1, 4, 2, str8);
      keys_array[7] = "error_inhalacion";
      values_array[7] = str8;
      SerialMon.println("error_inhalacion");
      SerialMon.println(values_array[7]);

    }
    else if (error_acelerometro){
      dtostrf(1, 4, 2, str8);
      keys_array[7] = "error_acelerometro";
      values_array[7] = str8;
      SerialMon.println("error_acelerometro");
      SerialMon.println(values_array[7]);

    }else{
      array_size = 7;
    }
  }
  SerialMon.println("Antes Publish");
  SerialMon.println(values_array[1]);
  PublishDataObject(keys_array, values_array, array_size);
  SerialMon.println("Despues Publish");
  SerialMon.println(values_array[1]);

}

void setup() 
{
  SerialMon.begin(115200);
  Serial2.begin(9600);
  SerialMon.println(F("***********************************************************"));
  SerialMon.println(F(" Initializing Modem"));
  pinMode(SIM_SELECT,OUTPUT);
  digitalWrite(SIM_SELECT,sim_selected);
  SerialMon.print(" Sim selected is the one on the ");
  SerialMon.println(sim_selected?"left":"right");
  ModemTurnOff();
  ModemTurnOn();
  SerialAT.begin(rate);
  analogReference(INTERNAL2V56);
  SerialMon.println(" Opening MQTT service ");
  CreatePDPContext(apn, gprsUser,  gprsPass);
  ActivatePDPContext();
  ConnectMQTTClient(client_id);

  pinMode(DI0, INPUT_PULLUP);

  pinMode(DI1, INPUT_PULLUP);
  // digitalWrite(DI1, LOW);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  // digitalWrite(LED1,HIGH);
  // digitalWrite(LED2,HIGH);
  // digitalWrite(LED3,HIGH);
  // digitalWrite(LED4,HIGH);
}

void loop() 
{
  current_time = millis();
  
  // PULSADOR:
  // Detectamos flanco ascendente del pulso para generar las espirometrias.

  prev_pulsador = pulsador;
  pulsador = 1 - digitalRead(DI0);  

  if (pulsador && !prev_pulsador){
    random_espirometria = random(3);
    i_espirometria = 0;
    pif = 0;
    volumen_espirometria = 0;
    is_pulsado = 1;
    inicio_pulso = current_time;
  }
  else if (!pulsador && prev_pulsador){
    random_espirometria = 0;
  }

  // AGITACION DEL INHALADOR:
  // el equipo esta conectado a un Mega. Cuando el Mega detecta que el acelerometro se agita por encima de un umbral por una duracion minima de 3 segundos manda un pulso de 1 segundo de ancho.
  // Detectamos el flanco ascendente de este pulso para determinar que hubo agitacion del inhalador.

  prev_pulso_acelerometro = pulso_acelerometro;
  pulso_acelerometro = 1 - digitalRead(DI1);

  current_time = millis();

  if (!hay_agitacion){
    if (pulso_acelerometro && !prev_pulso_acelerometro){
      tiempo_agitacion = current_time;
      hay_agitacion = true;
      digitalWrite(LED1,HIGH);
    }
  }
  else{
    if (current_time - tiempo_agitacion >= vencimiento_agitacion){
      hay_agitacion = false;
      periodo_refractario = false;
      digitalWrite(LED1,LOW);
    }
  }

  


  if (current_time - prev_graph > graph_time){
    prev_graph = current_time;
    // SerialMon.print("pulsador:"); SerialMon.print(pulsador*7); Serial.print("  ");
    // SerialMon.print("pulso acelerometro:"); SerialMon.print(pulso_acelerometro*7); Serial.print("  ");
    // SerialMon.print("ref:"); SerialMon.print(8); SerialMon.print(" ");
    // SerialMon.print("modo random:"); SerialMon.print(random_espirometria); SerialMon.print(" ");
    if (random_espirometria == 1){
      if (espirometria_mala[i_espirometria] > pif){
        pif = espirometria_mala[i_espirometria];
        pif_time = current_time;
      }
      volumen_espirometria += espirometria_mala[i_espirometria];
      // SerialMon.print("PIF:"); SerialMon.println(pif); Serial.print("  ");
      // SerialMon.print("Volumen:"); SerialMon.print(volumen_espirometria/100); Serial.print("  ");
      // SerialMon.print("espirometria:"); SerialMon.print(espirometria_mala[i_espirometria]); Serial.print("  ");
    }
    else if (random_espirometria == 2){
      if (espirometria_buena[i_espirometria] > pif){
          pif = espirometria_buena[i_espirometria];
          pif_time = current_time;
        }
      volumen_espirometria += espirometria_buena[i_espirometria];
      // SerialMon.print("PIF:"); SerialMon.println(pif); Serial.print("  ");
      // SerialMon.print("Volumen:"); SerialMon.print(volumen_espirometria/100); Serial.print("  ");
      // SerialMon.print("espirometria:"); SerialMon.print(espirometria_buena[i_espirometria]); Serial.print("  ");
    }
    else if (random_espirometria == 0){
      pif = 0;
      volumen_espirometria += 0;
      // SerialMon.print("PIF:"); SerialMon.println(pif); Serial.print("  ");
      // SerialMon.print("Volumen:"); SerialMon.print(volumen_espirometria/100); Serial.print("  ");
      // SerialMon.print("espirometria:"); SerialMon.print(senal_nula[i_espirometria]); Serial.print("  ");
      
    }
    i_espirometria ++;
    if(i_espirometria > 117){
      i_espirometria = 0;
      if (is_pulsado && !periodo_refractario){
        if (hay_agitacion){
          periodo_refractario = true;
        }
        if (pif > 3 && hay_agitacion){
          is_inhalacion_buena = 1;
          is_inhalacion_mala = 0;
        }
        else{
          error_led_on = true;
          error_blink = true;
          is_inhalacion_buena = 0;
          is_inhalacion_mala = 1;
          if (pif <= 3){
            error_perfil = 1;
          }

          if (!hay_agitacion){
            error_acelerometro = 1;
          }

          if (error_perfil && error_acelerometro){
            error_ambos = 1;
          }
        }
        velocidad_inhalatorio = pif / (pif_time - inicio_pulso);
        dosis_restante -= 1;
        if (dosis_restante == 0){
          dosis_restante = capacidad_cartucho;
        }
        publicacion();
      }
      random_espirometria = 0;
      is_pulsado = 0;
      is_inhalacion_buena = 0;
      is_inhalacion_mala = 1;
      error_ambos = 0;
      error_perfil = 0;
      error_acelerometro = 0;
    }
  }

  current_time = millis();

  if (error_led_on){
    if (error_led_on_start_time == 0){
      error_led_on_start_time = current_time;
    }

    if(current_time - error_blink_start_time >= error_led_blink_time || error_blink_start_time == 0){
      error_blink_start_time = current_time;
      if (error_blink){
        digitalWrite(LED4,HIGH);
        error_blink = false;
      }
      else{
        digitalWrite(LED4,LOW);
        error_blink = true;
      }        
    }
  }

  if (current_time - error_led_on_start_time >= error_led_duration){
    error_led_on = false;
    error_blink = false;
    error_led_on_start_time = 0;
    error_blink_start_time = 0;
    digitalWrite(LED4,LOW);
  }
  
  // while (Serial2.available() > 0){
  //   String acelerometro = Serial2.readString();
  //   // Serial2.print("acelerometro:");
  //   // Serial2.println(acelerometro);
  //   SerialMon.print("acelerometro:");
  //   SerialMon.println(acelerometro);
  // }

}
