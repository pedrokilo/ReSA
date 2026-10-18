import json, os, pathlib

R = 'reversing/CppFourFilesUnified/runs'
S = 'reversing/CppFourFilesStudy/README.md'


def last(bank):
    return sorted(d for d in os.listdir(R) if d.startswith(bank + '-'))[-1]


def pairs(bank):
    return json.load(open(f'{R}/{last(bank)}/summary.json', encoding='utf-8'))['pairs']


def ref(bank):
    d = last(bank)
    return d.split('-', 1)[1], f'{R}\\{d}/summary.json'


a = json.load(open('reversing/CppOnlyMigration/approvals.json', encoding='utf-8'))
tail = (' Revalidación del banco histórico completo sobre la fuente nueva (sin script de auditoría propio: la evidencia '
        'es el resumen y el registro de la corrida). Sin cobertura exhaustiva de x87 más allá de los fixtures del banco.')


def entry(addr, ident, n, src, bank, own, scope):
    r, s = ref(bank)
    a[addr] = {
        'identity': ident, 'converted_source_functions': n, 'source': src,
        'shared_source': 'source/game_sa/X87Math.cpp', 'bank': bank, 'run': r, 'pairs': pairs(bank),
        'own_pairs': own, 'audit': s, 'differences': 0, 'null_comparison_records': 0, 'state_restored': True,
        'helper_codegen': S, 'scope': scope + tail,
    }


Q = 'source/game_sa/Core/Quaternion.cpp'
entry('0x0059C3E0', 'CQuaternion::Set (matriz)', 1, Q, 'DependenciesMathLeaf3', 1024,
      'Float puro en el mismo orden que el x87 (24 bits), cuatro ramas y escrituras intercaladas con la lectura de la matriz, bajo float_control(precise).')
entry('0x0059C600', 'CQuaternion::Set (eje y ángulo)', 1, Q, 'DependenciesMathLeaf3', 1024,
      'QuaternionFromAxisAngleCpp: un solo fsin con el seno sin redondear para las tres multiplicaciones y fcos.')
B = 'source/game_sa/Entity/Vehicle/Boat.cpp'
entry('0x006F1770', 'CBoat::ProcessControl (rumbo bloqueado)', 1, B, 'Boat23', 1024,
      'Atan2FromCpp: float(bloqueado - atan2(-x, y)) con el fpatan sin redondear; sin matriz, resta float.')
entry('0x006F0A10', 'CBoat::ProcessControlInputs (amortiguado del ratón)', 1, B, 'Boat23', 1024,
      'PowScaleFloatCpp: float(pow(0.985f, paso) * valor) con el factor sin redondear antes de multiplicar.')
T = 'source/game_sa/Entity/Vehicle/Trailer.cpp'
entry('0x006CFDF0', 'CTrailer::SetTowLink (HeadingX87)', 1, T, 'Sys13', 1024,
      'TrigAtan2Cpp(-x, y) con matriz y rumbo compacto sin ella. Para que Sys13 aprobara se interceptó también la entrada de la DLL de CMemoryMgr::Malloc (antes fallaba igual en HEAD: CTrailer::Constructor 71/256 y SetupSuspensionLines 120/512).')
entry('0x006CF590', 'CTrailer::ProcessAI (SteerTowardsTractor)', 1, T, 'Sys13', 1024,
      'TrailerSteerCpp: dos fpatan con A y B en la pila x87, comparación contra pi+B sin reasociar y un solo redondeo a float.')
E = 'source/game_sa/Events/Event.cpp'
entry('0x004B2850', 'CEvent::GetSoundLevel', 1, E, 'Events52', 1024,
      'Log10Times10Cpp: fyl2x inline y un solo redondeo a float. Para que Events52 aprobara se añadió la interceptación de las entradas de la DLL de CPedGroups::GetPedsGroup y CPedGeometryAnalyser::ComputeEntityHitSide (antes fallaban igual en HEAD: tres AffectsPed).')
entry('0x004AC050', 'CEvent::CalcSoundLevelIncrement', 1, E, 'Events52', 1024,
      'SoundLevelIncrementCpp: (pow2 + pow1) / pow1 en x87, fyl2x inline y un solo redondeo a float.')
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)


def parse(p):
    d = {}
    for l in pathlib.Path(p).read_text().splitlines():
        h, n = l.split(' *')
        d[n.split('/')[-1]] = h
    return d


b = parse('reversing/CppFourFilesStudy/artifacts/before.sha256')
af = parse('reversing/CppFourFilesStudy/artifacts/after.sha256')
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted({**b, **af}.items()))
txt = f'''# Quaternion, Boat, Trailer y Event sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `Quaternion.cpp`, `Boat.cpp`, `Trailer.cpp` y `Event.cpp` (8 apariciones, 8 funciones fuente). El análisis de cada archivo lo preparó un subagente (desensamblado del EXE y parche), con la compilación y los bancos en serie. Ningún banco se relajó.

## Conversiones

| Función | Cómo |
| --- | --- |
| `CQuaternion::Set(matriz)` (0x59C3E0) | float puro en el orden del EXE, bajo `float_control(precise)` |
| `CQuaternion::Set(eje, ángulo)` (0x59C600) | `QuaternionFromAxisAngleCpp` (nuevo) |
| `CBoat::ProcessControl` (0x6F1770) | `Atan2FromCpp` (nuevo) |
| `CBoat::ProcessControlInputs` (0x6F0A10) | `PowScaleFloatCpp` (nuevo, usa `ExeCompatiblePowCpp`) |
| `CTrailer::SetTowLink` (0x6CFDF0) | `TrigAtan2Cpp` (ya existía) |
| `CTrailer::ProcessAI` (0x6CF590) | `TrailerSteerCpp` (nuevo) |
| `CEvent::GetSoundLevel` (0x4B2850) | `Log10Times10Cpp` (nuevo) |
| `CEvent::CalcSoundLevelIncrement` (0x4AC050) | `SoundLevelIncrementCpp` (nuevo) |

Los ayudantes nuevos de `X87Math` salen del stash `lote-28-archivos-sin-terminar` (solo los de estos archivos), verificados contra el desensamblado. En el objeto real se comprobó que `fsin`, `fcos`, `fpatan` y `fyl2x` salen en línea, que `TrailerSteerCpp` mantiene A y B en la pila x87 y compara contra `pi + B` sin reasociar, y que `SoundLevelIncrementCpp` hace `fadd` y después `fdivrp`.

## Bancos (cero diferencias en las funciones convertidas)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| DependenciesMathLeaf3 | 2048 | Quaternion Set matriz (1024) y eje-ángulo (1024) |
| Boat23 | 12288 | ProcessControl (1024) y ProcessControlInputs (1024) |
| Sys13 | 15872 | ProcessAI (1024) y SetTowLink (1024) |
| Events52 | 13888 | GetSoundLevel (1024) y CalcSoundLevelIncrement (1024) |

## Lo que reveló la validación

Events52 y Sys13 fallaban en HEAD por funciones ajenas al lote, con el patrón ya conocido: el código de la DLL llama directamente a funciones que el banco solo interceptaba por su dirección del EXE.

- **Events52:** `CEventPedCollisionWithPed::AffectsPed` (71/1024), `CEventVehicleCollision::AffectsPed` (9/1024) y `CEventVehicleOnFire::AffectsPed` (394/1024). Se añadieron en `reversing/Events52/LiveComparison.hpp` los `EntryPatch` de las entradas de la DLL de `CPedGroups::GetPedsGroup` y de `CPedGeometryAnalyser::ComputeEntityHitSide` (sobrecarga `const CVector&, CEntity&`), con el mismo Fake. Aprobó con 13888 pares.
- **Sys13:** `CTrailer::Constructor` (71/256) y `SetupSuspensionLines` (120/512), por `CMemoryMgr::Malloc`. Se sustituyó el parche de 0x72F420 por un `MallocGuard` que intercepta también las dos sobrecargas de la DLL. Aprobó con 15872 pares.
- Las corridas fallidas quedan en `CppFourFilesUnified/runs/Events52-20261014-173725-946034` y `Sys13-20261014-174116-728583`.
- `limpiar_regenerables.py` tenía un fallo al leer `tasklist` (sin salida decodificable); corregido.

## Límites

Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos. Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')

p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de `Quaternion.cpp`, `Boat.cpp`, `Trailer.cpp` y `Event.cpp` con seis ayudantes nuevos en `X87Math` "
       "(`QuaternionFromAxisAngleCpp`, `Atan2FromCpp`, `PowScaleFloatCpp`, `TrailerSteerCpp`, `Log10Times10Cpp`, `SoundLevelIncrementCpp`); "
       "bancos DependenciesMathLeaf3, Boat23, Sys13 y Events52 sin diferencias, con la interceptación de las entradas de la DLL añadida a Events52 y Sys13 "
       "([detalle y hashes](../CppFourFilesStudy/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de `BulletTraces.cpp`')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
