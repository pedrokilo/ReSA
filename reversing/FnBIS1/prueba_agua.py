# Prueba automatica del agua oscura: lanza el juego, teletransporta a CJ UNA vez al punto del agua (agua_pos.json), espera y vuelca
# variables de estado (UnderWaterness, CColourSet actual, niebla, camara) en D:/Original/sim_out/agua_<etiqueta>.json.
# Uso: python prueba_agua.py <archivo.args> [etiqueta] [--z -3.0] [--solo-inicio]
import captura
import ctypes, struct, subprocess, sys, time, json, os
from ctypes import wintypes
aqui=os.path.dirname(os.path.abspath(__file__))
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
CS=['AmbR','AmbG','AmbB','AmbObjR','AmbObjG','AmbObjB','DirR','DirG','DirB']+['SkyTopR','SkyTopG','SkyTopB','SkyBotR','SkyBotG','SkyBotB','SunCoreR','SunCoreG','SunCoreB','SunCorR','SunCorG','SunCorB']
def colourset(m):
    b=m.rd(0xB7C4A0,172)
    if not b: return {}
    o={}
    f=struct.unpack_from('<9f',b,0)
    for n,v in zip(CS[:9],f): o[n]=round(v,3)
    u=struct.unpack_from('<12H',b,36)
    for n,v in zip(CS[9:],u): o[n]=v
    o['SunSize'],o['SpriteSize'],o['SpriteBright']=[round(x,3) for x in struct.unpack_from('<3f',b,60)]
    o['Shadow'],o['LightShadow'],o['PoleShadow']=struct.unpack_from('<3h',b,72)
    o['FarClip'],o['FogStart'],o['LightsOnGround']=[round(x,3) for x in struct.unpack_from('<3f',b,80)]
    o['LowClouds']=list(struct.unpack_from('<3H',b,92)); o['Fluffy']=list(struct.unpack_from('<3H',b,98))
    o['Water']=[round(x,3) for x in struct.unpack_from('<4f',b,104)]
    o['PostFx1']=[round(x,3) for x in struct.unpack_from('<4f',b,120)]
    o['PostFx2']=[round(x,3) for x in struct.unpack_from('<4f',b,136)]
    o['CloudAlpha']=round(struct.unpack_from('<f',b,152)[0],3); o['HighLight']=struct.unpack_from('<i',b,156)[0]
    o['WaterFogAlpha']=struct.unpack_from('<H',b,160)[0]
    o['Illum'],o['LodMult']=[round(x,3) for x in struct.unpack_from('<2f',b,164)]
    return o
def estado(m):
    inf=m.u32(0xB7CD74); ped=m.u32(0xB7CD98+inf*0x190); mat=m.u32(ped+0x14) if ped else 0
    e={'area':m.u32(0xB72914),'gameState':m.u32(0xC8D4C0)}
    if mat: e['pos']=[round(m.f32(mat+0x30+4*i),3) for i in range(3)]
    e['UnderWaterness']=round(m.f32(0xC8132C),4); e['WaterDepth']=round(m.f32(0xC81330),3)
    e['WaterFogFX']=round(m.f32(0xC81444),3)
    e['FogReduction']=m.u32(0xB79E48); e['ExtraColourInter']=round(m.f32(0xB79E3C),3)
    e['WaterFXStart']=round(m.f32(0x8D514C),3); e['waterEnable']=(m.rd(0xC402D3,1) or b'\xff')[0]
    e['CurrentStored']=m.u32(0xB79FD0)
    b=m.rd(0xB7CB10,4); e['BelowHorizonGrey']=list(b) if b else None
    e['cs']=colourset(m)
    e['vis']=[m.u32(0xB76838),m.u32(0xB7683C),m.u32(0xB76840),m.u32(0xB76844)]
    e['stream']={'memUsed':m.u32(0x8E4CB4),'requested':m.u32(0x8E4CB8),'prio':m.u32(0x8E4BA0)}
    e['waterGeom']=[m.u32(0xC22884),m.u32(0xC22888),m.u32(0xC2288C)]
    e['waterFog']=[(m.rd(0x8D37D4,2) or b'??')[0],(m.rd(0xC228E8,1) or b'?')[0],m.u32(0x8D37E0),round(m.f32(0x8D37E4),3)]
    e['WaterColor']=list(m.rd(0xC2116C,4) or b'')
    e['tcWaterF']=[round(m.f32(0xB7C508+4*i),2) for i in range(4)]
    cm=m.u32(0xB6F028+0x14)
    if cm: e['camPos']=[round(m.f32(cm+0x30+4*i),3) for i in range(3)]
    rwc=m.u32(0xC1703C)  # Scene.m_pRwCamera
    if rwc: e['rwCamNearFar']=[round(m.f32(rwc+0x80),3),round(m.f32(rwc+0x84),3)]
    return e
PID_PROPIO=[None]
def keep(m,seg):
    t=time.time()
    while time.time()-t<seg: m.wr(0x8D621C,struct.pack('<I',1)); time.sleep(0.5)
def main():
    try: return _main()
    finally:
        if PID_PROPIO[0]: subprocess.run(['taskkill','/F','/PID',str(PID_PROPIO[0])],capture_output=True)
def _main():
    args=sys.argv[1]; et=sys.argv[2] if len(sys.argv)>2 and not sys.argv[2].startswith('--') else 'agua'
    z=float(sys.argv[sys.argv.index('--z')+1]) if '--z' in sys.argv else -3.0
    pos=json.load(open(os.path.join(aqui,'agua_pos.json')))
    t=time.time()
    while (pid() or os.path.exists('D:/Original/gta_lock') or os.path.exists(os.path.join(aqui,'respaldo_pendiente'))) and time.time()-t<900: time.sleep(3)
    if pid() or os.path.exists('D:/Original/gta_lock'): print('ERROR hay otro juego/candado tras 900 s'); return 3
    os.makedirs('D:/Original/sim_out',exist_ok=True)
    asi=os.environ.get('AGUA_ASI'); var=['-Variante','X','-AsiPath',asi] if asi else ['-Variante','A']
    lan=subprocess.Popen(['pwsh','-NoProfile','-File',os.path.join(aqui,'jugar.ps1')]+var+['-Etiqueta',et,'-ArgsFile',args,'-Auto','240','-Silencioso','-NoEnfocar'],cwd=aqui,stdout=open('D:/Original/sim_out/jugar_%s.log'%et,'w'),stderr=subprocess.STDOUT)
    p=None; t0=time.time()
    while time.time()-t0<90 and not p:
        p=pid(); time.sleep(1)
    if not p: print('ERROR sin juego'); return 3
    PID_PROPIO[0]=p; m=M(p); ok=False
    while time.time()-t0<230:
        if lan.poll() is not None: print('ERROR el juego se cerro'); return 3
        m.wr(0x8D621C,struct.pack('<I',1))
        inf=m.u32(0xB7CD74); ped=m.u32(0xB7CD98+inf*0x190); mat=m.u32(ped+0x14) if ped else 0
        if m.u32(0xC8D4C0)==9 and mat: ok=True; break
        time.sleep(0.25)
    if not ok: print('ERROR no llego a jugar; gameState',m.u32(0xC8D4C0),'inf',m.u32(0xB7CD74),'ped',hex(m.u32(0xB7CD98+m.u32(0xB7CD74)*0x190)),flush=True); return 3
    keep(m,20)   # asentamiento
    out={'args':open(args).read(),'inicio':estado(m)}
    print('INICIO',json.dumps({k_:out['inicio'].get(k_) for k_ in ('area','pos','UnderWaterness')}),flush=True)
    if '--solo-inicio' in sys.argv: return 2
    inf=m.u32(0xB7CD74); ped=m.u32(0xB7CD98+inf*0x190); mat=m.u32(ped+0x14)
    # UNA sola teletransportacion: posicion en la matriz, area 0 (global y de la entidad), velocidad 0
    m.wr(mat+0x30,struct.pack('<3f',pos['x'],pos['y'],z)); m.wr(ped+0x2F,bytes([0])); m.wr(0xB72914,struct.pack('<I',0)); m.wr(ped+0x44,struct.pack('<3f',0,0,0))
    snaps=[]; prev=0; hold='--sin-hold' not in sys.argv
    def espera(seg):
        t=time.time()
        while time.time()-t<seg:
            m.wr(0x8D621C,struct.pack('<I',1))
            if hold:
                mat2=m.u32(m.u32(0xB7CD98+m.u32(0xB7CD74)*0x190)+0x14)
                m.wr(mat2+0x30,struct.pack('<3f',pos['x'],pos['y'],z)); m.wr(m.u32(0xB7CD98+m.u32(0xB7CD74)*0x190)+0x44,struct.pack('<3f',0,0,0))
            time.sleep(0.05)
    # serie rapida de los primeros 3 s (maximo UnderWaterness)
    serie=[]; t1=time.time()
    while time.time()-t1<3:
        espera(0.1); e=estado(m); serie.append((round(time.time()-t1,2),e['pos'][2],e['UnderWaterness'],e['cs'].get('FarClip'),e['cs'].get('FogStart')))
    out['serie']=serie; prev=3
    for seg in (8,20,35):
        espera(seg-prev); prev=seg
        e=estado(m); e['t']=seg
        try: e['pantalla']=captura.capturar('D:/Original/sim_out/agua_%s_%d.png'%(et,seg),traer=False)
        except Exception as ex: e['pantalla']=str(ex)
        snaps.append(e)
    out['snaps']=snaps
    json.dump(out,open('D:/Original/sim_out/agua_%s.json'%et,'w'),indent=1)
    s=snaps[-1]
    print('FINAL',json.dumps({k_:s.get(k_) for k_ in ('area','pos','camPos','UnderWaterness','WaterDepth','rwCamNearFar')}),flush=True)
    print('CS',json.dumps(s['cs']),flush=True)
    # CRITERIO: bajo el agua (UnderWaterness>=0.9) la ventana no puede estar casi negra: media de brillo > 15
    pa=s.get('pantalla'); lum=sum(pa['media_rgb'])/3 if isinstance(pa,dict) else -1
    bien=s['UnderWaterness']>=0.9 and lum>15
    print('brillo medio',round(lum,1),'VEREDICTO','BIEN' if bien else 'MAL',flush=True)
    return 0 if bien else 1
sys.exit(main())
