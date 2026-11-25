# -*- coding: utf-8 -*-
# VIGILANTE PARA EL USUARIO: dejalo abierto mientras juegas (standalone, EXE original + ASI o EXE original puro). No toca el juego: solo lee memoria y captura la ventana.
# Cuando detecta la anomalia de skinning (matrices de hueso no finitas/enormes/escala rara, o un fotograma "distinto de los dos vecinos") guarda los 3 s anteriores
# y los 3 s posteriores (PNG a tamano completo) y las matrices en la carpeta de evidencias.
# Uso:   python -I vigilante_skin.py [--carpeta C:\skin_evidencia] [--segundos 3] [--sin-imagen] [--pid N] [--log ruta\log.log] [--mapa ruta\gta_sa_standalone.map]
# Con el standalone y GTASA_SKIN_VIGILANCIA=1 en el entorno del juego (ver LEEME_skin.md) ademas se leen los contadores del propio juego.
# Ctrl+C para terminar: escribe vigilante_resumen.json (maximos vistos, lecturas, eventos).
import sys, os, time, subprocess, json
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import skinwatch as W
def arg(n, d=None): return sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
def pids():
    r = []
    for nombre in ('gta_sa_standalone', 'gta_sa'):
        o = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq %s.exe' % nombre, '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout
        for l in o.splitlines():
            if nombre + '.exe' in l.lower() or nombre in l: r.append((nombre, int(l.split('","')[1])))
    return r
def main():
    carpeta = arg('--carpeta', r'C:\skin_evidencia'); seg = float(arg('--segundos', '3'))
    evid = os.path.join(carpeta, time.strftime('vigilante_%Y%m%d_%H%M%S')); os.makedirs(evid, exist_ok=True)
    pid = int(arg('--pid')) if arg('--pid') else None; nombre = ''
    print('Esperando a gta_sa.exe / gta_sa_standalone.exe ...', flush=True)
    while not pid:
        p = pids()
        if p: nombre, pid = p[0]
        else: time.sleep(2)
    modo = 'std' if nombre == 'gta_sa_standalone' else 'exe'
    mapa = arg('--mapa') or os.path.join(aqui, '..', 'FnFase8', 'salida', 'gta_sa_standalone.map')
    logruta = arg('--log')
    log = open(os.path.join(evid, 'vigilante.log'), 'w', encoding='utf-8')
    def P(*a):
        t = time.strftime('%H:%M:%S ') + ' '.join(str(x) for x in a); print(t, flush=True); log.write(t + '\n'); log.flush()
    P('vigilando', nombre, 'pid', pid, 'evidencias en', evid)
    v = W.Vigilante(pid, modo, evid, ring_s=seg, post_s=seg, log=logruta, mapa=mapa if modo == 'std' else None, P=P, imagen='--sin-imagen' not in sys.argv)
    v.iniciar(); ult = time.time()
    try:
        while W.k.WaitForSingleObject(v.m.h, 1000) == 0x102:
            if time.time() - ult > 60: ult = time.time(); P('estado', json.dumps(v.stats, default=str))
    except KeyboardInterrupt: pass
    P('el juego termino o Ctrl+C; cerrando'); v.detener(); P('resumen', json.dumps(v.stats, default=str))
main()
