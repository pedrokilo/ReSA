import json, os, pathlib

R = 'reversing/CppBulletTracesUnified/runs'
S = 'reversing/CppBulletTracesStudy/README.md'


def last(bank):
    return sorted(d for d in os.listdir(R) if d.startswith(bank + '-'))[-1]


def pairs(bank):
    return json.load(open(f'{R}/{last(bank)}/summary.json', encoding='utf-8'))['pairs']


def ref(bank):
    d = last(bank)
    return d.split('-', 1)[1], f'{R}\\{d}/summary.json'


def add(bank):
    r, s = ref(bank)
    return {'bank': bank, 'run': r, 'pairs': pairs(bank), 'summary': s}


a = json.load(open('reversing/CppOnlyMigration/approvals.json', encoding='utf-8'))
tail = (' Revalidación del banco histórico completo sobre la fuente nueva (sin script de auditoría propio: la evidencia '
        'es el resumen y el registro de la corrida). Sin cobertura exhaustiva de x87 más allá de los fixtures del banco.')


def entry(addr, ident, own, scope):
    r, s = ref('Fx43')
    a[addr] = {
        'identity': ident,
        'converted_source_functions': 1,
        'source': 'source/game_sa/BulletTraces.cpp',
        'shared_source': 'source/game_sa/X87Math.cpp',
        'bank': 'Fx43',
        'run': r,
        'pairs': pairs('Fx43'),
        'own_pairs': own,
        'audit': s,
        'differences': 0,
        'null_comparison_records': 0,
        'state_restored': True,
        'helper_codegen': S,
        'additional_banks': [add(b) for b in ('Data7', 'Replay47', 'WeaponFinal2')],
        'scope': scope + tail,
    }


entry('0x00723750', 'CBulletTraces::AddTrace (PlayFrontEndSoundForTrace)', 1024,
      'Volumen del sonido de la bala con AudioLog10Times20Cpp(factor), igual que el fyl2x del EXE (0x7238F7).')
entry('0x00723C10', 'CBulletTraces::Render', 512,
      'Progreso 1 - transcurrido/duración con OneMinusUnsignedRatioCpp (enteros sin signo cargados como el EXE).')
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)


def parse(p):
    d = {}
    for l in pathlib.Path(p).read_text().splitlines():
        h, n = l.split(' *')
        d[n.split('/')[-1]] = h
    return d


b = parse('reversing/CppBulletTracesStudy/artifacts/before.sha256')
af = parse('reversing/CppBulletTracesStudy/artifacts/after.sha256')
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted({**b, **af}.items()))
txt = f'''# BulletTraces sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `BulletTraces.cpp` (2 apariciones, 2 funciones fuente). Ningún banco se relajó.

## Conversiones

| Función | Cómo |
| --- | --- |
| `PlayFrontEndSoundForTrace` (dentro de `CBulletTraces::AddTrace`, 0x723750) | `volume = AudioLog10Times20Cpp(factor)` (ya existía en `X87Math`) |
| `CBulletTraces::Render` (0x723C10) | `OneMinusUnsignedRatioCpp(restante, vida)`: helper nuevo en `X87Math` (recuperado del stash `lote-28-archivos-sin-terminar`) |

## Bancos (cero diferencias en las funciones de CBulletTraces)

| Banco | Pares | Papel |
| --- | ---: | --- |
| Fx43 | 15552 | Valida AddTrace (1024), Render (512), AddTrace-Wrapper (512), Update (256) e Init (64) |
| Data7 | 13760 | Dependencia (falsea Update y Render) |
| Replay47 | 18208 | Dependencia (falsea Init) |
| WeaponFinal2 | 7808 | Dependencia (falsea AddTrace y su wrapper) |

## Lo que reveló la validación

- **Fx43 falló en la primera corrida (y fallaba igual en HEAD) por funciones ajenas** a BulletTraces: `CFire::ProcessFire` (609/1024), `CMovingThings::Init` (64/64), `FxMemoryPool_c::Init` (64/64) y `Optimise` (128/128). Causa: el banco solo interceptaba las direcciones del EXE de `CPointLights::AddLight`, `CPointLights::Init`, `CMemoryMgr::Malloc` y `CMemoryMgr::Realloc`, y el código de la DLL las llama directamente. Se añadieron los `EntryPatch` de las entradas de la DLL en `reversing/Fx43/LiveComparison.hpp` (sin quitar ninguna compuerta) y el banco aprobó con 15552 pares. La corrida fallida queda en `CppBulletTracesUnified/runs/Fx43-20261014-003615-046612`.
- **Disco D: lleno.** Cada sonda deja un `.pdb` de 1,7 GB; los de esta tanda se movieron (sin borrarse) fuera de `reversing/*/artifacts`.

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
ins = ("Se eliminó el ensamblador de `BulletTraces.cpp` con el helper nuevo `OneMinusUnsignedRatioCpp` en `X87Math` y "
       "`AudioLog10Times20Cpp`; bancos Fx43, Data7, Replay47 y WeaponFinal2 sin diferencias, con la interceptación de las "
       "entradas de la DLL añadida a Fx43 ([detalle y hashes](../CppBulletTracesStudy/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de `AEAudioEnvironment.cpp`')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
