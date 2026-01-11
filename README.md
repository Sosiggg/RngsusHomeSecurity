# Rngsus Home Security

ESP32 + Arduino-based home security system that arms with a keypad/LCD, monitors temperature and humidity, detects motion/laser beam breaks, sounds a buzzer, and raises an alert via SMS-style push notifications (Pushbullet or Pushover).

## Features
- Keypad + LCD for arming/disarming with a password
- Motion sensor and laser/photodiode tripwire for intrusion detection
- Audible buzzer alarm on breach
- Temperature and humidity readout on the LCD
- ESP32 Wi-Fi bridge for cloud alerts
- Pushbullet or Pushover notifications (can be configured to reach SMS)

## Hardware (reference build)
- ESP32 dev board (Wi-Fi and HTTPS client)
- Arduino Mega (I/O for sensors, keypad, LCD, buzzer, laser/receiver)
- 4x4 matrix keypad
- 16x2 or 20x4 character LCD (I2C backpack recommended)
- PIR motion sensor
- Laser diode + light-dependent resistor (or photodiode) for beam-break detection
- Active buzzer
- DHT11/DHT22 for temperature/humidity
- Dupont jumpers, breadboard/PCB, suitable power supply

### Serial link between Mega and ESP32
- ESP32 `RX1` ⟷ Mega TX (e.g., `Serial1` on pins 18/19)
- ESP32 `TX1` ⟷ Mega RX
- Common ground between boards

## Firmware layout
- `esp32_final/esp32_final.ino` — connects to Wi-Fi, listens on `Serial1` for `ALERT:<reason>` messages from the Mega, then sends Pushbullet or Pushover notifications.

## Setup
1) Clone: `git clone https://github.com/Sosiggg/RngsusHomeSecurity.git`
2) Open `esp32_final/esp32_final.ino` in the Arduino IDE or VS Code with the Arduino extension.
3) Update credentials in the sketch:
	- `ssid` / `password` for your Wi-Fi
	- `pushbulletApiKey` **or** `pushoverApiToken` + `pushoverUserKey`
4) Select the ESP32 board in the IDE, choose the correct COM port, and upload.
5) On the Mega side, ensure your main sketch sends `ALERT:<reason>` over the hardware serial connected to the ESP32 when an alarm triggers.

## Alert delivery
- **Pushbullet:** creates a note push; you can set a Pushbullet SMS bridge on your phone for text delivery.
- **Pushover:** sends a high-priority message; mobile app can mirror as SMS depending on your carrier/app settings.

## Operation
- Arm the system from the keypad; LCD shows status and sensor readings.
- When the PIR triggers or the laser beam is broken, the Mega sends `ALERT:<reason>` to the ESP32.
- The ESP32 posts the alert to Pushbullet/Pushover and logs responses to the serial monitor.
- The buzzer sounds locally while notifications are dispatched.

## Troubleshooting
- If serial shows "Wi-Fi disconnected", the ESP32 automatically retries every 5 seconds.
- If no notifications arrive, verify API keys and that HTTPS ports are not blocked on your network.
- Confirm serial wiring and matching baud (`9600`) between Mega and ESP32.

## Future ideas
- Add MQTT for richer event logs
- SD card logging of arming/disarming events
- OTA updates for the ESP32 bridge
