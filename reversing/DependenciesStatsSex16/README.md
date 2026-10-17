<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesStatsSex16 — CStats::UpdateSexAppealStat

CStats::UpdateSexAppealStat, entrada 0x55BF20, cohorte GAME original. Conversión a C++ puro (sin `__asm` ni `naked`) de las 108 instrucciones hasta `jmp 0x559760` (CheckForStatsMessage); el desensamblado completo del EXE está en `UpdateSexAppealStat.dis.txt` (verificado instrucción a instrucción contra `gta-sa.exe`). `source-proposal.patch` es la propuesta anterior con funciones `naked` y **no debe aplicarse** (política C++ pura); queda sólo como referencia histórica.

## Técnica

- `source/game_sa/Stats.cpp`: `StatsSexAppealBody<kExe>` (hook `StatsUpdateSexAppealStatExe`, `RH_ScopedNamedGlobalInstall(... "UpdateSexAppealStat", 0x55BF20)`) y `CStats::UpdateSexAppealStat()` (kExe = false). Mismo orden de llamadas que el EXE: `FindPlayerVehicle(-1,false)`; si hay vehículo, segundo `FindPlayerVehicle`, `[0xB79530] = vehículo` y `RegisterReference(&[0xB79530])`; con caché no nula, `FindPlayerCoors(-1)`, distancia en 2D contra 35.0f, salud del vehículo, clase por la tabla de 11 bytes de 0x55C0B4 y escritura de `StatTypesFloat[STAT_SEX_APPEAL]` (0xB793E4).
- Aritmética x87 (24 bits) en `X87Math.cpp`: `SexAppealHalfCpp`, `SexAppealNearCpp`, `SexAppealVehiclePartCpp`, `SexAppealFinishCpp`. Stats.cpp no contiene operaciones float propias (se compila con SSE y cambiaría MXCSR y las banderas de la x87). La comparación que ramifica (fsqrt y `fcomp [35.0f]`) va dentro de `SexAppealNearCpp`. Cada ayudante deja en la pila x87 los mismos valores muertos que el tramo del EXE (comprobado con un arnés aislado que compara el FNSAVE de una transcripción del EXE con la cadena de ayudantes en las 30 rutas: TOP, FCW, FSW, FTW, FOP, FDP y los 8 registros ST iguales). El último acceso x87 es `fstp dword [0xB793E4]` dentro de `SexAppealFinishCpp`, como en el EXE (0x55C093).
- Residuo hacia CheckForStatsMessage (`jmp`, sin retorno propio): ECX/EDX con que entra son los de la última instrucción que los escribió en el EXE: sin caché, ECX = 0 y EDX del primer FindPlayerVehicle; distancia >= 35 o NaN, los de FindPlayerCoors; distancia < 35, ECX = movsx(modelo) y EDX = ms_modelInfoPtrs[ECX]. Los de los colaboradores se capturan con `StatsCaptureEcxEdxCpp` justo tras la llamada y la cola llama a `StatsCheckForStatsMessageExe(ecx, edx)`. El llamador del EXE (CGameLogic::Update, 0x442ADF) no lee ningún registro.
- Colaboradores reales (sin mocks): 0x56E0D0, 0x56E010, 0x571B70, 0x559760 (reconstruidos y activos en la DLL en ambos lados), más 0x53FB70, 0x56E120 y 0x56E210 (rutas comprobadas, usados por los anteriores).

## Banco

- 4096 pares (128 perfiles x 32 valores): jugador nulo / fuera / con bInVehicle sin vehículo / en vehículo; caché nula / anterior / actual; matriz o placeable en vehículo y jugador; referencia nueva / duplicada / existente; distancia < 35, == 35, > 35, NaN, ±inf; salud NaN con carga, señalizante, ±inf, ±0, subnormal, ±FLT_MAX, ±1e30; 16 clases de vehículo; apariencia con NaN, ±inf, ±0, subnormal, FLT_MAX; cheat activo / apagado. `fixture_oracle.py` define las fixtures, repite el ensamblador con floats de Python (`r24` para lo que queda en registros x87) y exige las rutas y el estrés; `run_local.py` comprueba además que la estadística 0xB793E4 de ambos lados coincide con el oráculo cuando ningún NaN decide el patrón de bits.
- Sale pronto en CheckForStatsMessage con `CPad::GetPad(0)->JustOutOfFrontEnd = 1` (EAX = 0xB73458 y CL = 1; los 24 bits altos de ECX y todo EDX son los del residuo que se prueba). Se guardan y restauran el bloque de estadísticas, PlayerInfo, foco, cheat, ese byte del pad, tabla de referencias completa, ranura de modelo, búferes de ped/vehículos/matrices/modelo, x87 completo y MXCSR.
- FNSAVE de 108 bytes tras cada lado: FIP original en 0x55C093 y, en la DLL, una instrucción del ASI; en ambos lados los bytes en FIP son `fstp dword [0xB793E4]` (D9 1D E4 93 B7 00). No se exige FDP = 0xB793E4: esta CPU deja FDP, FCS y FOP a cero tras un fstp a memoria, también en el EXE (corrida DependenciesStatsSex16-20261016-203816-358413: FDP = 0 en las 4096 filas de los dos lados); FDP/FCS/FOP/FDS se comparan igualmente enteros entre EXE y DLL. Sólo los cuatro bytes de FIP se normalizan. `kCompareDeadX87Registers = true` (valor por defecto) exige además igualdad del contenido crudo de los registros ST vacíos; cada fila lleva `fp_difference` con los campos que difieren (`stN(dead)` = registro vacío). Cambiarla a `false` (ignorar el residuo crudo de registros vacíos, que ninguna instrucción puede leer sin cargarlo antes) relaja la compuerta y es una decisión de política del propietario del proyecto: no se ha tomado.
- Registro compacto: los bloques de más de 16 KB (tabla de referencias) se guardan como longitud + FNV-1a cuando el par es igual; la igualdad se decide siempre sobre los bytes completos.

## Ejecución

Desde la raíz del repositorio, con el proyecto compilado con la fuente convertida (Stats.cpp, X87Math.cpp/.h):

```
python -B reversing/DependenciesStatsSex16/fixture_oracle.py
python -B reversing/DependenciesStatsSex16/build_probe.py
python -B reversing/DependenciesStatsSex16/run_local.py
python -B reversing/DependenciesStatsSex16/run_local.py --analyze <comparisons.jsonl>
```

o, con el Python de herramientas y el clon de pruebas: `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de salida> DependenciesStatsSex16`. Antes de dar por buena una corrida: `python -B contrib/audit_register_residue.py` (objeto Stats).
