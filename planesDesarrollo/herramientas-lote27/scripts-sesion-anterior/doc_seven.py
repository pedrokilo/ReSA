import json, os, pathlib

R = 'reversing/CppFourFilesCUnified/runs'
S = 'reversing/CppFourFilesCStudy/README.md'


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


G = 'source/game_sa/'
entry('0x0054ECE0', 'Common::TransformPoint', 1, G + 'common.cpp', 'DependenciesMathSprite2', 1024,
      'CommonTransformPointCpp: cos/sin sin redondear, x escrito antes de releer in[0]/in[1] (la salida puede ser la entrada) y z al final.')
entry('0x0054EE30', 'Common::TransformVectors (con CSimpleTransform)', 1, G + 'common.cpp', 'CEntA', 0,
      'TrigSimpleCosCpp / TrigSimpleSinCpp con un solo redondeo, lectura de la entrada en el mismo orden que el EXE (escribe out.x y relee in.x), z copiada como dword y contador uint32 decreciente. SIN banco propio y sin pares propios: no está hookeada; CEntA la ejecuta con su cuerpo real dentro de otras funciones y aprueba, pero la función no se valida por separado.')
entry('0x006E4160', 'WaterCreature_c::Init (GetHeading)', 1, G + 'WaterCreature_c.cpp', 'World52', 1024,
      'TrigAtan2Cpp(-forward.x, forward.y) para el rumbo con matriz (el mismo ayudante se usa en Update).')
entry('0x006E4670', 'WaterCreature_c::Update (GetForward)', 1, G + 'WaterCreature_c.cpp', 'World52', 1024,
      'Dos ayudantes locales bajo float_control(precise): adelante·velocidad con -sin y cos sin redondear (rama del grupo) y -sin·paso con cos redondeado a float (rama en solitario), como los dos sitios del EXE (0x6E4898 y 0x6E4D45). La conversión del stash redondeaba el seno antes de multiplicar y se corrigió.')
entry('0x00628C80', 'CTaskComplexDestroyCarArmed::CalculateSearchPositionAndRanges', 1,
      G + 'Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp', 'CTasks10a', 256,
      'DestroyCarDistanceCpp: sqrt((x·x + y·y) + z·z) con la suma sin reasociar (float_control(precise)); los rangos 0,5/0,9/1,1/0,8 quedan en float.')
entry('0x005BBAC0', 'CTimeCycle::Initialise (dirección de la luz al sol)', 1, G + 'TimeCycle.cpp', 'Time20', 64,
      'TimeCycleDirnToSunCpp: {cos(b)·cos(a), sin(b)·cos(a), sin(a)} con cos(a) sin redondear. Era un envoltorio naked en línea (no una entrada del EXE): no hay registros de entrada ni de salida que conservar.')
entry('0x005603D0', 'CTimeCycle::CalcColoursForPoint (vector al sol)', 1, G + 'TimeCycle.cpp', 'Time20', 1024,
      'TimeCycleVectorToSunCpp: x = sunX + sin(a), z = sunZ - cos(a) con seno y coseno sin redondear. Era un envoltorio naked en línea.')
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)


def parse(p):
    d = {}
    for l in pathlib.Path(p).read_text().splitlines():
        h, n = l.split(' *')
        d[n.split('/')[-1]] = h
    return d


b = parse('reversing/CppFourFilesCStudy/artifacts/before.sha256')
af = parse('reversing/CppFourFilesCStudy/artifacts/after.sha256')
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted({**b, **af}.items()))
txt = f'''# common, WaterCreature_c, TaskComplexDestroyCarArmed y TimeCycle sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `common.cpp`, `WaterCreature_c.cpp`, `TaskComplexDestroyCarArmed.cpp` y `TimeCycle.cpp` (8 apariciones de `__asm` y 2 `naked`, 7 funciones fuente). Son los cuatro archivos que quedaban del stash `lote-28-archivos-sin-terminar`. Un subagente por archivo preparó el análisis (desensamblado del EXE y parche); la compilación y los bancos corrieron en serie. Ningún banco se relajó y los cuatro aprobaron a la primera.

## Conversiones

| Función | Cómo |
| --- | --- |
| `Common::TransformPoint` (0x54ECE0) | `CommonTransformPointCpp` (nuevo) |
| `Common::TransformVectors` (0x54EE30) | `TrigSimpleCosCpp` / `TrigSimpleSinCpp` y bucle en el orden del EXE |
| `WaterCreature_c::Init` / `Update` (0x6E4160 / 0x6E4670) | `TrigAtan2Cpp` y dos ayudantes locales `precise` para adelante·velocidad |
| `CTaskComplexDestroyCarArmed::CalculateSearchPositionAndRanges` (0x628C80) | `DestroyCarDistanceCpp` (nuevo) |
| `CTimeCycle::Initialise` (0x5BBAC0) | `TimeCycleDirnToSunCpp` (nuevo) |
| `CTimeCycle::CalcColoursForPoint` (0x5603D0) | `TimeCycleVectorToSunCpp` (nuevo) |

Los dos `naked` de `TimeCycle.cpp` eran los envoltorios de los dos bloques `__asm` (fragmentos en línea de hooks ya escritos en C++), sin registros de entrada ni de salida que conservar, así que no hizo falta `ResidueRegsCpp` ni `EntryEaxCpp`.

Correcciones del análisis respecto al stash:
- `WaterCreature_c`: el stash redondeaba el seno a float antes de multiplicar por la velocidad; el EXE lo deja sin redondear en un sitio y redondea solo el coseno en el otro. Se corrigió.
- `common.cpp` (`TransformVectors`): el EXE escribe `out.x` y después relee `in.x`; con entrada igual a salida el resultado cambia. Se corrigió el orden de lectura y el contador del bucle.

## Bancos (cero diferencias)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| DependenciesMathSprite2 | 3072 | TransformPoint (1024) |
| World52 | 18384 | WaterCreature_c Init (1024) y Update (1024) |
| CTasks10a | 15488 | CalculateSearchPositionAndRanges (256) |
| Time20 | 7488 | Initialise (64) y CalcColoursForPoint (1024) |
| CEntA | 16384 | solo como cobertura indirecta de TransformVectors |

## Límites

- `Common::TransformVectors` no tiene banco propio (no está hookeada): CEntA ejecuta su cuerpo real dentro de otras funciones y aprueba, pero la función no se valida por separado. Para cobertura real habría que hookearla y derivar un banco.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos (denormales, desbordes). Los colaboradores no se validan por probar a sus consumidores. No se revisó el código máquina de los ayudantes nuevos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')

p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de `common.cpp`, `WaterCreature_c.cpp`, `TaskComplexDestroyCarArmed.cpp` y `TimeCycle.cpp` (los cuatro archivos que quedaban del stash `lote-28-archivos-sin-terminar`) con cuatro ayudantes nuevos en `X87Math` "
       "(`CommonTransformPointCpp`, `DestroyCarDistanceCpp`, `TimeCycleDirnToSunCpp`, `TimeCycleVectorToSunCpp`); "
       "bancos DependenciesMathSprite2, World52, CTasks10a y Time20 sin diferencias; `Common::TransformVectors` solo con cobertura indirecta "
       "([detalle y hashes](../CppFourFilesCStudy/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de `Ropes.cpp`')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
