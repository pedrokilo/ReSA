import struct,sys
exec(open('exedis.py').read().split('a=int')[0])
for s in sys.argv[1:]:
    a=int(s,16); o=off(a)
    e=exe.index(b'\0',o)
    print(hex(a),repr(exe[o:e]))
