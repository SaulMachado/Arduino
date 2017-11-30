#include <SoftwareSerial.h>

SoftwareSerial SIM900(10, 11); // 10 y 11 para el Arduino Mega. Configura el puerto serie para el SIM900

//HardwareSerial &SIM900 = Serial1;
char incoming_char = 0; //Variable para guardar los caracteres que envia el SIM900


void setup() {
      SIM900.begin(9600); //Configura velocidad del puerto serie para el SIM900
      Serial.begin(9600); //Configura velocidad del puerto serie del Arduino
      Serial.println("OK");
      //delay (1000);
      //SIM900.println("AT+IPR=19200");
      //SIM900.println("AT + CPIN = \"0000\""); //Comando AT para introducir el PIN de la tarjeta
      delay(20000); //Tiempo para que encuentre una RED
      //Serial.println("PIN OK");
      //SIM900.print("AT+CLIP=1\r"); // Activamos la identificación de llamadas
      //delay(100);
      SIM900.print("AT+CMGF=1\r"); //Configura el modo texto para enviar o recibir mensajes
      delay(100);
      SIM900.print("AT+CNMI=2,2,0,0,0\r"); //Configuramos el módulo para que nos muestre los SMS recibidos por comunicacion serie
      delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:
  /*if (Serial.available()) {
        Serial1.write(Serial.read());
    } 
    else if (Serial1.available()) {
        Serial.write(Serial1.read());
    }*/
  if (SIM900.available() > 0)
  {
    incoming_char = SIM900.read(); //Guardamos el caracter que llega desde el SIM900
    Serial.print(incoming_char); //Mostramos el caracter en el monitor serie
  }
}
