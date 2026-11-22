import sys, time, os, ctypes
ctypes.windll.user32.SetProcessDPIAware()
from ctypes import wintypes
sys.path.insert(0,'D:/Original/wv-int/reversing/FnFase8')
import captura
OUT='D:/Original/sim_out'
et=sys.argv[1]; seg=int(sys.argv[2]); ini=int(sys.argv[3]); walk0=int(sys.argv[4]); walk1=int(sys.argv[5])
while not os.path.exists('D:/Original/gta_lock/owner_render'): time.sleep(2)
t0=time.time(); n=0; down=False
f=open('%s/rn_%s_shots.log'%(OUT,et),'w')
while time.time()-t0<seg:
    time.sleep(2.5)
    t=time.time()-t0
    h=captura.buscar()
    if h and walk0<=t<walk1 and not down:
        captura.u.ShowWindow(h,9); captura.u.SetForegroundWindow(h); time.sleep(0.5)
        captura.u.keybd_event(0x57,0x11,0,0); down=True
    if down and t>=walk1:
        captura.u.keybd_event(0x57,0x11,2,0); down=False
    if t<ini: continue
    n+=1
    try:
        r=captura.capturar('%s/rn_%s_s%02d.png'%(OUT,et,n),traer=False)
        f.write('t=%d n=%d %s\n'%(t,n,r)); f.flush()
    except Exception as e:
        f.write('err %r\n'%e); f.flush()
if down: captura.u.keybd_event(0x57,0x11,2,0)
