# captura periodica de la ventana completa del standalone (cliente entero) + tamano de ventana. Uso: python shot.py etiqueta segundos intervalo inicio
import sys, time, os, ctypes
ctypes.windll.user32.SetProcessDPIAware()
from ctypes import wintypes
sys.path.insert(0,'D:/Original/wv-int/reversing/FnFase8')
import captura
OUT='D:/Original/sim_out'
et=sys.argv[1]; seg=int(sys.argv[2]); iv=int(sys.argv[3]); ini=int(sys.argv[4])
while not os.path.exists('D:/Original/gta_lock/owner_render'): time.sleep(2)
t0=time.time(); n=0
f=open('%s/rn_%s_shots.log'%(OUT,et),'w')
while time.time()-t0<seg:
    time.sleep(iv)
    if time.time()-t0<ini: continue
    n+=1
    try:
        h=captura.buscar(); rc=wintypes.RECT(); captura.u.GetClientRect(h,ctypes.byref(rc))
        r=captura.capturar('%s/rn_%s_s%02d.png'%(OUT,et,n),traer=True)
        f.write('t=%d n=%d cliente=%dx%d cap=%s\n'%(time.time()-t0,n,rc.right,rc.bottom,r)); f.flush()
    except Exception as e:
        f.write('err %r\n'%e); f.flush()
