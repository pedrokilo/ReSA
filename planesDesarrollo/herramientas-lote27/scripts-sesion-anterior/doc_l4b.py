import json, os, pathlib, hashlib

R = 'reversing/CppThreeLevel4Unified/runs'


def runs(b):
    out = []
    for d in sorted(x for x in os.listdir(R) if x.startswith(b + '-')):
        s = json.load(open(f'{R}/{d}/summary.json', encoding='utf-8'))
        if s.get('passed'):
            out.append((d, s))
    return out


def last(b):
    return runs(b)[-1]


p = 'reversing/CppOnlyMigration/approvals.json'
a = json.load(open(p, encoding='utf-8'))


def tail_of(e):
    return e['scope'][e['scope'].index(' Revalidación'):]


def refresh(addr, bank, extra=()):
    e = a[addr]
    d, s = last(bank)
    e['run'] = d.split('-', 1)[1]
    e['pairs'] = s['pairs']
    e['own_pairs'] = s['pairs']
    e['audit'] = f'{R}\\{d}/summary.json'
    if extra:
        e['additional_banks'] = [{'bank': b, 'run': last(b)[0].split('-', 1)[1], 'pairs': last(b)[1]['pairs'],
                                  'summary': f'{R}\\{last(b)[0]}/summary.json'} for b in extra]


refresh('0x005FB800', 'DependenciesTask10', ('Game22', 'ScriptsFinal3', 'GroupPlacerPending'))
refresh('0x005FB8C0', 'DependenciesTask10')
refresh('0x005FB930', 'DependenciesTask10')
a['0x005FB800']['scope'] = ('AddGroupExe/InitExe/CleanUpForShutDownExe (__fastcall con el ECX/EDX de entrada) terminan en ResidueRegsCpp con los valores de EAX, ECX y EDX con que sale el EXE '
                            '(modelo del EDX que deja RemoveMember y Flush a partir del análisis del EXE, no del residuo de la DLL). Vacía el grupo como el EXE. El banco DependenciesTask10 ahora compara '
                            'EAX, ECX y EDX con semillas por caso y devuelve a los colaboradores sus bytes del EXE durante cada llamada. Límite: el modelo supone nulos los eventos y asignadores +0x34, +0x38, '
                            '+0x2C0 y +0x2C4 del grupo y la rama de miembro no jugador está deducida del EXE sin ejercerse en el banco. Verificado por contrib/audit_register_residue.py.'
                            + tail_of(a['0x005FB800']))
for addr in ('0x005FB8C0', '0x005FB930'):
    e = a[addr]
    e['scope'] = e['scope'].split(' Revalidación')[0] + ' Reproduce también EAX, ECX y EDX del EXE (ver AddGroup).' + tail_of(e)
refresh('0x00572800', 'DependenciesZones13')
e = a['0x00572800']
e['scope'] = ('Float puro con _ftol2 emulado con enteros (Zones13Ftol2, contrastado contra una simulación del ensamblador) y celda bajo float_control(precise); termina en ResidueRegsCpp con los EAX, ECX y EDX '
              'con que sale el EXE en los cuatro caminos (también el residuo de GetLevelFromPosition y FindPlayerCoors). El banco DependenciesZones13 ahora compara los registros con semillas por caso, '
              'desactiva durante la compuerta los hooks de las tres colaboradoras para ejecutar las del EXE e incorpora oráculos independientes. Verificado por contrib/audit_register_residue.py (toda salida pasa por el ayudante).'
              + tail_of(e))
refresh('0x006F5240', 'World11', ('Data23', 'WeaponFinal2', 'CTaskDead', 'PickWeap1', 'WeaponInstant355'))
e = a['0x006F5240']
e['scope'] = ('ExeAsinDegreesCpp con ExeAsinDoubleCpp reescrito para todas las ramas de _CIasin (0x821E70): |x|<1, ±1 exacto (π/2 de 64 bits), dominio (|x|>1 e infinitos: real indefinido y errno=EDOM) y NaN '
              '(propia x silenciada). Los fixtures de World11, Data23 y WeaponFinal2 se ampliaron con entradas ±1, ±1,5, ±2, ±1e30, NaN, ±inf, ±0 y subnormales sin tocar compuertas; los bancos de los demás usuarios '
              '(BulletInfo, Weapon, TaskSimpleDead) aprobaron. Además m_fFullAnimLean se multiplica con HandlingTimesFloatCpp (fmul x87): con los fixtures nuevos la multiplicación en SSE de la DLL descartaba '
              'subnormales que el EXE conserva (6 de 256 pares).' + tail_of(e))
for addr in ('0x005BD830', '0x006F5080', '0x006F5290'):
    refresh(addr, 'World11')
e = a['0x006F5290']
e['scope'] = e['scope'].split(' Revalidación')[0] + ' m_fFullAnimLean con HandlingTimesFloatCpp (fmul x87).' + tail_of(e)
json.dump(a, open(p, 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)

rp = pathlib.Path('reversing/CppThreeLevel4Study/README.md')
t = rp.read_text(encoding='utf-8')
banks = sorted({d.split('-')[0] for d in os.listdir(R)})
btab = '\n'.join(f'| {b} | {last(b)[1]["pairs"]} |' for b in banks)
i = t.index('| Banco | Pares |')
j = t.index('\n\n', i)
t = t[:i] + '| Banco | Pares |\n| --- | ---: |\n' + btab + t[j:]
rows = {q.name: hashlib.sha256(q.read_bytes()).hexdigest() for q in sorted(pathlib.Path('reversing/CppThreeLevel4Study/artifacts').glob('*.obj'))}
i = t.index('| Objeto | SHA256 |')
t = t[:i] + '| Objeto | SHA256 |\n| --- | --- |\n' + '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items())) + '\n'
a0 = t.index('## Qué se hizo')
a1 = t.index('## Bancos')
t = t[:a0] + '''## Qué se hizo

- `cHandlingDataMgr`: tres ayudantes nuevos de `X87Math` (`HandlingAtofTimes04Cpp`, `HandlingReduceVelocityCpp`, `HandlingSinScaledCpp`) y `ExeAsinDegreesCpp` para el `naked` del arcoseno.
- `PedGroups`: C++ plano con un núcleo común (`AddGroupCore`, `InitCore`, `CleanUpForShutDownCore`) y tres envolturas `__fastcall` (`AddGroupExe`, `InitExe`, `CleanUpForShutDownExe`) registradas con los mismos nombres de hook, que terminan en `ResidueRegsCpp` con los EAX, ECX y EDX del EXE.
- `TheZones`: float puro con `_ftol2` emulado con enteros y la celda bajo `float_control(precise)`; la función termina en `ResidueRegsCpp` con los registros del EXE en los cuatro caminos.

## Seguimiento: los límites que se cerraron (15/10/2026)

Tras la primera versión quedaban tres límites escritos; los tres se resolvieron.

- **`_CIasin` con |x| ≥ 1.** Se desensambló `_CIasin` entero (0x821E70) y `ExeAsinDoubleCpp` cubre ahora todas sus ramas: ±1 exacto devuelve π/2 de 64 bits, el dominio (|x| > 1 e infinitos) devuelve el real indefinido y pone `errno` a EDOM, y NaN devuelve la propia x silenciada. La versión antigua daba 90,0 en lugar de 89,99999 para ±1. Se amplió el generador de entradas de `World11`, `Data23` y `WeaponFinal2` con ±1, ±1,5, ±2, ±1e30, NaN, ±inf, ±0 y subnormales (sin quitar compuertas). Los fixtures nuevos destaparon una diferencia ya existente en `ConvertBikeDataToWorldUnits`: `m_fFullAnimLean * constante` se multiplicaba en SSE y descartaba subnormales que el EXE conserva (6 de 256 pares); se corrigió con `HandlingTimesFloatCpp` (fmul x87), también en `ConvertBikeDataToGameUnits`. `World11`, `Data23`, `WeaponFinal2`, `CTaskDead`, `PickWeap1` y `WeaponInstant355` aprueban.
- **Residuo de registros de `PedGroups` y `TheZones`.** Ahora se reproducen EAX, ECX y EDX del EXE. Los bancos `DependenciesTask10` y `DependenciesZones13` los comparan (con semillas por caso, devolviendo a los colaboradores sus bytes del EXE durante cada llamada y con oráculos independientes) y el auditor `contrib/audit_register_residue.py` incluye ambos objetos, exige que toda salida de `TheZones` pase por el ayudante, que `PedGroups` conserve al menos tres llamadas y que no haya `/JMC` en las envolturas; también admite el `nop` que el compilador sin optimizar deja tras una llamada de resultado descartado (no toca registros). Los dos bancos aprobaron con la comparación de registros activa.
- **Finales de línea.** No era un problema: el repositorio guarda LF en el índice (`eol=lf`) y el directorio de trabajo está en CRLF por `autocrlf`; Git normaliza al confirmar. Los tres archivos que pasé a LF no producen diferencias.

''' + t[a1:]
a0 = t.index('## Límites')
a1 = t.index('## Objetos')
t = t[:a0] + '''## Límites

- `PedGroups`: el modelo del EDX supone nulos los eventos y asignadores `+0x34`, `+0x38`, `+0x2C0` y `+0x2C4` del grupo (el banco los deja nulos) y la rama de `RemoveMember` para un miembro que no es jugador está deducida del EXE sin ejercerse en el banco; supone además que todo destructor de tarea termina en `CTask::operator delete` (0x61A5B0).
- `_CIasin`: las banderas de estado x87 pegajosas (IE y PE) y el flag 0xC9ABFC (sin escritores en el EXE, supuesto 0 por análisis estático) no se reproducen.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

''' + t[a1:]
t = t.replace('Ningún banco se relajó y los cinco aprobaron a la primera, sin tocar la instrumentación.',
              'Ningún banco se relajó. Los cinco bancos aprobaron a la primera; después se ampliaron las comprobaciones (registros y entradas del arcoseno) y se repitieron.')
rp.write_text(t, encoding='utf-8')
