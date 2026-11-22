"""Comprueba contra un gta_sa.exe EN EJECUCION (con o sin la DLL) que las constantes x87 de CrtStaticInitTable.inc valen lo que dejaron los inicializadores del EXE.
Uso: python -X utf8 contrib/fase8_verificar_x87.py source/game_sa/Crt/CrtStaticInitTable.inc  (lanzar antes el juego; espera a que el proceso exista y a que las constantes esten escritas).
Calcula con float64 de Python (= x87 de 53 bits) y compara byte a byte con la memoria del proceso. Resultado de la rama vivo-p5: 1487 iguales, 0 distintas.
"""
import ctypes,struct,re,subprocess,time,sys
from ctypes import wintypes
k=ctypes.WinDLL('kernel32',use_last_error=True); k.OpenProcess.restype=wintypes.HANDLE
def pid():
    o=subprocess.run(['tasklist','/FI','IMAGENAME eq gta_sa.exe','/FO','CSV','/NH'],capture_output=True,text=True,encoding='cp850',errors='replace').stdout or ''
    for l in o.splitlines():
        if 'gta_sa' in l: return int(l.split('","')[1])
t0=time.time(); p=None
while time.time()-t0<400 and not p:
    p=pid(); time.sleep(2)
print('pid',p)
h=k.OpenProcess(0x38|0x400,False,p); print('h',h,ctypes.get_last_error())
def rd(a,n):
    b=ctypes.create_string_buffer(n); r=ctypes.c_size_t()
    return b.raw if k.ReadProcessMemory(h,ctypes.c_void_p(a),b,n,ctypes.byref(r)) else None
def espera():
    t1=time.time()
    while time.time()-t1<200:
        b=rd(0x8585B8,4)
        d=rd(0x8E3EE0,4)
        if b and d and d!=bytes(4): return True
        time.sleep(1)
    return False
print('listo',espera())
snap={}
for pg in range(0x858000,0x1200000,0x1000):
    b=rd(pg,0x1000)
    if b: snap[pg]=b
print('paginas',len(snap))
def rd2(a,n):
    out=b''
    while n>0:
        pg=a&~0xFFF; b=snap.get(pg)
        if b is None: return None
        c=min(n,pg+0x1000-a); out+=b[a-pg:a-pg+c]; a+=c; n-=c
    return out
rd_live=rd
rd=rd2
rows=[]
for l in open(sys.argv[1],encoding='utf-8'):
    m=re.match(r'\{(\d+), (0x[0-9A-F]+), (0x[0-9A-F]+), (0x[0-9A-F]+), (0x[0-9A-F]+), (\d+)\}',l)
    if m: rows.append([int(x,0) for x in m.groups()])
def ld(a,d):
    b=rd(a,8 if d else 4)
    if b is None: raise RuntimeError('lectura fallida en %x err %d'%(a,ctypes.get_last_error()))
    return struct.unpack('<d' if d else '<f',b)[0]
ok=bad=0; first=[]
for sh,dst,a,b,c,sz in rows:
    A=ld(a,sz&1);B=ld(b,sz&2)
    try:
        r={0:lambda:A/B,1:lambda:A*B,2:lambda:ld(c,sz&4)/(A*B),3:lambda:A-B}[sh]()
    except ZeroDivisionError: r=float('inf')
    exp=struct.pack('<d' if sz&8 else '<f',r) if abs(r)<3e38 or sz&8 else None
    act=rd(dst,8 if sz&8 else 4)
    if act is None: nolei=globals().get('nolei',0)+1; continue
    if exp==act: ok+=1
    else:
        bad+=1
        if len(first)<8: first.append((hex(dst),sh,A,B,r,act.hex(),exp and exp.hex()))
print('iguales',ok,'distintas',bad,'sin lectura',globals().get('nolei',0)); print(first)
