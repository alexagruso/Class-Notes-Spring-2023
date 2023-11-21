import math
import sys

def divides(d, n):
    q = n // d
    return n == d * q

def isPrime(n):
    for i in range(2, n):
        if divides(i, n):
            return False
    
    return True

def primefac(n):
    current = n
    i = 2

    while i <= n:
        if isPrime(i) and divides(i, current):
            print(i)
            current = current // i

            if current == 1:
                return
        else:
            i = i + 1

n = int(sys.argv[1])

primefac(n)