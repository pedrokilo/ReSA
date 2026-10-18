import json, os, pathlib, hashlib

R = 'reversing/CppThreeLevel4Unified/runs'
S = 'reversing/CppThreeLevel4Study/README.md'


def passed_runs(bank):
    out = []
    for d in sorted(x for x in os.listdir(R) if x.startswith(bank + '-')):
        s = json.load(open(f'{R}/{d}/summary.json', encoding='utf-8'))
        if s.get('passed'):
            out.append((d, s))
    return out


def summ(bank):
    r = passed_runs(bank)
    assert r, bank
    return r[-1]


a = json.load(open('reversing/CppOnlyMigration/approvals.json', encoding='utf-8'))
tail = (' Revalidación del banco histórico completo sobre la fuente nueva (sin script de auditoría propio: la evidencia '
        'es el resumen y el registro de la corrida). Sin cobertura exhaustiva de x87 más allá de los fixtures del banco.')
total = 0


def entry(addr, ident, n, src, bank, scope, extra=()):
    global total
    assert addr not in a, addr
    d, s = summ(bank)
    e = {'identity': ident, 'converted_source_functions': n, 'source': 'source/game_sa/' + src,
         'shared_source': 'source/game_sa/X87Math.cpp', 'bank': bank, 'run': d.split('-', 1)[1], 'pairs': s['pairs'],
         'own_pairs': s['pairs'], 'audit': f'{R}\\{d}/summary.json', 'differences': 0, 'null_comparison_records': 0,
         'state_restored': True, 'helper_codegen': S}
    if extra:
        e['additional_banks'] = []
        for b in extra:
            dd, ss = summ(b)
            e['additional_banks'].append({'bank': b, 'run': dd.split('-', 1)[1], 'pairs': ss['pairs'], 'summary': f'{R}\\{dd}/summary.json'})
    e['scope'] = scope + tail
    a[addr] = e
    total += n


entry('0x005BD830', 'cHandlingDataMgr::LoadHandlingData (campo 13: AtofTimes04X87)', 1, 'cHandlingDataMgr.cpp', 'World11',
      'HandlingAtofTimes04Cpp: float(atof · 0.4 en double) con un solo redondeo a 24 bits.')
entry('0x006F5080', 'cHandlingDataMgr::ConvertDataToGameUnits (SubtractVelocityStepX87 y TimesOneSixthX87)', 2, 'cHandlingDataMgr.cpp', 'World11',
      'HandlingReduceVelocityCpp bajo float_control(precise): el bucle de reducción de velocidad con las constantes double del EXE.')
entry('0x006F5290', 'cHandlingDataMgr::ConvertBikeDataToGameUnits (SinX87)', 1, 'cHandlingDataMgr.cpp', 'World11',
      'HandlingSinScaledCpp: float(sin(ángulo · k)) con el producto redondeado a 24 bits.')
entry('0x006F5240', 'cHandlingDataMgr::ConvertBikeDataToWorldUnits (AsinDegreesHandlingX87 naked)', 1, 'cHandlingDataMgr.cpp', 'World11',
      'ExeAsinDegreesCpp (ya existía). La rama |x| >= 1 de _CIasin del EXE no se reproduce (ni antes ni ahora); el banco usa entradas en (-0,99; 0,99).')
entry('0x005FB800', 'CPedGroups::AddGroup', 1, 'PedGroups.cpp', 'DependenciesTask10',
      'C++ plano con un ayudante anónimo que vacía el grupo como el EXE (RemoveMember por hueco, Flush de inteligencia y bandera de misión a falso).', ('Game22', 'ScriptsFinal3'))
entry('0x005FB8C0', 'CPedGroups::Init y los puentes naked a RemoveMember y Flush', 2, 'PedGroups.cpp', 'DependenciesTask10',
      'Igual: la bandera de actividad baja antes de vaciar y ScriptReferenceIndex[g]=1 en los ocho grupos.')
entry('0x005FB930', 'CPedGroups::CleanUpForShutDown', 2, 'PedGroups.cpp', 'DependenciesTask10',
      'Vacía los ocho grupos sin tocar actividad ni índices SCM.')
entry('0x00572800', 'CTheZones::SetCurrentZoneAsUnlocked y los cuatro trampolines naked (FindPlayerCoors, GetLevelFromPosition, ZonesVisited y _ftol2)', 5, 'TheZones.cpp', 'DependenciesZones13',
      'Float puro: celda = (uint8)(int64)((pos + 3000) · 0x3ADA740E) como el fld/fadd/fmul a 24 bits y _ftol2 más movzx al; índice 10·cx − cy + 9 sin acotar; la función es void y el banco no compara EAX/ECX/EDX.')
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)
print('funciones:', total)

D = pathlib.Path('reversing/CppThreeLevel4Study/artifacts')
rows = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(D.glob('*.obj'))}
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items()))
banks = sorted({d.split('-')[0] for d in os.listdir(R)})
btab = '\n'.join(f'| {b} | {summ(b)[1]["pairs"]} |' for b in banks)
txt = f'''# Tres archivos de nivel 4 (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `cHandlingDataMgr.cpp`, `PedGroups.cpp` y `TheZones.cpp` ({total} funciones fuente). Un subagente por archivo preparó el análisis; la compilación y los bancos corrieron en serie. Ningún banco se relajó y los cinco aprobaron a la primera, sin tocar la instrumentación.

## Qué se hizo

- `cHandlingDataMgr`: tres ayudantes nuevos de `X87Math` (`HandlingAtofTimes04Cpp`, `HandlingReduceVelocityCpp`, `HandlingSinScaledCpp`) y `ExeAsinDegreesCpp` (ya existía) para el `naked` del arcoseno.
- `PedGroups`: C++ plano; los dos puentes naked a `RemoveMember` y `Flush` y las tres funciones pasan a un ayudante anónimo que vacía el grupo como el EXE.
- `TheZones`: float puro en `SetCurrentZoneAsUnlocked`; los cuatro trampolines naked a colaboradores del EXE desaparecen.

## Bancos (todos con cero diferencias)

| Banco | Pares |
| --- | ---: |
{btab}

## Límites

- La rama `|x| >= 1` de `_CIasin` del EXE (0x821EA1) no se reproduce ni antes ni ahora (la comparten `BulletInfo` y `Weapon`); el banco no la ejerce.
- `PedGroups` y `TheZones` no reproducen el residuo de EAX/ECX/EDX: los bancos no lo comparan y los llamadores son C++.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')
p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de tres archivos más (`cHandlingDataMgr`, `PedGroups` y `TheZones`); "
       "cinco bancos sin diferencias ([detalle y hashes](../CppThreeLevel4Study/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de cinco archivos más (`EventSource`')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
