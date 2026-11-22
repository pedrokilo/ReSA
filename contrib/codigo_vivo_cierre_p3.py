"""Cierre de la parte 3 del plan de codigo vivo (lotes S1-S4): funciones del EXE sustituidas por C++ propio.

No recalcula el analisis (eso lo hace codigo_vivo.py y tardaria horas en cambiar toda la clasificacion); registra, por funcion y por lote, lo que
se hizo en `reversing/FnP3` y en `source/`, y lo vuelca en:
  docs/CodigoVivoExe/cierre-p3.csv      una fila por direccion (19: las 18 del plan que se sustituyen mas 0x85742B, que se sustituye con S3; 0x825456 es de otra rama)
  docs/CodigoVivoExe/cierre.csv         filas S1..S4 (lo lee codigo_vivo_plan.py)
  docs/CodigoVivoExe/plan.csv y plan-lotes.md   estado y nota de S1..S4 (mismo efecto que volver a ejecutar codigo_vivo_plan.py, sin tocar el resto)
  docs/CodigoVivoExe/funciones-vivas.csv        columna `cierre` de esas funciones
Uso: python -X utf8 contrib/codigo_vivo_cierre_p3.py
"""
import csv
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / 'docs' / 'CodigoVivoExe'

# va, bytes, lote, funcion, sustituto, enganche, verificacion, nota
FUNCIONES = [
    ('0x00818650', 26, 'S1', 'puente QuickTime InitializeQTML', 'QTMLRuntime.cpp: QtmlInitializeBridge', 'si', 'razonada (sin banco)',
     'nula segura: OSErr -2095 como 0x8194D0; nada del juego llega a ella (QuickTime retirado en EXT1A)'),
    ('0x00818670', 26, 'S1', 'puente QuickTime TerminateQTML', 'QTMLRuntime.cpp: QtmlTerminateBridge', 'si', 'razonada (sin banco)', 'nula segura: no hay nada que cerrar'),
    ('0x0081A3C0', 26, 'S1', 'puente QuickTime SoundConverterFillBuffer', 'QTMLRuntime.cpp: QtmlConverterFillBridge', 'si', 'razonada (sin banco)',
     'nula segura: OSErr -2095; AEMFDecoder.cpp la llama ya sin direccion'),
    ('0x004CB730', 100, 'S2', 'PushFreePaletteIndex (ESI = indice)', 'D3D9RasterDriver.cpp (ya existia)', 'no (usercall)', 'banco FnP3 (via RasterCreate)',
     'pierde sus llamadores del EXE (0x4CB956 ya es C++ y 0x4CBB6B/0x4CBBDD estan en RasterDestroy, C++); funcion local'),
    ('0x004CB7C0', 512, 'S2', 'creacion de textura (EDI = extension)', 'D3D9RasterDriver.cpp: RasterCreateTexture', 'no (usercall)', 'banco FnP3 (25.000+ casos por helper, 0 diferencias)',
     'unico llamador 0x4CCE60'),
    ('0x004CB9C0', 310, 'S2', 'creacion de textura de destino de render (ESI = extension)', 'D3D9RasterDriver.cpp: RasterCreateRenderTargetTexture', 'no (usercall)',
     'banco FnP3 (0 diferencias)', 'unico llamador 0x4CCE60'),
    ('0x004CC5C0', 907, 'S2', 'comprobacion de formato de raster (EAX = banderas, EDI = raster)', 'D3D9RasterDriver.cpp: RasterCheckFormat', 'no (usercall)', 'banco FnP3 (0 diferencias)',
     'unico llamador 0x4CCE60'),
    ('0x004CCD70', 236, 'S2', 'superficie de profundidad (ESI = extension, EDI = raster)', 'D3D9RasterDriver.cpp: RasterCreateDepthSurface', 'no (usercall)', 'banco FnP3 (0 diferencias)',
     'unico llamador 0x4CCE60'),
    ('0x004CCE60', 457, 'S2', 'rasterCreate de la tabla de dispositivo (cdecl)', 'D3D9RasterDriver.cpp: RasterCreate', 'si', 'banco FnP3 (400.000 casos, 0 diferencias)',
     'D3D9RasterFormat.cpp la llama ya sin direccion; la tabla de _rwD3D9DeviceSystemStandards sigue con la direccion 0x4CCE60 (salta al enganche)'),
    ('0x00857000', 127, 'S3', 'transformar puntos (x87)', 'RwMathX87.cpp: TransformPoints', 'si', 'banco FnP3 (400.000 casos, 0 diferencias)', 'solo se usa sin SSE'),
    ('0x0085707F', 112, 'S3', 'transformar vectores (x87)', 'RwMathX87.cpp: TransformVectors', 'si', 'banco FnP3 (0 diferencias)', 'solo se usa sin SSE'),
    ('0x008570EF', 113, 'S3', 'transformar un punto (x87)', 'RwMathX87.cpp: TransformPoint', 'si', 'banco FnP3 (0 diferencias)', 'solo se usa sin SSE'),
    ('0x00857160', 98, 'S3', 'transformar un vector (x87)', 'RwMathX87.cpp: TransformVector', 'si', 'banco FnP3 (0 diferencias)', 'solo se usa sin SSE'),
    ('0x008572A0', 355, 'S3', 'multiplicar matrices (x87)', 'RwMathX87.cpp: MatrixMultiply', 'si', 'banco FnP3 (400.000 casos, 0 diferencias)', 'se instala siempre (FINALIZESTART)'),
    ('0x00857404', 39, 'S3', 'palabra de control x87 a 24 bits', 'RwMathX87.cpp: SetFpuControlWord', 'si', 'banco FnP3 (8.312 casos, 0 diferencias)', 'guarda la anterior en 0xCAF004'),
    ('0x0085742B', 7, 'S3', 'restaura la palabra de control x87 (fldcw [0xCAF004])', 'RwMathX87.cpp: RestoreFpuControlWord', 'si', 'banco FnP3 (0 diferencias)',
     'no estaba en el plan S3 (lote V2, abierta por la cita de rwcore.cpp); se sustituye junto con 0x857404'),
    ('0x00821B40', 117, 'S4', '_ftol2 (argumento en ST0, resultado en EDX:EAX)', 'CrtMath.cpp: ExeCrt::FloatToInt64', 'no (ST0)', 'banco FnP3 (400.000 casos, 0 diferencias)',
     'sin llamadores vivos: los 633 del EXE estan en funciones reemplazadas; el C++ propio ya lo expresa en linea'),
    ('0x0082872C', 660, 'S4', 'manejador de errores matematicos (matherr + errno)', 'CrtMath.cpp: ExeCrt::MathError', 'si', 'banco FnP3 (400.001 casos, 0 diferencias)',
     'CrtMath.cpp y CrtPow.cpp la llaman ya sin direccion'),
    ('0x00833605', 3, 'S4', 'matherr por defecto (xor eax, eax; ret)', 'CrtMath.cpp: ExeCrt::DefaultMatherr', 'puntero 0x8E3128', 'banco FnP3',
     'pegada a la funcion siguiente (no admite salto): CrtMathInjectHooks sustituye el puntero 0x8E3128 si sigue valiendo 0x833605'),
]
LOTES = {
    'S1': ('sustituido', '3 puentes de QuickTime sustituidos por la implementacion nula segura (OSErr -2095, QuickTime no disponible); no se usa Media Foundation: nada llega a ellos '
                         '(QuickTime retirado en EXT1A) y los formatos que solo abria QuickTime siguen sin cargarse. Sin banco (no banqueable: el EXE necesita el despachador de QuickTime)'),
    'S2': ('sustituido y verificado con banco', '6 funciones: RasterCreate (0x4CCE60, con hook) y sus 4 ayudantes usercall en C++ puro con los registros como parametros; 0x4CB730 pierde sus llamadores. '
                                                'Banco FnP3 (reversing/FnP3/bench_d3d.cpp) con falsos de IDirect3D9/IDirect3DDevice9 y comparacion de la traza completa: 0 diferencias; prueba negativa 15/15'),
    'S3': ('sustituido y verificado con banco', '6 callbacks x87 (+ 0x85742B) sustituidos con aritmetica float escalar en el mismo orden (RwMathX87.cpp); banco FnP3 (bench_p3.cpp): 0 diferencias; prueba negativa 8/8'),
    'S4': ('parcial: 3 de 4 sustituidas y verificadas con banco', '3 de las 4 (0x821B40 como equivalente sin enganche, 0x82872C con hook, 0x833605 por puntero); la cuarta, ___updatetlocinfo (0x825456), pertenece a la rama del locale del CRT. '
                                                'Banco FnP3: 0 diferencias; prueba negativa 5/5 de las mutaciones con efecto (la sexta, floor en lugar de redondear al mas cercano, es equivalente: la correccion posterior la absorbe)'),
}


def main():
    with (OUT / 'cierre-p3.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'bytes', 'lote', 'funcion', 'sustituto', 'enganche', 'verificacion', 'nota'])
        for f in FUNCIONES:
            w.writerow(f)
    # cierre.csv
    with (OUT / 'cierre.csv').open(encoding='utf-8', newline='') as s:
        filas = [r for r in csv.DictReader(s) if r['lote'] not in LOTES]
    for l, (estado, nota) in LOTES.items():
        filas.append({'lote': l, 'estado': estado, 'nota': nota})
    with (OUT / 'cierre.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=['lote', 'estado', 'nota'])
        w.writeheader()
        w.writerows(filas)
    # plan.csv
    with (OUT / 'plan.csv').open(encoding='utf-8', newline='') as s:
        plan = list(csv.DictReader(s))
        campos = list(plan[0])
    for r in plan:
        if r['lote'] in LOTES:
            estado, nota = LOTES[r['lote']]
            base = r['nota'].split(' | ')[0]
            r['estado'] = estado
            r['nota'] = base + ' | ' + nota
    with (OUT / 'plan.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos)
        w.writeheader()
        w.writerows(plan)
    # plan-lotes.md
    md = (OUT / 'plan-lotes.md').read_text(encoding='utf-8')
    for l, (estado, _) in LOTES.items():
        md = re.sub(r'^(\| %s \|.*\| )(?:pendiente|sustituido[^|]*?|parcial[^|]*?) \|$' % re.escape(l), lambda m: m.group(1) + estado + ' |', md, flags=re.M)
    (OUT / 'plan-lotes.md').write_text(md, encoding='utf-8')
    # funciones-vivas.csv
    with (OUT / 'funciones-vivas.csv').open(encoding='utf-8', newline='') as s:
        filas = list(csv.DictReader(s))
        campos = list(filas[0])
    cierre = {f[0]: 'sustituida (S3)' if f[0] == '0x0085742B' else 'sustituida' for f in FUNCIONES}
    for r in filas:
        if r['va'] in cierre and r['va'] != '0x0085742B':
            r['cierre'] = cierre[r['va']]
    with (OUT / 'funciones-vivas.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos)
        w.writeheader()
        w.writerows(filas)
    print('cierre P3 aplicado:', len(FUNCIONES), 'funciones')


if __name__ == '__main__':
    main()
