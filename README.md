# Real-time-Web-based-Geotagging-AQI-Monitoring-System

--- copyright of KMUTT ---

The project contain 3 parts(Hardware-Server,Server,Server-Website)
These codes is the first part(Hardware-Server) of my project and it's are my responsibility in the project

As you see, I program in side of device to control the sensor and prepare data before send it to firebase by using


1. Arduino UNO - received data and clean it, clear falut, average it before sent it to nodemcu
2. NodeMCU - received data from Arduino and prepared data, connected internet before sent them to firebase


--- Sensors ---

1.Dust sensor(PMS7003) - Measure PM1,PM2.5,and PM10

2.Tempurature and Humidity sensor(DHT11) - Measure Tempurature, and Humidity

3.GPS sensor(NEO6M) - Measure Latitude  and Longitude

--- Controller ---

1.Arduino UNO
2.NodeMCU

--- Server ---

Firebase

--- Compiler ---

Arduino IDE - base on C,C++,C#
