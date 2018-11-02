try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

import unittest
import sys
import time
from serial import Serial

PORT = '/dev/maple'

class TestCom(unittest.TestCase):
    def setUp(self):
        self.held, sys.stdout = sys.stdout, StringIO()

    def test_port(self):
        ser=Serial(PORT, 2000000, timeout=3)
        time.sleep(0.1)
        ser.write('AT\r')
        time.sleep(0.1)

        response = ser.read()
        ser.close()

        self.assertEqual(response, 'AT')

# class TestCommands(unittest.TestCase):
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
