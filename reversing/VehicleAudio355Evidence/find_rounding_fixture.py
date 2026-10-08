import struct,math,random
random.seed(355)
f=lambda x:struct.unpack('<f',struct.pack('<f',x))[0]
for _ in range(10000):
 v=[f(random.random()) for _ in range(3)];s=sum(x*x for x in v);c=f(math.sqrt(s))
 if (s<c*c)!=(f(s)<f(c*c)):
  print(v,s,c,c*c,f(s),f(c*c));break
