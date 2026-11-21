"""Cierre de la parte 2 del plan de codigo vivo: puentes del C++ que llaman por direccion a funciones ya reemplazadas.

Parte de la lista original (referencias-cpp-por-direccion.csv, generada por codigo_vivo_auditoria.py antes de retirar nada), vuelve
a escanear `source/` con el mismo criterio (literales hexadecimales en 0x401000-0x857FFF fuera de comentarios, separando los
enganches RH_Scoped*) y marca, por direccion y por lote B1..B18 y B-R1..B-R5, cuantas referencias que NO son enganche quedan.

  retirada   = ya no queda ninguna referencia fuera de los enganches (el C++ llama a la funcion C++ directamente)
  parcial    = quedan menos referencias que al inicio
  pendiente  = igual que al inicio; la columna `motivo` agrupa por que no es una sustitucion mecanica

Salidas: docs/CodigoVivoExe/cierre-p2.csv (una fila por direccion) y filas B* / B-R* en cierre.csv (lo lee codigo_vivo_plan.py).
Uso: python -X utf8 contrib/codigo_vivo_cierre_p2.py [--aplicar]
"""
import argparse
import csv
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
import codigo_vivo as cv  # noqa: E402

OUT = ROOT / 'docs' / 'CodigoVivoExe'


def leer(p):
    with open(p, encoding='utf-8-sig', newline='') as s:
        return list(csv.DictReader(s))


def escribir(p, filas, campos):
    with open(p, 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos)
        w.writeheader()
        w.writerows(filas)


def trozos(lista, n):
    return [lista[i:i + n] for i in range(0, len(lista), n)]


def texto_linea(ref):
    try:
        return open(ROOT / ref[0], encoding='utf-8', errors='replace').read().splitlines()[ref[1] - 1].strip()
    except OSError:
        return ''


def motivo(va, refs, es_entrada=True):
    """agrupa las referencias que quedan por la razon de que no sean sustituibles de forma mecanica."""
    import re
    archivos = {r[0] for r in refs}
    if not es_entrada:
        t = texto_linea(refs[0])
        if 'ExeProtectionStub' in refs[0][0]:
            return 'protector del EXE (V1: se retira con ExeProtectionStub.cpp)'
        if re.search(r'//.*0[xX]%06X' % va, t, re.I):
            return 'comentario con la direccion que el escaner no reconoce (separador de digitos o literal de caracter en la linea): no es una referencia'
        if any('JPegCompressPlugin' in a for a in archivos):
            return 'libjpeg: direccion interior de una funcion o puntero guardado en estructuras (se retira con el EXE)'
        if any(a.startswith('source/game_sa/RenderWare/') for a in archivos):
            return 'RenderWare: direccion interior de funcion o dato del .text (callbacks, tablas); se revisa con S2/P6'
        if re.search(r'[&|^~]\s*0[xX]|0[xX][0-9A-Fa-f]+\s*[&|^]|<<|>>|color|0[xX][0-9A-Fa-f]{6}ff|== ?0[xX]|!= ?0[xX]|\+=|-=', t):
            return 'constante numerica (mascara, color o valor) que coincide con una direccion del .text: no es una referencia'
        return 'direccion interior o dato del .text: revisar caso a caso (P6)'
    if va >= 0x820000:
        return 'CRT y arranque (ramas crt4a-locale / new-delete-d3dx9; lote E2 o F)'
    if any('JPegCompressPlugin' in a for a in archivos):
        return 'libjpeg: puntero a funcion guardado en estructuras que lee codigo del EXE (se retira con el EXE)'
    if any(a.startswith('source/game_sa/RenderWare/') for a in archivos):
        return 'RenderWare/D3D9: puntero guardado o funcion de uso interno sin API C++ publica (S2 o retirada con el EXE)'
    if any('ExeProtectionStub' in a or 'ProtectionStub' in a for a in archivos):
        return 'protector del EXE (V1: se retira con ExeProtectionStub.cpp)'
    if any('PedAttractor' in a or 'AEUserRadioTrackManager' in a for a in archivos):
        return 'vector de la STL del EXE llamado por direccion para que los bancos lo vieran (retirable ahora que no hay bancos, pero es reescribir plantillas)'
    if any('/Audio/' in a for a in archivos):
        return 'audio: se pasan dwords exactos (canal sin signo, bits de float) por la ABI del EXE; la funcion C++ recorta a int16'
    if any('/Tasks/' in a or 'Population' in a or 'GangWars' in a or 'Train' in a for a in archivos):
        return 'constructor, destructor o cuerpo de tarea con ABI propia (thiscall/fastcall con EDX, tamano de reserva) o clase sin constructor C++ completo'
    return 'ABI propia del EXE (registros residuales, bits de float) o puntero de funcion cuyo valor compara el EXE: revisar caso a caso'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--aplicar', action='store_true')
    args = ap.parse_args()
    base = leer(OUT / 'referencias-cpp-por-direccion.csv')
    funcs = cv.cargar_funciones()
    hooks, refs, _ = cv.escanear_fuente(set(funcs), None)
    # las direcciones que no son entrada de funcion tambien se buscan como literales
    from datos_exe import quitar_comentarios  # noqa: F401
    por_va_extra = defaultdict(list)
    wanted = {int(x['va'], 16) for x in base}
    sin_func = wanted - set(refs)
    # direcciones fuera del mapa de funciones: escaneo directo
    if sin_func:
        import os
        for dp, _, fs in os.walk(ROOT / 'source'):
            for f in fs:
                if not f.endswith(('.h', '.cpp', '.hpp', '.inc', '.c')):
                    continue
                p = os.path.join(dp, f)
                rel = os.path.relpath(p, ROOT).replace('\\', '/')
                txt = quitar_comentarios(open(p, encoding='utf-8', errors='replace').read())
                for ln, linea in enumerate(txt.split('\n'), 1):
                    for m in cv.HEX6.finditer(linea):
                        v = int(m.group(1), 16)
                        if v in sin_func:
                            forma = 'enganche' if re.search(r'RH_Scoped\w*\s*\(', linea) else 'literal'
                            por_va_extra[v].append((rel, ln, forma))
    filas = []
    for x in base:
        va = int(x['va'], 16)
        todas = [r for r in (refs.get(va) or por_va_extra.get(va, [])) if r[2] != 'enganche']
        # tabla de instalacion de enganches (Cheat.cpp: { direccion, CCheat::Metodo, ... }): registra el enganche, no llama al EXE
        actuales = [r for r in todas if not re.match(r'\{\s*0[xX][0-9A-Fa-f]{6},\s*CCheat::', texto_linea(r))]
        solo_tabla = bool(todas) and not actuales
        antes = int(x['referencias'])
        if solo_tabla:
            est = 'no es puente'
        elif not actuales:
            est = 'retirada'
        elif len(actuales) < antes:
            est = 'parcial'
        else:
            est = 'pendiente'
        es_fn = x['tipo'].startswith('entrada')
        filas.append(dict(va=x['va'], tipo=x['tipo'], estado_funcion=x['estado'], referencias_antes=antes,
                          referencias_ahora=len(actuales), cierre=est,
                          motivo='' if est == 'retirada' else motivo(va, actuales, es_fn),
                          primera_ahora=(f'{actuales[0][0]}:{actuales[0][1]}' if actuales else ''),
                          lote=''))
    # lotes (misma asignacion que codigo_vivo_plan.py)
    pf = sorted([f for f in filas if f['tipo'] == 'entrada de función' and f['estado_funcion'] in ('reemplazada', 'enganchada')], key=lambda f: f['va'])
    pn = sorted([f for f in filas if f['tipo'] != 'entrada de función'], key=lambda f: f['va'])
    lotes = {}
    for k, c in enumerate(trozos(pf, 25), 1):
        for f in c:
            f['lote'] = f'B{k}'
        lotes[f'B{k}'] = c
    for k, c in enumerate(trozos(pn, 40), 1):
        for f in c:
            f['lote'] = f'B-R{k}'
        lotes[f'B-R{k}'] = c
    print('direcciones:', len(filas), dict(Counter(f['cierre'] for f in filas)))
    puentes = [f for f in filas if f['lote'].startswith('B') and not f['lote'].startswith('B-R')]
    print('puentes (B1-B18):', len(puentes), dict(Counter(f['cierre'] for f in puentes)),
          'referencias antes/ahora:', sum(f['referencias_antes'] for f in puentes), sum(f['referencias_ahora'] for f in puentes))
    print('motivos de lo pendiente:', Counter(f['motivo'] for f in puentes if f['cierre'] != 'retirada').most_common())
    for l, c in lotes.items():
        cc = Counter(f['cierre'] for f in c)
        print(' ', l, len(c), dict(cc))
    if not args.aplicar:
        return
    escribir(OUT / 'cierre-p2.csv', sorted(filas, key=lambda f: f['va']), list(filas[0]))
    previo = {x['lote']: x for x in leer(OUT / 'cierre.csv')} if (OUT / 'cierre.csv').exists() else {}
    for l, c in lotes.items():
        cc = Counter(f['cierre'] for f in c)
        n = len(c)
        if l.startswith('B-R'):
            est = 'clasificado' if cc.get('pendiente', 0) + cc.get('parcial', 0) else 'cerrado'
            nota = (f'{cc.get("retirada", 0)} de {n} direcciones ya sin referencia; el resto se clasifica por motivo en cierre-p2.csv '
                    '(constantes numericas, comentarios que el escaner no reconoce y punteros a funcion guardados); no son puentes de llamada')
        else:
            est = 'cerrado' if cc.get('retirada', 0) == n else ('parcial' if cc.get('retirada', 0) else 'pendiente')
            nota = (f'{cc.get("retirada", 0)} de {n} direcciones sin llamada por direccion (sustituidas por la funcion C++); '
                    f'{cc.get("parcial", 0)} parciales, {cc.get("pendiente", 0)} pendientes con motivo en cierre-p2.csv')
        previo[l] = dict(lote=l, estado=est, nota=nota)
    escribir(OUT / 'cierre.csv', list(previo.values()), ['lote', 'estado', 'nota'])


if __name__ == '__main__':
    main()
