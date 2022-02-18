This Release Note describes the software release information for the QPG6105 Matter Software Development Kit (SDK). The
SDK provides Matter light and Matter lock example applications. It also contains several simple reference applications
to demonstrate how to use the QPG6105 peripherals and their drivers.

> This Qorvo evaluation board, development board, or development kit and all the features available through it is sold
> and/or licensed to you for evaluation purposes only. You may not use this evaluation board, development board, or
> development kit or any of its features in any revenue-generating products or services.

# v0.7.1.0 QPG6105 Matter SDK Alpha Release
This is an early stage SDK in which the functionality is limited tested. This SDK can be used for product development in
engineering phase. This SDK should not be used for commercial products.

## Changes
- [SDP011-182] - Base Components (gpNvm, Bluetooth LE controller, Bluetooth LE Host) that are used for Matter and proven
to be stable, are now pushed to ROM. Using QPG6105 ROM offloaded SW maximizes application flash availability.
- [SDP011-600/SDP011-636] - fixed buffer overflow issue when a write to NVM is done when the key-payload length exceeds
264 bytes.
- [SDP011-625] - Added check at start-up to verify HW revision of QPG6105. This version of the DK is only compatible
with QPG6105 containing ROM v1.

## Known Issues


## Release Management
- This SDK release is based on Base Components v2.10.2.0. For release notes for these specific components, please refer
to the [Documents/Release Notes](Documents/Release%20Notes) folder.

## Certification
Not applicable for this release

# v0.7.0.0 QPG6105 Matter SDK Alpha Release
This is an early stage SDK in which the functionality is limited tested. This SDK can be used for product development in
engineering phase. This SDK should not be used for commercial products.

## Changes
- This is the initial release of the QPG6105 Matter SDK. This version allows Qorvo's customers to develop Matter based
applications on Qorvo's state-of-the-art QPG6105 SoC.
- This SDK includes ready to use applications that are grouped like below:
    - Matter: Includes Matter light and Matter lock project examples.
    - Peripherals: Includes the peripheral example applications.

## Known Issues
- [SDP011-600] - a write to Nvm (sub-paged flash) of a key-payload combination with a combined length that exceeds
264 bytes, will cause a buffer overflow of 12 bytes. This overwrites some ram content, following the affected buffer,
impacting the actual location where the data is written.

## Release Management
- This SDK release is based on Base Components v2.10.2.0. For release notes for these specific components, please refer
to the [Documents/Release Notes](Documents/Release%20Notes) folder.


## Certification
Not applicable for this release
