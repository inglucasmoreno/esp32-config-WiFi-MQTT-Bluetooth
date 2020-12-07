/*
  Configuración de conexion WiFi via Bluetooth
  Autor: Ing. Moreno Lucas Omar  
  Año: 2020
 */

#include "BluetoothSerial.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <PubSubClient.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

// Credenciales WiFi
String ssid = "";
String password = "";

// Credenciales MQTT
String mqtt_server = "";
int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

// Flags
bool wifi_enable = true;
bool mqtt_enable = true;

// Funcion: Grabar EEPROM
void grabar_eeprom(int addr, String text){
  int tamano = text.length();
  char inChar[50];
  text.toCharArray(inChar, tamano+1);
  for(int i = 0; i < tamano; i++){
    EEPROM.write(addr+i, inChar[i]);
  }
  for(int i = tamano; i < 50; i++){
    EEPROM.write(addr+i, 255);    
  }
  EEPROM.commit();
}

// Funcion: Leer EEPROM
String leer_eeprom(int addr){
  byte lectura;
  String strLectura;
  for(int i = addr; i < addr+50; i++){
    lectura = EEPROM.read(i);
    if(lectura != 255){
      strLectura += (char)lectura;
    }
  }
  return strLectura;
}

// Funcion: Conexion/Reconexion a MQTT
void reconnect_mqtt(){
  Serial.println("");
  Serial.println("Intentando conexion a MQTT");
  while(!client.connected() && WiFi.status() == WL_CONNECTED && mqtt_enable){
    if(SerialBT.available()) decodificadorSerial();
    String clientId = "iot_1_";
    clientId = clientId + String(random(0xffff, HEX));    
    if(client.connect(clientId.c_str())){
      Serial.println("Conexion Exitosa a Broker MQTT...");
    }else{
      Serial.println("Fallo de conexion");
      Serial.println(client.state());
      Serial.println("Se intentara de nuevo en 2 segundos");
      digitalWrite(BUILTIN_LED, HIGH);
      delay(1000);
      digitalWrite(BUILTIN_LED, LOW);
      delay(1000);
    }    
  }  
}

// Funcion: Conexion/Reconexion a red WiFi
void reconnect_wifi(){
  Serial.println("Conexion WiFi");
  Serial.print("Conectando a -> ");
  Serial.println(ssid);
  WiFi.begin(ssid.c_str(), password.c_str());
  while(WiFi.status() != WL_CONNECTED && wifi_enable){
    if(SerialBT.available()) decodificadorSerial();       // -> Se puede mandar un comando mientras reconecta     
    Serial.print(".");
    delay(200);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(200);
    digitalWrite(BUILTIN_LED, LOW);
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.println("Conexion exitosa!");
  
    Serial.println("Mi IP es -> ");
    Serial.println(WiFi.localIP());    
  }
}

// Funcion: Se realiza la accion en funcion al "comando" y "valor" recibido
void realizarAccion(String comando, String valor){
  
  // Seteo de SSID - WiFi
  if(comando.equals("ssid")){
    Serial.print("Nuevo SSID configurado - ");
    Serial.println(valor);
    ssid = valor;
    grabar_eeprom(0, valor); // 0 - 50 -> SSID
    SerialBT.write('y');
  }
  // Seteo de Password - WiFi
  else if(comando.equals("password")){
    Serial.print("Nuevo Password configurado - ");
    Serial.println(valor);
    password = valor;
    grabar_eeprom(50, valor); // 50 - 100 -> Password
    SerialBT.write('y');
  }
  // Conexion
  else if(comando.equals("connect")){
    // Conectar a red WiFi + Habilitar la reconexion WiFi
    if(valor.equals("wifi")){
      reconnect_wifi();
      wifi_enable = true;
    // Conectar a server MQTT + Habilitar la reconexion MQTT      
    }else if(valor.equals("mqtt")){
      mqtt_enable = true;
    // Conexion total   
    }else if(valor.equals("all")){
      wifi_enable = true;
      mqtt_enable = true;     
    }
    SerialBT.write('y');
  }
  // Desconexion
  else if(comando.equals("disconnect")){
    // Desconexion de Red WiFi + Inhabilitar la reconexion WiFi
    if(valor.equals("wifi")){
      WiFi.disconnect();
      wifi_enable = false;
      Serial.println("Conexion WiFi finalizada..."); 
    }
    // Desconectar del servidor MQTT + Inhabilitar la reconexion MQTT
    else if(valor.equals("mqtt")){
      client.disconnect();
      mqtt_enable = false;
      Serial.println("Conexion MQTT finalizada...");
    }
    // Desconexion Total
    else if(valor.equals("all")){
      client.disconnect();
      WiFi.disconnect();
      wifi_enable = false;
      mqtt_enable = false;
      Serial.println("Conexion WiFi y MQTT finalizadas...");
    }
    SerialBT.write('y');
    delay(100);
  }
  // Reconexion
  else if(comando.equals("reconnect")){
    // Reconexion a red WiFi
    if(valor.equals("wifi")){
      Serial.println("Reconectando WiFi....");
      WiFi.disconnect();
      wifi_enable = true;    
    }
    // Reconexion a servidor MQTT
    else if(valor.equals("mqtt")){
      Serial.println("Reconectando MQTT....");
      client.disconnect();
      mqtt_enable = true;
    }
    // Reconexion total
    else if(valor.equals("all")){
      Serial.println("Reconexion total....");
      WiFi.disconnect();
      client.disconnect();
      wifi_enable = true;
      mqtt_enable = true;
    }
    SerialBT.write('y');
    delay(100);
  }
  
  // Seteo de Servidor MQTT
  else if(comando.equals("mqttserver")){
    mqtt_server = valor;
    grabar_eeprom(100, valor);  // 100 - 150 -> MQTT Server (Direccion IP)
    Serial.print("Nuevo MQTT Server configurado - ");
    Serial.println(valor);
    SerialBT.write('y');    
  }
  
  // Seteo de Puerto MQTT
  else if(comando.equals("mqttport")){
    mqtt_port = valor.toInt();
    grabar_eeprom(150, valor);  // 150 - 200 -> MQTT Port
    Serial.print("Nuevo puerto MQTT configurado - ");
    Serial.println(valor);
    SerialBT.write('y');    
  }
  
 // --------------- Comando desconocido ----------------
 
  else{
    Serial.println("Comando desconocido");
    SerialBT.write('n');
  }
  
}

// Funcion: Decodificacion de mensaje Bluetooth
void decodificadorSerial(){
  String mensaje = SerialBT.readStringUntil('\n');
  Serial.print("Mensaje: ");
  Serial.println(mensaje);  
  int posicionGuion = mensaje.indexOf('-');
  int posicionSaltoLinea = mensaje.length();
  String comando = mensaje.substring(0,posicionGuion);
  String valor = mensaje.substring(posicionGuion + 1, posicionSaltoLinea);
  Serial.print("Comando: ");
  Serial.print(comando);
  Serial.print(" | ");
  Serial.print("Valor: ");
  Serial.println(valor);
  realizarAccion(comando, valor);
}

// Funcion: Configuracion inicial de WiFi
void config_wifi(){
  ssid = leer_eeprom(0);
  password = leer_eeprom(50);  
}

// Funcion: Configuracion inicial de bluetooth
void config_bluetooth(){
  SerialBT.begin("ESP32_ET"); // Nombre de dispositivo Bluetooth
  Serial.println("Dispositivo inicializado...");
  Serial.println("Ahora puedes establecer una conexion bluetooth!");  
}

void config_mqtt(){
  mqtt_server = leer_eeprom(100);
  mqtt_port = leer_eeprom(150).toInt();
  client.setServer(mqtt_server.c_str(), mqtt_port);  
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  config_bluetooth();
  config_wifi();
  config_mqtt();
}

void loop() {

  // Led - Conexion WiFi y MQTT correcta
  if(WiFi.status() == WL_CONNECTED && client.connected()) digitalWrite(BUILTIN_LED, HIGH);
  else digitalWrite(BUILTIN_LED, LOW);
  
  // Reconexion WiFi
  if(WiFi.status() != WL_CONNECTED && wifi_enable) reconnect_wifi();

  // Reconexion MQTT 
  if(!client.connected() && WiFi.status() == WL_CONNECTED && mqtt_enable) reconnect_mqtt();
  client.loop();
 
  // Bluetooth -> Puerto Serie
  if(SerialBT.available()) decodificadorSerial();
  
  delay(20);
}
