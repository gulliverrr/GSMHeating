# GSMHeating
Arduino (Pro Micro) based heating controller for summer houses with no internet connection. The famous Siemens TC35 is used as the GSM modem. DHT11 is used as the temperature and humidity sensor.

## Connections
Arduino board used is Arduino Pro Micro. Serial is used for debugging (USB socket). Serial1 is used to talk to TC35 (pins Rx, Tx)
Digital Pin 2: TC35-IGT, pin to start (Ignition) modem connecting to the GSM network
Digital Pin 3: DHT11 Data pin (A 10kΩ pullup resistor is connected between Data and Vcc pins of DTH11)
Digital Pin 4: Relay signal (active LOW)
Rx: TC35-Rx 
Tx: TC35-Tx
RAW: 5V input supply

A strong 5V power supply should be used at least higher than 1A. A 2A is recommended. We have seen in some cases a 1A power supply did start the controller but once a call was initiated the modem locked and had to be restarted! Avoid small psus even if they "look" ok.
