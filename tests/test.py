try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

import unittest
import sys
import time
from serial import Serial

PORT = '/dev/ttyACM0'

class TestCommands(unittest.TestCase):
    def setUp(self):
        self.held, sys.stdout = sys.stdout, StringIO()

    def test_00_echo(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('AT\r')
        time.sleep(0.1)
        response = ser.readline()
        ser.close()
        self.assertEqual(response, 'AT\r\n')

    def test_01_version(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('ATV\r')
        time.sleep(0.1)
        response = ser.readline()
        ser.close()
        self.assertEqual(response, 'ChaosDuino 0.1.2 for PCB rev3 running...\r\n')

    def test_02_LED(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('ATLR0\r')
        ser.write('ATLG0\r')
        ser.write('ATLB0\r')
        j = 0
        for i in range(50):
            if j & 1 == 1:
                ser.write('ATLB1\r')
            if j & 1 == 0:
                ser.write('ATLB0\r')
            if j & 2 == 0:
                ser.write('ATLG1\r')
            if j % 4 != 0:
                ser.write('ATLG0\r')
            if j % 8 == 0:
                ser.write('ATLR1\r')
            if j % 8 != 0:
                ser.write('ATLR0\r')
            j+=1
            time.sleep(0.1)
        ser.write('ATLR0\r')
        ser.write('ATLG0\r')
        ser.write('ATLB0\r')
        ser.close()

# class TestEntropy(unittest.TestCase):
#     def setUp(self):
#         self.held, sys.stdout = sys.stdout, StringIO()

#     def test_dual_classes(self):
#         jam1 = Jam({'issue': {'rmamba': 0.5}})
#         jam2 = Jam({'issue': {'ledi_mambix': 1.5}})
#         self.assertEqual(jam1.jam, {'issue': {'rmamba': 0.5}})
#         self.assertFalse(jam1.modified, 'Should not be modified!!!')
#         self.assertEqual(jam2.jam, {'issue': {'ledi_mambix': 1.5}})
#         self.assertFalse(jam2.modified, 'Should NOT be modified!!!')
