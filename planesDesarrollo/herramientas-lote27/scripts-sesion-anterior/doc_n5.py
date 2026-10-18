import json, os, pathlib, hashlib

R = 'reversing/CppFiveLevel2Unified/runs'
S = 'reversing/CppFiveLevel2Study/README.md'


def summ(bank):
    d = sorted(x for x in os.listdir(R) if x.startswith(bank + '-'))[-1]
    s = json.load(open(f'{R}/{d}/summary.json', encoding='utf-8'))
    assert s['passed'], bank
    return d, s


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


entry('0x00719670', 'CFont::GetScriptLetterSize (naked auxiliar ScriptSubFontNativeAbi)', 1, 'Font.cpp', 'DependenciesWorldFxFont5',
      'C++ plano: lectura por puntero crudo de IntroTextLines y gFontData como el EXE, suma entera y un solo producto float.')
entry('0x00683340', 'CTaskComplexWaitForPed::Constructor y TaskCtorRegisterReference', 2, 'Tasks/TaskTypes/TaskComplexWaitForPed.cpp', 'DependenciesTaskCtor4',
      'Marco SEH manual en fs:[0], ResidueRegsCpp y reproducción del EDX que deja 0x571B70 (tiempo con byte bajo = booleano si no inserta referencia; cabeza anterior de la lista si inserta). Límite: si 0x571B70 agota las tres listas de libres sin referencia, el EDX del EXE (resto de sus bucles de poda) no se reproduce; el llamador del EXE no lo lee y el banco no lo ejerce. Verificado por contrib/audit_register_residue.py.')
entry('0x00711EF0', 'CBirds::CreateNumberOfBirds y Update (BirdsAtan2, BirdsAddAtan2, BirdsRotate)', 3, 'Birds.cpp', 'CppTrigCompound6Birds',
      'TrigAtan2Cpp, BirdsAddAtan2Cpp y BirdsRotateCpp (seno y coseno sin redondear, un redondeo por producto, x e y leídos antes de escribir).', ('CppTrigBirdsRender',))
entry('0x00441DB0', 'CPlaceable::GetHeading y GetRoll (naked), y PlaceableHeadingFromForward', 3, 'Entity/Placeable.cpp', 'World16',
      'PlaceableHeadingDblCpp y PlaceableRollDblCpp devuelven double con el fpatan sin redondear en ST0; Placeable.cpp los enlaza con /alternatename como float y se compila con /O2 /Oi /arch:IA32 /fp:fast (source/CMakeLists.txt) para devolver ST0 sin pasar por memoria. Comprobado en el objeto: call y ret sin fstp/fld.')
entry('0x0059B390', 'CMatrix::RotateZ en CTaskSimpleHoldEntity::SetPedPosition y atan2 de ProcessPed (HoldAtan2X87, HoldHeadingMinusX87, HoldRotateZ_59B390)', 3, 'Tasks/TaskTypes/TaskSimpleHoldEntity.cpp', 'CTasks14b',
      'TrigAtan2Cpp, Atan2MinusCpp y HoldEntityRotateZCpp (coseno sin redondear, seno guardado como float, un redondeo por operación).')
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)
print('funciones:', total)

D = pathlib.Path('reversing/CppFiveLevel2Study/artifacts')
rows = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(D.glob('*.obj'))}
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items()))
banks = sorted({d.split('-')[0] for d in os.listdir(R)})
btab = '\n'.join(f'| {b} | {summ(b)[1]["pairs"]} |' for b in banks)
txt = f'''# Cinco archivos de nivel 2 más (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `Font.cpp`, `Tasks/TaskTypes/TaskComplexWaitForPed.cpp`, `Birds.cpp`, `Entity/Placeable.cpp` y `Tasks/TaskTypes/TaskSimpleHoldEntity.cpp` ({total} funciones fuente). Un subagente por archivo preparó el análisis; la compilación y los bancos corrieron en serie. Ningún banco se relajó.

## Qué se hizo

- `Font`, `Birds` y `TaskSimpleHoldEntity`: C++ plano o ayudantes nuevos de `X87Math` (`BirdsAddAtan2Cpp`, `BirdsRotateCpp`, `HoldEntityRotateZCpp`) y los ya existentes (`TrigAtan2Cpp`, `Atan2MinusCpp`).
- `Placeable`: `GetHeading` y `GetRoll` devuelven el `fpatan` sin redondear en ST0 (World16 compara 80 bits). Los ayudantes `PlaceableHeadingDblCpp` y `PlaceableRollDblCpp` devuelven `double` y se enlazan desde `Placeable.cpp` como `float` con `/alternatename`; el archivo pasa a compilarse con `/O2 /Oi /arch:IA32 /fp:fast` (como `ConversationNode.cpp`) y el objeto muestra solo `call` y `ret`. Esto afecta a todas las funciones del archivo y World16 completo las revalidó.
- `TaskComplexWaitForPed::Constructor`: marco SEH manual, `ResidueRegsCpp` y reproducción del EDX que deja `CEntity::RegisterReference` (0x571B70); `contrib/audit_register_residue.py` incluye ahora el objeto.

## Qué se encontró

- `DependenciesTaskCtor4` falló la primera vez (512 de 2048 pares de `CTaskComplexWaitForPed::Constructor`, solo el EDX): el constructor llama ahora al `RegisterReference` en C++, que deja otro EDX que la rutina del EXE. Se reprodujo el EDX de las dos salidas de 0x571B70 sin tocar el banco; repetido, aprobó con 4096 pares. La corrida fallida se conserva en `reversing/CppFiveLevel2Unified/runs`.
- Al guardar los objetos «antes» sobrescribí por error `X87Math.actual-before.obj` de la carpeta ignorada `reversing/CppFiveFilesStudy/artifacts` con el de este lote; es evidencia local no versionada y no se puede recuperar.

## Bancos (todos con cero diferencias)

| Banco | Pares |
| --- | ---: |
{btab}

## Límites

- Si `RegisterReference` agota las tres listas de libres sin conseguir una referencia, el EDX del EXE (restos de sus bucles de poda) no se reproduce; el llamador del EXE no lo lee y el banco no lo ejerce.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')
p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de otros cinco archivos de nivel 2 (`Font`, `TaskComplexWaitForPed`, `Birds`, `Placeable` y `TaskSimpleHoldEntity`); "
       "seis bancos sin diferencias ([detalle y hashes](../CppFiveLevel2Study/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de diecinueve archivos de nivel 2')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
