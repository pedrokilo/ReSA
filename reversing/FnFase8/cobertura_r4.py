# Ronda 4: amplia la cobertura dinamica del standalone (proceso de wv-r4) tras el control libre: subir a la bici (INTRO), pedalear (W), menu de pausa (ESC), reanudar.
# Uso: python cobertura_r4.py <etiqueta> [segundos]. Capturas en D:/Original/sim_out/r4_<etiqueta>_*.png
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
def tecla(vk,sc,t=0.15):
    captura.u.keybd_event(vk,sc,0,0); time.sleep(t); captura.u.keybd_event(vk,sc,2,0)
et=sys.argv[1]; seg=int(sys.argv[2]) if len(sys.argv)>2 else 300
p=None
while not p: p=pid(); time.sleep(1)
h=k.OpenProcess(0x38|0x400,False,p); t0=time.time()
while time.time()-t0<seg:
    if u32(h,0xB7CB84)>205000: break
    time.sleep(1)
else: print('sin control libre'); sys.exit(1)
hw=captura.buscar(); captura.u.ShowWindow(hw,9); captura.u.SetForegroundWindow(hw); time.sleep(2)
tecla(0x0D,0x1C); print('INTRO',flush=True); time.sleep(4)
print(captura.capturar('%s/r4_%s_bici.png'%(OUT,et),traer=False),flush=True)
captura.u.keybd_event(0x57,0x11,0,0)
for i in range(8): tecla(0x20,0x39,0.05); time.sleep(0.5)   # pedalear (espacio) con W pulsado
captura.u.keybd_event(0x57,0x11,2,0)
print(captura.capturar('%s/r4_%s_pedaleando.png'%(OUT,et),traer=False),flush=True)
tecla(0x1B,0x01); time.sleep(5)
print(captura.capturar('%s/r4_%s_menu.png'%(OUT,et),traer=False),flush=True)
tecla(0x28,0x50); time.sleep(1); tecla(0x1B,0x01); time.sleep(5)
print(captura.capturar('%s/r4_%s_reanudado.png'%(OUT,et),traer=False),flush=True)
print('fin')
