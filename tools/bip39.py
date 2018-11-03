# Parse BIP39 english words and include them in the arduino project
# https://raw.githubusercontent.com/bitcoin/bips/master/bip-0039/english.txt

import requests

r = requests.get('https://raw.githubusercontent.com/bitcoin/bips/master/bip-0039/english.txt')
text = r.text
# print(text)
words = text.split('\n')
string = '", "'.join(words[:-1])
with open('../chaosDuino/BIP39.h', 'w') as w:
    w.write('const char* const BIP39[] PROGMEM = {{"{}"}};'.format(string))
