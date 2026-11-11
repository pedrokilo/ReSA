<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/11/2026 (Ciudad de México):** 884 grupos / 9985 funciones reconstruidas; 9985 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Car84G: CCarCtrl::SteerAICarWithPhysics 0x437C20 (lote 84G)

Estado: **APROBADO dentro del juego** (01/11/2026, arbol principal, HEAD abd32cf51 + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados, salida de CADA lado igual a la del oraculo independiente (`fixture_oracle.py`, integrado en `run_local.py`) y microtests de la x87 del hardware (2578).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car84g/20261101-234159-107639` (el manifiesto de construccion es el de esta corrida; la primera, `...233437-213388`, dio el mismo resultado con los 18432 pares).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `static void CCarCtrl::SteerAICarWithPhysics(CVehicle*)` (antes `plugin::Call<0x437C20>`, sin hook) | 0x437C20 | `CCarCtrl` / `SteerAICarWithPhysics` / S (`RH_ScopedInstall`) | `CarCtrl.cpp` (usa `Veh83DMul/Add/Sub/Store`, `Veh83CMulStore/AddStore`, `TrigSimpleSinCpp/CosCpp`) |

Fila de `docs/hooks.csv`: `CCarCtrl,SteerAICarWithPhysics,0x00437C20,1,0,1,S`.

## Revision contra el EXE (0x437C20..0x4382D4, 0x6B5 bytes, cdecl, un argumento, `ret`, void)
1. Con id de grabacion (+0x424, con signo) >= 0 y `bUseCarAI[id]` (0x97D6C0) a 0 no hace nada. `SwitchBetweenPhysicsAndGhost` (0x4222A0); +0x420 = 0. La mision (+0x3BA) menos 15 indexa la tabla de clases de 0x4382E4 (41 bytes): clase 0 sin objetivo -> mision 0, clase 1 sin objetivo -> 0x2A, clase 2 nada.
2. bIsStopped (bit 0 de +0x3DC): `AreNodesLoadedForArea` (0x44DD10, ECX = 0x96F050) con x -+ 270 e y -+ 270 (posicion de la matriz o de la transformacion simple); con nodos se borra el bit y se llama a `JoinCarWithRoadAccordingToMission` (0x432CB0). Salidas: giro 0, acelerador 0, freno 0.2f.
3. bIsParked (bit 1) con mision distinta de 0 y 0x0B: accion temporal 22 durante 2000 ms y los nodos se invierten (+0x394 <- +0x390, +0x390 <- +0x394, +0x398 <- +0x390).
4. Accion temporal (+0x3BB, tabla de saltos de 0x438310, 24 casos): 1/24 espera o freno (0.2f / 1.0f; al vencer: accion 0 y +0x3AC / +0x3B0 = reloj); 3 marcha atras (`OnlyMission`, giro negado, producto escalar velocidad . adelante contra 0.04f); 13/14 `FindMaxSteerAngle` con acelerador -0.75f; 4..9 curvas con/sin freno de mano, hacia delante; 10/11/20/21 esquivar con +-0.25f y el borde tiempo - 1250 (20/21: espera 4000 ms al vencer); 19 choque frontal (giro +-0.5 con 0.05 por paso, constantes double 0x859068/0x859060/0x859058); 22 marcha atras recta (producto escalar contra 0.1f); 23 empuje (velocidad += adelante * (paso * 0.012f), adelante de la matriz o (-sin h, cos h, 0)); 15..18 expiran y siguen por defecto; resto: `OnlyMission` (0x436A90).
5. Final: freno (+0x4A0), giro (+0x494), bit 5 de +0x428 (freno de mano), acelerador (+0x49C) y, con el modelo 0x21B (Vortex), +0x994 = acelerador y +0x988 = giro.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado cuando lo tienen): `SwitchBetweenPhysicsAndGhost` 0x4222A0, `AreNodesLoadedForArea` 0x44DD10 (thiscall, 4 floats), `JoinCarWithRoadAccordingToMission` 0x432CB0, `FindMaxSteerAngle` 0x427FE0 (float en st0) y `SteerAICarWithPhysics_OnlyMission` 0x436A90 (envoltura en la DLL). Registran el estado del vehiculo en cada llamada, EN ORDEN, y escriben los valores del caso; tras el evento numero `mut_at` mutan el estado (14 tipos: mision, banderas de movimiento, accion temporal, su tiempo, objetivo, reloj, velocidad, adelante de la matriz, volante, banderas, modelo, nodos, paso).
Corren de verdad el producto escalar (0x40FDB0), v * f (0x40FEC0), a + b (0x40FE30) y `CPlaceable::GetForward` (0x41CCB0) en el EXE frente a los ayudantes x87 de la DLL, `bUseCarAI`, `CTimer` y las constantes de .rdata (se vigilan sus bytes y la tabla de clases).
18432 casos, 18 familias (16384 + 2x1024): aleatorio, grabacion, tabla de clases, espera con y sin matriz, aparcado, espera/freno, marcha atras con umbral exacto del producto escalar, marcha atras con giro, curvas, esquivar, choque frontal, marcha atras recta, empuje con y sin matriz, aviones y rama por defecto, barrido de mutaciones, Vortex, especiales y tiempos con envolvimiento. Cobertura de ramas con minimos (`coverage.json`).

## Prueba negativa
Produccion mutada y restaurada (`CarCtrl.cpp` sha256 bueno `2a1650734ae9...`; despues de restaurar es identico, comprobado con `cmp`): acelerador de la marcha atras `-0.5f` -> `-0.25f`, borde del esquivar `1250u` -> `1000u` y factor del empuje `0.012f` -> `0.013f`. Falla por diferencia de resultado, sin caida (`resultados-car84g/20261101-233830-884190`: **2052/18432 diferencias**).

## Bancos repetidos
Ninguno: ningun otro banco de `reversing/` cita 0x437C20 ni el simbolo.

## Fuera de los fixtures
Vehiculo sin matriz fuera de la rama de espera y de la accion 23 (el EXE falla), `bUseCarAI` con valores distintos de 0 y 1, NaN con cargas distintas (las fixtures usan un unico patron), rumbo de la transformacion simple enorme o no finito (seno y coseno de la x87 frente al de la DLL) y bytes altos del booleano de freno de mano que escribe `OnlyMission`.
