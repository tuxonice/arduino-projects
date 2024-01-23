from oled import OledDisplay
from ds18b20 import DS18B20Sensor
import config
import time

oled = OledDisplay()
oled.splash_screen()
sensor = DS18B20Sensor()

oled.write("Hello!", 0, 0)
oled.show()

max_temp = {}

while True:
    readings = sensor.read()

    for rom, t in readings.items():
        if rom not in max_temp or t > max_temp[rom]:
            max_temp[rom] = t

        oled.clear()
        oled.write("T:{:.2f} C".format(t), 0, 0)
        oled.write("M:{:.2f} C".format(max_temp[rom]), 0, 16)
        oled.show()

        print("ROM:", rom)
        print("Temperature:", t, "C")
        print("Max Temp:", max_temp[rom], "C\n")

    time.sleep(config.MAIN_LOOP_DELAY)