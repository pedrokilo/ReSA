# BIS1: simulador del oraculo para probar la logica de ddmin.py sin jugar.
# Modelo: el fallo aparece si queda ENCENDIDO algun culpable (OR). Con probabilidad `ruido` una ronda que deberia dar ERROR
# da OK por falso negativo (el jugador no robo el coche "bueno" a tiempo, aunque confirme los 3 robos).
# Uso: python simular.py [N] [pruebas] [ruido]
import random, sys, statistics
import ddmin

def una(N, k, ruido, rng, max_rondas=200):
    C = set(rng.sample(range(N), k)); resp = []; rondas = 0
    def oraculo(off):
        hay_culpable_on = bool(C - set(off))
        if hay_culpable_on and rng.random() >= ruido: return 'E'
        if hay_culpable_on: return 'O'   # falso negativo
        return 'O'
    # linea base
    rondas += 1; resp.append(([], oraculo([])))
    if resp[0][1] != 'E': return None
    while rondas < max_rondas:
        acc, S, info = ddmin.planificar(list(range(N)), resp)
        if acc == 'fin': return (set(S) == C, rondas, info)
        rondas += 1; resp.append((S, oraculo(S)))
    return (False, rondas, 'sin fin')

if __name__ == '__main__':
    N = int(sys.argv[1]) if len(sys.argv) > 1 else 994
    pruebas = int(sys.argv[2]) if len(sys.argv) > 2 else 200
    ruido = float(sys.argv[3]) if len(sys.argv) > 3 else 0.10
    rng = random.Random(12345)
    print(f'N={N} pruebas={pruebas} ruido={ruido} (rondas incluyen la linea base y las confirmaciones)')
    for k in (1, 2, 3):
        for rr in (0.0, ruido):
            res = [una(N, k, rr, rng) for _ in range(pruebas)]
            res = [r for r in res if r]
            ok = sum(1 for r in res if r[0]); rd = [r[1] for r in res]
            print(f'culpables={k} ruido={rr:.2f}: correctos {ok}/{len(res)}  rondas media={statistics.mean(rd):.1f} mediana={statistics.median(rd)} p90={sorted(rd)[int(len(rd)*.9)]} max={max(rd)}')


def flujo_completo(k_rutas=1, ruido=0.0, semilla=1):
    """Recorre el circuito real de ddmin.py (estado, fase de rutas, bajada a funciones, resultado) con un oraculo oculto."""
    import tempfile, os
    rng = random.Random(semilla)
    p = os.path.join(tempfile.mkdtemp(), 'estado.json')
    st = ddmin.nuevo_estado(); ddmin.guardar(p, st)
    culp_rutas = set(rng.sample(st['U'], k_rutas)); fn_culp = None
    rondas = 0
    while True:
        st = ddmin.cargar(p); s = ddmin.siguiente(st)
        if s['accion'] == 'fin':
            assert set(s['culpables']) == (culp_rutas if st['fase'] == 'rutas' else {fn_culp}), (s, culp_rutas, fn_culp)
            if st['fase'] == 'rutas':
                hay = ddmin.avanzar(st); ddmin.guardar(p, st)
                fn_culp = rng.choice(st['U']); print(f"  fase rutas OK en {rondas} rondas; {len(st['U'])} funciones; culpable oculto: {fn_culp}")
                continue
            ddmin.avanzar(st); ddmin.guardar(p, st); break
        oculto = culp_rutas if st['fase'] == 'rutas' else {fn_culp}
        off = set(s['apagar']); en = bool(oculto - off)
        res = 'E' if en and rng.random() >= ruido else 'O'
        ddmin.registrar(st, res, duracion='1'); ddmin.guardar(p, st); rondas += 1
    st = ddmin.cargar(p)
    print(f"  fin: {rondas} rondas; culpable funcion = {st['culpables_funciones']} info={st['info_funciones'][st['culpables_funciones'][0]]}")
    return rondas
