import serial
import time

ser=serial.Serial('COM28', 1000000, timeout=3)
step = 10000
steps = 5000

data = []

with open('chaosDuino_entropy_50MiB_2.txt', 'wb') as f:
    time.sleep(1)
    s = ser.read()
    ser.write('ATE1\r')
    t = time.time()
    for x in range(steps):
        if (x%10 == 0):
            print(x)
        ser.write('ATPOOL\r')
        s = ser.read(step)
        data.append(s)
    diff = time.time() - t
    for s in data:
        f.write(s)

ser.close()

print('Collected {} bytes in {:.2f} seconds: {:.2f} Bps'.format(step*steps, diff, step*steps/diff))
print('File saved!!!')

# Collected 50000000 bytes in 245.00 seconds: 204081.63 Bps
# File saved!!!
