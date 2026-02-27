from datetime import datetime
import serial
import sys

if len(sys.argv) != 3:
    print("Usage: set_clock [port] [baud]")
    exit(1)

port = sys.argv[1]
baud = int(sys.argv[2])
sync_b = (0xAA).to_bytes(1, 'big')
zero_b = (0x00).to_bytes(1, 'big')

# open serial port
ser = serial.Serial(port, baud, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)

# get time
now = datetime.now()
hour = now.hour
minute = now.minute
second = now.second
hour_b = hour.to_bytes(1, 'big')
minute_b = minute.to_bytes(1, 'big')
second_b = second.to_bytes(1, 'big')

# write to port
ser.write(sync_b + hour_b + minute_b + second_b + zero_b + zero_b + zero_b)

print(f"Set time to {hour}:{minute}:{second}")

