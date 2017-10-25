f = open('original.csv', 'r')

for line in f:
    tab = line.strip().split('\t')
    print( tab[1] + ''.join(tab[6:]) )

f.close()
