<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 09/11/2026 (Ciudad de México):** 870 grupos / 9498 funciones reconstruidas; 9498 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn85F: `CTaskSimpleFight::FindTargetOnGround` (lote 85F)

Estado: **APROBADO dentro del juego** (02/11/2026, árbol principal sobre HEAD 2472c1b60 + los cambios del lote). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/restauración en ambos lados, y cada lado coincide con el oráculo independiente (`fixture_oracle.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn85f/20261102-163218-315517` (archivada en `reversing/CppTwelveLevel6Final/runs/`). Una función de código de juego reconstruida en C++ puro y enganchada (tipo S).

| Función | Dirección | Hook | Fuente |
|---|---|---|---|
| `CTaskSimpleFight::FindTargetOnGround` (antes el envoltorio `FightFindTargetOnGroundExe`) | 0x61C9B0 | `CTaskSimpleFight` / `FindTargetOnGround` / S | `TaskSimpleFight.cpp` |

El envoltorio desaparece: `MakeAbortable` y `ProcessPed` llaman al método (`void FindTargetOnGround(CPed*)`, thiscall, `ret 4`). El nombre viene de la cabecera original; la función reparte en realidad el peso de las cuatro animaciones de movimiento de pelea (0x29..0x2C).

## Revisión contra el EXE (0x61C9B0..0x61CB93, `ret 4`)
- Pide con `RpAnimBlendClumpGetAssociation(clump, id)` (0x4D68B0, cdecl) las asociaciones 0x29, 0x2A, 0x2B y 0x2C; el clump (+0x18 del ped) se relee en cada llamada.
- Después lee el byte +0x26 (siguiente) y el +0x24 (combo) de la tarea: si ambos son cero, parada.
- Si no, lee x (+0xC) e y (+0x10) de los datos del jugador (+0x480 del ped, leídos DESPUÉS de las llamadas) y calcula con la x87 a 24 bits la longitud `sqrt(x*x + y*y)`. **`fcomp` + `test ah,5` + `jp` salta a movimiento con longitud mayor o IGUAL a 0.1f o con NaN**: solo una longitud menor de 0.1f va a parada. (La primera versión comparaba con `<=` y el banco lo detectó en los 109 casos de longitud exactamente 0.1f.)
- Parada: cada asociación existente recibe `+0x1C = -8.0f` (en el orden 0x29, 0x2A, 0x2B, 0x2C) y combo, último comando (+0x27) y siguiente quedan a cero.
- Movimiento: `k = 1 / (|y| + |x|)` (24 bits), `fx = float(k*x)`, `fy = float(k*y)`. Eje x: `fx > 0` anula la 0x2A y pesa la 0x2C; `fx < 0` anula la 0x2C y pesa la 0x2A con `-fx`; cero o NaN no toca nada. Eje y: `fy < 0` anula la 0x2B y pesa la 0x29 con `-fy`; `fy > 0` anula la 0x29 y pesa la 0x2B. La asociación que falta se crea con `AddAnimation(clump, 0, id)` (0x4D3AA0). Al final combo = 1, último = siguiente (releído) y siguiente = 0.

## Banco
Falsos idénticos en la dirección del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco): `RpAnimBlendClumpGetAssociation` 0x4D68B0 y `CAnimManager::AddAnimation` 0x4D3AA0. Registran sus argumentos en orden y devuelven asociaciones de arenas propias (las cuatro existentes o nulas según el caso, y las nuevas de `AddAnimation`). Todo lo demás (cuadrados, suma, raíz, cociente, productos, guardado a float) corre de verdad en ambos lados.
Mutaciones de una lectura (una vez por sitio): tras la asociación 0x29 cambian el comando siguiente y el combo; tras la 0x2C los datos del jugador pasan a otros o sus floats cambian; tras la primera `AddAnimation` cambia el clump (+0x18, se relee en cada llamada) o los floats de los datos (no se releen). Familias de 1024 casos (16): sin gate, movimiento menor, umbral, cuatro cuadrantes, ejes nulos, especiales, enormes, desnormales, relaciones simples, mutaciones, las 16 combinaciones de asociaciones y mezcla. `coverage.json` = 80 % de lo medido (mínimo 100) sobre 39 ramas del modelo.

## Fuera de los fixtures
NaN con cargas distintas de 0xFFC00000, ped sin datos de jugador (+0x480 nulo), `AddAnimation` que devuelva nulo y asociaciones con basura en campos distintos de los dos escritos (+0x18, +0x1C).

## Prueba negativa
Producción mutada en seis puntos: retardo de parada -8 a -4, `Weight(2, fy, 0)` a `Weight(2, fy, 1)`, signo de `Weight(1, -fx, 3)`, umbral 0x3DCCCCCD a 0x3DCCCCCE, `m_nLastCommand = 0` en lugar de copiar el siguiente, e id de animación `0x29 + index` a `0x2A + index`. El banco falla en 15455 de 16384 pares, sin caída ni accesos inválidos (`resultados-fn85f/20261102-162918-981572`). Restaurado desde copia y comprobado con `cmp` y con el hash (`be2829a6…`) antes de la corrida final.

## Bancos repetidos
`FightAbort31` (`MakeAbortable` llama al método; falseaba 0x61C9B0 solo en la dirección del EXE): adaptado, ahora lleva el falso también en el cuerpo de la DLL; **APROBADO**, 4608 pares (`resultados-fightabort31/20261102-163623-439374`).
`FightProcessPed31` (`ProcessPed`, mismo caso): adaptado igual; **APROBADO**, 4608 pares (`resultados-fightprocessped31/20261102-163810-246266`).
`FightMeleeData31` solo cita la dirección en un comentario de su `run_local.py`. Ningún otro banco cita la dirección ni el símbolo.
