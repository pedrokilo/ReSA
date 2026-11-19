# PL1: biseccion automatica por clases. Senal = aviso "ha caido a traves del mapa" en el log de la ejecucion
# (el fallo del usuario); la DLL Debug completa lo da en ~35 s tras cargar la partida, el EXE solo (--unhook-all), nunca.
# Uso: python bisect_pl1.py <estado.json>   (reanuda; guarda cada resultado)
import csv, json, subprocess, sys, os, glob, time
here = os.path.dirname(os.path.abspath(__file__))
dump = os.path.join(here, 'clases_dump.csv')
rutas = json.load(open(os.path.join(here, 'rutas.json')))
classes = sorted(f'{c}/{k}' for c, v in rutas.items() for k in v if not any(x in k for x in ', '))

def correr(etq, modo, lista, auto=100):
    arg = {'except': '--unhook-except=', 'some': '--unhook='}[modo] + ','.join(lista)
    out = os.path.join(here, 'tmp', etq + '.out')
    af = os.path.join(here, 'tmp', etq + '.args'); open(af, 'w').write(arg)
    for f in (out, out + '.done'):
        if os.path.exists(f): os.remove(f)
    while True:
        if b'gta_sa.exe' in subprocess.run(['tasklist'], capture_output=True).stdout.lower(): time.sleep(20); continue
        p = subprocess.run(['pwsh', '-NoProfile', '-File', os.path.join(here, 'jugar.ps1'), '-Variante', 'A', '-Auto', str(auto), '-ParaAlCaer',
                            '-Etiqueta', '-' + etq, '-ArgsFile', af], capture_output=True, text=True)
        open(out, 'w', encoding='utf-8').write(p.stdout + p.stderr)
        if 'esta tomado por otra prueba' in p.stdout + p.stderr or 'ya esta en marcha' in p.stdout + p.stderr: time.sleep(20); continue
        break
    d = sorted(glob.glob(os.path.join(here, 'salidas', 'A-' + etq + '-*')))[-1]
    r = json.load(open(os.path.join(d, 'resumen.json'), encoding='utf-8-sig'))
    return dict(caidas=r['avisos_caida_a_traves_del_mapa'], pool=r['lineas_Pool_h_207'], nose=r.get('lineas_no_se_pudo'),
                cambiados=r.get('lineas_hook_cambiado'), dur=r['duracion_segundos'], salida=r['codigo_salida'], dir=d)

if __name__ == '__main__':
    estado_p = sys.argv[1]
    st = json.load(open(estado_p)) if os.path.exists(estado_p) else {'hist': [], 'S': classes}
    S = st['S']
    if not st['hist']:
        r = correr('bs00all', 'except', S)
        st['hist'].append(dict(etq='bs00all', n=len(S), res=r)); print('todo', len(S), r, flush=True)
        json.dump(st, open(estado_p, 'w'), indent=1)
        if r['caidas'] == 0: print('SENAL AUSENTE con todos los hooks reactivados'); sys.exit(2)
    while len(S) > 1:
        h = len(S) // 2
        A, B = S[:h], S[h:]
        res = None
        for nombre, mitad in (('a', A), ('b', B)):
            etq = f"bs{len(st['hist']):02d}{nombre}"
            r = correr(etq, 'except', mitad)
            st['hist'].append(dict(etq=etq, n=len(mitad), res=r))
            json.dump(st, open(estado_p, 'w'), indent=1)
            print(etq, len(mitad), r, flush=True)
            if r['caidas'] > 0:
                res = mitad; break
        if res is None:
            print('NINGUNA mitad falla sola: interaccion entre mitades'); break
        S = res; st['S'] = S
        json.dump(st, open(estado_p, 'w'), indent=1)
    print('FINAL', S)
