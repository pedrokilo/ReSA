# Sonda dinamica de 'dato en dos sitios' (DatosExe). Se lanza como sonda_juego.py (mismos argumentos) y, ademas de ejecutar los escenarios,
# vigila por memoria (desde fuera del proceso) la memoria del EXE original (.bss 0x8E4000-0xCA0000) donde NO deberia escribir nadie que use
# objetos DatosExe: cualquier byte distinto de cero dentro de la extension de un objeto propio indica un escritor por literal/direccion.
# Compara ademas el objeto propio (direccion real segun el .map) con la memoria del EXE. Vuelca <salida>/datos_dyn.json cada 10 s y al final.
# Uso: python -I sonda_datos.py <salida> <escenarios> --pid N --dir <carpeta> --log <log> --mapa <gta_sa_standalone.map> [--sizes sizes.json]
import sys, os, json, time, threading, struct
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui); sys.path.insert(0, os.path.join(aqui, '..', 'FnJuego')); sys.path.insert(0, os.path.join(aqui, '..', 'FnTraf'))
import objmap
import sonda_traf as S
import sonda_juego as SJ
def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
LO, HI = 0x8E4000, 0xCA0000
TT = bytes([0] + [1] * 255)
def tabla():
    syms = objmap.cargar(arg('--mapa'))
    sizes = {}
    if arg('--sizes') and os.path.exists(arg('--sizes')): sizes = {int(k): v for k, v in json.load(open(arg('--sizes'))).items()}
    objs = []
    if os.path.exists(os.path.join(aqui, 'sizes_slots.json')): sizes.update({int(k): v for k, v in json.load(open(os.path.join(aqui, 'sizes_slots.json'))).items()})
    for i, (va, nom, ea, ns, objf) in enumerate(syms):
        if ns not in ('Bss', 'R5') or nom.startswith('?kConstruct'): continue
        if not (LO <= ea < HI): continue
        sz = sizes.get(ea)
        if sz is None:
            nxt = syms[i + 1][0] if i + 1 < len(syms) else va + 4
            sz = min(max(1, nxt - va), 4096)
        objs.append({'ea': ea, 'va': va, 'sz': sz, 'nom': nom})
    return objs
def vigilar(sal, pid):
    m = S.M(pid); objs = tabla(); ever = bytearray(HI - LO); ever_i = [0]; t0 = time.time(); first = {}; n = 0
    def leer():
        buf = bytearray(HI - LO); ok = 0
        for a in range(LO, HI, 0x10000):
            b = m.rd(a, min(0x10000, HI - a))
            if b: buf[a - LO:a - LO + len(b)] = b; ok += len(b)
        return buf, ok
    ultimo = {'buf': None}; obj_nz = set()
    def volcar(final=False):
        buf, ok = leer()
        if ok: ultimo['buf'] = buf
        elif ultimo['buf'] is not None: buf = ultimo['buf']
        else: return 0
        res = []
        for o in objs:
            e = o['ea'] - LO; n_ = o['sz']; b = bytes(buf[e:e + n_]); ev = ever[e:e + n_]
            if any(ev):
                ob = (m.rd(o['va'], n_) if ok else None) or b''
                offs = [i for i, x in enumerate(ev) if x]
                res.append({'ea': hex(o['ea']), 'sz': n_, 'nom': o['nom'], 'exe_ahora': b.hex()[:1024], 'offs_nz': offs[:64], 'exe_alguna_vez_nz': len(offs), 'obj': ob.hex()[:1024], 'primera_t': first.get(o['ea'])})
        json.dump({'t': round(time.time() - t0, 1), 'leido': ok, 'objetos': len(objs), 'final': final, 'obj_nz': sorted(obj_nz), 'hallazgos': res}, open(os.path.join(sal, 'datos_dyn.json.tmp'), 'w'))
        os.replace(os.path.join(sal, 'datos_dyn.json.tmp'), os.path.join(sal, 'datos_dyn.json'))
        return len(res)
    while S.pid('gta_sa_standalone'):
        buf, ok = leer()
        if ok:
            nzm = bytes(buf).translate(TT); ever_i[0] |= int.from_bytes(nzm, 'little'); ever[:] = ever_i[0].to_bytes(HI - LO, 'little')
            for o in objs:
                ob_ = m.rd(o['va'], o['sz'])
                if ob_ and any(ob_): obj_nz.add(o['ea'])
            for o in objs:
                e = o['ea'] - LO
                if o['ea'] not in first and any(ever[e:e + o['sz']]): first[o['ea']] = round(time.time() - t0, 1)
        n += 1
        if n % 3 == 0: volcar()
        time.sleep(3)
    volcar(True)

def esc_extra(j):
    """menu de pausa, climas por trucos, punetazos, muerte y reaparicion (CRestart/CReplay/CWeather/MenuManager)"""
    SJ.suelo_libre(j); j.al_frente()
    SJ.pulsar('esc', 0.2); j.dormir(4); j.foto('extra_menu', 'menu de pausa'); SJ.pulsar('esc', 0.2); j.dormir(3)
    nm = SJ.nombres_cheats(); mp = SJ.mapa_trucos(j); inv = {v: k for k, v in nm.items()}
    for cod, idx in list(mp.items()):
        n = inv.get(idx, '')
        if not j.alive(): break
        if 'WEATHER' in n or 'STORM' in n or 'FOG' in n:
            j.al_frente(); j.escribir_truco(cod); j.P('clima', n, cod); j.dormir(6)
    j.al_frente()
    for _ in range(25):
        SJ.raton_boton('l', 1); j.dormir(0.15); SJ.raton_boton('l', 0); j.dormir(0.25)
    j.P('punetazos hechos; pos', j.pos())
    p, _ = j.ped()
    if p:
        j.m.wr(p + 0x540, __import__('struct').pack('<f', 0.0)); j.P('salud a 0'); j.dormir(25); j.foto('extra_muerte', 'tras morir')
        j.P('tras morir: area', j.area(), 'pos', j.pos(), 'vivo', j.alive())
    j.dormir(10)
SJ.ESCENARIOS['extra'] = esc_extra
if __name__ == '__main__':
    sal = sys.argv[1]; os.makedirs(sal, exist_ok=True)
    pid = None
    for _ in range(300):
        pid = S.pid('gta_sa_standalone')
        if pid: break
        time.sleep(1)
    if '--sin-gimnasio' in sys.argv:   # el exe sin la correccion del gimnasio (96f1c36f) cae al entrar: no se teletransporta a su puerta
        _tp = SJ.J.tp
        def tp2(self, x, y, z):
            if abs(x - 2229.63) < 1 and abs(y + 1721.63) < 1: return
            return _tp(self, x, y, z)
        SJ.J.tp = tp2
    th = threading.Thread(target=vigilar, args=(sal, pid), daemon=True); th.start()
    SJ.main()
    time.sleep(4)
