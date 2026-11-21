# Busca llamadores (call rel32 / jmp) de una direccion en el .text del EXE (solo lectura) y los imprime con su inicio de funcion aproximado.
import sys,struct
sys.path.insert(0,'.')
from desens import rd,base,secs
import re
def calls_to(t,lo=0x401000,hi=0x830000):
    d=rd(lo,hi-lo); out=[]
    for i in range(len(d)-5):
        if d[i] in (0xE8,0xE9):
            a=lo+i; dst=(a+5+struct.unpack_from('<i',d,i+1)[0])&0xffffffff
            if dst==t: out.append((a,d[i]))
    return out
def iat_refs(t,lo=0x820000,hi=0x850000):
    d=rd(lo,hi-lo); out=[]
    for i in range(len(d)-6):
        if d[i]==0xFF and d[i+1] in (0x15,0x25) and struct.unpack_from('<I',d,i+2)[0]==t: out.append(lo+i)
    return out
if __name__=='__main__':
    for t in sys.argv[1:]:
        t=int(t,16); print(hex(t),[(hex(a),'call' if k==0xE8 else 'jmp') for a,k in calls_to(t)])
