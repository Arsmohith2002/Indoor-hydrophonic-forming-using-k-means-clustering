#include <DHT.h>

int pHSense = A0;
int tdsSense = A1;
int tempSense = A2;
int waterLevelSense = A3;  // Assuming water level sensor is connected to analog pin A3
int dhtPin = 7;            // Assuming DHT11 sensor is connected to digital pin 7
int buzzerPin = 12;        // Buzzer pin
int fanPin1 = 8;           // First fan relay pin
int fanPin2 = 9;           // Second fan relay pin
int pumpPin = 10;          // Pump relay pin
int drainPumpPin = 11;      // Drain pump relay pin
int ledPin = 13;     
const unsigned long duration = 7UL * 24 * 60 * 60 * 1000;
// Define the duration for the LED to be off after 7 days (30 minutes = 30 * 60 * 1000 milliseconds)
const unsigned long offDuration = 30UL * 60 * 1000;

unsigned long startTime;
DHT dht(dhtPin, DHT11);

void setup() {
  Serial.begin(9600);
  pinMode(pHSense, INPUT);
  pinMode(tdsSense, INPUT);
  pinMode(tempSense, INPUT);
  pinMode(waterLevelSense, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(fanPin1, OUTPUT);
  pinMode(fanPin2, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(drainPumpPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  startTime = millis();
  dht.begin();
}

void loop() {
  // Read analog value from pH sensor
  int pHValue = analogRead(pHSense);
  float pHVoltage = pHValue * (3/ 1023.0);
  float pH = map(pHVoltage * 100, 0, 5 * 100, 0, 14 * 100) / 100.0;

  // Read analog value from TDS sensor
  int tdsValue = analogRead(tdsSense);
  float tdsVoltage = tdsValue * (3.3 / 1024.0);
  // Adjust this calibration factor based on your sensor
  float tdsCalibrationFactor = 500;
  float tds = tdsVoltage * tdsCalibrationFactor;

  // Read analog value from temperature sensor
  int tempValue = analogRead(tempSense);
  float tempVoltage = tempValue * (1.6/ 1023.0);
  // Assuming a linear relationship between voltage and temperature, adjust this calibration factor based on your sensor
  float tempCalibrationFactor = 100.0;
  float temperature = tempVoltage * tempCalibrationFactor;

  // Read analog value from water level sensor
  int waterLevelValue = analogRead(waterLevelSense);
  // Adjust this calibration factor based on your water level sensor
  float waterLevelCalibrationFactor = 100.0;
  float waterLevel = waterLevelValue * waterLevelCalibrationFactor;

  // Read data from DHT11 sensor
  float humidity = dht.readHumidity();
  float dhtTemperature = dht.readTemperature();

  // Control buzzer based on pH and TDS levels
  if (pH < 5.5 || pH > 6.5) {
    digitalWrite(buzzerPin, LOW);  // Activate buzzer if pH is out of range or TDS is high
  } else {
    digitalWrite(buzzerPin, HIGH);  // Deactivate buzzer if pH is within range and TDS is normal
  }
  if (tds > 1000) {
    digitalWrite(ledPin, LOW );  // Activate buzzer if pH is out of range or TDS is high
  } else {
    digitalWrite(ledPin, HIGH);  // Deactivate buzzer if pH is within range and TDS is normal
  }

  // Control fan based on temperature
  if (dhtTemperature < 28) {
    digitalWrite(fanPin1, HIGH);  // Activate first fan if temperature is high
    digitalWrite(fanPin2, HIGH);  // Activate second fan if temperature is high
  } else {
    digitalWrite(fanPin1, LOW);  // Deactivate first fan if temperature is normal
    digitalWrite(fanPin2, LOW);  // Deactivate second fan if temperature is normal
  }

  // Control pump based on water level
  if (waterLevel < 67000) {
    digitalWrite(pumpPin, LOW);  // Activate pump if water level is low
  } else {
    digitalWrite(pumpPin, HIGH);  // Deactivate pump if water level is normal
  }
 unsigned long elapsedTime = millis() - startTime;
 // Serial.println(elapsedTime);
  // Check if the elapsed time is less than the duration
  if (elapsedTime < duration) {
    // Turn the drainPumpPin on (HIGH is the voltage level)
    digitalWrite(drainPumpPin, HIGH);
     if (tds < 1000) {
    digitalWrite(drainPumpPin,HIGH);  // Activate buzzer if pH is out of range or TDS is high
   } else {
        unsigned long pumpOffTime = startTime + duration + 30 * 60 * 1000; // Calculate the time to turn off the pump
        if (millis() >= pumpOffTime) {
            // Turn off the drain pump after 30 minutes
            digitalWrite(drainPumpPin, LOW);
        }
    }
  } else if (elapsedTime < duration + offDuration) {
    // Turn the drainPumpPin off
    digitalWrite(pumpPin, HIGH); 
    digitalWrite(drainPumpPin, LOW);
  } else {
    // Reset the start time to begin the next cycle
    startTime = millis();
  }
  // Drain water if it has been present for more than a week
  
  // Print sensor data to serial monitor
  //Serial.print("pH: ");
  Serial.print(pH);
  Serial.print(",");
  Serial.print(tds);
  Serial.print(",");
  Serial.print(temperature);
  Serial.print(",");
  Serial.print(humidity);
  Serial.print(",");
  Serial.print(dhtTemperature);
  Serial.print(",");
  Serial.println(waterLevel);

  delay(3000);  // Delay for 3 seconds before the next measurement
}
