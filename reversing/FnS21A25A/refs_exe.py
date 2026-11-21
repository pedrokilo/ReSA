# Busca en el EXE (solo lectura) las referencias a las 120 direcciones del lote: llamadas directas (E8/E9) y dwords inmediatos (punteros a funcion guardados/comparados).
import struct,csv,sys,re
p="E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe"
d=open(p,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
so=pe+24+opt
secs=[]
for i in range(nsec):
    n,vs,va,rs,rp=struct.unpack_from('<8sIIII',d,so+i*40); secs.append((n.rstrip(b'\0').decode(),va+base,vs,rp,rs))
rows=list(csv.DictReader(open('docs/ExeIndependiente/raices.csv',encoding='utf-8')))
mine=[(x['lote'],int(x['direccion'],16)) for x in rows if x['lote'] in ('S21A','S22A','S23A','S24A','S25A')]
def va_of(off):
    for n,va,vs,rp,rs in secs:
        if rp<=off<rp+rs: return n,va+off-rp
for lote,a in sorted(mine):
    pat=struct.pack('<I',a)
    out=[]
    i=0
    while True:
        i=d.find(pat,i)
        if i<0: break
        r=va_of(i)
        if r: out.append((r[0],r[1],d[max(0,i-1):i]))
        i+=1
    calls=[]
    # rel32 calls/jmps
    for n,va,vs,rp,rs in secs:
        if n!='.text': continue
        seg=d[rp:rp+rs]
        for m in re.finditer(rb'[\xe8\xe9]',seg):
            k=m.start()
            if k+5>len(seg): continue
            rel=struct.unpack_from('<i',seg,k+1)[0]
            if (va+k+5+rel)&0xffffffff==a: calls.append(va+k)
    print(f"{lote} {a:08X}: dwords={[(n,'%X'%v,b.hex()) for n,v,b in out]} calls={['%X'%c for c in calls]}")
