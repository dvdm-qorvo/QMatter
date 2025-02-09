# Matter QPG6105 lighting example application

Qorvo's QPG6105 Matter lighting example shows how to create a remotely controlled dimmable color light bulb, with Thread
connectivity, using Matter. It is using Bluetooth&trade; LE to perform Matter provisioning. This example can be used as reference for
creating your own Matter light bulb.

Buttons are used as input to trigger device state changes, LEDs are used as output to visualize the device states.

Features of this application are:
1. Remote lighting control (dimming, color, on/off) using a Matter controller.
2. Bluetooth LE for Matter commissioning procedure.
3. Factory reset implementation based on number of reset cycles.


---

- [Button control](#button-control)
- [LED output](#led-output)
- [Factory reset](#factory-reset)
- [Logging output](#logging-output)
- [Building and flashing](#building-and-flashing)
- [Testing the example](#testing-the-example)
  - [Android chip-tool](#android-chip-tool)
  - [POSIX CLI chip-tool](#posix-cli-chip-tool)

---

## Button control

This application uses following buttons of the DK board:

- `SW2 (PB2)`: Used to toggle the light between two levels
- `SW6 (RADIO RESET)`: Used to perform a HW reset for the full board. If the reset is triggered 10 times in a row, factory reset will
be triggered.
- `SW4 (PB1)`: Used to toggle the off/on state of the simulated light
- `SW5 (PB4)`: Used to perform, depending on the time the button is kept pressed,
  - Trigger OTA (released 0-3s)
  - Factory Reset (released after 6s)

The buttons `SW1`, `SW3` and the slider switch `SW7` are unused.

## LED output

The following LEDs are used during the application:

- `LD3` - RGB led:
  - Off: Light is off
  - Dim: Light is set to low level
  - Bright: Light is set to high level
- `LD4` - RED led - shows the device state and its connectivity. Following states are defined:
  - Short blink every 1s: Bluetooth LE advertising. The device is not commissioned yet.
  - Fast blinks: Bluetooth LE connected and subscribed but not yet commissioned.
  - On: Full service connectivity

## Factory reset

Factory reset of the Matter light can be triggered by pressing the reset (`SW6`) exactly 10 times in a row or by holding
`SW5` at least 6 seconds. If the factory reset is triggered by the reset sequence, the light will blink one second ON and
one second OFF, two times.

## Logging output

For instructions to view the serial logging, refer to [Enable serial logging](../../../README.md#enable-serial-logging)

At startup you will see:

```
NRT ROM v1
qvCHIP <version> ROMv1/1 (CL:0) r:3
ResetCount[0]
[P][DL] BLEManagerImpl::Init() complete
[P][-] Initializing OpenThread stack
[P][DL] OpenThread started: OK
[P][DL] Setting OpenThread device type to ROUTER
[P][-] Starting OpenThread task
[P][-] Starting Platform Manager Event Loop
[P][-] ============================
[P][-] Qorvo Lighting-app Launching
[P][-] ============================


```

## Building and flashing

See [Building and flashing the example applications](../../../README.md#building-and-flashing-the-example-applications) section to get instructions how to build and program the Matter base example application.

## Testing the example

The Matter light will start Bluetooth LE advertising automatically at start-up so it is discoverable for a Matter
controller to start the Matter commissioning.

The commissioning procedure is done over Bluetooth LE where a connection is setup between a Matter device and a Matter
controller. This Matter controller takes the role of a commissioner.
The commissioner needs to get information from the Matter device to start the commissioning. This information can be
obtained by a QR code or from the serial output of the Matter device.

The final phase in the commissioning procedure is Thread provisioning. This involves sending the Thread network
credentials over Bluetooth LE to the Matter device. Once this is done, the device joins the Thread network and
communication with other Thread devices in the network can be achieved.

### Android chip-tool

For a commissioning guide that makes use of the Android chip-tool, please refer to [Commissioning Qorvo Matter device with Android chip-tool](../../../Documents/Guides/commissioning_android_chiptool.md)

### POSIX CLI chip-tool

For a commissioning guide that makes use of the POSIX cli chip-tool, please refer to [Commissioning Qorvo Matter device with POSIX CLI chip-tool](../../../Documents/Guides/commissioning_posix_cli_chiptool.md)
