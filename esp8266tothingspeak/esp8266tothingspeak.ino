#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "********";
const char *password = "******";
const int buzzerPin = D3; // Define the buzzer pin

const char *thingSpeakAPI = "http://api.thingspeak.com/update?api_key=IS2P0GXRZ4ZH4HSH";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    int sensor1Value, sensor2Value, sensor3Value, sensor4Value, sensor5Value, sensor6Value;

    char *token = strtok(const_cast<char*>(data.c_str()), ",");
    if (token != nullptr) {
      sensor1Value = atoi(token);
      token = strtok(nullptr, ",");
    }
 
    if (token != nullptr) {
      sensor2Value = atoi(token);
      token = strtok(nullptr, ",");
    }

    if (token != nullptr) {
      sensor3Value = atoi(token);
      token = strtok(nullptr, ",");
    }

    if (token != nullptr) {
      sensor4Value = atoi(token);
      token = strtok(nullptr, ",");
    }

    if (token != nullptr) {
      sensor5Value = atoi(token);
      token = strtok(nullptr, ",");
    }

    if (token != nullptr) {
      sensor6Value = atoi(token);
    }

    float sensor1Float = static_cast<float>(sensor1Value);
    float sensor2Float = static_cast<float>(sensor2Value);
    float sensor3Float = static_cast<float>(sensor3Value);
    float sensor4Float = static_cast<float>(sensor4Value);
    float sensor5Float = static_cast<float>(sensor5Value);
    float sensor6Float = static_cast<float>(sensor6Value);

    Serial.print("Sensor 1: ");
    Serial.println(sensor1Float);
    Serial.print("Sensor 2: ");
    Serial.println(sensor2Float);
    Serial.print("Sensor 3: ");
    Serial.println(sensor3Float);
    Serial.print("Sensor 4: ");
    Serial.println(sensor4Float);
    Serial.print("Sensor 5: ");
    Serial.println(sensor5Float);
    Serial.print("Sensor 6: ");
    Serial.println(sensor6Float);

    // Check if sensor readings are out of range
    if (sensor1Float < 6.5 || sensor1Float > 7.5 || sensor2Float > 1000) {
      digitalWrite(buzzerPin, HIGH);  // Activate buzzer if sensor1 is out of range or sensor2 is high
    } else {
      digitalWrite(buzzerPin, LOW);  // Deactivate buzzer if sensor1 is within range and sensor2 is normal
    }

    // Send data to ThingSpeak
    sendToThingSpeak(sensor1Float, sensor2Float, sensor3Float, sensor4Float, sensor5Float, sensor6Float);
  }
}

void sendToThingSpeak(float sensor1, float sensor2, float sensor3, float sensor4, float sensor5, float sensor6) {
  String url = String(thingSpeakAPI);
  url += "&field1=" + String(sensor1);
  url += "&field2=" + String(sensor2);
  url += "&field3=" + String(sensor3);
  url += "&field4=" + String(sensor4);
  url += "&field5=" + String(sensor5);
  url += "&field6=" + String(sensor6);

  WiFiClient client;  // Create a WiFiClient object

  HTTPClient http;
  http.begin(client, url);  // Pass the WiFiClient object to the begin function

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Data sent to ThingSpeak successfully");
    } else {
      Serial.println("Failed to send data to ThingSpeak");
    }
  } else {
    Serial.println("Error in HTTP request");
  }

  http.end();
}
