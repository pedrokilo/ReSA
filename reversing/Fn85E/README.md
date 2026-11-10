<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 879 grupos / 9839 funciones reconstruidas; 9839 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn85E: `CTaskSimpleFight::GetAvailableComboSet` (lote 85E)

Estado: **APROBADO dentro del juego** (02/11/2026, árbol principal sobre HEAD b90f378d9..47d74b044 + los cambios del lote). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/restauración en ambos lados, y cada lado coincide con el oráculo independiente (`fixture_oracle.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn85e/20261102-160333-556082` (archivada en `reversing/CppTwelveLevel6Final/runs/`). Una función de código de juego reconstruida en C++ puro y enganchada (tipo S).

| Función | Dirección | Hook | Fuente |
|---|---|---|---|
| `CTaskSimpleFight::GetAvailableComboSet` (antes el envoltorio `FightGetAvailableComboSetExe`) | 0x61C7F0 | `CTaskSimpleFight` / `GetAvailableComboSet` / S | `TaskSimpleFight.cpp` |

El envoltorio desaparece: `ProcessPed` llama al método (`int8 GetAvailableComboSet(CPed*, int32 command)`; el EXE solo lee el byte bajo del comando, con signo). No se ha tocado `FightTargetInReachExe` (0x61D6F0): es mucho mayor (más de 0x200 B, x87 y unos 15 colaboradores) y queda para un lote propio.

## Revisión contra el EXE (0x61C7F0..0x61C99F, `ret 8`, AL)
- Comando negativo: con el grupo actual (+0xC) distinto de 0x21 y sin referencia (+0xA == 0) toma el bloque del grupo (puntero de `ms_aAnimAssocGroups`; si es nulo, `GetAnimBlockName` + `GetAnimationBlock`); si `IsLoaded` es distinto de cero, `AddAnimBlockRef(indice)` y +0xA = 1. Devuelve siempre 0.
- Comando que no es 0, 2 ni 0xB..0xE: devuelve 0.
- Si no: `GetWeaponInfo(tipo del arma de la ranura activa, 1)`; estilo = byte +0x6E (con signo). Con comando 0xC, o con estilo 4 y comando 0 o 2, el estilo es el de pelea del ped (+0x72D), que el EXE lee DESPUÉS de `GetWeaponInfo`. Con comando 0 y sin el bit 2 del byte alto de las banderas (+0x85) de la fila (estilo − 4, sin acotar) devuelve 4.
- Fila: grupo 0x21 devuelve el estilo; si el grupo coincide con +0xC y hay referencia devuelve el estilo; si no, con referencia `RemoveAnimBlockRef` (índice `(puntero del bloque del grupo actual − ms_aAnimBlocks) >> 5`, también con puntero nulo), +0xA = 0 y +0xC = grupo de la fila (RELEÍDO tras la llamada). Luego, con el grupo +0xC: bloque cargado (cualquier byte ≠ 0) → `AddAnimBlockRef`, +0xA = 1, devuelve el estilo; si no, `RequestModel(0x63E7 + indice, 8)`, un comando siguiente (+0x26, con signo) en 0xB..0xE pasa a 0xB (RELEÍDO tras `RequestModel`) y devuelve 4.

## Banco
Falsos idénticos en la dirección del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco): `CWeaponInfo::GetWeaponInfo` 0x743C60, `CAnimManager::GetAnimBlockName` 0x4D3A30, `GetAnimationBlock(const char*)` 0x4D3940, `AddAnimBlockRef` 0x4D3FB0 y `CStreaming::RequestModel` 0x4087E0 (todos con hook) y, sin hook RH (función C++ de la DLL), `CAnimManager::RemoveAnimBlockRef` 0x4D3FD0 (falso también en su cuerpo). Registran sus argumentos en orden.
Los bloques de animación son los reales de `ms_aAnimBlocks` (0..15; solo se escribe su byte `IsLoaded`), el puntero `ms_aAnimAssocGroups` (0xB4EA34) apunta a una arena de 48 grupos y las 12 filas de la tabla de combos (0xC170D0) se fijan por caso (grupo +0, banderas altas +0x85); todo se restaura tras cada lado.
Mutaciones de una lectura (una sola vez por sitio): tras `RemoveAnimBlockRef` cambia el grupo de todas las filas, tras `RequestModel` cambia el comando siguiente y dentro de `GetWeaponInfo` cambia el estilo de pelea del ped. Familias de 1024 casos (16); cobertura de ramas medida sobre la entrada y el modelo del oráculo (`coverage.json` = 80 % de lo medido, mínimo 100).

## Fuera de los fixtures
Estilo del arma o de pelea fuera de 4..15 (la fila quedaría fuera de la tabla), grupos de animación fuera de 0..39, bloques fuera de 0..15, ranura activa fuera de −3..12, ped o tarea nulos y bytes de `IsLoaded` distintos de 0/1 solo con 2, 0x80 y 0xFF (el EXE compara distinto de cero). No hay floats en la función.

## Prueba negativa
Producción mutada en cinco puntos: comando 0xC → 0xD en el origen del estilo, bit 2 → bit 1 en la bandera de la fila, `<= 0xE` → `<= 0xD` en el comando siguiente, y `return style` → `return 4` tanto con el grupo 0x21 como con el bloque ya cargado. El banco falla en 5926 de 16384 pares, sin caída ni accesos inválidos (`resultados-fn85e/20261102-160122-970080`). Restaurado desde copia y comprobado con `cmp` y con el hash (`cd874a05…`) antes de la corrida final.

## Bancos repetidos
Solo `FightProcessPed31` (llama al método desde `ProcessPed`; falseaba 0x61C7F0 solo en la dirección del EXE): adaptado, ahora parchea también el cuerpo de la DLL de `GetAvailableComboSet` (`pAvail` con `Body(&CTaskSimpleFight::GetAvailableComboSet)` y 0x61C7F0 fuera de `EXE_ONLY`); **APROBADO**, 4608 pares (`resultados-fightprocessped31/20261102-160555-377395`). Ningún otro banco cita la dirección ni el símbolo.
