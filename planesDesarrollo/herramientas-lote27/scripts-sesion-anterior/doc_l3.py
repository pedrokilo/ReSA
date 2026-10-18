import json, os, pathlib, hashlib

R = 'reversing/CppFiveLevel3Unified/runs'
S = 'reversing/CppFiveLevel3Study/README.md'


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


entry('0x004ABAC0', 'CEventSource::ComputeEventSourceType y los tres saltos naked a IsThreatenedBy, IsFriendlyWith e IsPlayer', 4, 'Events/EventSource.cpp', 'DependenciesTaskPed5',
      'C++ sin tocar ECX/EDX tras las llamadas: el EDX de entrada se lee con CarEnterExitEntryEdxCpp, GetSourceEntity y los colaboradores se llaman como uint64(__fastcall*) encadenando el EDX, el resultado se prueba sobre AL y el tipo no ped sale con ResidueRegsCpp. Verificado por contrib/audit_register_residue.py (nueva comprobación de que nada escribe ECX/EDX tras las llamadas).')
entry('0x0041BD90', 'CGeneral::GetRandomNumberInRange<float>', 1, 'General.cpp', 'CGeneral',
      'GeneralRandomInRangeCpp(GetRandomNumber(), min, max). La instrumentación de build_probe.py de CGeneral cuenta ahora las llamadas con un ++ al principio de la función (antes se insertaba un inc junto al push ecx del ensamblador).')
entry('0x0053CC70', 'CGeneral::GetATanOfXY', 1, 'General.cpp', 'Core9',
      'GeneralATanOfXYDblCpp devuelve double (extern "C") y General.cpp lo enlaza como float con /alternatename: el fpatan puro sale sin redondear en ST0 (una primera versión devolvía float y Core9 dio 97/1024 diferencias de ST0).')
entry('0x0053CBE0', 'CGeneral::GetRadianAngleBetweenPoints', 1, 'General.cpp', 'Core9',
      'GeneralRadianAngleCpp con el fpatan en un ayudante aparte sin precise. Cuatro ramas por los signos estrictos de dx y dy.')
entry('0x0053CE30', 'CGeneral::SolveQuadratic', 1, 'General.cpp', 'Core9',
      'GeneralSolveQuadraticCpp: discriminante en el rango extendido de la x87, sqrt en ayudante aparte, x2 se escribe antes que x1.')
entry('0x0061A1D0', 'CPlayerRelationshipRecorder::RecordRelationshipWithPlayer y los tres saltos naked a GetActiveTask, Kill y Follow', 4, 'PlayerRelationshipRecorder.cpp', 'DependenciesDecisionRelationship6',
      'RecordRelationshipWithPlayerRegs(__fastcall) con ResidueRegsCpp; las escrituras de Relationship 3 y 7 son código muerto (ningún camino pone a3 distinto de 0). El banco necesitó dos cambios de instrumentación: el hook apuntaba al envoltorio thiscall (ahora a ...Regs) y la ruta original ejecutaba el GetActiveTask de la DLL (que deja ECX=0) en lugar del del EXE, por lo que se añadió una copia exacta del bucle del EXE como EntryPatch solo para esta función. Verificado por contrib/audit_register_residue.py.')
entry('0x006171F0', 'CBoneNode_c::EulerToQuat', 1, 'Ragdoll/BoneNode.cpp', 'BoneNodes',
      'BoneNodeEulerToQuatCpp: semiángulos bajo precise, sy y sz sin redondear, normalización 1/(((w²+z²)+y²)+x²).', ('IK20',))
entry('0x00617080', 'CBoneNode_c::QuatToEuler', 1, 'Ragdoll/BoneNode.cpp', 'BoneNodes',
      'BoneNodeQuatToEulerCpp: un fpatan en línea por ángulo, rama de bloqueo cardán con comparación ordenada y q releído tras escribir out[1].')
entry('0x00617650', 'CBoneNode_c::Limit', 1, 'Ragdoll/BoneNode.cpp', 'BoneNodes',
      'BoneNodeLimitClampCpp: comparaciones ordenadas, mínimo antes que máximo, factor de la cabeza en double.')
entry('0x00616CD0', 'CBoneNode_c::CalcWldMat', 1, 'Ragdoll/BoneNode.cpp', 'BoneNodes',
      'BoneNodeLocalMatrixCpp: s=2/(((x²+y²)+z²)+w²) y productos en el orden del EXE bajo precise; RwMatrixMultiply del EXE y recursión por GetHead/GetNext.')
entry('0x0067FD30', 'CTaskSimpleClimb (ClimbHeadingOf, ClimbAddHeading, ClimbSubHeading y ClimbHeadingSub)', 4, 'Tasks/TaskTypes/TaskSimpleClimb.cpp', 'CppTrigClimb',
      'TrigAtan2Cpp, BirdsAddAtan2Cpp, Atan2FromCpp y Atan2MinusCpp con el argumento x negado. CppTrigClimb cubre GetCameraStickModifier, GetCameraTargetPos y TestForClimb; ScanToGrab y ScanToGrabSectorList (ClimbHeadingOf y ClimbSubHeading) solo las ejerce CTasks15, donde dieron cero diferencias, pero ese banco no aprueba entero: ProcessPed falla 1 par de 2048 (caso 285) también con la fuente anterior a la conversión, por la contaminación de las 247 celdas de sectores sin aislar descrita en reversing/CppClimbWorldIsolationProposal.')
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)
print('funciones:', total)

D = pathlib.Path('reversing/CppFiveLevel3Study/artifacts')
rows = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(D.glob('*.obj'))}
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items()))
banks = sorted({d.split('-')[0] for d in os.listdir(R)})
lines = []
for b in banks:
    ok = passed_runs(b)
    if ok:
        lines.append(f'| {b} | {ok[-1][1]["pairs"]} | aprobado |')
    else:
        lines.append(f'| {b} | - | falla (ver abajo) |')
btab = '\n'.join(lines)
txt = f'''# Cinco archivos de nivel 3 (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `Events/EventSource.cpp`, `General.cpp`, `PlayerRelationshipRecorder.cpp`, `Ragdoll/BoneNode.cpp` y `Tasks/TaskTypes/TaskSimpleClimb.cpp` ({total} funciones fuente). Un subagente por archivo preparó el análisis y otros diagnosticaron los fallos de banco (sin ejecutar nada); compilación y bancos corrieron en serie. Ningún banco se relajó.

## Qué se hizo

- `General`: `GetRandomNumberInRange<float>`, `GetRadianAngleBetweenPoints`, `GetATanOfXY` y `SolveQuadratic` con ayudantes `General*Cpp` de `X87Math`. `GetATanOfXY` devuelve el `fpatan` puro sin redondear en ST0 con el mismo mecanismo que `Placeable` (ayudante `extern "C"` que devuelve `double` y alias `/alternatename` como `float`).
- `BoneNode`: cuatro ayudantes `BoneNode*Cpp` (`EulerToQuat`, `QuatToEuler`, `Limit`, `CalcWldMat`).
- `TaskSimpleClimb`: los cuatro ayudantes locales pasan a `TrigAtan2Cpp`, `BirdsAddAtan2Cpp`, `Atan2FromCpp` y `Atan2MinusCpp`, sin ayudantes nuevos.
- `EventSource` y `PlayerRelationshipRecorder`: residuo de registros con `ResidueRegsCpp`, `CarEnterExitEntryEdxCpp` y llamadas `uint64(__fastcall*)`; `contrib/audit_register_residue.py` ahora incluye estos objetos y una comprobación nueva de que nada escribe ECX/EDX tras las llamadas de `EventSource`.

## Qué se encontró

- `GetATanOfXY`: la primera versión devolvía `float` y `Core9` dio 97/1024 diferencias de ST0 (el resultado salía redondeado a 24 bits). Se corrigió devolviendo `double` y enlazando como `float`.
- `CGeneral` falló porque su `build_probe.py` contaba llamadas insertando un `inc` junto al `push ecx` del ensamblador, que ya no existe. Ahora cuenta con un `++` al principio de la función.
- `EventSource`: 1504/2048 pares; el código sin optimizar escribía ECX y EDX tras las llamadas (`movzx ecx,al`). Se reformuló sin escribirlos; 8192 pares sin diferencias.
- `RecordRelationshipWithPlayer`: 410/2048, solo ECX en los casos sin tarea. El hook del banco apuntaba al envoltorio y la ruta original ejecutaba el `GetActiveTask` de la DLL (ECX=0) en vez del del EXE; se corrigió la instrumentación del banco con una copia exacta del bucle del EXE. 6272 pares sin diferencias.
- `BoneNodes`: el primer intento se cayó (0xC0000409) durante el arranque del juego, en `CPopCycle::Initialise` (`POPCYCLE.DAT` no se pudo abrir en ese momento), antes de empezar el banco; el análisis estático de las conversiones no encontró ningún defecto y el segundo intento aprobó con 12256 pares. La causa exacta del fallo de entorno no se determinó.
- `CTasks15` falla con 5 funciones de `CTaskComplexAvoidOtherPedWhileWandering` (la DLL llamaba directamente a `CPedGeometryAnalyser::ComputeRouteRoundSphere` y a `CTaskComplexGangFollower::CalculateOffsetPosition`, que el banco solo interceptaba por la dirección del EXE) y se añadieron en `reversing/CTasks15/LiveComparison.hpp` los `EntryPatch` en la entrada de la DLL con el mismo Fake. Quedó solo `CTaskSimpleClimb::ProcessPed` con 1 par de 2048 (caso 285).

## Bancos

| Banco | Pares | Resultado |
| --- | ---: | --- |
{btab}

`CTasks15` no aprueba entero: `ProcessPed` difiere en 1 de 2048 pares (caso 285, solo en la traza: el EXE alcanza cuatro entidades reales del mundo con `GetBoundCentre` que la DLL ya encuentra marcadas). Con la fuente de `TaskSimpleClimb` anterior a la conversión el resultado es idéntico (corrida `CTasks15-20261015-095116-951158`), así que no lo causa este lote; es la contaminación por las 247 celdas de sectores que el banco no aísla, ya descrita en `reversing/CppClimbWorldIsolationProposal`. Las seis funciones restantes de `CTasks15` (incluidas `ScanToGrab` y `ScanToGrabSectorList`) dieron cero diferencias en esa corrida.

## Límites

- `TaskSimpleClimb`: sin una corrida de `CTasks15` completamente aprobada; la evidencia de `ScanToGrab` y `ScanToGrabSectorList` son sus pares sin diferencias dentro de una corrida que no aprobó por `ProcessPed`.
- `GetRadianAngleBetweenPoints`: el EXE decide el signo de `dy` con las banderas de `fcom` leídas tras `fdiv`/`fpatan`, y se tradujo a `dy>0`; `Core9` (1024 pares) lo compara con el EXE y no dio diferencias, pero Intel documenta esas banderas como indefinidas.
- `CalcWldMat` deja campos de relleno sin inicializar como el EXE.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')
p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de cinco archivos más (`EventSource`, `General`, `PlayerRelationshipRecorder`, `BoneNode` y `TaskSimpleClimb`); "
       "los bancos aprobaron sin diferencias salvo un par de `CTasks15` que ya fallaba antes ([detalle y hashes](../CppFiveLevel3Study/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de otros cinco archivos de nivel 2')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
