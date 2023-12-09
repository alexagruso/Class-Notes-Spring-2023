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
    if isPrime(n):
        print(n)
        return

    current = n

    while divides(2, current):
        print(2)
        current = current // 2

        if current == 1:
            return
    
    i = 3

    while i <= n:
        if isPrime(i):
            while divides(i, current):
                print(i)
                current = current // i

            if current == 1:
                return

        i = i + 2

n = int(sys.argv[1])

primefac(n)