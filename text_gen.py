words = []

for i in range(100):
    for c in range(ord('a'), ord('z')+1):
        for j in range(ord('a'), ord('z')+1):
            string = i*chr(c)
            string += chr(j)
            words.append(string)

words = list(set(words))
words.sort()

with open("text.txt", 'w') as f:
    for word in words:
        f.write(word)
        f.write(' ')
print(len(words))