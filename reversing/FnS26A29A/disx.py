# Desensambla las raices de los lotes S26A-S29A del EXE (solo lectura) y anota llamadas a funciones propias/conocidas.
import struct,csv,sys
from capstone import *
p="E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe"
d=open(p,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
so=pe+24+opt
secs=[]
for i in range(nsec):
    n,vs,va,rs,rp=struct.unpack_from('<8sIIII',d,so+i*40); secs.append((va+base,vs,rp,rs))
def rd(a,n):
    for va,vs,rp,rs in secs:
        if va<=a<va+max(vs,rs): return d[rp+a-va:rp+a-va+n]
rows=list(csv.DictReader(open('docs/ExeIndependiente/raices.csv',encoding='utf-8')))
md=Cs(CS_ARCH_X86,CS_MODE_32)
for lote in sys.argv[1:]:
  for r in sorted([x for x in rows if x['lote']==lote],key=lambda x:x['direccion']):
    a=int(r['direccion'],16); n=int(r['bytes'])
    print(f"\n=== {r['direccion']} ({n} bytes) {lote}")
    for i in md.disasm(rd(a,n+16),a):
        if i.address>=a+n: break
        print(f"{i.address:x} {i.mnemonic} {i.op_str}")
