#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//WIFI CONFIG
const  char * ssid = "" ;
const  char * password = "" ;

//MQTT CONFIG
const String HOSTNAME  = ""; //NOME DO DEVICE, deverá ter um nome unico.
const char * MQTT_SUBSCREVE = ""; 
const char * MQTT_PUBLIC = ""; //Topico onde o Device publica.

const char* IP_BROKER = ""; //IP ou DNS do Broker MQTT

// Credrenciais ao broker mqtt. Caso nao tenha AUTH meter a false.
#define MQTT_AUTH true
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""


WiFiClient wclient;
PubSubClient client(IP_BROKER, 1883, wclient);

//INICIAR A LIGAÇÃO WIFI
void LigarWifi() {

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Serial.print("\nA tentar ligar ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println(String("\nLigado a rede: (") + ssid + ")" );
}

//INICIAR O MQTT
//Verifica se o estado da ligação está ativa e se não estiver tenta conectar-se
bool checkMqttConnection() {
  if (!client.connected()) {
    if (MQTT_AUTH ? client.connect(HOSTNAME.c_str(), MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str())) {
      Serial.println("Ligado ao broker mqtt " + String(IP_BROKER));
      //SUBSCRIÇÃO DE TOPICOS
      client.subscribe(MQTT_SUBSCREVE);
    }
  }
  return client.connected();
}

void setup() {
  Serial.begin(115200);

  //definir gpio7 como output
  pinMode(D4, OUTPUT);

  //Conecta a rede wifi criada
  LigarWifi();
  client.setCallback(callback); // resposta as mensagens vindas do MQTT
}

//Chamada de recepção de mensagem
void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  String topicStr = String(topic);
  if (topicStr.equals(MQTT_SUBSCREVE)) {
    if (payloadStr.equals("1")) {
      digitalWrite(D4, HIGH);
      client.publish(MQTT_PUBLIC, "1");
      Serial.print("LED LIGOU");
    } else if (payloadStr.equals("0")) {
      digitalWrite(D4, LOW);
      client.publish(MQTT_PUBLIC, "0");
      Serial.print("LED DESLIGOU");
    }

  }

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (checkMqttConnection()) {
      client.loop();
    }
  }
}
