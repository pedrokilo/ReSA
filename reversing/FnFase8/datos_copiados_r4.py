# Ronda 4: mide cuantos bytes de la imagen de datos copiada del gta_sa.exe siguen intactos (y distintos de cero) en el proceso vivo.
# Uso: python datos_copiados_r4.py "<gta-sa.exe>" [segundos_de_espera_tras_arrancar]
import os, ctypes, struct, subprocess, sys, time
from ctypes import wintypes
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE
def pid():
    o = subprocess.run(['powershell','-NoProfile','-Command',"Get-CimInstance Win32_Process -Filter \"Name='gta_sa_standalone.exe'\" | ForEach-Object { \"$($_.ProcessId)|$($_.CommandLine)\" }"],capture_output=True,text=True).stdout
    for l in o.splitlines():
        if os.environ.get('OBS_FILTRO', 'wv-r4') in l: return int(l.split('|')[0])
exe = open(sys.argv[1],'rb').read(); espera = int(sys.argv[2]) if len(sys.argv)>2 else 120
pe = struct.unpack_from('<I',exe,0x3C)[0]; ns = struct.unpack_from('<H',exe,pe+6)[0]; so = struct.unpack_from('<H',exe,pe+20)[0]
tab = pe+24+so; secs=[]
for i in range(ns):
    n,vs,va,rs,ro = struct.unpack_from('<8sIIII',exe,tab+40*i); secs.append((n.rstrip(b'\0').decode(),0x400000+va,vs,rs,ro))
p=None
while not p: p=pid(); time.sleep(1)
time.sleep(espera)
h=k.OpenProcess(0x38|0x400,False,p)
def rd(a,n):
    b=ctypes.create_string_buffer(n); r=ctypes.c_size_t()
    return b.raw[:r.value] if k.ReadProcessMemory(h,ctypes.c_void_p(a),b,n,ctypes.byref(r)) or r.value else None
tot=dict(copiados=0,iguales=0,iguales_nocero=0,distintos=0)
out=[]
for n,va,vs,rs,ro in secs:
    if n not in ('.rdata','.data','_TEXT_HA','_rwdseg','.rsrc'): continue
    nb=min(rs,vs); orig=exe[ro:ro+nb]; vivo=rd(va,nb)
    if vivo is None or len(vivo)!=nb: out.append((n,'sin lectura')); continue
    ig=sum(1 for a,b in zip(orig,vivo) if a==b); ign=sum(1 for a,b in zip(orig,vivo) if a==b and a!=0)
    # distintos por bloques de 4 KB: paginas sin ningun cambio
    out.append((n,nb,ig,ign,nb-ig))
    tot['copiados']+=nb; tot['iguales']+=ig; tot['iguales_nocero']+=ign; tot['distintos']+=nb-ig
for o in out: print(o)
print(tot)
# bytes de .rdata+.data+_TEXT_HA sin tocar por pagina de 4 KB
