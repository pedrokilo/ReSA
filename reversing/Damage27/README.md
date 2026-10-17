<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de daños del vehículo (lista de 27)

Validadas el 04/10/2026 con **7.744 pares original/candidato dentro de GTA**, todos exactos: 27 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CDamageManager` | `Init`, `ResetDamageStatus`, `ResetDamageStatusAndWheelDamage`, `FuckCarCompletely`, `ApplyDamage`, `GetComponentGroup`, `GetCarNodeIndexFromPanel`, `GetCarNodeIndexFromDoor`, `ProgressAeroplaneDamage`, `ProgressWheelDamage`, `ProgressPanelDamage`, `ProgressEngineDamage`, `ProgressDoorDamage`, `Get`/`SetAeroplaneCompStatus`, `Get`/`SetEngineStatus`, `Get`/`SetDoorStatus`, `Get`/`SetDoorStatus_Component`, `Get`/`SetWheelStatus`, `Get`/`SetPanelStatus`, `Get`/`SetLightStatus` |

**Diferencias reales corregidas:**
- `GetComponentGroup`:
  - 11..15 (alas y parabrisas) son el grupo 5 y 16 o más el grupo 0; el componente 0 es el grupo 6 con índice 0; negativo devuelve falso;
  - compara el dword completo con signo y escribe el grupo como dword;
  - el hook es `__stdcall` (el EXE limpia 3 argumentos; la versión estática de la DLL era cdecl).
- `ApplyDamage`:
  - componente como dword; con un componente negativo el grupo conserva el valor del argumento (índice de la tabla 0x8D32A0);
  - NaN no daña (`test ah,41h`).
- `ProgressPanelDamage`, `ProgressDoorDamage`, `FuckCarCompletely`: tiradas con los bits bajos de `rand()` (`& 1`, `& 7`).
- `FuckCarCompletely`:
  - la rueda que se pierde sale de `rand() * (1/32768) * -3` (0..2, nunca la 3);
  - pone a cero las dwords completas de luces y paneles.
- `ResetDamageStatus`: pone a cero las dwords completas de luces y paneles; `Init` limpia las 6 dwords (incluido el byte de relleno).
- `GetAeroplaneCompStatus`: desplazaba por el estado de los paneles en vez del componente; devuelve EAX completo.
- `SetEngineStatus`: compara el dword completo (300 da 250, no 44).
- `ProgressEngineDamage`: tiene un argumento (`ret 4`) y devuelve falso.
- `GetPanelStatus`, `GetLightStatus`: devuelven EAX completo; `SetPanelStatus`, `SetLightStatus`, `SetAeroplaneCompStatus`: estado de 32 bits.
- `SetDoorStatus`, `SetDoorStatus_Component`, `GetCarNodeIndexFromPanel`, `GetCarNodeIndexFromDoor`: índice como dword; `GetCarNodeIndexFromDoor` devuelve -1 fuera de rango (quitado el `NOTSA_UNREACHABLE`).
- `Get`/`SetWheelStatus`, `ProgressWheelDamage`: sin comprobación de límites (índice dword o byte como el EXE).
- `Init` y `ResetDamageStatusAndWheelDamage` devuelven `this`.

**Método:**
- El gestor falso son 0x18 bytes dentro de un búfer de 0x200 con bytes aleatorios alrededor (índices fuera de rango a la vista); estados coherentes o aleatorios.
- Vehículo falso de 0xA00 bytes (flag 0x868 bit 4, puertas en 0x5B8).
- Se simulan y trazan `rand` (EXE y `CGeneral::GetRandomNumber`), `CDoor::Open` y `CAutomobile::SetBumperDamage`/`SetPanelDamage`/`SetDoorDamage` (EXE y DLL).
- Se comparan el retorno (EAX completo donde el EXE lo define, AL en los bool), los 0x18 bytes, el hash del búfer y del vehículo, las salidas de `GetComponentGroup` y la traza.
- Fuera de los fixtures: componentes negativos grandes en `ApplyDamage` (el EXE lee fuera de la tabla) e índices de rueda fuera del búfer.

Repetir: `python reversing/Damage27/build_probe.py` y `python reversing/run.py --case damage27`. Evidencia: [validation-summary.json](validation-summary.json).
