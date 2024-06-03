# MCU name
MCU = atmega32a

# Processor frequency
F_CPU = 16000000

# Bootloader selection
BOOTLOADER = usbasploader

# Build Options
BOOTMAGIC_ENABLE = no
MOUSEKEY_ENABLE = no
EXTRAKEY_ENABLE = yes
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
NKRO_ENABLE = no
BACKLIGHT_ENABLE = no
RGBLIGHT_ENABLE = no
AUDIO_ENABLE = no
OLED_ENABLE = yes
OLED_DRIVER = ssd1306
ENCODER_ENABLE = yes
WPM_ENABLE = yes
LTO_ENABLE = yes
EXTRAFLAGS += -flto
UNICODE_ENABLE = no
UNICODEMAP_ENABLE = no
SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = no
COMBO_ENABLE = no
KEY_LOCK_ENABLE = no
AUTO_SHIFT_ENABLE = no
LEADER_ENABLE = no
AVR_USE_MINIMAL_PRINTF = yes
MUSIC_ENABLE = no