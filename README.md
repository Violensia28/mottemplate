# MOT Smart Firmware — Template

Template PlatformIO project for ESP32 with two environments (`minimal`, `full`).

## Build (local)
```bash
pio run -e full
# or
pio run -e minimal
```

## Flash offsets (ESP32 default)
- bootloader.bin @ `0x1000`
- partitions.bin @ `0x8000`
- firmware.bin   @ `0x10000`

First flash via USB/OTG; next updates may use OTA if enabled.
