#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include "RTClib.h"
 
 //Declaracion de variables para la Shield Ethernet
//Asignamos una dirección MAC
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

//Asignar dirección IP
IPAddress ip(192,168,1,200);
IPAddress servidor_dns(8,8,8,8);
IPAddress puerta_enlace(192,168,1,1);
IPAddress mascara_red(255,255,255,0);
 
//Inicializar instancia de la libreria ethernet
EthernetClient client;
 
//Dirección IP del servidor con la página PHP
char server[] = "192.168.1.250"; 
 
//Variables que neceita para funcionar
String codigo;        //Aquí se almacena la respuesta del servidor
//String id_mySql;      //Aqui se almacena el id del horario obtenido de mysql
String hora1_mySql;        //Aquí se almacena la hora1 que recuperamos de MySQL
String hora2_mySql;        //Aquí se almacena la hora2 que recuperamos de MySQL
String hora3_mySql;        //Aquí se almacena la hora3 que recuperamos de MySQL
String descanso_mySql;        //Aquí se almacena el valor de descanso que recuperamos de MySQL
String hora4_mySql;        //Aquí se almacena la hora4 que recuperamos de MySQL
String hora5_mySql;        //Aquí se almacena la hora5 que recuperamos de MySQL
String hora6_mySql;        //Aquí se almacena la hora6 que recuperamos de MySQL
String salida_mySql;        //Aquí se almacena la salida que recuperamos de MySQL
boolean fin = false;  
boolean pregunta = true;

/* DECLARACION DE VARIABLES PARA RTC-DS3231*/
RTC_DS3231 rtc; 
String daysOfTheWeek[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
String monthsNames[12] = { "Enero", "Febrero", "Marzo", "Abril", "Mayo",  "Junio", "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };
String hora;

/* DECLARACION DE VARIABLES PARA EL RELÉ*/
//Variables para relé
const int control = 2;

void setup() {
  // Iniciar puerto serie
  Serial.begin(9600);
  // Dar un respiro a Arduino
  delay(500);
  //Habilitamos los pines por donde será controlado el relé
  pinMode(control, OUTPUT);
  //digitalWrite(control,HIGH);
  delay(1000);
  //Iniciar la coneción de red
  Ethernet.begin(mac, ip);
  // Imprimir la dirección IP
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  delay(1000);
  //Inicializamos el RTC
  if (!rtc.begin()) {
      Serial.println(F("No hay modulo RTC"));
      while (1);
   }
   if (rtc.lostPower()) {
      // Fijar a fecha y hora de compilacion
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }
   //Mostramos la fecha y hora
   mostrar_fecha();
}

void loop() {
  //ejecutamos la recepcion de mensajes, recibimos el horario
  if (Serial.available()) {
    Serial1.write(Serial.read());
  } 
  else if (Serial1.available()) {
    Serial.write(Serial1.read());
  }
  if(pregunta){
    httpRequest(8);
    pregunta = false;
  }

  horario_manana(rtc.now());


  //hora = getHora(rtc.now());
  //Serial.println(hora);

  //while(1);
}

void mostrar_fecha(){
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}
// Con esta función hacemos la conecion con el servidor
int httpRequest(int identificador) {
 
  // Comprobar si hay conexión
  if (client.connect(server, 80)) {
    Serial.println("Conectado");
    // Enviar la petición HTTP
    //Dirección del archivo php dentro del servidor
    //client.print("GET conexion_arduino.php?id=");
    client.print("GET /Timbre/conexion_arduino.php?horarios%5B%5D=");
    //Mandamos la variable junto a la línea de GET
    client.print(identificador);
    //client.print("&enviar=Enviar");
    client.println(" HTTP/1.1");
    //IP del servidor
    client.println("Host: 192.168.1.250");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
  }
  else {
    // Si no conseguimos conectarnos
    Serial.println("Conexión fallida");
    Serial.println("Desconectando");
    client.stop();
  }
 
  delay(500);
  //Comprobamos si tenemos respuesta del servidor y la 
  //almacenamos en el string ----> codigo.
  while (client.available()) {
    char c = client.read();
    codigo += c;
    //Habilitamos la comprobación del código recibido
    fin = true;
 
  }
  //Si está habilitada la comprobación del código entramos en el IF
  if (fin)  {
   // Serial.println(codigo);
   //Analizamos la longitud del código recibido
    int longitud = codigo.length();
    //Buscamos en que posición del string se encuentra nuestra variable
    int hora1 = codigo.indexOf("HORA1=");
    int hora2 = codigo.indexOf("HORA2=");
    int hora3 = codigo.indexOf("HORA3=");
    int descanso = codigo.indexOf("DESCANSO=");
    int hora4 = codigo.indexOf("HORA4=");
    int hora5 = codigo.indexOf("HORA5=");
    int hora6 = codigo.indexOf("HORA6=");
    int salida = codigo.indexOf("SALIDA=");
    //Borramos lo que haya almacenado en el string 
    hora1_mySql= "";
    hora2_mySql= "";
    hora3_mySql= "";
    descanso_mySql="";
    hora4_mySql= "";
    hora5_mySql= "";
    hora6_mySql= "";
    salida_mySql= "";
 
    //Analizamos el código obtenido y almacenamos el nombre en el string nombre
    for (int i = hora1 + 6; i < longitud; i ++){
      if (codigo[i] == ';') i = longitud;
      else hora1_mySql += codigo[i];
    }
    for (int i = hora2 + 6; i < longitud; i ++){
      if (codigo[i] == ';') i = longitud;
      else hora2_mySql += codigo[i];
    }
    for (int i = hora3 + 6; i < longitud; i ++){
      if (codigo[i] == ';') i = longitud;
      else hora3_mySql += codigo[i];
    }
    for (int i = descanso + 9; i < longitud; i ++){
      if (codigo[i] == ';') i = longitud;
      else descanso_mySql += codigo[i];
    }
    for (int i = hora4 + 6; i < longitud; i ++){
      if (codigo[i] == ';') i = longitud;
      else hora4_mySql += codigo[i];
    }
    for (int i = hora5 + 6; i < longitud; i ++){
      if (codigo[i] == ';') i = longitud;
      else hora5_mySql += codigo[i];
    }
    for (int i = hora6 + 6; i < longitud; i ++){
      if (codigo[i] == ';') i = longitud;
      else hora6_mySql += codigo[i];
    }
    for (int i = salida + 7; i < longitud; i ++){
      if (codigo[i] == ';') i = longitud;
      else salida_mySql += codigo[i];
    }
    //Deshabilitamos el análisis del código
    fin = false;
    //Imprimir el nombre obtenido
    Serial.println("Valor de la Hora 1 es: " + hora1_mySql);
    Serial.println("Valor de la Hora 2 es: " + hora2_mySql);
    Serial.println("Valor de la Hora 3 es: " + hora3_mySql);
    Serial.println("Valor del descanso es: " + descanso_mySql);
    Serial.println("Valor de la Hora 4 es: " + hora4_mySql);
    Serial.println("Valor de la Hora 5 es: " + hora5_mySql);
    Serial.println("Valor de la Hora 6 es: " + hora6_mySql);
    Serial.println("Valor de la salida es: " + salida_mySql);
    //Cerrar conexión
    Serial.println("Desconectar");
    client.stop();
  }
  //Borrar código y salir de la función//Dirección IP del servidor
  codigo="";
  return 1;
}
String getHora(DateTime date){
  char horap[8];
  int h = (date.hour());
  int m = (date.minute());
  int s = (date.second());

  sprintf(horap, "%.2d:%.2d:%.2d", h,m ,s);
  return String(horap);
}

void horario_manana(DateTime now){

  String hora_actual = getHora(now);
  if(daysOfTheWeek[now.dayOfTheWeek()] == "Lunes" || daysOfTheWeek[now.dayOfTheWeek()] == "Martes" || daysOfTheWeek[now.dayOfTheWeek()] == "Miercoles"
    || daysOfTheWeek[now.dayOfTheWeek()] == "Jueves" || daysOfTheWeek[now.dayOfTheWeek()] == "Viernes"){
    //Serial.println("Hoy es dia de semana");
    //Serial.print("la hora actual es: ");
    Serial.println(hora_actual);
    if(hora1_mySql == hora_actual){
      digitalWrite(control, HIGH);
      delay(3000);
      digitalWrite(control, LOW);
    }
    if(hora2_mySql == hora_actual){
      digitalWrite(control, HIGH);
      delay(3000);
      digitalWrite(control, LOW);
    }
    if(hora3_mySql == hora_actual){
      digitalWrite(control, HIGH);
      delay(3000);
      digitalWrite(control, LOW);
    }
    if(descanso_mySql == hora_actual){
      digitalWrite(control, HIGH);
      delay(3000);
      digitalWrite(control, LOW);
    }
    if(hora4_mySql == hora_actual){
      digitalWrite(control, HIGH);
      delay(3000);
      digitalWrite(control, LOW);
    }
    if(hora5_mySql == hora_actual){
      digitalWrite(control, HIGH);
      delay(3000);
      digitalWrite(control, LOW);
    }
    if(hora6_mySql == hora_actual){
      digitalWrite(control, HIGH);
      delay(3000);
      digitalWrite(control, LOW);
    }
    if(salida_mySql == hora_actual){
      digitalWrite(control, HIGH);
      delay(3000);
      digitalWrite(control, LOW);
    }
  }
}

