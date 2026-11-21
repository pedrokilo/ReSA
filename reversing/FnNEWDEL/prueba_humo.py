# Copia de FnS31AFILE/prueba_humo.py (que a su vez copia FnBIS1/prueba_ropero.py): prueba la DLL de ESTE arbol (wv-nd) y no lanza nada si el juego del usuario sigue activo tras 900 s.
import sys; sys.path.insert(0, r'D:\Original\wv-8\reversing\FnBIS1')
# Prueba automatica del ropero: lanza el juego con un archivo de argumentos, espera al interior (area 14), teletransporta al jugador
# frente al ropero (ropero_pos.json), captura la ventana y mide los pixeles rojos del marcador. Imprime BIEN/MAL.
# Uso: python prueba_ropero.py <archivo.args> [etiqueta] [--umbral N]
import ctypes, struct, subprocess, sys, time, json, os, math
from ctypes import wintypes
import captura
aqui=r'D:\Original\wv-8\reversing\FnBIS1'
sys.path.insert(0,aqui)
ASI=r'D:\Original\wv-nd\bin\Debug\gta_reversed.asi'
k=ctypes.WinDLL('kernel32',use_last_error=True); k.OpenProcess.restype=wintypes.HANDLE
def pid():
    o=subprocess.run(['tasklist','/FI','IMAGENAME eq gta_sa.exe','/FO','CSV','/NH'],capture_output=True,text=True).stdout
    for l in o.splitlines():
        if 'gta_sa' in l: return int(l.split('","')[1])
class M:
    def __init__(s,p): s.h=k.OpenProcess(0x38|0x400,False,p)
    def rd(s,a,n):
        b=ctypes.create_string_buffer(n); r=ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h,ctypes.c_void_p(a),b,n,ctypes.byref(r)) else None
    def wr(s,a,d): r=ctypes.c_size_t(); return k.WriteProcessMemory(s.h,ctypes.c_void_p(a),d,len(d),ctypes.byref(r))
    def u32(s,a): b=s.rd(a,4); return struct.unpack('<I',b)[0] if b else 0
    def f32(s,a): b=s.rd(a,4); return struct.unpack('<f',b)[0] if b else float('nan')

def texnames(m,rw):
    """nombres de textura de todos los materiales del objeto RenderWare (clump o atomico)"""
    out=[]
    t=m.rd(rw,1)
    if not t: return out
    atomics=[]
    if t[0]==1: atomics=[rw]
    elif t[0]==2:
        first=m.u32(rw+8); l=first; n=0
        while l and l!=rw+8 and n<32: atomics.append(l-0x40); l=m.u32(l); n+=1
    for a in atomics:
        g=m.u32(a+0x18)
        if not g: out.append('<sin geometria>'); continue
        mats=m.u32(g+0x20); nm=m.u32(g+0x24)
        for i in range(min(nm,64)):
            mt=m.u32(mats+4*i); tx=m.u32(mt) if mt else 0
            if tx:
                nmb=m.rd(tx+0x10,32); out.append(nmb.split(bytes([0]))[0].decode('latin1') if nmb else '?')
            else: out.append('<sin textura>')
    return out
def volcar(m,pos,radio=7.0):
    res={'entidades':[],'marcadores':[]}
    for nombre,pool_ptr,tam in (('edificio',0xB74498,0x38),('objeto',0xB7449C,0x19C),('dummy',0xB744A0,0x38)):
        p=m.u32(pool_ptr); objs=m.u32(p); flags=m.u32(p+4); size=m.u32(p+8)
        fl=m.rd(flags,size) or b''
        for i in range(size):
            if fl[i]&0x80: continue
            e=objs+i*tam; mat=m.u32(e+0x14)
            x,y,z=(m.f32(mat+0x30),m.f32(mat+0x34),m.f32(mat+0x38)) if mat else (m.f32(e+4),m.f32(e+8),m.f32(e+12))
            if not(abs(x-pos['x'])<radio and abs(y-pos['y'])<radio and abs(z-pos['z'])<radio): continue
            rw=m.u32(e+0x18); mod=struct.unpack('<H',m.rd(e+0x22,2))[0]
            res['entidades'].append({'tipo':nombre,'modelo':mod,'pos':[round(x,2),round(y,2),round(z,2)],'rw':bool(rw),'tex':sorted(texnames(m,rw)) if rw else None})
    res['entidades'].sort(key=lambda d:(d['tipo'],d['modelo'],d['pos']))
    for i in range(32):
        r=m.rd(0xC7DD58+i*0xA0,0xA0)
        if not r: continue
        x,y,z=struct.unpack('<3f',r[0x30:0x3C]); atom=struct.unpack('<I',r[0x48:0x4C])[0]; t=struct.unpack('<H',r[0x50:0x52])[0]
        if t!=257 and atom: res['marcadores'].append({'i':i,'tipo':t,'pos':[round(x,1),round(y,1),round(z,1)],'color':list(r[0x58:0x5C])})
    return res

def diag(m,mod=2877):
    out={}
    info=m.u32(0xA9B0C8+4*mod); out['modelinfo']=hex(info)
    if info:
        out['refs']=struct.unpack('<H',m.rd(info+4,2))[0]; out['rwObject']=hex(m.u32(info+0x14)); out['txd']=struct.unpack('<h',m.rd(info+0xA,2))[0]
    st=0x8E4CC0+20*mod; b=m.rd(st,20); out['stream_loadstate']=b[0x10] if b else None; out['stream_raw']=b.hex() if b else None
    # todos los objetos con ese modelo
    p=m.u32(0xB7449C); objs=m.u32(p); flags=m.u32(p+4); size=m.u32(p+8); fl=m.rd(flags,size); lst=[]
    for i in range(size):
        if fl[i]&0x80: continue
        e=objs+i*0x19C
        if struct.unpack('<H',m.rd(e+0x22,2))[0]==mod: lst.append({'i':i,'rw':hex(m.u32(e+0x18)),'flags':m.rd(e+0x1C,4).hex()})
    out['objetos']=lst
    return out
PID_PROPIO=[None]
def main():
    try: return _main()
    finally:
        if PID_PROPIO[0]: subprocess.run(['taskkill','/F','/PID',str(PID_PROPIO[0])],capture_output=True)
def _main():
    args=sys.argv[1]; et=sys.argv[2] if len(sys.argv)>2 and not sys.argv[2].startswith('--') else 'rop'
    umbral=int(sys.argv[sys.argv.index('--umbral')+1]) if '--umbral' in sys.argv else 40
    pos=json.load(open(os.path.join(aqui,'ropero_pos.json')))
    t=time.time()
    while (pid() or os.path.exists('D:/Original/gta_lock') or os.path.exists(os.path.join(aqui,'respaldo_pendiente'))) and time.time()-t<900: time.sleep(3)
    if pid() or os.path.exists('D:/Original/gta_lock') or os.path.exists(os.path.join(aqui,'respaldo_pendiente')): print('NO LANZADO: el juego del usuario sigue activo tras 900 s'); return 4
    if os.environ.get('GTASA_CMD_BYPASS_FILE'): os.environ['GTASA_CMD_BYPASS']=os.environ['GTASA_CMD_BYPASS_FILE']
    lan=subprocess.Popen(['pwsh','-NoProfile','-File',os.path.join(aqui,'jugar.ps1'),'-Variante','X','-AsiPath',ASI,'-Etiqueta',et,'-ArgsFile',args,'-Auto','240','-Silencioso','-NoEnfocar'],cwd=aqui,stdout=open('D:/Original/sim_out/jugar_%s.log'%et,'w'),stderr=subprocess.STDOUT)
    p=None; t0=time.time()
    while time.time()-t0<90 and not p:
        p=pid(); time.sleep(1)
    if not p: print('ERROR sin juego'); return 3
    PID_PROPIO[0]=p; m=M(p); ok=False
    t_ent=0
    while time.time()-t0<240:
        if lan.poll() is not None: print('ERROR el juego se cerro'); return 3
        m.wr(0x8D621C,struct.pack('<I',1))      # ForegroundApp=1: no pausar en segundo plano
        try:
            inf=m.u32(0xB7CD74); ped=m.u32(0xB7CD98+inf*0x190); mat=m.u32(ped+0x14) if ped else 0
            if m.u32(0xC8D4C0)==9 and mat:
                area=m.u32(0xB72914)
                if area==pos['area']: ok=True; break
                # En la casa: pisar el marcador CHANGER (vestidor) para que el juego haga la transicion al ropero
                if area==3 and time.time()-t_ent>0.4:
                    t_ent=time.time(); ex,ey,ez=2492.4,-1708.5,1019.3
                    m.wr(mat+0x30,struct.pack('<3f',ex,ey,ez)); m.wr(ped+0x558,struct.pack('<2f',3.1416,3.1416))
        except Exception: pass
        time.sleep(0.25)
    if not ok: print('ERROR no llego al interior'); lan.kill(); return 3
    time.sleep(8)   # deja cargar el interior y asentarse
    inf=m.u32(0xB7CD74); ped=m.u32(0xB7CD98+inf*0x190); mat=m.u32(ped+0x14)
    antes=(m.f32(mat+0x30),m.f32(mat+0x34),m.f32(mat+0x38)) if mat else None
    print('posicion antes del teletransporte',antes,flush=True)
    t1=time.time(); res=None
    while time.time()-t1<8:       # sin segundo teletransporte: solo se mantiene ForegroundApp y se espera
        m.wr(0x8D621C,struct.pack('<I',1)); time.sleep(0.5)
    time.sleep(3)
    inf=m.u32(0xB7CD74); ped=m.u32(0xB7CD98+inf*0x190); mat=m.u32(ped+0x14)
    print('posicion despues',(m.f32(mat+0x30),m.f32(mat+0x34),m.f32(mat+0x38)),flush=True)
    try: captura.capturar('D:/Original/sim_out/ropero_%s.png'%et,traer=False)
    except Exception as e: print('captura fallo',e)
    print('DIAG',json.dumps(diag(m)),flush=True)
    v=volcar(m,pos); json.dump(v,open('D:/Original/sim_out/estado_%s.json'%et,'w'),indent=1)
    print(et,'entidades',len(v['entidades']),'marcadores',len(v['marcadores']))
    puerta=any(d['modelo']==2877 and d['rw'] for d in v['entidades'])
    marc=any(abs(q['pos'][0]-pos['x'])<8 and abs(q['pos'][1]-pos['y'])<8 and abs(q['pos'][2]-pos['z'])<8 for q in v['marcadores'])
    print('puerta',puerta,'marcador',marc)
    bien=puerta and marc
    print('VEREDICTO','BIEN' if bien else 'MAL')
    subprocess.run(['taskkill','/F','/PID',str(p)],capture_output=True); lan.wait(timeout=60)
    return 0 if bien else 1
sys.exit(main())
