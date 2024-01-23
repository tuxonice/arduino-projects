from machine import Pin, SoftI2C
import ssd1306
import config
import time


class OledDisplay:
    def __init__(self,
                 scl=config.OLED_SCL_PIN,
                 sda=config.OLED_SDA_PIN,
                 width=config.OLED_WIDTH,
                 height=config.OLED_HEIGHT):
        
        i2c = SoftI2C(scl=Pin(scl), sda=Pin(sda))
        self.oled = ssd1306.SSD1306_I2C(width, height, i2c)
        self.width = width
        self.height = height

    def clear(self):
        self.oled.fill(0)

    def write(self, text, x=0, y=0):
        self.oled.text(text, x, y)

    def show(self):
        self.oled.show()
        
    def splash_screen(self):
        self.clear()
        self.write("Device Booting...", 0, 0)
        self.write("ESP32 System", 0, 12)
        self.write("v1.0", 0, 24)
        self.show()
        time.sleep(2)

        # Second page with details
        self.clear()
        self.write("Pins:", 0, 0)
        self.write("OLED  SCL={}".format(config.OLED_SCL_PIN), 0, 12)
        self.write("OLED  SDA={}".format(config.OLED_SDA_PIN), 0, 20)
        self.show()
        time.sleep(2)

        self.clear()
        self.write("Sensors:", 0, 0)
        self.write("DS18B20 pin={}".format(config.DS18B20_PIN), 0, 12)
        self.show()
        time.sleep(2)

        self.clear()
