import sys,struct
exec(open('exedis.py').read().split("a=int(sys.argv[1]")[0])
a=int(sys.argv[1],16); n=int(sys.argv[2],16)
d=exe[off(a):off(a)+n]
for i in range(0,n,4): print("%x: %08x"%(a+i,struct.unpack_from('<I',d,i)[0]))
