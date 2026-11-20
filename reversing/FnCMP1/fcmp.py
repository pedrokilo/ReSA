"""Lista las comparaciones x87 (fcom/fcomp/fucom + fnstsw + test/sahf + jcc) de una funcion del EXE
y calcula que salto se toma con st0 > op, st0 < op, st0 == op y NO ORDENADO (NaN).
Uso: python fcmp.py <dir hex> [max bytes]   (solo lectura del EXE)"""
import sys,struct,capstone
P="E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe"
d=open(P,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
ns=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
so=pe+24+opt
secs=[struct.unpack_from('<8sIIII',d,so+i*40) for i in range(ns)]
def rd(a,n):
    r=a-base
    for nm,vs,va,rs,rp in secs:
        if va<=r<va+max(vs,rs): o=rp+r-va; return d[o:o+n]
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32); md.detail=False
CASES=[('GT',0x00),('LT',0x01),('EQ',0x40),('NaN',0x45)]
def par(x): return bin(x&0xff).count('1')%2==0
def flags_test(ah,mask):
    r=ah&mask; return dict(ZF=r==0,PF=par(r),SF=bool(r&0x80),CF=False,OF=False)
def flags_sahf(ah):
    return dict(CF=bool(ah&1),PF=bool(ah&4),ZF=bool(ah&0x40),SF=bool(ah&0x80),OF=False)
def taken(j,f):
    c=j[1:]
    t={'e':f['ZF'],'z':f['ZF'],'ne':not f['ZF'],'nz':not f['ZF'],'p':f['PF'],'np':not f['PF'],'pe':f['PF'],'po':not f['PF'],
       'a':not f['CF'] and not f['ZF'],'ae':not f['CF'],'b':f['CF'],'be':f['CF'] or f['ZF'],
       's':f['SF'],'ns':not f['SF']}
    return t.get(c)
def analyze(a,maxb=0x1200):
    code=rd(a,maxb); ins=[]
    for i in md.disasm(code,a):
        ins.append(i)
        if i.mnemonic=='ret':
            nxt=code[i.address-a+i.size:i.address-a+i.size+2]
            if nxt[:1]==b'\xcc' or nxt==b'' or nxt==b'\x90\x90': break
    res=[]
    for k,i in enumerate(ins):
        if i.mnemonic=='fnstsw':
            # instruccion de comparacion previa
            pre=[x for x in ins[max(0,k-3):k] if x.mnemonic.startswith(('fcom','fucom','ficom','ftst'))]
            pre=pre[-1] if pre else None
            prev=ins[max(0,k-6):k]
            if k+2>=len(ins): continue
            mask=None; sah=False; tk=None; andflag=False
            for q in range(k+1,min(k+5,len(ins))):
                tt=ins[q]
                if tt.mnemonic=='test' and tt.op_str.startswith('ah,'): mask=int(tt.op_str.split(',')[1],0); tk=q; break
                if tt.mnemonic=='sahf': sah=True; tk=q; break
                if tt.mnemonic=='and' and tt.op_str.startswith('eax, 0x'):
                    v=int(tt.op_str.split(',')[1],0)
                    if v&0xff==0 and v>0: mask=v>>8; tk=q; andflag=True; break
            if tk is None:
                t=ins[k+1]
                res.append((i.address,pre,'?? '+t.mnemonic+' '+t.op_str,[],prev)); continue
            jx_i=tk
            # saltar al primer jcc
            while jx_i<len(ins) and not ins[jx_i].mnemonic.startswith('j'): jx_i+=1
            jx=ins[jx_i]; outs=[]
            for nm,ah in CASES:
                f=flags_sahf(ah) if sah else flags_test(ah,mask)
                if andflag: f['PF']=True
                outs.append('%s:%s'%(nm,'SALTA' if taken(jx.mnemonic,f) else 'sigue'))
            res.append((i.address,pre,('sahf ' if sah else ('and eax,%#x '%(mask<<8) if andflag else 'test ah,%#x '%mask))+jx.mnemonic+' '+jx.op_str,outs,prev))
    return res
if __name__=='__main__':
    a=int(sys.argv[1],16); mb=int(sys.argv[2],0) if len(sys.argv)>2 else 0x1200
    for addr,pre,desc,outs,_pv in analyze(a,mb):
        ps='%x %s %s'%(pre.address,pre.mnemonic,pre.op_str) if pre else '-'
        print('%x | %s | %s | %s'%(addr,ps,desc,' '.join(outs)))
