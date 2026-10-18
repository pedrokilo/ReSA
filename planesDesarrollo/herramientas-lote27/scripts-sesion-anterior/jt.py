import struct,sys
exec(open('exedis.py').read().split('a=int')[0])
def dw(a,n):
    return [struct.unpack_from('<I',exe,off(a+4*i))[0] for i in range(n)]
def by(a,n):
    return list(exe[off(a):off(a)+n])
print('arg1 table 55b6d4', [hex(x) for x in dw(0x55b6d4,7)])
print('55b6f0 (x%d)'%0, [hex(x) for x in dw(0x55b6f0,4)])
print('byte idx 55b6f8', by(0x55b6f8,0x2b))
print('55b724 type switch', [hex(x) for x in dw(0x55b724,10)])
print('55b74c', [hex(x) for x in dw(0x55b74c,14)])
print('55b784 bytes', by(0x55b784,0xf4-0)[:0xf4])
print('55b878', [hex(x) for x in dw(0x55b878,8)])
print('55b898', [hex(x) for x in dw(0x55b898,18)])
print('55b8e0', [hex(x) for x in dw(0x55b8e0,8)])
