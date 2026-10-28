<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlayerCrosshair74: CPlayerInfo::ProcessCrosshair (0x56EC80)

Estado: **APROBADO dentro del juego (27/10/2026, worktree `D:\Original\wy-4`, HEAD f13a750cf + `source.patch`). Evidencia final: ver el final de este archivo. Plantillas: `reversing/HeatFire73` (lote 73), `reversing/Helpers66A`. Contrato: `reversing/_handoff/BRIEF.md`.**

| Función | Dirección | Hook (clase / nombre / tipo) | Colaboradores sustituidos |
|---|---|---|---|
| `CPlayerInfo::ProcessCrosshair(uint32 playerIndex, CPad* pad)` (helper sin símbolo 0x56EC80; antes una llamada por dirección desde `CPlayerInfo::Process`) | 0x56EC80 | `CPlayerInfo` / `ProcessCrosshair` / S (`RH_ScopedNamedGlobalInstall(ProcessCrosshair_Hook, ...)`, envoltura `__fastcall` con `this = &m_nCrosshairActivated`) | `CPad::GetSteeringLeftRight` 0x53FB80, `CPad::GetSteeringUpDown` 0x53FBD0, `CPad::GetCarGunFired` 0x53FF90, `CamShakeNoPos` 0x50A970, `CCamera::FindCamFOV` 0x50AD20, `CWeapon::FireInstantHit` 0x73FB10 (todos con hook RH, apagado durante el banco; falso idéntico en la entrada del EXE y en el cuerpo de la DLL) |

Quién valida a cada colaborador (`docs/hooks.csv`, `reversing/validation-registry.json`): `CPad::GetSteeringLeftRight/GetSteeringUpDown/GetCarGunFired` -> `reversing/RequestedPadSelectors` (3876 pares); `CamShakeNoPos` y `CCamera::FindCamFOV` -> `reversing/CameraFinal4`;
`CWeapon::FireInstantHit` -> `reversing/WeaponInstant355` (20480 pares); `CWeapon::CWeapon` (0x73B430), que corre de verdad en ambos lados (el EXE entra por su hook a la DLL), -> `reversing/PickWeap1`.

Fila de `docs/hooks.csv` (la añade el coordinador): `CPlayerInfo,ProcessCrosshair,0x0056EC80,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh PlayerCrosshair74        # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/PlayerCrosshair74/build_probe.py
python -B reversing/PlayerCrosshair74/run_local.py
python -B reversing/PlayerCrosshair74/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aun no existe o no esta completo
python -B reversing/PlayerCrosshair74/fixture_oracle.py <carpetaEvidencia>/comparisons.jsonl          # oraculo independiente (Python): 0 lados distintos del modelo
python -B reversing/PlayerCrosshair74/nan_rules_hw.py        # microtest de hardware x87 de Cross74*Cpp (1,8 millones de combinaciones; imprime «OK»)
python -B reversing/PlayerCrosshair74/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-playercrosshair74/<corrida>"   # sólo lo ejecuta el coordinador
```
Archivo de arranque en el clon: `playercrosshair74-runtime.start`; salida `playercrosshair74-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de esta función respecto a HEAD)

- `PlayerInfo.cpp/.h`: nuevo `CPlayerInfo::ProcessCrosshair(uint32, CPad*)` (C++ puro) + envoltura `ProcessCrosshair_Hook` y `RH_ScopedNamedGlobalInstall`; `Process` la llama directamente (antes `reinterpret_cast<void(__thiscall*)(uint32*, uint32, CPad*)>(0x56EC80)`).
- `X87Math.cpp/.h` (bloque «Lote 74» al final, prefijo `Cross74`): `Cross74StepCpp` (paso de la palanca: `fild`, `fmul`, `fmul`, `fadd/fsubr`, `fstp`, con las reglas de NaN de la x87) y `Cross74AimCpp` (dirección del disparo: `fptan` consumido sin redondear, cadena de `fsub`/`fmul` con guardados a float en los mismos puntos que el EXE).

## Revisión contra el EXE (re-desensamblado y contrastado)

**0x56EC80 (ProcessCrosshair).** thiscall `ret 8`, `ecx = &m_nCrosshairActivated`, args `(playerIndex, pad)`. Si el byte bajo del DWORD es 0 sale sin tocar nada. `x += float(GetSteeringLeftRight()) * ms_fTimeStep * 0.00033333333f` y `y = (bInvertLook4Pad != 0) ? y + ... : y - ...` (`fsubr`);
cada producto y la suma a 24 bits con exponente extendido; el paso se lee ANTES de llamar al mando. Recorte ordenado a +-0.9f (x alto, x bajo, y alto, y bajo; con NaN no recorta). `GetCarGunFired() == 0` (AX con signo, la mitad alta es basura) sale. Si no: `CamShakeNoPos(&TheCamera, 0.2f)`, `FindCamFOV()` dos veces
(la segunda para la componente horizontal), objetivo = posición de la matriz de la cámara (0xB6F9CC) + 200 * (delante - derecha * x * tan(fov2 * pi/360) - arriba * (tan(fov1 * pi/360) / aspecto) * y) con los guardados a float del EXE; `CWeapon(31, 5000)` en la pila; origen = posición de la cámara copiada por palabras
(matriz o posición compacta); se apaga el bit 25 del DWORD +0x470 del ped del jugador durante `FireInstantHit(ped, &origen, &origen, nullptr, &objetivo, nullptr, true, true)` y se restaura (el DWORD se relee tras la llamada; el ped se relee de `CWorld::Players[i]`). Sin valor de retorno; el único llamador (`CPlayerInfo::Process`) no lee EAX/ECX/EDX.

## Hallazgos (discrepancias halladas y corregidas durante el banco)

1. **`muzzlePosn` de `FireInstantHit` es el mismo puntero que `origin`.** La primera lectura del desensamblado (`lea edx,[esp+48h]` y `lea eax,[esp+4Ch]` con distinto ESP) daba dos direcciones distintas; el banco (13916 de 20480 pares con diferencias en el argumento) mostró que el EXE pasa la copia local del origen en ambos; corregido en `PlayerInfo.cpp`.
2. **Defecto del oráculo (no de producción):** el signo de un valor finito negativo construido a mano (`fild` de una palanca negativa, `tan` negativo) se perdía en el modelo Python y daba infinitos con el signo cambiado; corregido con `fixture_x87.mk`.
3. Sin otras discrepancias: el microtest de hardware (`nan_rules_hw`) dio 0 discrepancias de bits (incluidas las cargas de NaN, `fptan` de NaN/infinito) a la primera.

## Qué se compara

Original = EXE con el hook RH de la función apagado; candidato = DLL con SOLO ese hook encendido. Falsos idénticos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL), que registran sus argumentos en orden. Por caso se compara toda la memoria que cambia en las arenas (CPlayerInfo, CPad, CPed, matriz de la cámara, con guardas de 0x40),
la traza de eventos (incluidos los campos del arma que escribe el constructor, el origen, el objetivo y las banderas del ped en el momento de la llamada), ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 con TOP/etiquetas/ST válidos), ruta, `collaborators_dll`, un FNV de todas las regiones globales tocadas (reloj, paso, byte de inversión, aspecto, cámara, matriz, punteros de jugador)
y estado restaurado tras quitar los parches. ECX/EDX/EAX de salida sólo se registran.

**Familias (1024 casos cada una, 20480 pares):** inactiva con basura arriba; sin disparo (moderada, palancas extremas, bordes del recorte, inversión 0/1/otros); disparo normal; FOV, aspecto, matriz, mira y posición de cámara especiales (NaN no canónicos, infinitos, desnormales, ceros con signo);
magnitudes diminutas y grandes; valores de AX de `GetCarGunFired`; jugador 1; ceros con signo y cancelaciones; bordes del recorte con palanca +-1/+-3000; banderas del ped y lo que invierte el falso.

## Lo que NO prueba este banco

- `|fov * 0.0087| >= 2^63`: `fptan` fuera de rango deja la pila x87 del propio EXE desequilibrada (no se genera).
- Índices de jugador distintos de 0 y 1 (el EXE indexa `CWorld::Players` sin acotar) y un ped nulo (el EXE no lo comprueba).
- El interior de `FireInstantHit` y del constructor `CWeapon` (los validan sus bancos; el constructor corre de verdad).
- Un SNaN devuelto por `FindCamFOV`: el falso lo devuelve por ST0 (`fld` lo silencia en ambos lados).

## Negativa

Mutacion de produccion `Cross74StepCpp(&target[1], steer, timeStep, !inverted)` -> `inverted` (se invierte el signo del eje vertical): **FALLO por diferencia de resultado** (14612 de 20480 pares difieren, ninguna caida; informe `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-playercrosshair74\20261027-035310-695996`, el ultimo FALLO de esa serie), fuente restaurada y banco repetido hasta APROBADO.

## Evidencia final

`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-playercrosshair74\20261027-042927-646077`: 20480 pares, cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados, `fp_difference` vacio, cobertura por familia (20 x 1024) y de ramas (`coverage.json`) y el oraculo (`fixture_oracle.py`) coincide con los dos lados de los 20480 pares. Microtest de hardware `nan_rules_hw.py`: 1 804 096 combinaciones, 0 discrepancias.
