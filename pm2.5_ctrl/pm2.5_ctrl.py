import serial
import time
import signal
import sys

PORT = 'COM5'
MAX_CNT = 10
start = '\xaa\x01\x00\x00\x00\x00\x01\x66\xbb'
get   = '\xaa\x02\x00\x00\x00\x00\x01\x67\xbb'
stop  = '\xaa\x03\x00\x00\x00\x00\x01\x68\xbb'

def close_serial():
    ret = ser.write(stop)
    ret = ser.read(ret)
    print ret.encode("HEX")
    ser.close()

def handler_ctrl_c(signal_num, frame):
    print "\nyou pressed Ctrl-C, will stop PM2.5 sensor"
    close_serial()
    sys.exit(signal_num)

ser = serial.Serial(port=PORT, baudrate=9600, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)

signal.signal(signal.SIGINT, handler_ctrl_c)

ret = ser.write(start)
ret = ser.read(ret)
print ret.encode("HEX")

cnt = 0
while(cnt < MAX_CNT):
    time.sleep(5)
    ret = ser.write(get)
    hex_str = ser.read(ret)
    pm10  = ord(hex_str[2]) * 256 + ord(hex_str[3])
    pm2_5 = ord(hex_str[4]) * 256 + ord(hex_str[5])
    print "PM10: %d     PM2.5: %d" % (pm10, pm2_5)
    cnt += 1

close_serial()
