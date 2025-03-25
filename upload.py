import serial
import time
import paho.mqtt.client as mqtt

MQTT_BROKER = 'localhost'
MQTT_TOPIC = "sensor/data"
MQTT_PORT = 1883

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

def main():
        connect_mqtt()
        mqtt_client.loop_start()

        try:
                while True:
                        xbee_data = read_xbee_data()
                        print(f"Found xbee data: {xbee_data}")
                        if xbee_data:
                                publish_to_mqtt(xbee_data)
                        else:
                                print("No data available")
                        time.sleep(1)
        except KeyboardInterrupt:
                print("Exiting")
        finally:
                xbee_serial.close()
                mqtt_client.loop_stop()
if __name__ == "__main__":
        main()