# WeatherBus Lite
A super simple telemetry protocol for IoTweather monitoring systems!

## What it can do
- Supports simple query-response communication
- ASCII based protocol for readability and easy debugging
- Designed to be lightweight and easy to implement.
- Uses standard RS485 bus for communication.
- Can be extended to support more sensors.

## What it can't do

- Does not support multiple devices on the same bus.For example, if you have two temperature sensors on the same bus, they cannot coexist.
- No autodiscovery of devices on the bus. This protocol has no addressing scheme.
- No error checking or correction. If a sensor doesn't respond, the master will try a couple of times and then give up.
- No support for multiple masters on the same bus. Only one master can be connected to the bus at a time.