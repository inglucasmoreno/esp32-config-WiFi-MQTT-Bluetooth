# esp32-config-WiFi-MQTT-Bluetooth
Configuracion de conexion WiFi y MQTT via Bluetooth

### Requisitos
- Placa ESP32
- Libreria WiFi.h
- Libreria BluetoothSerial.h
- Libreria EEPROM.h
- Libreria PubSubClient.h
- Celular con programa Bluetooth Terminal (Para enviar los comandos)

### Comandos via Bluetooth Terminal
- Setear SSID de red WiFi -> ssid-[ssid de red a conectar]
- Setear Password de red WiFi -> password-[password de red a conectar]
- Setear Direccion IP de Broker MQTT -> mqttserver-[Direccion IP del Broker MQTT]
- Setear Puerto para comunicacion MQTT -> mqttport-[Puerto para comunicacion MQTT]
- Conectar a Red WiFi + Habilitar reconexion -> connect-wifi
- Reconectar WiFi + Habilitar reconexion WiFi -> reconnect-wifi
- Desconectar de Red WiFi + Deshabilitar reconexion -> disconnect-wifi
- Conectar a Broker MQTT + Habilitar reconexion MQTT -> connect-mqtt
- Reconectar MQTT + Habilitar reconexion MQTT -> reconnect-mqtt
- Desconectar de Broker MQTT + Deshabilitar reconexion MQTT -> disconnect-mqtt
- Conectar WiFi y MQTT + Habilitar sus reconexiones -> connect-all
- Desconectar WiFi y MQTT + Deshabilitar sus reconexiones -> disconnect-all
- Reconexion de WiFi y MQTT + Habilitar sus reconexiones -> reconnect-all

Nota: No colocar los corchetes [ ]

### Memoria EEPROM
- Se aprovecha la memoria EEPROM de la ESP32 para almacenar SSID y Password de Red WiFi, ademas del Servidor MQTT y el Puerto de conexion MQTT
- Cuando el sistema inicia, levanta automaticamente SSID y Password de Red WiFi a conectar, ademas de la IP del servidor MQTT y puerto de conexion MQTT desde la memoria

### Builtin LED
- Se aprovecha el Builtin LED de la ESP 32 para indicar estado de conexion
- BUILTIN LED Apagado -> Desconectado de red WiFi y/o MQTT | Ademas esta deshabilitada la reconexion
- BUILTIN LED Parpadeando ( rapido [200ms] ) -> Intentando de conectar a Red WiFi
- BUILTIN LED Parpadeando muy lento -> Intentando de conectar al Broker MQTT
- BUILTIN LED Encendido -> Conexion a red WiFi y MQTT Broker establecida

