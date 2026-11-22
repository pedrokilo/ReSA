"""Plan por partes y lotes para quitar el codigo vivo del EXE (lee docs/CodigoVivoExe/*.csv; escribe plan.csv, plan-lotes.md y
anade la columna `lote` a funciones-vivas.csv).

Uso: python -X utf8 contrib/codigo_vivo_plan.py [--salida docs/CodigoVivoExe]
Reglas: los lotes de trabajo con codigo (SUSTITUIR/ENLAZAR/FASE8/puentes) tienen como maximo 25-45 funciones y comparten decision y grupo;
los descartes verificables (sin codigo) se parten por rango de direcciones solo para poder revisarlos.
"""
import argparse
import csv
from collections import Counter, defaultdict
from pathlib import Path

DOCS = Path(__file__).resolve().parent.parent / 'docs'


def leer(p):
    with open(p, encoding='utf-8-sig', newline='') as s:
        return list(csv.DictReader(s))


def trozos(lista, n):
    for i in range(0, len(lista), n):
        yield lista[i:i + n]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--salida', default=str(DOCS / 'CodigoVivoExe'))
    out = Path(ap.parse_args().salida)
    filas = leer(out / 'funciones-vivas.csv')
    puentes = leer(out / 'referencias-cpp-por-direccion.csv')
    huecos = leer(out / 'huecos.csv')
    lotes, asign = [], {}

    def add(lote, parte, tipo, grupo, decision, destino, items, riesgo, depende, nota, bytes_=None):
        vas = [x['va'] for x in items]
        if tipo not in ('puente a C++', 'revisar direcciones', 'verificar huecos', 'raices por valor'):
            for v in vas:
                asign[v] = lote
        b = bytes_ if bytes_ is not None else sum(int(x['bytes']) for x in items)
        lotes.append(dict(lote=lote, parte=parte, tipo=tipo, grupo=grupo, decision=decision, destino=destino, funciones=len(items),
                          bytes=b, va_min=min(vas) if vas else '', va_max=max(vas) if vas else '',
                          riesgo=riesgo, depende_de=depende, estado='pendiente', nota=nota))

    def sel(f):
        return sorted((x for x in filas if f(x)), key=lambda x: x['va'])

    # ------------------------------------------------ parte 1: descartes verificables (sin escribir codigo)
    g = sel(lambda x: x['grupo'] == 'Protección del EXE')
    add('V1', 'P1', 'verificar y retirar', 'Protección del EXE', 'DESCARTAR', 'protección', g, 'bajo', '',
        '18 bloques del protector; 6 tienen una referencia literal en ExeProtectionStub.cpp que hay que retirar con el archivo')
    g = sel(lambda x: x['grupo'].startswith('Juego excluido'))
    for k, c in enumerate(trozos(g, 45), 1):
        add(f'V2-{k}', 'P1', 'verificar y retirar', 'Juego excluido (cerrado por inspección)', 'DESCARTAR', 'cubierta/salto/fragmento', c, 'bajo', '',
            'confirmar con funciones-vivas.csv que no queda llamador vivo; 2 tienen referencia del C++ (0x4A2BB0, 0x85742B)')
    g = sel(lambda x: x['grupo'].startswith('Excepciones') and x['decision'] == 'DESCARTAR')
    for k, c in enumerate(trozos(g, 600), 1):
        add(f'V3-{k}', 'P1', 'verificar y retirar', 'Excepciones (Unwind/Catch)', 'DESCARTAR', 'huérfano de excepciones', c, 'bajo', '',
            'funclets de funciones ya reemplazadas; verificable con codigo_vivo_informe.py (dueño reemplazado)')
    g = sel(lambda x: x['grupo'] == 'D3DX9' and x['decision'] == 'DESCARTAR')
    for k, c in enumerate(trozos(g, 250), 1):
        add(f'V4-{k}', 'P1', 'verificar y retirar', 'D3DX9', 'DESCARTAR', 'biblioteca embebida sin raíces vivas', c, 'bajo', 'E3',
            'el cuerpo embebido sobra si D3DX9 se enlaza (d3dx9_43.dll); ningún C++ lo llama por dirección')
    for grp, pref in (('CRT y arranque', 'V5'), ('QuickTime/QTML', 'V6'), ('RenderWare no reconstruible', 'V7'), ('libpng y zlib', 'V8')):
        g = sel(lambda x, grp=grp: x['grupo'] == grp and x['decision'] == 'DESCARTAR')
        for k, c in enumerate(trozos(g, 70), 1):
            add(f'{pref}-{k}', 'P1', 'verificar y retirar', grp, 'DESCARTAR', 'sin alcance (posible muerto)', c, 'medio', '',
                'ni el C++, ni la entrada ni una tabla la alcanzan; antes de retirar, buscar llamadas indirectas por registro en el código que la rodea')

    # ------------------------------------------------ parte 2: puentes del C++ por direccion
    pf = sorted([x for x in puentes if x['tipo'] == 'entrada de función' and x['estado'] in ('reemplazada', 'enganchada')],
                key=lambda x: x['va'])
    for k, c in enumerate(trozos(pf, 25), 1):
        add(f'B{k}', 'P2', 'puente a C++', 'Reemplazada', 'REEMPLAZAR_LLAMADA', 'llamar a la función C++', [dict(va=x['va'], bytes=0) for x in c],
            'bajo', '', 'sustituir plugin::Call/puntero por la llamada C++ directa (la función ya existe)', bytes_=0)
    pn = sorted([x for x in puentes if x['tipo'] != 'entrada de función'], key=lambda x: x['va'])
    for k, c in enumerate(trozos(pn, 40), 1):
        add(f'B-R{k}', 'P2', 'revisar direcciones', 'No son entrada de función', 'REVISAR', 'revisar', [dict(va=x['va'], bytes=0) for x in c], 'bajo', '',
            'literales en 0x401000-0x857FFF que no son inicio de función (constantes numéricas, saltos interiores, datos del .text)', bytes_=0)

    # ------------------------------------------------ parte 3: sustituir por C++ propio
    g = sel(lambda x: x['decision'] == 'SUSTITUIR' and x['grupo'] == 'QuickTime/QTML')
    add('S1', 'P3', 'sustituir', 'QuickTime/QTML', 'SUSTITUIR', 'Media Foundation / sin QuickTime', g, 'medio', '',
        'los 3 puentes que aún cita QTMLRuntime.cpp; el resto del puente (31) ya no tiene referencia (lote EXT1A)')
    g = sel(lambda x: x['decision'] == 'SUSTITUIR' and x['grupo'] == 'RenderWare no reconstruible' and x['region'] != 'rwcseg')
    add('S2', 'P3', 'sustituir', 'RenderWare no reconstruible', 'SUSTITUIR', 'C++ propio (RenderWare D3D9)', g, 'alto', '',
        'usercall del controlador D3D9 (0x4CB730-0x4CCE60): escribir la versión C++ con los registros como parámetros o reescribir al llamador')
    g = sel(lambda x: x['decision'] == 'SUSTITUIR' and x['region'] == 'rwcseg')
    add('S3', 'P3', 'sustituir', 'RenderWare no reconstruible', 'SUSTITUIR', 'C++ propio (vectores)', g, 'medio', '',
        'callbacks x87 de vectores de rwcore.cpp:7490: usar los SSE2 ya reconstruidos o una versión C++ con x87 a 24 bits')
    g = sel(lambda x: x['decision'] == 'SUSTITUIR' and x['grupo'] == 'CRT y arranque')
    add('S4', 'P3', 'sustituir', 'CRT y arranque', 'SUSTITUIR', 'C++ propio (ExeCrt)', g, 'alto', '',
        'conversiones numéricas del CRT con redondeo idéntico (CrtMath.cpp cita 0x82872C)')

    # ------------------------------------------------ parte 4: enlazar bibliotecas
    g = sel(lambda x: x['decision'] == 'ENLAZAR' and x['grupo'] == 'libpng y zlib')
    for k, c in enumerate(trozos(g, 25), 1):
        add(f'E1-{k}', 'P4', 'enlazar', 'libpng y zlib', 'ENLAZAR', 'libpng 1.0.12 + zlib 1.1.3 (libs/)', c, 'medio', '',
            'los hooks LIB2A ya enlazan la lectura; completar con las funciones citadas por C++ y cerrar el banco')
    roles = ['excepciones', 'montón', 'locale', 'E/S y stdio', 'cadenas y memoria', 'estado por hilo y bloqueos', 'numérico', 'sin rol']
    g_all = sel(lambda x: x['decision'] == 'ENLAZAR' and x['grupo'] == 'CRT y arranque')
    por_rol = defaultdict(list)
    for x in g_all:
        r = x['subgrupo'].replace(' (por vecinos)', '')
        por_rol[r if r in roles else 'sin rol'].append(x)
    for r in roles:
        for k, c in enumerate(trozos(por_rol.get(r, []), 40), 1):
            add(f'E2-{r.split()[0]}-{k}', 'P4', 'enlazar', 'CRT y arranque', 'ENLAZAR', 'CRT de MSVC', c, 'alto' if r == 'locale' else 'medio',
                'S4' if r == 'numérico' else '', f'rol {r}: lo aporta el CRT enlazado; mantener los ExeCrt solo donde el redondeo lo exija')
    g = [x for x in filas if x['grupo'] == 'D3DX9' and x['estado'] == 'enganchada']
    add('E3', 'P4', 'enlazar', 'D3DX9', 'ENLAZAR', 'D3DX9 real (d3dx9_43.dll)', g or [dict(va='0x007652AE', bytes=0)], 'bajo', '',
        'sin funciones vivas: activar los 4 enganches desactivados de d3dx9math.cpp (0x7652AE..0x7654B8) y enlazar la biblioteca', bytes_=0)

    # ------------------------------------------------ parte 5: fase 8 (arranque y enlace)
    g = sel(lambda x: x['decision'] == 'FASE8' and x['grupo'] == 'CRT y arranque' and not x['subgrupo'].startswith('inicializador'))
    add('F1', 'P5', 'fase 8', 'CRT y arranque', 'FASE8', 'arranque del CRT enlazado', g, 'alto', 'E2-excepciones-1',
        'entry 0x824570, _cinit, _initterm, atexit...: lo genera el enlace; WinMain pasa a ser el punto de entrada propio')
    g = sel(lambda x: x['decision'] == 'FASE8' and x['subgrupo'].startswith('inicializador'))
    add('F2', 'P5', 'fase 8', 'CRT y arranque', 'FASE8', 'inicializadores estáticos', g, 'alto', 'F1',
        '4 inicializadores de Ghidra más 1.669 en 4 huecos (huecos.csv): inicializar los globales del C++ propio (DatosExe) y no ejecutar _initterm del EXE')
    g = sel(lambda x: x['decision'] == 'FASE8' and x['grupo'].startswith('Importaciones'))
    add('F3', 'P5', 'fase 8', 'Importaciones y carga retardada', 'FASE8', 'enlace (delay-load)', g, 'bajo', '',
        'thunks de d3d9/dinput8/dsound/ddraw y KERNEL32::RtlUnwind: los genera el enlazador')
    g = sel(lambda x: x['decision'] == 'REVISAR')
    add('F4', 'P5', 'revisar', 'Excepciones (Unwind/Catch)', 'REVISAR', 'dueño no resuelto', g, 'bajo', '',
        'funclets de 0x838190+ sin llamador directo de su envoltura: casi seguro huérfanos; confirmar con el desensamblado')

    # ------------------------------------------------ parte 6: codigo sin funcion de Ghidra
    hc = [h for h in huecos if h['categoria'].startswith('código con referencia')]
    for k, c in enumerate(trozos(sorted(hc, key=lambda h: h['va']), 45), 1):
        add(f'H{k}', 'P6', 'verificar huecos', 'Huecos del .text', 'REVISAR', 'código sin función de Ghidra',
            [dict(va=h['va'], bytes=h['bytes']) for h in c], 'medio', '',
            'bytes ejecutables con llamador o referencia pero sin función en Ghidra; decidir si son funciones, envolturas o datos')

    # ------------------------------------------------ parte 7: raices por valor (viva sin hook; contrib/raices_valor_p6.py, reclasificacion-raices-valor.md)
    cola_csv = out / 'cola-raices-valor-p6.csv'
    if cola_csv.exists():
        cola = leer(cola_csv)
        temas = [('arranque', 'arranque del EXE propio: medidas en la fase 8 y su tabla de registro (rasters D3D9, plugins de RwCamera/RpWorld)', lambda x: x['ejecucion'] != 'sin medir'),
                 ('RenderWare', 'rutinas de RpWorld y RenderWare citadas como valor o por direccion y sin medir', lambda x: x['ejecucion'] == 'sin medir' and x['region'] == 'renderware'),
                 ('juego', 'rutinas del juego/CRT citadas como valor o por direccion y sin medir', lambda x: x['ejecucion'] == 'sin medir' and x['region'] != 'renderware')]
        k_ = 0
        for tema, desc, f_ in temas:
            for c in trozos([x for x in cola if f_(x)], 25):
                k_ += 1
                add(f'R{k_}', 'P7', 'raices por valor', f'Viva sin hook ({tema})', 'RECONSTRUIR', 'C++ propio con enganche', [dict(va=x['va'], bytes=int(x['bytes'])) for x in c],
                    'medio', '', desc + '; cola ordenada en cola-raices-valor-p6.csv')

    # ------------------------------------------------ estado de cierre (lo escriben codigo_vivo_cierre.py y los cierres de P2)
    cierre_csv = out / 'cierre.csv'
    if cierre_csv.exists():
        with open(cierre_csv, encoding='utf-8', newline='') as s:
            cierre = {r['lote']: r for r in csv.DictReader(s)}
        for l in lotes:
            c = cierre.get(l['lote'])
            if c:
                l['estado'] = c['estado']
                l['nota'] = (l['nota'] + ' | ' if l['nota'] else '') + c['nota']
    cierre_fn = {}
    cierre_p1 = out / 'cierre-p1.csv'
    if cierre_p1.exists():
        with open(cierre_p1, encoding='utf-8', newline='') as s:
            cierre_fn = {r['va']: r['cierre'] for r in csv.DictReader(s)}

    # ------------------------------------------------ salida
    with open(out / 'plan.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(lotes[0]))
        w.writeheader()
        w.writerows(lotes)
    campos_f = list(filas[0]) + (['lote'] if 'lote' not in filas[0] else []) + (['cierre'] if 'cierre' not in filas[0] else [])
    for x in filas:
        x['lote'] = asign.get(x['va'], '')
        x['cierre'] = cierre_fn.get(x['va'], '')
    with open(out / 'funciones-vivas.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos_f)
        w.writeheader()
        w.writerows(filas)
    partes = {'P1': 'Descartes verificables (sin escribir código)', 'P2': 'Retirar los puentes del C++ por dirección',
              'P3': 'Sustituir por C++ propio', 'P4': 'Enlazar bibliotecas', 'P5': 'Fase 8: arranque y enlace',
              'P6': 'Huecos del .text con código sin función', 'P7': 'Raíces por valor: viva sin hook (cola de reconstrucción)'}
    md = ['# Plan de retirada del código vivo del EXE por partes y lotes', '',
          '[Auditoría y decisiones](README.md) · [Lotes (CSV)](plan.csv) · [Funciones vivas (CSV)](funciones-vivas.csv)', '',
          'Generado por `contrib/codigo_vivo_plan.py`. Los lotes de código tienen 25–45 funciones; los descartes verificables se parten por rango solo para revisarlos. '
          'En los lotes de puentes (B), de huecos (H) y de raíces por valor (R) «funciones» cuenta direcciones/objetos.', '',
          '| Parte | Descripción | Lotes | Funciones | Bytes |', '| --- | --- | ---: | ---: | ---: |']
    for p, d in partes.items():
        ls = [l for l in lotes if l['parte'] == p]
        md.append(f"| {p} | {d} | {len(ls)} | {sum(l['funciones'] for l in ls):,} | {sum(l['bytes'] for l in ls):,} |")
    md += ['', '## Lotes', '',
           '| Lote | Parte | Tipo | Grupo | Decisión | Destino | Func. | Bytes | Rango | Riesgo | Depende de | Estado |',
           '| --- | --- | --- | --- | --- | --- | ---: | ---: | --- | --- | --- | --- |']
    for l in lotes:
        md.append(f"| {l['lote']} | {l['parte']} | {l['tipo']} | {l['grupo']} | {l['decision']} | {l['destino']} | {l['funciones']:,} | {l['bytes']:,} | "
                  f"{l['va_min']}–{l['va_max']} | {l['riesgo']} | {l['depende_de']} | {l['estado']} |")
    (out / 'plan-lotes.md').write_text('\n'.join(md) + '\n', encoding='utf-8')
    print(len(lotes), 'lotes;', sum(1 for x in filas if x['lote']), 'funciones con lote de', len(filas), dict(Counter(l['parte'] for l in lotes)))


if __name__ == '__main__':
    main()
