import json, os, pathlib, hashlib

R = 'reversing/CppTwoLevel5Unified/runs'
S = 'reversing/CppTwoLevel5Study/README.md'


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


rp = 'Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp'
bridges = [('0x004D1680', 'SkinnedWithVelocityExtraction (2D, skinned)'), ('0x004D1A50', 'SkinnedWith3dVelocityExtraction'),
           ('0x004D1DB0', 'CompressedSkinned (3D)'), ('0x004D2100', 'NonSkinnedWithVelocityExtraction (2D)'),
           ('0x004D2450', 'NonSkinnedWith3dVelocityExtraction'), ('0x004D27F0', 'CompressedNonSkinned (3D)')]
for addr, name in bridges:
    entry(addr, f'RpAnimBlend: puente al EXE de FrameUpdateCallBack...{name}', 1, rp, 'RpAnim43',
          'La plantilla FrameUpdateCallBackExeVelocity<IsCompressed, IsSkinned, Is3D> reescribe en C++ el cuerpo del EXE (posición del ped asignada, loopedDeltaV con GetEndTranslation, producto escalar del cuaternión con NaN, bloque de z solo en skinned 2D, banderas IGNORE_ROOT_TRANSLATION y DONT_ADD_TO_PARTIAL_BLEND según la variante) y reutiliza NormaliseQuatExe y AnimQuaternionDotNegativeCpp. Los puentes solo corrían en la ruta candidata del banco; los envoltorios de producción siguen desactivados (DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS).')
entry('0x005DEF60', 'CPed::GetLocalDirection (__asm)', 1, 'Entity/Ped/Ped.cpp', 'CPedC',
      'PedLocalDirectionRawCpp: fpatan(-x, y) sin redondear, menos la rotación, más π/4, más 2π mientras sea negativo, por 2/π y _ftol2; el módulo 4 queda en enteros. El residuo de ECX/EDX de _ftol2 no se reproduce (el banco no los compara).')
entry('0x005DF910', 'CPed::SetPedPositionInCar (naked PedE_AtanNegXY)', 1, 'Entity/Ped/Ped.cpp', 'CPedE',
      'PedAtanNegXYDblCpp devuelve double (extern "C") y Ped.cpp lo enlaza como float con /alternatename: el fpatan queda sin redondear en ST0.')
entry('0x005DFDF0', 'CPed::PositionAttachedPed (naked PedE_AtanNegXYAdd)', 1, 'Entity/Ped/Ped.cpp', 'CPedE',
      'PedAtanNegXYAddDblCpp, igual que SetPedPositionInCar con la suma final a 24 bits.')
entry('0x005E1FA0', 'CPed::ProcessBuoyancy (bloque pow)', 1, 'Entity/Ped/Ped.cpp', 'CPedB',
      'PowScaleVector3Cpp(&copia, 0,9f, paso) y volcado de x e y siempre; z solo si z < 0 (con NaN no).')
entry('0x005E65A0', 'CPed::PreRenderAfterTest (oscilación)', 1, 'Entity/Ped/Ped.cpp', 'CPedB',
      '-TrigSimpleSinMulCpp(m_Wobble, 5).')
entry('0x005E5380', 'CPed::DoFootLanded (puentes EmitFootRainParticles 0x5E3630 y EmitFootDustParticles 0x5E37C0)', 2, 'Entity/Ped/Ped.cpp', 'CppPedFootEmitters1',
      'Los dos puentes que llamaban a funciones del EXE con argumentos en EBX/EDI/ESI/EAX se reescriben en C++ como funciones estáticas; el producto forward·1,5 usa X87TimesFloatCpp (fmul x87: el SSE de la DLL descartaba subnormales que el EXE conserva, 77 de 1536 pares del banco nuevo). Banco propio nuevo CppPedFootEmitters1 (EXE por dirección con adaptador de ensamblador dentro del banco frente a la estática de la DLL, trazas completas con argumentos en bits, 1536 pares por emisor). reversing/CPed (completion) valida DoFootLanded con tolerancia 1e-5 (compuerta histórica, no se tocó).', ('CPedCompletion',))
json.dump(a, open('reversing/CppOnlyMigration/approvals.json', 'w', encoding='utf-8', newline=''), ensure_ascii=False, indent=2)
print('funciones:', total)

D = pathlib.Path('reversing/CppTwoLevel5Study/artifacts')
rows = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(D.glob('*.obj'))}
obj = '\n'.join(f'| {n} | {h} |' for n, h in sorted(rows.items()))
banks = sorted({d.split('-')[0] for d in os.listdir(R)})
btab = '\n'.join(f'| {b} | {summ(b)[1]["pairs"]} |' for b in banks)
txt = f'''# Dos archivos de nivel 5 (15/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp` y `Entity/Ped/Ped.cpp` ({total} funciones fuente). Un subagente por archivo preparó el análisis; otro escribió un banco nuevo para los dos emisores de partículas de pisada; la compilación y los bancos corrieron en serie. Ningún banco se relajó.

## Qué se hizo

- `RpAnimBlend`: los seis puentes al EXE (`FrameUpdateCallBack*WithVelocityExtraction*`, que solo corrían en la ruta candidata del banco, porque los envoltorios de producción están desactivados) pasan a una plantilla C++ `FrameUpdateCallBackExeVelocity<IsCompressed, IsSkinned, Is3D>` reescrita desde el desensamblado de las seis funciones del EXE. El cuerpo C++ que había tras el `return` de los puentes no servía de base: asignaba la posición con `+=`, sumaba `t` en lugar de `GetEndTranslation`, restaba el cuaternión también donde el EXE siempre suma, y mezclaba banderas entre variantes.
- `Ped`: `GetLocalDirection`, los dos `naked` de `atan2` de `SetPedPositionInCar` y `PositionAttachedPed` (con `fpatan` sin redondear mediante ayudantes `double` y alias `/alternatename`), el bloque `pow` de `ProcessBuoyancy`, la oscilación de `PreRenderAfterTest` y los dos emisores de partículas de `DoFootLanded`, que eran puentes al EXE y se reescribieron en C++ (la parte menos verificada del lote: ver abajo).

## Qué se encontró

- Los emisores de pisada no tenían ningún banco que validara su cuerpo; el banco de `DoFootLanded` los contaba con falsos. Se derivó `reversing/CppPedFootEmitters1`. Su primera versión no compilaba (varios `auto` con tipos distintos en una declaración) y su comprobación previa exigía una dirección de DLL para `SurfaceInfos_c::ProducesFootDust`, que no está reconstruida (su símbolo es la propia dirección del EXE). Con el banco ya ejecutándose dio 77 de 1536 pares de diferencia en `EmitFootRainParticles`: `forward·1,5` se multiplicaba en SSE y descartaba subnormales que el EXE conserva; se corrigió con `X87TimesFloatCpp` (el ayudante antes llamado `HandlingTimesFloatCpp`, renombrado a un nombre genérico). Aprobó con 3072 pares.
- `CPedC`: `SetMoveAnim` (63 de 1024) por el patrón de siempre: la DLL llama directamente a `CPedGroups::IsInPlayersGroup` y `GetPedsGroup` y el banco solo los interceptaba por el EXE. Se añadieron los `EntryPatch` de la DLL.
- `reversing/CPed` (completion, 28672 pares): tres fallos de instrumentación del mismo tipo, ninguno causado por el código convertido: `CShadows::AddPermanentShadow` (el fake de sombras solo estaba en la dirección del EXE, falló en la entrada 5 de `DoFootLanded`), `CCarEnterExit::GetPositionToOpenCarDoor` (la DLL la llamaba directamente sobre un vehículo falso, el juego se cayó con acceso inválido en `PositionPedOutOfCollision`; se localizó resolviendo el volcado con el PDB de la sonda) y los emisores (contadores de la sonda, con los parches de `CompletionProbe.hpp`, `FootLandedComparison.hpp` y `build_completion_probe.py`). La sonda se reconstruye y su hash se actualiza en `run_completion.py`.

## Bancos

| Banco | Pares |
| --- | ---: |
{btab}

## Límites

- `reversing/CPed` compara con tolerancia absoluta de 1e-5 (compuerta histórica); `DoFootLanded` no se endureció a exacto. El banco nuevo de los emisores sí compara en bits.
- `RpAnimBlend`: las ramas de NaN no las ejercen los fixtures (cuaterniones normalizados, sin NaN en mezclas ni traslaciones) y el objeto usa SSE: subnormales en sumas o productos podrían diferir del x87; los rangos del banco son moderados.
- `GetLocalDirection` y las demás funciones de `Ped` no reproducen el residuo de ECX/EDX de `_ftol2` (los bancos no los comparan).
- El registro completo de `CPedCompletion` conserva solo el resumen (el log crudo se limpia); el hash del log está en `summary.json`.
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
{obj}
'''
pathlib.Path(S).write_text(txt, encoding='utf-8')
p = pathlib.Path('reversing/CppOnlyMigration/README.md')
t = p.read_text(encoding='utf-8')
ins = ("Se eliminó el ensamblador de `RpAnimBlend` y `Ped` (los seis puentes de velocidad y los emisores de partículas de pisada, con un banco nuevo propio); "
       "bancos sin diferencias ([detalle y hashes](../CppTwoLevel5Study/README.md)).\n\n")
i = t.index('Se eliminó el ensamblador de tres archivos más (`cHandlingDataMgr`')
j = t.index('\n\n', i) + 2
p.write_text(t[:j] + ins + t[j:], encoding='utf-8')
