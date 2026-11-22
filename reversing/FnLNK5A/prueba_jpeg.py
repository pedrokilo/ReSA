# LNK5A: prueba de la captura JPEG de la camara (CWeapon::ms_bTakePhoto + m_bSavePhotos -> JPegCompressScreenToFile -> Gallery/galleryN.jpg), sobre la base de prueba_partida_nueva.py
# Prueba de PARTIDA NUEVA con el ASI (AGUA_ASI): SaveGameToLoad=0 en un INI temporal (el INI de la copia de pruebas lo respalda y restaura
# jugar_nueva.ps1; ademas este script verifica por hash al acabar y restaura de su propia copia). Mide ~90 s por memoria:
# gGameState==9, posicion cerca de Grove Street, contador de frames, ausencia de popups y de 'Unknown param type' en el log.
# Uso: AGUA_ASI=<asi> python prueba_partida_nueva.py [etiqueta] [--segundos N] [--entrada]
import ctypes, struct, subprocess, sys, time, json, os, shutil, hashlib, math
from ctypes import wintypes
BIS1='D:/Original/wv-8/reversing/FnBIS1'
sys.path.insert(0,BIS1)
import captura
aqui=BIS1  # recursos (jugar_nueva.ps1, ini, captura.py) de FnBIS1; este script solo anade la prueba de la foto JPEG
JUEGO='E:/JUEGOS/GTA San Andreas - CVehicle Test'
INIS=[JUEGO+'/gta-reversed.ini',JUEGO+'/scripts/gta-reversed.ini']
LOG=JUEGO+'/scripts/logs/log.log'
k=ctypes.WinDLL('kernel32',use_last_error=True); k.OpenProcess.restype=wintypes.HANDLE
u=ctypes.WinDLL('user32',use_last_error=True)
def pid():
    o=subprocess.run(['tasklist','/FI','IMAGENAME eq gta_sa.exe','/FO','CSV','/NH'],capture_output=True,text=True).stdout
    for l in o.splitlines():
        if 'gta_sa' in l: return int(l.split('","')[1])
class M:
    def __init__(s,p): s.p=p; s.h=k.OpenProcess(0x38|0x400,False,p)
    def rd(s,a,n):
        b=ctypes.create_string_buffer(n); r=ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h,ctypes.c_void_p(a),b,n,ctypes.byref(r)) else None
    def wr(s,a,d): r=ctypes.c_size_t(); return k.WriteProcessMemory(s.h,ctypes.c_void_p(a),d,len(d),ctypes.byref(r))
    def u32(s,a): b=s.rd(a,4); return struct.unpack('<I',b)[0] if b else 0
    def f32(s,a): b=s.rd(a,4); return struct.unpack('<f',b)[0] if b else float('nan')
def sha(p):
    try: return hashlib.sha256(open(p,'rb').read()).hexdigest()
    except Exception: return None
def ventanas(p):
    r=[]; P=ctypes.WINFUNCTYPE(wintypes.BOOL,wintypes.HWND,wintypes.LPARAM)
    def cb(h,l):
        q=wintypes.DWORD(); u.GetWindowThreadProcessId(h,ctypes.byref(q))
        if q.value==p and u.IsWindowVisible(h):
            t=ctypes.create_unicode_buffer(256); c=ctypes.create_unicode_buffer(128); u.GetWindowTextW(h,t,256); u.GetClassNameW(h,c,128); r.append((c.value,t.value))
        return True
    u.EnumWindows(P(cb),0); return r
def jug(m):
    inf=m.u32(0xB7CD74); ped=m.u32(0xB7CD98+inf*0x190); mat=m.u32(ped+0x14) if ped else 0
    return (ped,(m.f32(mat+0x30),m.f32(mat+0x34),m.f32(mat+0x38))) if mat else (ped,None)
PID_PROPIO=[None]
import glob
def galeria():
    r=[]
    for pat in (os.path.expanduser('~/Documents/GTA San Andreas User Files/Gallery/*.jpg'),JUEGO+'/userfiles/Gallery/*.jpg',JUEGO+'/GTA San Andreas User Files/Gallery/*.jpg','D:/Original/_fase8b_run/userfiles/Gallery/*.jpg'):
        for f in glob.glob(pat): r.append((f,os.path.getsize(f),os.path.getmtime(f)))
    return sorted(r)
def main():
    et=sys.argv[1] if len(sys.argv)>1 and not sys.argv[1].startswith('--') else 'pn'
    seg=int(sys.argv[sys.argv.index('--segundos')+1]) if '--segundos' in sys.argv else 90
    entrada='--entrada' in sys.argv
    asi=os.environ.get('AGUA_ASI')
    if not asi: print('Falta AGUA_ASI'); return 3
    cop=os.path.join(aqui,'ini_original_respaldo'); os.makedirs(cop,exist_ok=True)
    hs=[sha(p) for p in INIS]
    for i,p in enumerate(INIS):
        if os.path.exists(p) and not os.path.exists(os.path.join(cop,'%d.ini'%i)): shutil.copy(p,os.path.join(cop,'%d.ini'%i))
    base=open(os.path.join(aqui,'..','FnPL1','variantes','ini-actual.ini'),encoding='utf-8').read()
    nuevo=base.replace('SaveGameToLoad = 2','SaveGameToLoad = 0'); assert nuevo!=base
    tmp=os.path.join(aqui,'ini_partida_nueva.ini'); open(tmp,'w',encoding='utf-8',newline='').write(nuevo)
    try: return _main(et,seg,entrada,asi,tmp)
    finally:
        if PID_PROPIO[0]: subprocess.run(['taskkill','/F','/PID',str(PID_PROPIO[0])],capture_output=True)
        time.sleep(8)
        for i,p in enumerate(INIS):   # restauracion garantizada (jugar_nueva.ps1 ya restaura; esto es un segundo cinturon)
            if hs[i] is not None and sha(p)!=hs[i] and os.path.exists(os.path.join(cop,'%d.ini'%i)):
                time.sleep(10)
                if sha(p)!=hs[i]: shutil.copy(os.path.join(cop,'%d.ini'%i),p); print('INI restaurado por el script',p)
        print('INI restaurados OK:',[sha(p)==hs[i] for i,p in enumerate(INIS)])
def _main(et,seg,entrada,asi,tmp):
    t0w=time.time()
    t=time.time()
    while (pid() or os.path.exists('D:/Original/gta_lock') or os.path.exists(os.path.join(aqui,'respaldo_pendiente'))) and time.time()-t<900: time.sleep(3)
    logIni=os.path.getsize(LOG) if os.path.exists(LOG) else 0
    lan=subprocess.Popen(['pwsh','-NoProfile','-File',os.path.join(aqui,'jugar_nueva.ps1'),'-Variante','X','-AsiPath',asi,'-IniPath',tmp,'-Etiqueta',et,'-Auto','%d'%(seg+150),'-Silencioso','-NoEnfocar'],cwd=aqui,stdout=open('D:/Original/sim_out/jugar_%s.log'%et,'w'),stderr=subprocess.STDOUT)
    p=None; t0=time.time()
    while time.time()-t0<90 and not p: p=pid(); time.sleep(1)
    if not p: print('ERROR sin juego'); return 3
    PID_PROPIO[0]=p; m=M(p); t0=time.time()
    estado9=None; hist=[]; popup=[]; muerto=False; caps=0; ult_cap=0; entrada_hecha=False; cs_prev=[None]; t_cs=[time.time()]; t_fin=0
    while time.time()-t0<seg+120:
        if lan.poll() is not None or pid()!=p: muerto=True; print('El proceso se cerro a los %.0f s'%(time.time()-t0)); break
        m.wr(0x8D621C,struct.pack('<I',1))
        for w in ventanas(p):
            if w[0] not in ('Grand theft auto San Andreas','ConsoleWindowClass') and w not in popup: popup.append(w); print('VENTANA EXTRA',w,flush=True)
        gs=m.u32(0xC8D4C0); fr=m.u32(0xB7CB4C); ped,pos=jug(m)
        if gs==9 and estado9 is None: estado9=time.time(); print('gGameState=9 a los %.0f s'%(estado9-t0),flush=True)
        if estado9:
            hist.append((time.time()-estado9,fr,pos))
            if time.time()-ult_cap>25 and caps<14:
                ult_cap=time.time(); caps+=1
                try: print('captura',caps,captura.capturar('D:/Original/sim_out/pn_%s_%d.png'%(et,caps),traer=False),'pos',pos,'frame',fr,flush=True)
                except Exception as e: print('captura fallo',e)
            cs=m.rd(0xB5F851,1); cs=cs[0] if cs else 1
            if cs!=cs_prev[0]: cs_prev[0]=cs; t_cs[0]=time.time(); print('CCutsceneMgr::ms_running=%d a los %.0f s tras estado 9'%(cs,time.time()-estado9),flush=True)
            if not entrada_hecha and cs==0 and time.time()-t_cs[0]>20 and time.time()-estado9>40:
                entrada_hecha=True; t_fin=time.time()+25
                antes=galeria()
                m.wr(0xBA6830,bytes([1])); m.wr(0xC8A7C1,bytes([1]))
                print('Foto solicitada (m_bSavePhotos=1, ms_bTakePhoto=1); galeria antes:',antes,flush=True)
            if False:
                time.sleep(5); entrada_hecha=True; t_fin=time.time()+10
                print('Simulando entrada (stick adelante via CPad) desde',pos,flush=True)
                for _ in range(120):
                    m.wr(0xB73458+2,struct.pack('<h',-128)); time.sleep(0.03)
                time.sleep(1); print('posicion tras entrada',jug(m)[1],flush=True)
                try: captura.capturar('D:/Original/sim_out/pn_%s_entrada.png'%et,traer=False)
                except Exception: pass
            if (entrada_hecha and time.time()>t_fin) or (not entrada and time.time()-estado9>seg) or time.time()-estado9>seg: break
        time.sleep(0.5)
    h=hist
    r={'gGameState9':estado9 is not None,'proceso_cerrado':muerto,'ventanas_extra':popup}
    if h:
        r['frames']=(h[0][1],h[-1][1]); r['frames_suben']=h[-1][1]>h[0][1]+100
        ps=[x[2] for x in h if x[2]]; r['pos_inicial']=ps[0] if ps else None; r['pos_final']=ps[-1] if ps else None
        g=(2488,-1666,13)
        r['dist_grove_final']=round(math.dist(ps[-1],g),1) if ps else None
        r['dist_grove_min']=round(min(math.dist(q,g) for q in ps),1) if ps else None
    try: captura.capturar('D:/Original/sim_out/pn_%s_final.png'%et,traer=False)
    except Exception: pass
    txt=''
    try:
        with open(LOG,'rb') as f: f.seek(min(logIni,os.path.getsize(LOG))); txt=f.read().decode('utf-8','replace')
    except Exception as e: print('log',e)
    r['log_Unknown_param']=txt.count('Unknown param type'); r['log_error']=txt.count('[error]'); r['log_Unreachable']=txt.count('Unreachable')
    r['log_lineas_error']=[l for l in txt.splitlines() if '[error]' in l or 'Unknown param' in l][:15]
    json.dump(r,open('D:/Original/sim_out/pn_%s.json'%et,'w'),indent=1,default=str); print(json.dumps(r,indent=1,default=str))
    fotos=[g for g in galeria() if g[2]>t0w]
    ok_jpeg=False
    for f,sz,_ in fotos:
        d=open(f,'rb').read(); print('JPEG nuevo',f,sz,d[:3].hex(),d[-2:].hex()); ok_jpeg=ok_jpeg or (sz>2000 and d[:3]==bytes([0xFF,0xD8,0xFF]) and d[-2:]==bytes([0xFF,0xD9]))
    r['jpeg_nuevos']=[(f,sz) for f,sz,_ in fotos]; r['jpeg_valido']=ok_jpeg
    print('JPEG',ok_jpeg)
    ok=ok_jpeg and r['gGameState9'] and not muerto and not popup and r.get('frames_suben') and r['log_Unknown_param']==0 and (r.get('dist_grove_min') or 1e9)<80
    print('VEREDICTO','BIEN' if ok else 'MAL')
    subprocess.run(['taskkill','/F','/PID',str(p)],capture_output=True)
    try: lan.wait(timeout=90)
    except Exception: lan.kill()
    return 0 if ok else 1
sys.exit(main())
