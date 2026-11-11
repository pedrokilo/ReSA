<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/11/2026 (Ciudad de México):** 885 grupos / 10129 funciones reconstruidas; 10129 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn104A: CAutomobile::SetRandomDamage (lote 104A)

Estado: **APROBADO dentro del juego** (05/11/2026, worktree `wv-2` sobre main 6b60ef33f + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn104a/20261105-230131-486795` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn104A-20261105-230131-486795`). Banco par a par dentro del juego, 16384 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, x87 igual (queda vacia), y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). Una corrida previa identica (`20261105-221813-756220`, mismo codigo) tambien aprobo; la de registro es la repeticion final tras la prueba negativa.

| Funcion | Direccion | Bytes | Fuente | Antes llamada desde |
|---|---|---|---|---|
| `CAutomobile::SetRandomDamage` | 0x6A2530 | 703 | `Entity/Vehicle/Automobile.cpp` | `CarCtrl::GenerateOneRandomCar` (0x431CB4, por direccion fija en `CarCtrlGenerateOneRandomCar.inl`) y el menu de herramientas |

## Revision contra el EXE (0x6A2530..0x6A27EC, thiscall, ECX = coche, `bRemoveStuff` en pila como dword del que solo se lee el byte, `ret 4`; void)
- La version anterior (NOTSA) usaba `CGeneral::GetRandomNumberInRange` y no era la del EXE: no reproduce la secuencia de tiradas ni el reparto de paneles. Ahora cada tirada es `rand()` del CRT del EXE (0x821B1E, la que avanza la semilla del juego) `& 0xFFFF`, a float por 1/32768 (0x858B14), por la constante del uso (6.0 0x858B44, 4.0 0x858B90, -3.0 0x859024 o el entero que entra por `fimul`) y truncada (`_ftol2`). Los productos son exactos en 24 bits (15 bits por 3), asi que los `float` de C++ (guardados en `volatile`) coinciden con la x87 del juego.
- Con `bRemoveStuff`: puertas = trunc(t*6); con puertas != 0, paneles = trunc(t2*4); con 0 puertas, paneles = 1 - trunc(t2*-3) (1 a 3). Sin `bRemoveStuff`: puertas = trunc(t) (siempre 0 con tiradas de 15 bits), paneles = 1 sin segunda tirada.
- Puertas: por cada una, tirada y n = trunc(t*(6-i)); la primera es n tal cual, las demas el n-esimo (base 0) de las no marcadas (bucle del EXE con ecx/esi y marcadores de un byte). Orden de llamadas del EXE por elemento: tirada, `GetCarNodeIndexFromDoor` (0x6C26F0), `GetDoorStatus` (0x6C2230, EAX completo); solo si es 0 y el bit (nodo & 31) de la mascara +0x310 de la estructura del model info (word +0x22 del coche con signo, tabla 0xA9B0C8, +0x5C) esta puesto: `SetDoorStatus(2)` (0x6C21C0) y `SetComponentVisibility(marco +0x648+4*nodo, 2)` (0x6D2700); despues marca el elemento tanto si se dano como si no.
- Paneles: igual con n = trunc(t*m), m = 4, 3, 2, 1; el n-esimo entre los paneles 0, 1, 5 y 6 aun no marcados (2, 3 y 4 = traseros y parabrisas arrancan marcados); estado dañado **1** (la version anterior ponia 2).
- Diferencias de comportamiento respecto a la version anterior: paneles dañados con estado 1, secuencia de tiradas y de colaboradores identica al EXE, la comprobacion de estado va antes que la mascara de danables.
- `InjectHooks`: `RH_ScopedInstall(SetRandomDamage, 0x6A2530)`. No hace falta opcion de compilacion por archivo (la aritmetica son floats exactos); la funcion no usa x87 propia.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo del hook de la DLL: `CDamageManager::GetCarNodeIndexFromDoor` 0x6C26F0, `GetCarNodeIndexFromPanel` 0x6C26A0, `GetDoorStatus` 0x6C2230, `GetPanelStatus` 0x6C2180, `SetDoorStatus` 0x6C21C0, `SetPanelStatus` 0x6C2150 y `CVehicle::SetComponentVisibility` 0x6D2700 (todos con hook RH, apagado durante el banco), y `rand()` del CRT 0x821B1E (sin hook; la DLL lo llama por su direccion; un solo falso que devuelve las tiradas del caso). Los hooks de `CDamageManager` son envoltorios estaticos con las comprobaciones del EXE y la funcion nueva llama a los METODOS de la clase en la DLL: por eso los mismos falsos se instalan ademas en los cuerpos de esos metodos (`dll_only`, via puntero a miembro).
NO se sustituyen (corren de verdad en ambos lados): aritmetica de las tiradas, busqueda del n-esimo sin marcar, marcadores, mascara del modelo y arreglo de nodos. Memoria real del juego: la entrada del modelo del caso en la tabla 0xA9B0C8 se sustituye por un model info propio (con su estructura de vehiculo propia) y se restaura tras cada lado.
Se observa: eventos de los falsos con todos sus argumentos (rand con su indice, puertas/paneles, estados, marco leido del arreglo de nodos), bytes cambiados de las tres arenas (coche, model info, estructura), la entrada de la tabla tras la llamada y el estado x87 final (vacio).
16 familias de 1024 casos (`Fixtures.hpp`): todo danable, mascaras aleatorias y de un bit, argumento con basura en los bytes altos y byte bajo != 0, bits altos de basura en las tiradas, umbrales de truncamiento de 6, 4 y -3 con vecinos, tiradas extremas, 5 puertas, nodos 32..47 (desplazamiento enmascarado), estados 0..4, modelos 0..19999.

## Fuera de los fixtures
Tiradas con el bit 15 puesto (con `0x8000` o mas el EXE saldria de sus marcadores en la pila y leeria basura: la rand real del CRT nunca lo hace), indices de modelo fuera de 0..19999 o negativos, nodos fuera de 0..47, estados con bits altos distintos de cero devueltos por los colaboradores, y la exactitud de los colaboradores falseados (cada uno tiene su banco).

## Prueba negativa
Copia buena (SHA256): `Automobile.cpp` f47532a1... Cada mutacion se compilo, se corrio el banco completo y se restauro la fuente (`cmp` contra la copia buena).
- m1 (`1 - Scaled(-3)` -> `2 -`): 3206 pares con diferencias; m2 (paneles x4 -> x3): 4834; m3 (puertas x6 -> x5): 6848 (corridas individuales).
- Ronda 2 (estado de panel 1 -> 2, estado de puerta 2 -> 1, marcadores preasignados de panel 4 sin marcar, mascara `& 31` -> `& 15`): 14467 pares con diferencias.
- Ronda 3 (`& 0xFFFF` -> `& 0x7FFF`, sin marcar la puerta elegida, bucle `<` -> `<=`, multiplicador inicial de paneles 4 -> 5): 15734.
- Ronda 4 (paneles sin segunda tirada 1 -> 0, `&&` -> `||` entre estado y mascara, una tirada extra en el camino sin `bRemoveStuff`): 10155.

## Tiempo
Una corrida de los 16384 pares: unos 1,5 min (mas la compilacion incremental de la sonda, unos 10 s, y la espera del candado del juego).

## Bancos repetidos y adaptados (lote 104A)
- `Car84L` (cita 0x6A2530 como falso; unico banco que lo cita): repetido sin cambios y aprobado (`runs/Car84L-20261105-230729-397024`).
- `PresetLoad74` (declaraba 0x72F430 sin hook): adaptado con `MakeCollab` y aprobado (`runs/PresetLoad74-20261105-231955-459996`).
- `DependenciesGarageSphere15` (necesitaba `HookRoute` para 0x54F1B0 y 0x59C890): adaptado y aprobado (`runs/DependenciesGarageSphere15-20261105-234438-490522`).
