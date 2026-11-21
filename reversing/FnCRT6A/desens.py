# Desensambla las raices de los lotes S30A-S33A (y 0x81F360) del EXE (solo lectura) con capstone; sigue el flujo hasta ret (no se detiene en el primer ret si hay saltos hacia adelante).
import struct,csv,sys
from capstone import *
p="E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe"
d=open(p,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
so=pe+24+opt
secs=[struct.unpack_from('<8sIIII',d,so+i*40) for i in range(nsec)]
def rd(a,n):
    r=a-base
    for nm,vs,va,rs,rp in secs:
        if va<=r<va+max(vs,rs): o=rp+r-va; return d[o:o+n]
md=Cs(CS_ARCH_X86,CS_MODE_32)
def dis(a,n):
    out=[]
    for i in md.disasm(rd(a,n),a):
        out.append(f"{i.address:x} {i.mnemonic} {i.op_str}")
    return out
if __name__=='__main__':
    if sys.argv[1]=='-l':
        lotes=sys.argv[2:]
        rows=list(csv.DictReader(open('docs/ExeIndependiente/raices.csv',encoding='utf-8')))
        for lote in lotes:
            f=open(f'reversing/FnCRT6A/dis_{lote}.txt','w')
            for x in sorted([x for x in rows if x['lote']==lote],key=lambda x:int(x['direccion'],16)):
                a=int(x['direccion'],16); n=int(x['bytes'])
                f.write(f"\n=== 0x{a:08X} ({n} bytes)\n"); f.write("\n".join(dis(a,n))+"\n")
    else:
        a=int(sys.argv[1],16); n=int(sys.argv[2]); print("\n".join(dis(a,n)))
