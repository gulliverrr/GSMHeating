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

## SMS commands
A pin is used in order to block other senders

Pin 2341 is used here in the example

2341 off
Turns heating OFF and calls sender back for 5 seconds (missed call) to avoid charges on remote SIM

2341 on
Turns heating ON and calls sender back for 10 seconds (missed call) to avoid charges on remote SIM

2341 ask
Reports back to sender the following SMS (with current state and values)

Heating is ON

Temperature: 30.0 'C

Humidity: 42%


## Call commands

If you call the GSM modem it will answer the call (there is no mic so you wont hear anything).

If heating is off it will hung up by itself after 5 seconds.

If heating is on it will hung up by itself after 10 seconds.

## Note

Please note that this contraption will require your original thermostat as it will not monitor or regulate the room temperature.
