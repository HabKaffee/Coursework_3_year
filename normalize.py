import re


with open("text2.txt", 'r') as f:
    strings = f.readlines()
string = ' '.join(strings)
string = string.lower().replace(',', '').replace('-', '').replace('.', '').replace('\'', '').replace('"', '')
string = string.replace('0','').replace('1','').replace('2','').replace('3','').replace('4','').replace('5','').replace('6','').replace('7','').replace('8','').replace('9','')
with open("text2.txt", 'w') as f:
    f.write(string)
