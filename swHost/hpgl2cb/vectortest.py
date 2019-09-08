# Tests for the vector class
import math

from vectorstuff.vector import vector

def addsubtest():
    a=vector(1,2)
    b=vector(3,4)
    c=a+b
    d=b+a
    e=a-b
    f=b-a

    print("   Results: ",a,b,c,d,e,f)

    if c==vector(4,6) and d==c and e==vector(-2,-2) and f==vector(2,2):
        print("OK Add/Sub Tests PASSED")
    else:
        print("!! Add/Sub Tests FAILED")

def magtest():
    a=vector(1,1)
    a_mag = a.mag()
    print(f"   mag of {a} == {a_mag}")

    if a_mag == math.sqrt(2):
        print("OK magtest PASSED")
    else:
        print("!! magtest FAILED")

def phitest():
    a=vector(1,1)
    b=vector(-1,1)
    c=vector(-1,-1)
    d=vector(1,-1)

    a_phi=a.phi()
    b_phi=b.phi()
    c_phi=c.phi()
    d_phi=d.phi()

    print(f"   Phi of {a} is {a_phi}")
    print(f"   Phi of {b} is {b_phi}")
    print(f"   Phi of {c} is {c_phi}")
    print(f"   Phi of {d} is {d_phi}")

# Execute Tests
addsubtest()
magtest()
phitest()