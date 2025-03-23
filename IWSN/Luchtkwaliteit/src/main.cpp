#include <Arduino.h>
#include <Wire.h>
#include "MutichannelGasSensor.h"
#include <DHT.h>

#define GAS_SENSOR_ADDRESS 0x08
#define DHTPIN 0 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// put function declarations here:
void wakeUp();
void readTemp();
void readGas();
void I2CChecker();
void I2CReset();
void I2CTest();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("Initializing!");
  I2CReset();
  Wire.begin();
  I2CTest();
  dht.begin();
}

void loop() {
  readTemp();
  //I2CChecker();
  readGas();
  delay(2000);
}

void readTemp() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (!isnan(temperature) && !isnan(humidity))
  {
    /* code */
    Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  } else {
     Serial.println("Failed to read data from DHT11");
  }
}

void readGas() {
  Wire.beginTransmission(GAS_SENSOR_ADDRESS);
  Wire.write(0x02); // Command to request data
  Wire.endTransmission(false);
  Wire.requestFrom(GAS_SENSOR_ADDRESS, 4); // 1 byte per gas, 4 gasses total

  if (Wire.available() == 4) {
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
        data[i] = Wire.read();
    }
    
    Serial.print("NO2: "); Serial.println(data[0]);
    Serial.print("NO2: "); Serial.println(data[1]);
    Serial.print("NH3: "); Serial.println(data[2]);
    Serial.print("CH4: "); Serial.println(data[3]);
    Serial.println("----------------------");
} else {
    Serial.println("No data received");
  }
}

void wakeUp() {
  Serial.println("Waking up gas sensor"); 
  Wire.beginTransmission(GAS_SENSOR_ADDRESS);
  Wire.write(0x11);
  Wire.endTransmission();
  delay(1000);
}

void I2CChecker() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning for devices...");
  for (address = 1; address < 127; address++)
  {
      /* code */
      Wire.beginTransmission(address);
      error = Wire.endTransmission();

      if (error == 0)
      {
          /* code */
          Serial.print("I2C Device found at 0x");
          Serial.println(address, HEX);
          nDevices++;
      }
      
  }
  
  if (nDevices == 0) Serial.println("No I2C devices found");
  else Serial.println("Done scanning");
  
  delay(5000);
}

void I2CReset() {
  pinMode(7, OUTPUT);
  for (int i = 0; i < 10; i++)
  {
    /* code */
    digitalWrite(7, HIGH);
    delay(5);
    digitalWrite(7, LOW);
    delay(5);
  }
  
  Serial.println("Bus reset!");
}

void I2CTest() {
  Wire.beginTransmission(GAS_SENSOR_ADDRESS);
  byte error = Wire.endTransmission();
  if (error == 0)
  {
    /* code */
    Serial.println("I2C Communication Successfull");
  } else {
    Serial.print("I2C Communication failed, error code: ");
    Serial.println(error);
  }
}