from machine import Pin
import onewire
import ds18x20
import time
import config


class DS18B20Sensor:
    def __init__(self, pin=config.DS18B20_PIN):
        self.ds_pin = Pin(pin)
        self.sensor = ds18x20.DS18X20(onewire.OneWire(self.ds_pin))
        self.roms = self.sensor.scan()

        if not self.roms:
            print("⚠️ No DS18B20 sensors found!")
        else:
            print("Found DS18B20 devices:", self.roms)

    def read(self):
        self.sensor.convert_temp()
        time.sleep_ms(config.DS18B20_CONVERSION_DELAY_MS)

        return self.sensor.read_temp(self.roms[0])

