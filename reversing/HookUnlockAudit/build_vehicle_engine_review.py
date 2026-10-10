"""Inventario estático: no constituye prueba de desbloqueo."""
import csv
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = Path(__file__).resolve().parent
FILES = {
    'CCollision': 'Collision/Collision.cpp', 'CBike': 'Entity/Vehicle/Bike.cpp',
    'CTrain': 'Entity/Vehicle/Train.cpp', 'CPlane': 'Entity/Vehicle/Plane.cpp',
    'CMonsterTruck': 'Entity/Vehicle/MonsterTruck.cpp', 'CRunningScript': 'Scripts/RunningScript.cpp',
    'CGenericGameStorage': 'GenericGameStorage.cpp', 'CProjectileInfo': 'ProjectileInfo.cpp',
    'CClothesBuilder': 'ClothesBuilder.cpp', 'CWeapon': 'Weapon.cpp',
    'CCarEnterExit': 'CarEnterExit.cpp', 'FxEmitterBP_c': 'Fx/FxEmitterBP.cpp',
}
RISKS = {
    'CCollision': 'buffers temporales, cache de planos, colmodels y llamadas anidadas de collision',
    'CBike': 'layout CBike/CVehicle, suspensiones, referencias ped y audio, despacho virtual híbrido',
    'CTrain': 'nodos de pista, lista enlazada de vagones, pools y referencias pasajeros',
    'CPlane': 'layout CPlane/CAutomobile, tren de aterrizaje, controles, streaming y FX',
    'CMonsterTruck': 'layout heredado, arrays de ruedas y suspensión, despacho virtual a CAutomobile',
    'CRunningScript': 'IP bytecode, parámetros globales, listas de scripts, tareas y referencias ped',
    'CGenericGameStorage': 'handle de fichero, buffer global, offsets/checksum y estado de carga/guardado',
    'CProjectileInfo': 'pool de proyectiles, referencias entidades y ownership de FX',
    'CClothesBuilder': 'refcounts RenderWare, geometría/skin, buffers globales y ownership de texturas',
    'CWeapon': 'estado ped/cámara, daño, RNG, raycast, referencias y creación de FX',
    'CCarEnterExit': 'tareas y asociaciones anim, puertas/pasajeros, referencias; ABI de llamadores EXE',
    'FxEmitterBP_c': 'listas partículas, manager/pool FX, estructuras RenderWare y estado render',
}
rows = [r for r in csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')) if r['class'] in FILES and r['locked']=='1']
registry = json.loads((ROOT/'reversing/validation-registry.json').read_text(encoding='utf-8'))['functions']
evidence = {(r['hook']['class'],r['hook']['fn_name'],r['hook']['type']):r for r in registry}
callers = defaultdict(list)
for c in csv.DictReader((ROOT/'docs/MemoryMap/binary_calls.csv').open(encoding='utf-8-sig')):
    if c['target_va']:
        callers[int(c['target_va'],16)].append(c)
results=[]
for row in rows:
    cls, name = row['class'], row['fn_name']
    src = ROOT/'source/game_sa'/FILES[cls]
    lines = src.read_text(encoding='utf-8-sig').splitlines()
    base = name.split('-')[0]
    matches = [(i+1,line.strip()) for i,line in enumerate(lines) if 'RH_Scoped' in line and not line.lstrip().startswith('//') and (re.search(r'\(\s*'+re.escape(base)+r'\s*[,<]',line) or '"'+name+'"' in line)]
    if len(matches)>1 and '-' in name:
        suffix=name.rsplit('-',1)[1]
        matches=[m for m in matches if '"'+suffix+'"' in m[1]]
    if len(matches)!=1:
        raise RuntimeError((cls,name,matches))
    lineno, registration=matches[0]
    addresses=re.findall(r'0x[0-9a-fA-F]+',registration)
    va=int(addresses[0],16)
    ev=evidence.get((cls,name,row['type']),{})
    ep=ev.get('evidence',{}).get('path','')
    bench=ROOT/Path(ep).parent/'LiveComparison.hpp'
    forced_unlock=bench.exists() and 'LockState(false)' in bench.read_text(encoding='utf-8-sig')
    priority='baja: dependencia de subsistema'
    if name in {'GetCurrentVersionNumber','GetProjectileInfo','IsNextStationAllowed','FindEngine','FindClosestTrackNode','FindMaximumSpeedToStopAtStations','FindNextStationPositionInDirection','PreprocessClothesDesc','Test2DLineAgainst2DLine'}:
        priority='primera tanda: fixture sin mundo real, aún requiere runtime'
    plan='Objeto/estado válido aislado; capturar estado; ON→OFF→ON por API normal y comprobar bytes/vtable; llamar EXE y DLL con vecinos ON y luego vecinos OFF; comparar retorno, estado y trazas; restaurar bytes, lock, globals/pools y ficheros; vigilar crash/hang durante frames posteriores.'
    if row['type']=='V':
        plan+=' Repetir despacho por slot EXE y vtable DLL con instancia de la clase y base; comprobar alias heredados y ausencia de bucles.'
    if cls=='CGenericGameStorage':
        plan+=' IO sólo doble de fichero/archivo temporal del banco: nunca saves del usuario; comparar cambios buffer/checksum y cierre de handles.'
    if cls=='CRunningScript':
        plan+=' Bytecode y scripts aislados; verificar IP/compareflag, scripts en listas y tareas antes/después.'
    if cls in {'CProjectileInfo','CWeapon','FxEmitterBP_c'}:
        plan+=' Aislar audio/render/FX, mantener referencias entre ambas transiciones y verificar allocator/cleanup.'
    results.append(dict(class_name=cls,fn_name=name,catalog_address=row['address'],resolved_exe_address=f'0x{va:08X}',hook_type=row['type'],catalog_locked=1,source_registration=f'{src.relative_to(ROOT).as_posix()}:{lineno}',registration=registration,source_lock='explícito' if '.locked' in registration else 'no explícito; reversed=true y locked=false por defecto',lock_reason='No hay justificación ni lock explícito en registro actual; CSV histórico debe contrastarse con inventario runtime',risk=RISKS[cls],direct_exe_calls=len(callers[va]),first_exe_call=callers[va][0]['instruction_va'] if callers[va] else '',historical_evidence=ep,historical_pairs=ev.get('evidence',{}).get('pairs',''),historical_scope=ev.get('scope',''),historical_bench_forces_unlock=forced_unlock,priority=priority,runtime_plan=plan,verdict='SIN PRUEBA NUEVA; no autoriza eliminar lock ni sobrescribir catálogo'))
with (OUT/'review-vehicle-engine.csv').open('w',encoding='utf-8',newline='') as f:
    w=csv.DictWriter(f,fieldnames=list(results[0]));w.writeheader();w.writerows(results)
counts=Counter(r['class_name'] for r in results)
body=['# Revisión de locks: vehículos, scripts, colisión y subsistemas', '',
      'Fecha: 10/10/2026. Alcance: las 178 filas `locked=1` catalogadas para estas 12 clases. No se ha ejecutado el juego, compilado, alterado una fuente ni retirado un lock en esta revisión.', '',
      '**Hallazgo:** los registros actuales de estas 178 funciones carecen de `.locked=true`. `HookInstallOptions` en `source/reversiblehooks/ReversibleHooks.h:103-106` inicializa `reversed=true`, `enabled=true`, `locked=!reversed` (false). Los comentarios generados y `docs/hooks.csv` son históricos; no demuestran que exista actualmente ese bloqueo dentro del juego. Hay que obtener el inventario runtime antes de cambiar un solo flag del catálogo.', '',
      'El CSV adyacente contiene una fila por hook: registro exacto y línea, dirección EXE resuelta incluso para virtuales, evidencia histórica, número de callsites directos del mapa binario, riesgos y plan de conmutación. Los callsites son del export existente; no incluyen despacho indirecto y no constituyen una revisión exhaustiva de ABI.', '',
      '| Clase | Filas catalogadas bloqueadas | Riesgo específico |', '|---|---:|---|']
body += [f'| {c} | {n} | {RISKS[c]} |' for c,n in counts.items()]
body += ['', '## Prueba requerida para cada fila', '',
         '1. Leer `Locked()`/`Hooked()` del hook real, guardar bytes originales, dirección de cuerpo DLL antes de parchear y slot virtual. Registrar resultado de la API normal, sin forzar `LockState(false)` para declarar éxito.',
         '2. Dentro del juego completo y con estado válido aislado, ejecutar ON→OFF→ON y ambas rutas de llamada. Combinar con dependencias activas y desactivadas; ningún patch del banco debe ocultar el hook auditado ni cambiar un cuerpo ya redirigido.',
         '3. Verificar retorno/estado/traza por par, bytes/vtable tras cada transición, estado restaurado, no crash/hang y ningún resultado nulo. Incluir frames de continuación para recursos persistentes. Una comparación antigua no demuestra estas transiciones ni el estado híbrido con callers EXE.',
         '4. Si el runtime ya informa lock=false, registrar discrepancia documental y la conmutación real; si informa lock=true, conservarlo hasta prueba específica segura. Todo fallo permanece visible en la tabla.', '',
         '## Precauciones concretas', '',
         '- `CCarEnterExit.cpp:35-51` conserva EDX mediante `ComputePassengerIndexFromCarDoor_KeepEdx`: el comentario documenta `SetPedInCarDirect` EXE en `0x650356` y `0x650360 push edx`. El flujo híbrido necesita probar puerta/pasajero; no retirar el wrapper.',
         '- `FxEmitterBP_c::CreateInstance` tiene comentario `// bad` en el registro (`0x4A2B40`): señal histórica sin explicación técnica, exige recrear/actualizar/liberar la instancia entre rutas. No equivale a lock activo.',
         '- Virtuales Bike/Train/Plane/Monster: mantener las direcciones y counts de vtable actuales; verificar que los cuerpos derivados no comparten target con hook heredado. `Bike::ResetSuspension` está registrado como simple, por lo que hay que comprobar también llamadores virtuales/heredados.',
         '- Storage usa estado de fichero/guardado; dobles aislados cubren equivalencia pero no prueban una partida cargada completa ni interoperabilidad de saves. No introducir pruebas destructivas sobre archivos reales.',
         '- Scripts: el IP y los parámetros compartidos sobreviven a llamadas híbridas; el sistema de command handlers personalizado puede esconder diferencias si se deja un vecino stubbed.',
         '- Bancos `ProjectileCore355`, `StorageCore355` y `TrainQueries355` llaman `LockState(false)` y restauran el flag después: su evidencia no prueba que la API normal permita conmutar. El CSV indica individualmente si el banco actual vinculado contiene esa instrucción.',
         '- FX/clothes: comparar ownership/refcounts y cleanup además del resultado; no desactivar funciones de render sobre texturas/cámaras falsas sin doubles adecuados.', '',
         '## Candidatos', '',
         'Priorizar consultas de tren, `CProjectileInfo::GetProjectileInfo`, `CGenericGameStorage::GetCurrentVersionNumber` y `CClothesBuilder::PreprocessClothesDesc` usando sus fixtures históricos. Son prioridades de experimentación, no funciones demostradas seguras. Procesamiento de mundo, render, creación/destrucción y IO requieren pruebas propias del subsistema. El resto queda especificado individualmente en CSV.', '']
(OUT/'review-vehicle-engine.md').write_text('\n'.join(body),encoding='utf-8')
print(json.dumps({'rows':len(results),'counts':counts,'explicit_locks':sum('.locked' in r['registration'] for r in results)},ensure_ascii=False))
