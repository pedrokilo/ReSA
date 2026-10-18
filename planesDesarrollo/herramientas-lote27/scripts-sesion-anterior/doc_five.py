import json, os, pathlib

R = 'reversing/CppFiveFilesUnified/runs'
S = 'reversing/CppFiveFilesStudy/README.md'


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


def entry(addr, ident, n, src, bank, own, scope, extra=()):
    r, s = ref(bank)
    e = {
        'identity': ident, 'converted_source_functions': n, 'source': src,
        'shared_source': 'source/game_sa/X87Math.cpp', 'bank': bank, 'run': r, 'pairs': pairs(bank),
        'own_pairs': own, 'audit': s, 'differences': 0, 'null_comparison_records': 0, 'state_restored': True,
        'helper_codegen': S,
    }
    if extra:
        e['additional_banks'] = [add(b) for b in extra]
    e['scope'] = scope + tail
    a[addr] = e


G = 'source/game_sa/'
entry('0x00539F00', 'CFireManager::StartFire sin objetivo (FireStartBurnTime)', 1, G + 'Fire.cpp', 'Fire24', 1024,
      'FireBurnTimeCpp: ((rnd·factorA)·factorB + offset)·tiempo + ahora con enteros sin signo como fild (+2^32) y float_control(precise).')
entry('0x0053A570', 'CFire::ProcessFire (parte fraccionaria)', 1, G + 'Fire.cpp', 'Fx43', 1024,
      'IntFractionCpp (fisub exacto) y UnsignedTimesFloatCpp para el tiempo restante.')
entry('0x0056EAB0', 'CPlayerInfo::AddHealth', 1, G + 'PlayerInfo.cpp', 'Player28', 1024,
      'FloatPlusIntCpp(salud, cantidad) y el mínimo/máximo con NaN como el EXE.')
entry('0x0056DAD0', 'CPlayerInfo::EvaluateCarPosition', 1, G + 'PlayerInfo.cpp', 'Player28', 1024,
      'PlayerInfoAtanFromCpp: float(frente - GetATanOfXY(dx, dy)) con el ST0 de GetATanOfXY sin redondear.')
entry('0x007000E0', 'CPointLights::AddLight', 1, G + 'PointLights.cpp', 'DependenciesPointLights1', 2560,
      'PointLightsInRangeXYCpp y PointLightsDistanceFadeCpp bajo float_control(precise): comparaciones estrictas (false con NaN), suma (dz²+oy²)+ox² sin redondear y atenuación del EXE.',
      ('DependenciesPointLights2',))
entry('0x006133F0', 'CPopulation::CreateWaitingCoppers', 1, G + 'Population.cpp', 'PopFinal4', 512,
      'TrigCompoundSinMulAdd / TrigCompoundCosMulAdd para la posición y TrigAtan2Cpp(-dx, dy) para el rumbo (ayudantes ya existentes).')
entry('0x0045D760', 'CReplay::ProcessLookAroundCam', 1, G + 'Replay.cpp', 'Replay47', 1024,
      'TrigAtan2Cpp(dx, dy) para el ángulo de cámara y ReplayLookAroundOffsetsCpp con cos(vista) sin redondear para los dos primeros productos.')
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)


def parse(p):
    d = {}
    for l in pathlib.Path(p).read_text().splitlines():
        h, n = l.split(' *')
        d[n.split('/')[-1]] = h
    return d


b = parse('reversing/CppFiveFilesStudy/artifacts/before.sha256')
af = parse('reversing/CppFiveFilesStudy/artifacts/after.sha256')
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted({**b, **af}.items()))
txt = f'''# Fire, PlayerInfo, PointLights, Population y Replay sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `Fire.cpp`, `PlayerInfo.cpp`, `PointLights.cpp`, `Population.cpp` y `Replay.cpp` (10 apariciones, 7 funciones fuente). El análisis de cada archivo lo preparó un subagente (desensamblado del EXE y parche); la compilación y los bancos corrieron en serie. Ningún banco se relajó y los 7 aprobaron a la primera.

## Conversiones

| Función | Cómo |
| --- | --- |
| `CFireManager::StartFire` sin objetivo (0x539F00) | `FireBurnTimeCpp` (nuevo, `float_control(precise)`) |
| `CFire::ProcessFire` (0x53A570) | `IntFractionCpp` (nuevo) y `UnsignedTimesFloatCpp` |
| `CPlayerInfo::AddHealth` (0x56EAB0) | `FloatPlusIntCpp` (ya existía) |
| `CPlayerInfo::EvaluateCarPosition` (0x56DAD0) | `PlayerInfoAtanFromCpp` (nuevo; recibe `GetATanOfXY` por puntero) |
| `CPointLights::AddLight` (0x7000E0) | `PointLightsInRangeXYCpp` y `PointLightsDistanceFadeCpp` (nuevos, `precise`) |
| `CPopulation::CreateWaitingCoppers` (0x6133F0) | `TrigCompoundSinMulAdd`, `TrigCompoundCosMulAdd` y `TrigAtan2Cpp` (ya existían) |
| `CReplay::ProcessLookAroundCam` (0x45D760) | `TrigAtan2Cpp` y `ReplayLookAroundOffsetsCpp` (nuevo) |

Los ayudantes de Fire, FireBurnTimeCpp e IntFractionCpp, y el de Replay salen del stash `lote-28-archivos-sin-terminar`, verificados contra el desensamblado del EXE. Los de PlayerInfo y PointLights son nuevos.

## Bancos (cero diferencias)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| Fire24 | 9344 | StartFire sin objetivo (1024) |
| Fx43 | 15552 | ProcessFire (1024) |
| Player28 | 7552 | AddHealth (1024) y EvaluateCarPosition (1024) |
| DependenciesPointLights1 | 2560 | AddLight (2560) |
| DependenciesPointLights2 | 1024 | Init y RemoveLightsAffectingObject (hash de PointLights.cpp cambiado) |
| PopFinal4 | 4096 | CreateWaitingCoppers (512) |
| Replay47 | 18208 | ProcessLookAroundCam (1024) |

Antes de convertir, los subagentes revisaron cada banco contra el código fuente buscando colaboradores que la DLL llame directo y el banco intercepte solo por la dirección del EXE (el fallo que apareció en lotes anteriores); no encontraron ninguno y no hizo falta tocar ningún banco.

## Límites

Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos (por ejemplo, desbordes o denormales). Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')

p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de `Fire.cpp`, `PlayerInfo.cpp`, `PointLights.cpp`, `Population.cpp` y `Replay.cpp` con seis ayudantes nuevos en `X87Math` "
       "(`FireBurnTimeCpp`, `IntFractionCpp`, `PlayerInfoAtanFromCpp`, `PointLightsInRangeXYCpp`, `PointLightsDistanceFadeCpp`, `ReplayLookAroundOffsetsCpp`); "
       "bancos Fire24, Fx43, Player28, DependenciesPointLights1 y 2, PopFinal4 y Replay47 sin diferencias ([detalle y hashes](../CppFiveFilesStudy/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de `Quaternion.cpp`')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
