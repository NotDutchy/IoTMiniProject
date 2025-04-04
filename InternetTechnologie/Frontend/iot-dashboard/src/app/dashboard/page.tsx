"use client";

import { FaDroplet } from "react-icons/fa6";
import { CiTempHigh } from "react-icons/ci";
import { MdCo2 } from "react-icons/md";
import { useEffect, useState } from "react";
import mqtt, {MqttClient} from "mqtt";

const MQTT_BROKER_URL = ""; //TODO Raspberry IP
const TOPIC = "sensor/data";

let Temperature: number = 20;
let Humidity: number = 40;

export default function Dashboard() {
    const [messages, setMessages] = useState<string[]>([]);
    const [client, setClient] = useState<MqttClient | null>(null);
    const [isConnected, setIsConnected] = useState(false);
    const [sensorData, setSensorData] = useState({
        temp: 0,
        humidity: 0,
        co2: 0,
        tvoc: 0,
    });

    useEffect(() => {
        const mqttClient = mqtt.connect(MQTT_BROKER_URL);
        mqttClient.on("connect", () => {
            setIsConnected(true);
            mqttClient.subscribe(TOPIC);
        });

        mqttClient.on("message", (topic, message) => {
            console.log(`Received message on ${topic}: ${message.toString()}`);
            try {
                const payload = JSON.parse(message.toString());

                setSensorData({
                    temp: payload.temp || 0,
                    humidity: payload.humidity || 0,
                    co2: payload.co2 || 0,
                    tvoc: payload.tvoc || 0,
                });
            } catch (error) {
                console.error("Error parsing MQTT message:", error);
            }
        });

        mqttClient.on("error", (err) => {
            console.error("MQTT Error:", err)
        });

        setClient(mqttClient);

        return () => {
            mqttClient.end();
        };
    }, []);

    return <div className="flex justify-center items-center bg-gray-100 p-4">
        <div className="card w-80 bg-white shadow-xl p-6 rounded-lg">
            <div className="flex items-center space-x-4">
                <div className="p-3 bg-blue-500 text-white rounded-full">
                    <FaDroplet size={24}/>
                </div>
                <div className="card-body text-black">
                    <div className="text-2xl">
                        Humidity
                    </div>
                    <div className="text-2xl">
                        {Humidity}%
                    </div>
                </div>
            </div>
        </div>
        <div className="card w-80 bg-white shadow-xl p-6 rounded-lg">
            <div className="flex items-center space-x-4">
                <div className="p-3 bg-blue-500 text-white rounded-full">
                    <CiTempHigh size={24}/>
                </div>
                <div className="card-body text-black">
                    <div className="text-2xl">
                        Temperature
                    </div>
                    <div className="text-2xl">
                        {Temperature} &deg;C
                    </div>
                </div>
            </div>
        </div>
        <div className="card w-80 bg-white shadow-xl p-6 rounded-lg">
            <div className="flex items-center space-x-4">
                <div className="p-3 bg-blue-500 text-white rounded-full">
                    <MdCo2 size={24}/>
                </div>
                <div className="card-body text-black">
                    Here comes the CO2 value!
                </div>
            </div>
        </div>
    </div>
}