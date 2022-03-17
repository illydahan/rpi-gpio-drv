import os
import struct


dev = open("/dev/gpio", "w")


dev.write(struct.pack("II", 26, 1).decode("utf-8"))
print(dev)
