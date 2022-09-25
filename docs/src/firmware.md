
For full functionality, some WLED usermods must be initialized.

Use the following platformio overrides:

```ini
[platformio]
default_envs = esp32dev

[env:esp32dev]
board = esp32dev
platform = ${esp32.platform}
platform_packages = ${esp32.platform_packages}
build_unflags = ${common.build_unflags}
build_flags = ${common.build_flags_esp32}
             -D WLED_RELEASE_NAME=ESP32
             -D USERMOD_DALLASTEMPERATURE
             -D USERMOD_DALLASTEMPERATURE_CELSIUS
             -D USERMOD_PWM_FAN
             -D PWM_PIN=16
             -D TACHO_PIN=17
lib_deps = ${esp32.lib_deps}
monitor_filters = esp32_exception_decoder
board_build.partitions = ${esp32.default_partitions}
```

Build the firmware then upload to the controllers (note right now the auto-reset does not work, so put the controller in download mode using the BOOT button)
