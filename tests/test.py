try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

import unittest
import sys
import time
from serial import Serial

PORT = '/dev/ttyACM0'

class TestSettings(unittest.TestCase):
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
        self.assertEqual(response, 'ChaosDuino v0.1.3 for PCB rev3 running...\r\n')

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

    def test_03_ATE(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('ATE?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, 'ATE1\r\n')

        ser.write('ATE0\r')
        time.sleep(0.1)
        ser.write('ATE?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, 'ATE0\r\n')
        ser.close()

    def test_04_ATP(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('ATP?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, 'ATP0\r\n')

        for i in range(3):
            ser.write('ATP{}\r'.format(i))
            time.sleep(0.1)
            ser.write('ATP?\r')
            time.sleep(0.1)
            response = ser.readline()
            self.assertEqual(response, 'ATP{}\r\n'.format(i))

        ser.write('ATP0\r')
        time.sleep(0.1)
        ser.write('ATP?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, 'ATP0\r\n')

        ser.close()

    def test_05_ATM(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('ATM?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, 'ATM1\r\n')

        for i in range(8):
            ser.write('ATM{}\r'.format(i))
            time.sleep(0.1)
            ser.write('ATM?\r')
            time.sleep(0.1)
            response = ser.readline()
            self.assertEqual(response, 'ATM{}\r\n'.format(i))

        ser.write('ATM1\r')
        time.sleep(0.1)
        ser.write('ATM?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, 'ATM1\r\n')

        ser.close()

    def test_06_OK(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('ATOK?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, 'ATOK0\r\n')

        ser.close()

    def test_07_POOL(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('ATPOOL?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, '10000\r\n')

        ser.close()

    def test_08_BIP39(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('BIP39W?\r')
        time.sleep(0.1)
        response = ser.readline()
        self.assertEqual(response, 'BIP39W24\r\n')

        for i in [15, 18, 21, 24]:
            ser.write('BIP39W{}\r'.format(i))
            time.sleep(0.1)
            ser.write('BIP39W?\r')
            time.sleep(0.1)
            response = ser.readline()
            self.assertEqual(response, 'BIP39W{}\r\n'.format(i))

        ser.close()

# class TestData(unittest.TestCase):
#     def setUp(self):
#         self.held, sys.stdout = sys.stdout, StringIO()

#     def test_dual_classes(self):
#         jam1 = Jam({'issue': {'rmamba': 0.5}})
#         jam2 = Jam({'issue': {'ledi_mambix': 1.5}})
#         self.assertEqual(jam1.jam, {'issue': {'rmamba': 0.5}})
#         self.assertFalse(jam1.modified, 'Should not be modified!!!')
#         self.assertEqual(jam2.jam, {'issue': {'ledi_mambix': 1.5}})
#         self.assertFalse(jam2.modified, 'Should NOT be modified!!!')

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
