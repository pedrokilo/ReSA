# Sonda de PRUEBAS DE GARAJES del standalone (SilentPatch, segunda tanda: trenes/bombas/pinturas en coches guardados, Pay 'n Spray, impound).
# Se lanza desde ejecutar_garajes.ps1 (candado gta_lock, carpeta D:\Original\_juego_run). Solo toca el proceso con --pid.
# Uso: python -I sonda_garajes.py <salida> <escenario[,escenario]> --pid N --dir <carpeta_ejecucion> --log <log.log>
# Escenarios: inspeccion, restaurar (trenes, bombas en coche y moto), pintura, pns (Pay 'n Spray), impound
import sys, os, struct, time, math, json
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import sonda_juego as SJ
S = SJ.S
J = SJ.J; arg = SJ.arg

GAR = 0x96C048; GSZ = 0xD8; NG = 50
SAFE = 0x96ABD8   # CStoredCar[20][4], 0x40 bytes cada uno
def garajes(j):
    r = []
    for i in range(NG):
        b = GAR + i * GSZ; d = j.m.rd(b, 0x50)
        if not d: continue
        tipo = d[0x4C]
        if tipo == 0: continue
        px, py, pz = struct.unpack('<3f', d[0:12]); ap = struct.unpack('<8f', d[0x1C:0x3C])
        r.append(dict(i=i, base=b, tipo=tipo, estado=d[0x4D], flags=d[0x4E], tipo0=d[0x4F], pos=(round(px, 1), round(py, 1), round(pz, 1)),
                      izq=round(ap[3], 1), der=round(ap[4], 1), frente=round(ap[5], 1), atras=round(ap[6], 1), pos_puerta=round(ap[7], 2),
                      nombre=d[0x44:0x4C].split(b'\0')[0].decode('latin1')))
    return r
def coche_guardado(j, casa, ranura):
    d = j.m.rd(SAFE + (casa * 4 + ranura) * 0x40, 0x40)
    pos = struct.unpack('<3f', d[0:12]); fl = d[0x10]; modelo = struct.unpack('<H', d[0x12:0x14])[0]
    mods = struct.unpack('<15h', d[0x14:0x32]); col = tuple(d[0x32:0x36]); radio = d[0x36]; comp = tuple(d[0x37:0x39]); bomba = d[0x39]; pint = d[0x3A]; nitro = d[0x3B]
    return dict(pos=pos, flags=fl, modelo=modelo, bomba=bomba, pintura=pint, nitro=nitro, col=col)
def escribir_guardado(j, casa, ranura, modelo, pos, bomba=0, pintura=0, flags=0, fwd=(0, 100, 0)):
    d = bytearray(0x40)
    d[0:12] = struct.pack('<3f', *pos); d[0x10] = flags; d[0x12:0x14] = struct.pack('<H', modelo); d[0x14:0x32] = struct.pack('<15h', *([-1] * 15))
    d[0x32:0x36] = bytes((1, 1, 1, 1)); d[0x36] = 0; d[0x37:0x39] = bytes((255, 255)); d[0x39] = bomba; d[0x3A] = pintura; d[0x3B] = 0
    d[0x3C:0x3F] = struct.pack('<3b', *fwd)
    j.m.wr(SAFE + (casa * 4 + ranura) * 0x40, bytes(d))
def vehiculos_det(j):
    r = []
    for v in SJ.vehiculos(j):
        mt = j.m.u32(v + 0x14)
        if not mt: continue
        r.append(dict(p=v, modelo=j.modelo(v), pos=j.vpos(v)))
    return r
def vcampos(j, v):
    return dict(modelo=j.modelo(v), tipo=j.m.u32(v + 0x590), subtipo=j.m.u32(v + 0x594), bomba=j.m.u8(v + 0x4A8) & 7, duenio=hex(j.m.u32(v + 0x4CC)),
                remap_cargado=struct.unpack('<h', j.m.rd(v + 0x598, 2))[0], remap_pend=struct.unpack('<h', j.m.rd(v + 0x59A, 2))[0],
                suciedad=round(j.m.f32(v + 0x4B0), 2), flags_fis=hex(j.m.u32(v + 0x40)), tren=hex(struct.unpack('<H', j.m.rd(v + 0x5B8, 2))[0]),
                estado=j.m.u8(v + 0x36) >> 3, salud=round(j.m.f32(v + 0x4C0), 1))

def esc_inspeccion(j):
    SJ.suelo_libre(j)
    j.P('NumGarages(aprox) y garajes:')
    for g in garajes(j): j.P(' ', json.dumps(g))
    for c in range(20):
        for s in range(4):
            g = coche_guardado(j, c, s)
            if g['modelo']: j.P(' guardado casa', c, 'ranura', s, json.dumps(g))
    j.P('jugador', j.pos(), 'vehiculos', [(hex(v['p']), v['modelo']) for v in vehiculos_det(j)][:10])


CJ = 9            # garaje 'cjsafe' (tipo 16), casa 0
PUERTA = (2505.5, -1688.0, 13.5)   # a pie, a unos 3 m del rectangulo del garaje (distancia^2 < 12.25 abre y restaura)
LEJOS = (2652.5, -1719.9, 13.8)    # a >70 m: la puerta se cierra y el garaje guarda los coches de dentro
def estado_garaje(j, i=CJ):
    d = j.m.rd(GAR + i * GSZ, 0x50); return d[0x4D], struct.unpack('<f', d[0x38:0x3C])[0]
def esperar_garaje(j, estado, tmax):
    t = time.time()
    while time.time() - t < tmax and j.alive():
        j.tick(); st, pp = estado_garaje(j)
        if st == estado: return True
        time.sleep(0.2)
    return False
def vehiculos_garaje(j):
    g = j.m.rd(GAR + CJ * GSZ, 0x40); x0, x1, y0, y1 = struct.unpack('<4f', g[0x28:0x38])
    r = []
    for v in vehiculos_det(j):
        if v['pos'] and x0 - 4 <= v['pos'][0] <= x1 + 4 and y0 - 4 <= v['pos'][1] <= y1 + 4: r.append(v)
    return r
def config_guardado(j, sel):
    cfg = {'trenes': [(538, (2505.4, -1695.2, 13.2), 0, 0)],
           'bombas': [(411, (2504.0, -1694.5, 13.2), 2, 0), (463, (2507.0, -1694.5, 13.0), 1, 0)],
           'pintura': [(562, (2505.4, -1695.0, 13.2), 0, 1)],
           'todo': [(411, (2504.0, -1694.0, 13.2), 2, 0), (463, (2507.0, -1694.0, 13.0), 1, 0), (562, (2505.4, -1697.0, 13.2), 0, 1)]}[sel]
    for r in range(4):
        j.m.wr(SAFE + r * 0x40 + 0x12, struct.pack('<H', 0))   # vacia las 4 ranuras de la casa 0 (modelo 0)
    for r, (mod, pos, bomba, pint) in enumerate(cfg): escribir_guardado(j, 0, r, mod, pos, bomba, pint)
    return cfg
def esc_restaurar(j, sel='todo', vuelta=True):
    SJ.suelo_libre(j)
    j.P('estado inicial garaje', estado_garaje(j), 'config', config_guardado(j, sel))
    j.tp(*PUERTA); t0 = time.time(); vistos = {}
    while time.time() - t0 < 45 and j.alive():
        j.tick(); st, pp = estado_garaje(j)
        for v in vehiculos_garaje(j):
            if v['p'] not in vistos:
                vistos[v['p']] = 1; j.P('  VEHICULO NUEVO en el garaje t=%.1f' % (time.time() - t0), hex(v['p']), json.dumps(vcampos(j, v['p'])), 'pos', v['pos'])
        if st != 0 and time.time() - t0 > 3 and not any(coche_guardado(j, 0, r)['modelo'] for r in range(4)): break
        time.sleep(0.25)
    j.P('tras restaurar: garaje', estado_garaje(j), 'ranuras', [coche_guardado(j, 0, r)['modelo'] for r in range(4)], 'vehiculos en garaje', [(hex(v['p']), v['modelo']) for v in vehiculos_garaje(j)])
    j.dormir(2.0)
    for v in vehiculos_garaje(j): j.P('  estado tras 2 s', hex(v['p']), json.dumps(vcampos(j, v['p'])), 'pos', v['pos'])
    j.foto('garaje_abierto', 'garaje de CJ tras restaurar')
    if not vuelta or not j.alive(): return
    j.tp(*LEJOS)     # a >70 m, sin mirar los coches
    ok = esperar_garaje(j, 0, 90)
    j.P('puerta cerrada y guardado hecho:', ok, 'garaje', estado_garaje(j))
    for r in range(4): j.P('  ranura', r, json.dumps(coche_guardado(j, 0, r)))
    j.P('vehiculos que quedan en el garaje:', [(hex(v['p']), v['modelo']) for v in vehiculos_garaje(j)])
def esc_trenes(j): esc_restaurar(j, 'trenes')
def esc_bombas(j): esc_restaurar(j, 'bombas')
def esc_pintura(j): esc_restaurar(j, 'pintura')
def esc_todo(j): esc_restaurar(j, 'todo')

KEEP = 0x969084   # CCarCtrl::apCarsToKeep[2] (StaticRef: direccion real del EXE)
def simbolo(nombre):
    """direccion de un DatosExe::Bss::<nombre> en el standalone (fuera de la imagen del EXE): se lee del .map del ejecutable"""
    import re
    mapa = os.path.join(aqui, '..', 'FnFase8', 'salida', 'gta_sa_standalone.map')
    for l in open(mapa, encoding='latin1'):
        m = re.search(r'\?' + nombre + r'@Bss@DatosExe@@\S+\s+([0-9a-f]{8})\s', l)
        if m: return int(m.group(1), 16)
def fijar_keep(j, i, v):
    j.m.wr(KEEP + 4 * i, struct.pack('<I', v)); j.m.wr(simbolo('b96907C') + 4 * i, struct.pack('<I', j.tiempo()))
def vehiculo_por_modelo(j, modelo, excluir=()):
    return [v for v in vehiculos_det(j) if v['modelo'] == modelo and v['p'] not in excluir]
def restaurar_en(j, lista, tmax=40, sin_gravedad=False):
    """escribe los coches guardados de la casa 0 (modelo, pos, bomba, pintura), se acerca a la puerta a pie y espera a que aparezcan todos; devuelve {modelo: puntero}"""
    for r in range(4): j.m.wr(SAFE + r * 0x40 + 0x12, struct.pack('<H', 0))
    for r, (mod, pos, bomba, pint) in enumerate(lista): escribir_guardado(j, 0, r, mod, pos, bomba, pint)
    j.tp(*PUERTA); t0 = time.time(); r = {}
    while time.time() - t0 < tmax and j.alive():
        j.tick()
        for mod, pos, b, p in lista:
            if mod not in r:
                for v in vehiculo_por_modelo(j, mod):
                    if v['pos'] and abs(v['pos'][0] - pos[0]) < 3 and abs(v['pos'][1] - pos[1]) < 3:
                        r[mod] = v['p']
                        if sin_gravedad: j.m.wr(v['p'] + 0x40, struct.pack('<I', j.m.u32(v['p'] + 0x40) & ~2)); j.m.wr(v['p'] + 0x44, struct.pack('<3f', 0, 0, 0))
        if len(r) == len(lista): break
        time.sleep(0.02 if sin_gravedad else 0.25)
    for mod, pos, b, p_ in lista:
        if mod not in r: j.P('  NO encontrado', mod, 'cerca de', pos, ':', [(hex(v['p']), v['modelo'], v['pos']) for v in vehiculos_det(j) if v['pos'] and abs(v['pos'][0] - pos[0]) < 40 and abs(v['pos'][1] - pos[1]) < 40], 'ranuras', [coche_guardado(j, 0, q)['modelo'] for q in range(4)])
    return r
def esc_carstokeep(j):
    """L54: un coche en apCarsToKeep que se destruye debe salir de la lista (el EXE dejaba el puntero colgado)"""
    SJ.suelo_libre(j)
    r = restaurar_en(j, [(411, (2504.0, -1694.5, 13.2), 0, 0)])
    v = r.get(411); j.P('Infernus restaurado en el garaje:', hex(v or 0))
    if not v: return
    fijar_keep(j, 0, v); j.P('apCarsToKeep antes', [hex(j.m.u32(KEEP + 4 * i)) for i in range(2)])
    j.dormir(1.0); j.tp(*LEJOS)
    ok = esperar_garaje(j, 0, 90)
    j.P('puerta cerrada (el garaje guarda y destruye el coche):', ok, 'ranuras', [coche_guardado(j, 0, r_)['modelo'] for r_ in range(4)])
    j.dormir(1.0); j.P('apCarsToKeep despues', [hex(j.m.u32(KEEP + 4 * i)) for i in range(2)], 'coche destruido era', hex(v), 'sigue en el pool:', v in [x['p'] for x in vehiculos_det(j)])
def esc_imp_zona(j):
    """L185: un coche lejano dentro de un garaje de impound (no escondite) no se elimina; fuera de cualquier garaje si"""
    SJ.suelo_libre(j)
    IMP = (1540.4, -1643.2, 5.6); CALLE = (1800.0, -1300.0, 13.5)
    r = restaurar_en(j, [(411, IMP, 0, 0), (410, CALLE, 0, 0)], tmax=40, sin_gravedad=True)
    j.P('restaurados', {k: hex(v) for k, v in r.items()}, 'garaje imp_la', [g for g in garajes(j) if g['nombre'] == 'imp_la'])
    for v in r.values(): j.m.wr(v + 0x40, struct.pack('<I', j.m.u32(v + 0x40) & ~2)); j.m.wr(v + 0x44, struct.pack('<3f', 0, 0, 0))   # sin gravedad: no cae al vacio (la colision de esa zona no esta cargada)
    j.tp(*LEJOS); t0 = time.time(); vivos = {}
    while time.time() - t0 < 60 and j.alive():
        j.tick(); ps = [x['p'] for x in vehiculos_det(j)]
        for mod, v in r.items():
            vivo = v in ps and j.modelo(v) == mod
            if vivos.get(mod, True) and not vivo: j.P('  coche', mod, 'ELIMINADO a los %.1f s' % (time.time() - t0)); vivos[mod] = False
        time.sleep(0.2)
    j.P('tras 60 s: vivos', {m: vivos.get(m, True) for m in r})
def safehouses(j):
    r = {}
    for c in range(20):
        for s_ in range(4):
            m = coche_guardado(j, c, s_)['modelo']
            if m: r[(c, s_)] = m
    return r
def esc_busted(j):
    """L186 (L55): al ser arrestado el impound guarda el coche (solo coches y motos). Dos vehiculos en apCarsToKeep: un Infernus y un Maverick."""
    SJ.suelo_libre(j)
    A = (2495.0, -1672.0, 13.6); B = (2520.0, -1672.0, 15.0)
    r = restaurar_en(j, [(411, A, 0, 0), (497, B, 0, 0)], tmax=60)
    j.P('restaurados', {k: hex(v) for k, v in r.items()})
    if len(r) < 2: j.P('no se restauraron los dos'); 
    j.tp(2505.0, -1668.0, 13.6); j.dormir(3.0)
    j.P('puerta CJ', estado_garaje(j), 'ranuras casa 0', [coche_guardado(j, 0, r_)['modelo'] for r_ in range(4)])
    for i, m in enumerate((411, 497)):
        if m in r: fijar_keep(j, i, r[m])
    antes = safehouses(j); j.P('apCarsToKeep', [hex(j.m.u32(KEEP + 4 * i)) for i in range(2)], 'guardados antes', antes)
    j.P('vehiculos en el pool', [(hex(v['p']), v['modelo']) for v in vehiculos_det(j)][:12])
    p, _ = j.ped(); j.P('estado del ped', j.m.u32(p + 0x530))
    j.m.wr(p + 0x530, struct.pack('<I', 63))   # PEDSTATE_ARRESTED
    t0 = time.time(); pos0 = j.pos()
    while time.time() - t0 < 40 and j.alive():
        j.tick(); time.sleep(0.5)
        pos = j.pos()
        if pos and pos0 and pos[0] == pos[0] and math.dist(pos, pos0) > 300 and time.time() - t0 > 6: break
    j.dormir(3.0); j.P('CJ ahora en', j.pos(), 'tras %.0f s' % (time.time() - t0))
    desp = safehouses(j); j.P('guardados despues', desp)
    nuevos = {k: v for k, v in desp.items() if antes.get(k) != v}; j.P('NUEVOS en ranuras:', nuevos)
    j.P('apCarsToKeep tras arresto', [hex(j.m.u32(KEEP + 4 * i)) for i in range(2)], 'pool', [(hex(v['p']), v['modelo']) for v in vehiculos_det(j)][:12])

PNS = 8   # 'sprLAe' (tipo 5, Pay 'n Spray de Idlewood)
def esc_pns(j):
    """L203: Pay 'n Spray ya no limpia el coche ANTES de cerrar la puerta (estado 2): se muestrea suciedad y posicion de la puerta"""
    SJ.suelo_libre(j)
    r = restaurar_en(j, [(411, (2504.0, -1694.5, 13.2), 0, 0)])
    v = r.get(411); j.P('Infernus restaurado:', hex(v or 0))
    if not v: return
    j.m.wr(0xB7CE50, struct.pack('<i', 5000)); j.dormir(1.0)
    for n in range(6):
        SJ.junto_puerta(j, v); j.dormir(1.0); j.al_frente(); SJ.pulsar('f', 0.15)
        t = time.time()
        while time.time() - t < 6 and not j.en_coche() and j.alive(): j.tick(); time.sleep(0.2)
        j.P('  intento %d: en_coche=%s' % (n + 1, j.en_coche()))
        if j.en_coche(): break
    if not j.en_coche(): return
    v = j.veh(); j.P('CJ en el coche', hex(v), 'dinero', j.m.i32(0xB7CE50))
    g = [x for x in garajes(j) if x['i'] == PNS][0]; cx = (g['izq'] + g['der']) / 2; cy = (g['frente'] + g['atras']) / 2
    mt = j.m.u32(v + 0x14); j.P('garaje PnS', json.dumps(g), 'estado', estado_garaje(j, PNS))
    j.m.wr(mt + 0x30, struct.pack('<3f', cx, cy, g['pos'][2] + 1.0)); j.m.wr(v + 0x44, struct.pack('<3f', 0, 0, 0))
    # orientacion paralela a la puerta: matriz identidad girada 0 (el garaje de ejemplo acepta cualquier angulo si las esferas caben)
    t0 = time.time(); last = None; escribir = True; muestras = []
    while time.time() - t0 < 25 and j.alive():
        st, pp = estado_garaje(j, PNS); d = j.m.f32(v + 0x4B0)
        if escribir and st == 1: j.m.wr(v + 0x4B0, struct.pack('<f', 14.0)); j.m.wr(v + 0x44, struct.pack('<3f', 0, 0, 0)); d = 14.0
        if st == 2: escribir = False
        k = (st, round(pp, 2), round(d, 1))
        if k != last: muestras.append((round(time.time() - t0, 2),) + k); last = k
        if st in (3, 4) and not escribir and time.time() - t0 > 3: break
        time.sleep(0.01)
    j.P('muestras (t, estado puerta, posicion puerta, suciedad):'); [j.P('   ', m) for m in muestras[:80]]
    j.P('dinero final', j.m.i32(0xB7CE50))
    j.foto('pns', 'Pay n Spray')

def esc_reposo120(j):
    """prueba de estabilidad: CJ en la calle ~100 s con el juego en marcha (sin garajes); sirve para contar excepciones del proceso"""
    SJ.suelo_libre(j); t0 = time.time()
    while time.time() - t0 < 100 and j.alive():
        j.tick(); time.sleep(0.5)
    j.P('reposo: vivo =', j.alive(), 'pos', j.pos())

ESC = {'reposo120': esc_reposo120, 'pns': esc_pns, 'carstokeep': esc_carstokeep, 'imp_zona': esc_imp_zona, 'busted': esc_busted, 'inspeccion': esc_inspeccion, 'trenes': esc_trenes, 'bombas': esc_bombas, 'pintura': esc_pintura, 'todo': esc_todo}
def main():
    sal = sys.argv[1]; esc = sys.argv[2].split(','); dirj = arg('--dir')
    j = J(sal, dirj)
    if not j.inicio(salir='--sin-salir' not in sys.argv): return
    import re
    for e in esc:
        j.P('=== ESCENARIO', e)
        try: ESC[e](j)
        except Exception as ex:
            import traceback; j.P('EXCEPCION en el escenario', e, repr(ex), traceback.format_exc()[-800:])
        if not j.alive(): j.P('EL PROCESO TERMINO durante', e); break
    j.P('vivo al final:', j.alive())
    for l in j.log_nuevas(arg('--log')):
        if re.search(r'error|NaN|crash|excep|fall', l, re.I): j.P('LOG', l[:240])
    json.dump(j.res, open(os.path.join(sal, 'resumen.json'), 'w'), indent=1, default=str)
    j.P('fin')
if __name__ == '__main__': main()
