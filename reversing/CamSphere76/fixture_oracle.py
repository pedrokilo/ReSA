"""Oraculo independiente (Python) de CamSphere76: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado de CCamera::IsSphereVisible(const CVector&, float) (EXE 0x420D40..0x420D8C),
lo que debe observarse (traza del colaborador 0x420C40 con sus argumentos EN ORDEN, AL de salida y bytes cambiados en las arenas) y lo compara con las filas de comparisons.jsonl
(lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado: esi = ECX (camara); llama a 0x420C40(origen, radio, esi + 0xA4C) con ECX = camara; si AL != 0 -> AL = 1 y sale; si no, lee el byte [esi + 0x970] (DESPUES de la
primera llamada); si es 0 -> AL = 0; si no, llama a 0x420C40(origen, radio, esi + 0xA94) y devuelve el AL de esa segunda llamada. Los bytes altos de EAX no se comparan.
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 camara, 1 origen.
Si el origen apunta dentro de la camara (origin_kind 1..4) las tres palabras que lee el falso son memoria de la camara (no conocida por el oraculo): comodin.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

GUARD = 0x40
ANY = None
CAM, ORG = 0, 1
ORIGIN_OFFSETS = {1: 0x0, 2: 0x970, 3: 0xA4C, 4: 0xA94}


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + GUARD + off


def model(inp):
    kind = inp['origin_kind']
    org_tag = tag(ORG) if kind == 0 else tag(CAM, ORIGIN_OFFSETS[kind])
    words = list(inp['origin']) if kind == 0 else [ANY, ANY, ANY]
    ev = [[1, tag(CAM), org_tag] + words + [inp['radius'], tag(CAM, 0xA4C)]]
    mirror = (inp['mut_val'] if inp['mutate'] else inp['mirror']) & 0xFF
    if inp['res1'] & 0xFF:
        al = 1
    elif mirror == 0:
        al = 0
    else:
        ev.append([1, tag(CAM), org_tag] + words + [inp['radius'], tag(CAM, 0xA94)])
        al = inp['res2'] & 0xFF
    cam_changes = []
    if inp['mutate'] and (inp['mut_val'] & 0xFF) != (inp['mirror'] & 0xFF):
        cam_changes.append([0x970, inp['mut_val'] & 0xFF])
    return ev, al, cam_changes


def match(a, b):
    return len(a) == len(b) and all(x is ANY or x == y for x, y in zip(a, b))


def check(inp, side_obs):
    ev, al, cam_changes = model(inp)
    obs = side_obs['observed']
    got = obs['events']
    if len(ev) != len(got) or not all(match(x, y) for x, y in zip(ev, got)):
        return 'eventos: esperado %s, observado %s' % (ev, got)
    if obs['al'] != al:
        return 'AL: esperado %d, observado %d' % (al, obs['al'])
    if obs['changes'][0] != cam_changes or obs['changes'][1] != []:
        return 'cambios: esperado %s, observado %s' % ([cam_changes, []], obs['changes'])
    return None


def run(path):
    rows = [json.loads(line) for line in open(path, encoding='utf-8')]
    bad = 0
    n = 0
    for r in rows:
        if r['event'] != 'comparison':
            continue
        n += 1
        for sname in ('original', 'candidate'):
            msg = check(r['input'], r[sname])
            if msg:
                bad += 1
                if bad <= 10:
                    print('caso %d (familia %d) lado %s: %s' % (r['case'], r['input']['family'], sname, msg))
    print('casos %d, lados distintos del modelo: %d' % (n, bad))
    return bad


if __name__ == '__main__':
    sys.exit(1 if run(sys.argv[1]) else 0)
