import serial
import time
import json
import paho.mqtt.client as mqtt
from influxdb import InfluxDBClient

MQTT_BROKER = 'localhost'
MQTT_TOPIC = "sensor/data"
MQTT_PORT = 1883

DB_HOST = "localhost"
DB_PORT = 8086
DB_NAME = "sensor_data"

SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600
TIMEOUT = 1

xbee_serial = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)

mqtt_client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
        print(f"Connected to MQTT Broker with result code {rc}")

def connect_mqtt():
        mqtt_client.on_connect = on_connect
        mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

def read_xbee_data():
        if xbee_serial.in_waiting > 0:
                data = xbee_serial.readline().decode('utf-8', errors='ignore').strip()
                return data
        return None

def publish_to_mqtt(data):
        if data:
                print("Publishing data to MQTT: {data}")
        mqtt_client.publish(MQTT_TOPIC, data)

def clean_data(data):
        start_index = data.find("{")
        if start_index != -1:
                json_data = data[start_index:]
                end_index = json_data.rfind("}") + 1
                json_clean = json_data[:end_index]
                parsed_data = json.loads(json_clean)
                try:
                        print(f"Parsed JSON: {parsed_data}")
                        return parsed_data
                except json.JSONDecodeError:
                        print("Invalid JSON data received")
                        return None

def insert_data(db_client, data):
        json_body = [
                {
                        "measurement": "sensor_readings",
                        "fields": {
                                "temp": float(data.get("temp", 0)),
                                "humidity": data.get("humidity", 0),
                                "co2": data.get("co2", 0),
                                "tvoc": data.get("tvoc",0)
                        }
                }
        ]
        db_client.write_points(json_body)
        print("Data inserted into InfluxDB:", json_body)

def main():
        connect_mqtt()
        db_client = InfluxDBClient(host=DB_HOST, port=DB_PORT, database=DB_NAME)
        mqtt_client.loop_start()

        try:
                while True:
                        xbee_data = read_xbee_data()
                        print(f"Found xbee data: {xbee_data}")
                        if xbee_data:
                                parsed_data = clean_data(xbee_data)

                                if parsed_data:
                                        insert_data(db_client, parsed_data)
                                        publish_to_mqtt(json.dumps(parsed_data))
                        else:
                                print("No data available")
                        time.sleep(1)
        except KeyboardInterrupt:
                print("Exiting")
        finally:
                xbee_serial.close()
                mqtt_client.loop_stop()
                db_client.close()
if __name__ == "__main__":
        main()