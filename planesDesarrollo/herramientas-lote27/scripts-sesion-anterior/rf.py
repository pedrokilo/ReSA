import struct,sys
exec(open('exedis.py').read().split('a=int')[0])
for s in sys.argv[1:]:
    a=int(s,16); o=off(a)
    if o is None: print(s,'??'); continue
    u=struct.unpack_from('<I',exe,o)[0]; f=struct.unpack_from('<f',exe,o)[0]
    print(hex(a),hex(u),repr(f))
