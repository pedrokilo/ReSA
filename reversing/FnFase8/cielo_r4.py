# Ronda 4: espera el control libre del standalone (proceso de wv-r4), sube la camara con el raton y captura el cielo.
# Uso: python cielo_r4.py <etiqueta> [segundos]
import os, ctypes, struct, subprocess, sys, time, os
from ctypes import wintypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import captura
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE
OUT = 'D:/Original/sim_out'
def pid():
    o = subprocess.run(['powershell','-NoProfile','-Command',"Get-CimInstance Win32_Process -Filter \"Name='gta_sa_standalone.exe'\" | ForEach-Object { \"$($_.ProcessId)|$($_.CommandLine)\" }"],capture_output=True,text=True).stdout
    for l in o.splitlines():
        if os.environ.get('OBS_FILTRO', 'wv-r4') in l: return int(l.split('|')[0])
def u32(h,a):
    b=ctypes.create_string_buffer(4); r=ctypes.c_size_t()
    return struct.unpack('<I',b.raw)[0] if k.ReadProcessMemory(h,ctypes.c_void_p(a),b,4,ctypes.byref(r)) else 0
et=sys.argv[1]; seg=int(sys.argv[2]) if len(sys.argv)>2 else 300
p=None; t=time.time()
while not p and time.time()-t<600: p=pid(); time.sleep(1)
h=k.OpenProcess(0x38|0x400,False,p); t0=time.time(); libre=None; n=0
while time.time()-t0<seg:
    tms=u32(h,0xB7CB84)
    if tms>120000 and libre is None:
        # el control libre se detecta como en observar_jugador (DisablePlayerControls a 0): basta pasar de ~200 s de juego
        if tms>200000: libre=time.time()
    if libre:
        hw=captura.buscar(); captura.u.ShowWindow(hw,9); captura.u.SetForegroundWindow(hw); time.sleep(1)
        print(captura.capturar('%s/r4_%s_%d_horizonte.png'%(OUT,et,n),traer=False),flush=True)
        for _ in range(100):  # palanca derecha arriba (camara hacia el cielo); se reescribe cada 20 ms (el pad se refresca por fotograma)
            r=ctypes.c_size_t(); k.WriteProcessMemory(h,ctypes.c_void_p(0xB73458+6),struct.pack('<h',-128),2,ctypes.byref(r)); time.sleep(0.02)
        time.sleep(0.5)
        print(captura.capturar('%s/r4_%s_%d_arriba.png'%(OUT,et,n),traer=False),flush=True)
        n+=1; libre=time.time()+10**9 if n>=2 else time.time()+25
        if n>=2: break
        time.sleep(20)
    time.sleep(1)
print('fin')
