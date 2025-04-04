#include <Arduino.h>
#include <Wire.h>
#include "MutichannelGasSensor.h"
#include <DHT.h>
#include "Adafruit_CCS811.h"
#include <XBee.h>
#include <ArduinoJson.h>

#define GAS_SENSOR_ADDRESS 0x08
#define AIRQUALITY_SENSOR_ADDRESS 0x5B
#define DHTPIN 0 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Adafruit_CCS811 airQualitySensor;
XBee xbee = XBee();
XBeeAddress64 coordinatorAddress = XBeeAddress64(0x13A200, 0x40E91CAD);

float temp;
float humidity;
uint16_t co2;
uint16_t tvoc;

// put function declarations here:
void wakeUp();
void readTemp();
void readGas();
void readAirQuality();
void I2CChecker();
void I2CReset();
void I2CTest();
void sendData();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600);
  while (!Serial && !Serial1);

  xbee.begin(Serial1);
  
  Serial.println("Initializing!");
  I2CReset();
  Wire.begin();
  if (!airQualitySensor.begin(AIRQUALITY_SENSOR_ADDRESS))
  {
    /* code */
    Serial.println("Failed to start sensor!");
    while (1);
  }
  while (!airQualitySensor.available());
  I2CTest();
  dht.begin();
  delay(5000); // delay 5 extra seconds to let air quality sensor get values that are not 0
}

void loop() {
  //I2CChecker();
  readTemp();
  readAirQuality();
  sendData();
  //readGas();
  
  delay(60000);
}

void readTemp() {
  temp = dht.readTemperature();
  humidity = dht.readHumidity();
  if (!isnan(temp) && !isnan(humidity))
  {
    /* code */
    Serial.print("Temperature: "); Serial.print(temp); Serial.println(" °C");
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

void readAirQuality() {
  if (airQualitySensor.available())
  {
    /* code */
    if (!airQualitySensor.readData())
    {
      /* code */
      co2 = airQualitySensor.geteCO2();
      tvoc = airQualitySensor.getTVOC();
      Serial.print("CO2 (ppm): ");
      Serial.print(co2);
      Serial.print(" | TVOC (ppb): ");
      Serial.println(tvoc);
    } else {
      Serial.println("Error reading CO2 Data");
    }
  }
}

void sendData() {
  Serial.println("Sending data");
  char payload[100];
  StaticJsonDocument<100> doc;
  doc["temp"] = temp;
  doc["humidity"] = humidity;
  doc["co2"] = co2;
  doc["tvoc"] = tvoc;

  size_t jsonLength = serializeJson(doc, payload, sizeof(payload));

  Tx64Request tx = Tx64Request(coordinatorAddress, (u_int8_t*)payload, jsonLength);
  xbee.send(tx);
  Serial.print("Sent Json: ");
  Serial.println(payload);
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