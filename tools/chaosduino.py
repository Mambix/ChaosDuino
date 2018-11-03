import serial
import time

ser=serial.Serial('COM28', 2000000, timeout=3)
step = 10000
MiBs = 1
steps = MiBs * 100

file = 'chaosDuino_entropy_{}MiB_{}.txt'

time.sleep(0.5)
ser.write('ATE1\r')
time.sleep(2)

for m in range(8):
    data = []
    ser.write('ATM{}\r'.format(m))
    time.sleep(0.2)
    with open(file.format(MiBs, m), 'wb') as f:
        t = time.time()
        for x in range(steps):
            if (x%25 == 0):
                print(x)
            ser.write('ATDUMP\r')
            s = ser.read(step)
            data.append(s)
        diff = time.time() - t
        for s in data:
            f.write(s)

    print('Collected {} bytes in {:.2f} seconds: {:.2f} Bps'.format(step*steps, diff, step*steps/diff))
    print('File saved!!!')

ser.close()

# Collected 50000000 bytes in 245.00 seconds: 204081.63 Bps
# File saved!!!

# Collected 1000000 bytes in 5.99 seconds: 167000.67 Bps
# File saved!!!

