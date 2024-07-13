#include <WiFiS3.h>

char ssid[] = "UbeeB402-2.4G";        // your network SSID (name)
char pass[] = "TASKAM0123";    // your network password (use for WPA, or use as key for WEP)
int status;     // the WiFi radio's status

// Variables para el sensor ultrasonico
int trigPin = 12;
int echoPin = 11;
int pingTravelTime;
int pinSpeaker = 8;

// IP o dominio del servidor y puerto
const char* server = "localhost:8080";
const int port = 80;  // Puerto para HTTP es 80
// Dirección IP del servidor local
IPAddress serverIP(192, 168, 0, 11);


WiFiClient client;  // Cliente WiFi para la conexión

void setup()
{
    Serial.begin(9600);
    networkInitialize();
    
    
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(pinSpeaker, OUTPUT);
}

void loop()
{
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  pingTravelTime = pulseIn(echoPin, HIGH);
  beep(pingTravelTime);
  delay(25);
  digitalWrite(trigPin, LOW);
  Serial.println(pingTravelTime);
}

void beep(int travelTime){
  if(travelTime > 250 && travelTime < 700){
    tone(pinSpeaker, 1500); 
    delay(200);             
    noTone(pinSpeaker);      
    delay(100);        
  } else if(travelTime < 250 && travelTime != 0){
    tone(pinSpeaker, 1000); 
    sendInfoToPaulLocal();
    delay(15000);             
    noTone(pinSpeaker);
  }
}

void sendInfoToPaulLocal(){
  // Hacer una solicitud HTTP GET
  if (client.connect(serverIP, 8080)) {
    Serial.println("Conectado al servidor local");

    // Hacer la solicitud GET al recurso deseado
    client.print("GET /wh HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(serverIP);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");

    // Leer la respuesta del servidor
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
      }
    }
    // Cerrar la conexión
    client.stop();
    Serial.println("\nConexión cerrada");
  } else {
    Serial.println("Error al conectar al servidor local");
  }
}

void sendInfoToPaulRemote(){
    // Hacer una solicitud HTTP GET
  if (client.connect(server, port)) {
    Serial.println("Conectado al endpoint para enviar informacion");
    
    // Hacer la solicitud GET al recurso deseado
    client.print("GET /hello HTTP/1.1\r\n");
    client.print("Host: ");
    client.print(server);
    client.print("\r\n");
    client.print("Connection: close\r\n\r\n");

    // Leer la respuesta del servidor
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }
    // Cerrar la conexión
    client.stop();
    Serial.println("\nConexión cerrada");
  } else {
    Serial.println("Error al conectar al servidor");
  }

}

void networkInitialize(){

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
  Serial.println();
}
