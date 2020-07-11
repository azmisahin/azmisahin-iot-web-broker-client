#include <ESP8266WiFi.h>
#include <PubSubClient.h>

char* ssid = "TT-automation";
char* ssid_password = "12345678";
int brokerPort = 1883;
char* brokerAddress = "broker-test.kiwimobility.com";

char* hub = "iot";
char* topic = "iot/1001";
char* device = "iot-1001";
char* device_id = "iot888888881001";
int pulse = 5000;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char data[MSG_BUFFER_SIZE];
char contents[MSG_BUFFER_SIZE];
int value = 0;


char* reserved = "0xFFFF";
char* command = "*SCOR";
char* vendor = "OM";
char* endInstraction = "#";

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, ssid_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("<=== [");
  Serial.print(topic);
  Serial.print("]");
  Serial.println();
  Serial.println("--------------------------------------------------");

  char* instruction_block;
  instruction_block = (char*)payload;
  Serial.print("<=== [");
  // Serial.print(instruction_block);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.print("]");
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);
    Serial.println("LIGHT OPEN ");
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.println("LIGHT CLOSE ");
  }
  Serial.println();
  Serial.println("==================================================");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = device;

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("wakeup");

      char* instruction = "Q0";

      int voltage = analogRead(0);
      int power = analogRead(1);
      int network_signal = analogRead(2);

      snprintf (contents, MSG_BUFFER_SIZE, "%d,%d,%d", voltage, power, network_signal);
      //snprintf (data, MSG_BUFFER_SIZE, "%s%s,%s,%s,%s,%s%s", reserved, command, vendor, device_id, instruction, contents, endInstraction );
      snprintf (data, MSG_BUFFER_SIZE, "%s,%s,%s,%s,%s%s",  command, vendor, device_id, instruction, contents, endInstraction );

      client.publish(hub, data);


      // ... and resubscribe
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(brokerAddress, brokerPort);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > pulse) {
    lastMsg = now;

    char* instruction = "H0";

    int device_status = 1;
    int current_voltage = analogRead(0);
    int network_signal = analogRead(1);
    int current_power = analogRead(2);
    int charhing_status = analogRead(3);

    snprintf (contents, MSG_BUFFER_SIZE, "%d,%d,%d,%d", current_voltage, network_signal, current_power, charhing_status );
    //snprintf (data, MSG_BUFFER_SIZE, "%s%s,%s,%s,%s,%s%s", reserved, command, vendor, device_id, instruction, contents, endInstraction );
    snprintf (data, MSG_BUFFER_SIZE, "%s,%s,%s,%s,%s%s",  command, vendor, device_id, instruction, contents, endInstraction );

    Serial.print("===> [");
    Serial.print(data);
    Serial.print("]");
    Serial.println();
    Serial.println("==================================================");

    client.publish(topic, data);
  }
}
