# Sonda 'dato en dos sitios' (grupo G5, lotes 1-30): encadena los escenarios de FnJuego/sonda_juego.py y, en un hilo aparte, lee por memoria
# (ReadProcessMemory sobre el PROCESO PROPIO, --pid) cada 6 s los bytes de cada objeto DatosExe en su direccion propia (mapa de enlace) y en la
# direccion del EXE original (VA). Guarda las lecturas distintas de cada objeto en datos_snap.json.
# Uso: python -I sonda_datos.py <salida> <escenarios> --pid N --dir <carpeta> --log <log> --objetos objetos.json
import sys, os, json, time, threading
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, os.path.join(aqui, '..', 'FnJuego')); sys.path.insert(0, os.path.join(aqui, '..', 'FnTraf'))
import sonda_juego as SJ, sonda_traf as S
def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
def hilo(sal, pid, objetos):
    m = S.M(pid); T = json.load(open(objetos)); vistos = {}; t0 = time.time(); n = 0
    LO, HI = 0x8E4000, 0xCB1000; acc = [0]; ultimo = bytearray(HI - LO)
    def vuelca():
        for a in range(LO, HI, 0x10000):
            b = m.rd(a, min(0x10000, HI - a))
            if b is None: continue
            ultimo[a - LO:a - LO + len(b)] = b
        acc[0] |= int.from_bytes(bytes(ultimo), 'little')
    def guarda():
        open(os.path.join(sal, 'bss_mask.bin'), 'wb').write(acc[0].to_bytes(HI - LO, 'little')); open(os.path.join(sal, 'bss_last.bin'), 'wb').write(bytes(ultimo))
    while True:
        if not S.pid('gta_sa_standalone'): break
        for o in T:
            if not o['own']: continue
            a = m.rd(o['va'], o['tam']); b = m.rd(o['own'], o['tam'])
            if a is None or b is None: continue
            k = (a.hex(), b.hex()); L = vistos.setdefault(o['name'], {})
            if k not in L and len(L) < 40: L[k] = round(time.time() - t0, 1)
        n += 1
        if n % 2 == 0: vuelca()
        if n % 3 == 0:
            guarda()
            json.dump({k: [(a, b, t) for (a, b), t in v.items()] for k, v in vistos.items()}, open(os.path.join(sal, 'datos_snap.json'), 'w'))
        time.sleep(6)
    guarda()
    json.dump({k: [(a, b, t) for (a, b), t in v.items()] for k, v in vistos.items()}, open(os.path.join(sal, 'datos_snap.json'), 'w'))
if __name__ == '__main__':
    sal = sys.argv[1]; os.makedirs(sal, exist_ok=True)
    th = threading.Thread(target=hilo, args=(sal, int(arg('--pid')), arg('--objetos')), daemon=True); th.start()
    SJ.main()
    time.sleep(7)
