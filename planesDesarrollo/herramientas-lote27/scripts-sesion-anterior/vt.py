import struct,sys
exec(open('exedis.py').read().split('a=int')[0])
a=int(sys.argv[1],16)
for i in range(int(sys.argv[2])):
    print(hex(a+4*i), hex(struct.unpack_from('<I',exe,off(a+4*i))[0]))
