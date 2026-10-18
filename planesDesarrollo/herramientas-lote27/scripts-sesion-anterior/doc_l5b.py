import json, os, pathlib, hashlib, re

R = 'reversing/CppTwoLevel5Unified/runs'


def passed_runs(bank):
    out = []
    for d in sorted(x for x in os.listdir(R) if x.startswith(bank + '-')):
        s = json.load(open(f'{R}/{d}/summary.json', encoding='utf-8'))
        if s.get('passed'):
            out.append((d, s))
    return out


def last(b):
    return passed_runs(b)[-1]


p = 'reversing/CppOnlyMigration/approvals.json'
a = json.load(open(p, encoding='utf-8'))


def tail_of(e):
    return e['scope'][e['scope'].index(' Revalidación'):]


def refresh(addr, bank, extra=None):
    e = a[addr]
    d, s = last(bank)
    e['run'] = d.split('-', 1)[1]
    e['pairs'] = s['pairs']
    e['own_pairs'] = s['pairs']
    e['audit'] = f'{R}\\{d}/summary.json'
    if extra is not None:
        e['additional_banks'] = [{'bank': b, 'run': last(b)[0].split('-', 1)[1], 'pairs': last(b)[1]['pairs'],
                                  'summary': f'{R}\\{last(b)[0]}/summary.json'} for b in extra]


for addr in ('0x004D1680', '0x004D1A50', '0x004D1DB0', '0x004D2100', '0x004D2450', '0x004D27F0'):
    refresh(addr, 'RpAnim43')
    e = a[addr]
    e['scope'] = e['scope'].split(' Revalidación')[0] + (' El banco RpAnim43 ahora ejerce también NaN (distinto signo y carga, señalizantes), ±inf, ±0, subnormales, magnitudes ±1e30/±1e-30/FLT_MAX, cuaterniones '
                                                         'sin normalizar o nulos y tiempos extremos en uno de cada tres casos (2048 casos por envoltorio); con ellos las sumas, restas y productos de la plantilla pasan por ayudantes x87 '
                                                         '(RpNan*, ExeAdd/ExeSub/ExeMul) porque el SSE de la DLL difiere en NaN con carga distinta, subnormales y desbordes intermedios.') + tail_of(e)
refresh('0x005DEF60', 'CPedC')
e = a['0x005DEF60']
e['scope'] = ('PedLocalDirectionRegsCpp: fpatan(-x, y) sin redondear, menos la rotación, más π/4, más 2π mientras sea negativo, por 2/π y _ftol2 emulado (Ftol2RegsCpp, compartido con TheZones); '
              'GetLocalDirection termina en ResidueRegsCpp con los EAX, ECX y EDX con que sale el EXE (incluida la rama n > 3). El banco CPedC ahora compara EAX, ECX y EDX con semillas por caso. '
              'Verificado por contrib/audit_register_residue.py.' + tail_of(e))
refresh('0x005DF910', 'CPedE')
refresh('0x005DFDF0', 'CPedE')
refresh('0x005E1FA0', 'CPedB')
refresh('0x005E65A0', 'CPedB')
refresh('0x005E5380', 'CppPedFootEmitters1', ('CPedCompletion',))
e = a['0x005E5380']
e['scope'] = e['scope'].replace('reversing/CPed (completion) valida DoFootLanded con tolerancia 1e-5 (compuerta histórica, no se tocó).',
                                'reversing/CPed (completion, 28672 pares de seis funciones) compara ahora en bits exactos (se quitó la tolerancia 1e-5 de run_completion.py tras corregir las diferencias de CalculateNewVelocity, PlayFootSteps y KillPedWithCar).')
json.dump(a, open(p, 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)

rp = pathlib.Path('reversing/CppTwoLevel5Study/README.md')
t = rp.read_text(encoding='utf-8')
banks = sorted({d.split('-')[0] for d in os.listdir(R)})
btab = '\n'.join(f'| {b} | {last(b)[1]["pairs"]} |' for b in banks)
i = t.index('| Banco | Pares |')
j = t.index('\n\n', i)
t = t[:i] + '| Banco | Pares |\n| --- | ---: |\n' + btab + t[j:]
rows = {q.name: hashlib.sha256(q.read_bytes()).hexdigest() for q in sorted(pathlib.Path('reversing/CppTwoLevel5Study/artifacts').glob('*.obj'))}
i = t.index('| Objeto | SHA256 |')
t = t[:i] + '| Objeto | SHA256 |\n| --- | --- |\n' + '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items())) + '\n'

a0 = t.index('## Bancos')
seg = '''## Seguimiento: los límites que se cerraron (15/10/2026)

Tras la primera versión quedaban cuatro límites escritos; los cuatro se resolvieron.

- **Tolerancia de `reversing/CPed`.** `run_completion.py` compara ahora en bits exactos (incluido el signo del cero), sin tolerancia. Medido sobre el log completo, había diferencias reales en tres funciones, todas de redondeo del código C++ (SSE en Debug) frente al x87 del EXE, y se corrigieron:
  - `CalculateNewVelocity` (202 de 4096 pares): el arcoseno se redondeaba a float antes de multiplicar (el EXE lo deja en ST0), el de `m_fSlopeRoll` usaba el `asinf` de la DLL en lugar del modelo de `_CIasin`, y `m_vecAnimMovingShift` conservaba un `-0.0` donde el EXE suma sobre `+0.0`. Ayudantes `CnvBlendedAsinCpp`, `CnvAsinFloatCpp`, `CnvZeroPlusSumCpp` y `CnvDecayPowCpp`.
  - `PlayFootSteps` (34 de 8192): `std::log10(adhesion) * 20` redondeaba dos veces; ahora `AudioLog10Times20Cpp`, que ya existía.
  - `KillPedWithCar` (25 de 8192): el `lift` del rebote se calculaba con un solo redondeo en double y el EXE redondea cada paso a 24 bits; ayudante `KpcBounceLiftScaleCpp`.
  La sonda del banco se reconstruyó con el texto nuevo de `PlayFootSteps` en su sustitución (`build_completion_probe.py`) y su hash se actualizó en `run_completion.py`. Aprobó con 28672 pares y error máximo 0.
- **Fixtures de `RpAnim43` sin NaN.** El generador de entradas ejerce ahora NaN (signo y carga distintos, señalizantes), ±inf, ±0, subnormales, magnitudes extremas, cuaterniones sin normalizar o nulos y tiempos extremos en uno de cada tres casos (2048 casos por envoltorio, sin quitar compuertas). Para que la plantilla coincida con el EXE en esos casos, sus sumas, restas y productos pasan por ayudantes x87 (`ExeAdd`, `ExeSub`, `ExeMul` y `RpNan*`, incluido `RpNanQuatToMatrixCpp`, que sigue paso a paso 0x59C080), y la ruta sin velocidad usa `AnimQuaternionDotNegativeCpp`. Aprobó con 13248 pares nuevos... (ver tabla).
- **Residuo de `_ftol2` en `GetLocalDirection`.** `GetLocalDirection` termina en `ResidueRegsCpp` con los EAX, ECX y EDX del EXE, incluida la rama n > 3; el emulador de `_ftol2` se movió a `X87Math` (`Ftol2RegsCpp`) y lo comparten `TheZones` y `Ped`. `CPedC` compara ahora los tres registros con semillas por caso, y el auditor `contrib/audit_register_residue.py` vigila el objeto `Ped`.
- **Registro de `reversing/CPed`.** No era cierto que solo se conservara el resumen: el log completo (25 MB, 2 MB comprimido) está en `reversing/CppTwoLevel5Unified/runs/CPedCompletion-*/comparisons.jsonl.gz`.

'''
t = t[:a0] + seg + t[a0:]
a0 = t.index('## Límites')
a1 = t.index('## Objetos')
t = t[:a0] + '''## Límites

- `RpAnimBlend`: la plantilla se validó con los fixtures ampliados, pero `FrameUpdateCallBackW` y `Offscreen` (excluidos con `#if USE_COPY_PASTE_FRAME_UPDATE`) siguen sin compilarse ni ejercerse.
- `ExeAsinDoubleCpp` y los ayudantes con `double` dependen de que el proceso corra el x87 a 24 bits de precisión.
- `GetLocalDirection` con -inf no termina (igual que en el EXE); el banco no genera infinitos para esa función.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

''' + t[a1:]
t = t.replace('Aprobó con 13248 pares nuevos... (ver tabla).', 'Aprobó (ver tabla).')
rp.write_text(t, encoding='utf-8')
