#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(10, 197, 12, 6);
IPAddress gateway(10,197,8,1); // Gateway da sua rede local
IPAddress dnsServer(8, 8, 8, 8);   // Servidor DNS (Google DNS)

const char* mqttServerDNS = "test.mosquitto.org";  // Endereço do servidor MQTT
const int mqttServerPort = 1883;

EthernetClient mqttServer;
PubSubClient client(mqttServer);

const int lightPort = 9;

void connectToMqtt() {
  while (!client.connected()) {
    Serial.print("Connecting...");
    if (client.connect("MqttServerClient")) {
      Serial.println("Coonection Stablished");
      client.subscribe("UnoController");  // Subscreve ao tópico para controle do LED
    } else {
      Serial.print("Conection not Stablished, error=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void lightController(char* topic, byte* payload, unsigned int length) {
  String message;
  Serial.print(topic);
  Serial.print(": Recived Message");
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
    Serial.print((char)payload[i]);
  }

  if (message == "on") {
    digitalWrite(lightPort, HIGH);
  } else if (message == "off") {
    digitalWrite(lightPort, LOW);
  }
  message = "";
}

void setup() {

  Serial.begin(9600);
  pinMode(lightPort, OUTPUT);
  Serial.println("Ethernet MQTT Device Server");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, dnsServer, gateway);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  client.setServer(mqttServerDNS, mqttServerPort);
  client.setCallback(lightController);

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  connectToMqtt();
}


void loop() {
  // listen for incoming clients
  if (!client.connected()) {
    connectToMqtt();  // Reconnecta se a conexão cair
  }
  client.loop();  // Mantém a conexão com o servidor MQTT
}
