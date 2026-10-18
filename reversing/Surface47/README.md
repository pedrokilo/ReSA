<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Superficies (lista de 34)

Banco preparado el 04/10/2026: **10.752 pares original/candidato dentro de GTA** para 47 funciones de 1 clase (pendiente de compilar y ejecutar).

| Clase | Funciones |
|---|---|
| `SurfaceInfos_c` | `GetSurfaceIdFromName`, `Init`, `GetAdhesionGroup`, `GetTyreGrip`, `GetWetMultiplier`, `GetSkidmarkType`, `GetFrictionEffect`, `GetBulletFx`, `IsSoftLanding`, `IsSeeThrough`, `IsShootThrough`, `IsSand`, `IsWater`, `IsShallowWater`, `IsBeach`, `IsSteepSlope`, `IsGlass`, `IsStairs`, `IsSkateable`, `IsPavement`, `GetRoughness`, `GetFlammability`, `CreatesSparks`, `CantSprintOn`, `LeavesFootsteps`, `ProducesFootDust`, `MakesCarDirty`, `MakesCarClean`, `CreatesWheelGrass/Gravel/Mud/Dust/Sand/Spray`, `CreatesPlants`, `CreatesObjects`, `CanClimb`, `IsAudioConcrete/Grass/Sand/Gravel/Wood/Water/Metal/LongGrass/Tile`, `GetAdhesiveLimit` |

**Diferencias reales corregidas (cargadores que usa `Init`):**
- `LoadAdhesiveLimits` (0x55D0E0): deja de llamar al EXE y se transcribe: matriz triangular sin límite de filas, el nombre se salta hasta espacio/tabulador sin mirar el fin de cadena, un valor que empieza por `-` vale 0 (también los negativos), y cada valor se copia como dword en `[fila][j]` y `[j][fila]`.
- `LoadSurfaceInfos` (0x55EB90):
  - cada bandera guarda el bit bajo del entero leído (y la rugosidad/inflamabilidad sus 2 bits bajos): `2` cuenta como 0; la DLL usaba `(bool)`;
  - `SANDY` guarda 2 y `MUDDY` guarda 1 en el tipo de marca de derrape (la DLL los tenía cruzados);
  - agarre y multiplicador mojado con `_ftol2` (truncación de 64 bits) y byte bajo; la DLL convertía a `uint32`;
  - `SurfaceInfo` sin `{}` y `sscanf` sin límites ni comprobación del resultado.
- `LoadSurfaceAudioInfos` (0x55F2B0): bit bajo de cada entero en vez de `(bool)`; `sscanf` sin `VERIFY`.
- `GetAdhesiveLimit` (0x55EB50): índice plano `grupoB * 6 + grupoA` sin límites como el EXE; el grupo ocupa 3 bits y con 6/7 el `std::array` de la DLL abortaba (0xC0000409). El fixture quita NaN/Inf de los floats 36..49 que esos grupos leen.
- Se quitan los bloques `FIX_BUGS` de los tres cargadores (fichero no abierto): el EXE no lo comprueba.

**Sin cambios (coinciden con el EXE):** las consultas de bits (desplazamientos y máscaras de los dwords +4/+8 de cada `SurfaceInfo_c`, comprobados uno a uno), `GetTyreGrip`/`GetWetMultiplier` (byte con signo, 0,1f/0,01f del EXE, `WetRoads`, + 1) y `GetSurfaceIdFromName` (las 179 cadenas y valores, por defecto 0).

**Método:**
- Se usa el global `g_surfaceInfos` (0xB79538, 0x8F4 bytes) y `CWeather::WetRoads`: se guardan, se rellenan por caso (límites como floats finitos, superficies con bytes aleatorios y a veces todo a 0/1) y se restauran en cada ejecución y al final.
- `surface.dat`, `surfinfo.dat` y `surfaud.dat` se sirven en memoria (`SetDir`, `OpenFile`, `CloseFile`, `LoadLine` simulados con trazas en el EXE y en la DLL), con comentarios y líneas vacías intercaladas, tabuladores, nombres desconocidos, grupos/marcas/efectos inválidos, enteros 2/3/-1/255, valores `-`, `x`, `1.5z`, filas cortas y de 0 a 8 filas de límites; números con fracciones binarias exactas.
- Las consultas de un id comparan 8 ids por caso; las booleanas comparan AL (todos los llamadores del EXE hacen `test al,al` o guardan AL). Se compara el estado completo (hash con las primeras diferencias si fallan) y los valores devueltos.
- Fuera de los fixtures: ids ≥ 179 (`std::array` de la DLL), líneas con menos campos (el EXE usa pila sin inicializar), nombres sin espacio detrás en `surface.dat` (el EXE lee fuera de la cadena) y el `OpenFile` nulo.

Repetir: `python reversing/Surface47/build_probe.py` y `python reversing/run.py --case surface47`.
