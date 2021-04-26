"""
KRY project 2
Author: Patrik Nemeth (xnemet04)
"""

import sys
from ast import literal_eval

# Constants from the project assignment
Fp = 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
a = -0x3
b = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b
G = (0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296,
    0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5)

def diveea(i,j):
    """Modular arithmetic division."""
    d,m,n = eea(Fp,j)
    m = m % Fp
    return multiply(i,m)

def eea(i, j):
    """From https://www.johannes-bauer.com/compsci/ecc/"""
    (s, t, u, v) = (1, 0, 0, 1)
    while j != 0:
        (q, r) = (i // j, i % j)
        (unew, vnew) = (s, t)
        s = u - (q * s)
        t = v - (q * t)
        (i, j) = (j, r)
        (u, v) = (unew, vnew)
    (d, m, n) = (i, u, v)
    return (d, m, n)
    
def multiply(i, j):
    """From https://www.johannes-bauer.com/compsci/ecc/"""
    n = i
    r = 0
    bitlength = i.bit_length() if i > j else j.bit_length()
    for bit in range(bitlength):
        if (j & (1 << bit)):
            r = (r + n) % Fp
        n = (n + n) % Fp
    return r

def point_add(P, Q):
    """Adds points P and Q of a prime field.
    P and Q must not be the same point."""
    px, py = P
    qx, qy = Q
    s = diveea((qy - py) % Fp, (qx - px) % Fp) % Fp
    rx = (((multiply(s, s) - px) % Fp ) - qx ) % Fp
    ry = (multiply(s, (px - rx) % Fp) - py) % Fp
    return (rx, ry)

def point_double(P):
    """Doubles point P of a prime field."""
    px, py = P
    numer = (multiply(multiply(px, px), 3) + a) % Fp
    denom = multiply(2, py)
    s = diveea(numer, denom) % Fp
    rx = (multiply(s, s) - multiply(2, px)) % Fp
    ry = (multiply(s, (px - rx) % Fp) - py) % Fp
    return (rx, ry)

def find_key(public):
    """Loop over every multiple of the origin until
    the public key is found. When  it is found,
    return it."""
    i = 2
    curr = point_double(G)
    while curr != public:
        curr = point_add(curr, G)
        i += 1
    return i

if __name__ == "__main__":
    public = literal_eval(sys.argv[1])
    key = find_key(public)
    print(key)
