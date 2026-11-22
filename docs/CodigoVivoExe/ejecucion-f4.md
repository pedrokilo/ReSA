# F4: los dos despachadores de comandos de script que dejaban 44 funclets sin dueño

Rama `vivo-f4` (desde `origin/main` 6fc1d58ba9, 21/11/2026). Alcance: el lote F4 de [plan-lotes.md](plan-lotes.md) (44 funclets `Unwind@`, [fase8-funclets-f4.csv](fase8-funclets-f4.csv)). Sus tablas de desenrollado cuelgan de las envolturas 0x83821C y 0x838A54, que citan los prólogos de dos **despachadores originales de comandos de script** que Ghidra no separó como función. Se reescribieron en C++ y los funclets dejan de tener razón de ser.

## 1. Qué se hizo

| Despachador del EXE | Comandos | Funclets | Función C++ | Archivo |
| --- | --- | ---: | --- | --- |
| 0x490DB0 (7.448 bytes, tabla de saltos 0x492AC8 de 94 entradas) | 1500–1599 | 34 | `CRunningScript::ProcessCommands1500To1599` | `source/game_sa/Scripts/RunningScriptCommands1500To1599.cpp` |
| 0x472310 (8.320 bytes, tabla de saltos 0x474390 de 100 entradas) | 2000–2099 | 10 | `CRunningScript::ProcessCommands2000To2099` | `source/game_sa/Scripts/RunningScriptCommands2000To2099.cpp` |

Cada uno es un `switch` sobre el número de comando con la misma forma que `ProcessCommands1200To1299`, `1700To1799` y `1900To1999`: thiscall con un argumento de pila (`{ .stackArguments = 1 }`), `OR_CONTINUE` (0) en los casos con o sin cuerpo y `OR_INTERRUPT` (-1) para los comandos sin caso. Se registran con `RH_ScopedInstall` en `RunningScript.cpp` (declaraciones en `RunningScript.h`) y en `docs/hooks.csv` (reversed=1, validated=0). Cada caso sigue el desensamblado (capstone sobre el EXE canónico): orden de `CollectParameters`/lecturas de `ScriptParams`/reservas de tarea, el mismo valor de retorno y las constantes de `.rdata` como `constexpr` locales con su dirección (no se tocó `DatosExe`). Sin `__asm`.

Cifras de casos:

| Despachador | Con cuerpo | Vacíos (devuelven 0 sin leer parámetros) | `OR_INTERRUPT` |
| --- | ---: | ---: | ---: |
| 1500–1599 | 71 | 23 (1503 1504 1505 1507–1512 1514 1519 1520 1544 1545 1548 1552 1559 1564 1567 1568 1579–1581) | 6 (1594–1599) |
| 2000–2099 | 68 | 24 (2001 2002 2004 2012 2018 2019 2025–2029 2036 2053 2055–2057 2060 2061 2063 2067 2075 2081 2084 2098) | 8 (2008 2009 2050 2091–2095) |

Se implementaron **todos** los casos del EXE, incluidos los de comandos que ya tienen manejador propio (`REGISTER_COMMAND_HANDLER`): `ProcessOneCommand` no los alcanza por esta vía, pero el despachador del EXE sí los contenía y así el cuerpo es completo y comparable (CmdBench). Los comandos que alcanzan el despachador (los 93 `original` de estas dos centenas) ahora ejecutan C++ propio.

Efectos fuera de los dos archivos:

* `TaskComplexFollowPatrolRoute.h/.cpp`: se sacó del espacio de nombres anónimo `CTaskComplexFollowPatrolRouteConstructorExe` (0x674930) y se declaró en el `.h`, porque el comando 2071 necesita llamar a ese constructor (la clase es abstracta). El enganche `RH_ScopedNamedGlobalInstall` queda igual.
* `reversing/CmdBench/LiveComparison.hpp`: los dos despachadores se apagan junto a los de 800/1200/1900 para que el lado EXE use el código original, y con `CMDBENCH_VIA_DISPATCHER=1` el lado C++ de esos comandos llama al despachador C++ en lugar del manejador propio (así el banco compara el despachador nuevo contra el del EXE). **No se ejecutó** (ver 3).

Datos del EXE que siguen leyéndose por dirección (pasar a `DatosExe` más adelante, lo lleva otro agente): `StaticRef<float>(0xC18CF0)` y `StaticRef<float>(0xC18DB4)` (flotantes de `.bss` que el EXE inicializa al arrancar), `StaticRef<CPointRoute>(0xC18D50)` y `reinterpret_cast<const CPatrolRoute*>(0xC18DB8)`. Puentes por dirección (`plugin::Call`): ninguno.

## 2. Estado de los 44 funclets

Los funclets `Unwind@` solo se ejecutan si un constructor lanza (`operator new`/constructores de tarea) o al desenrollar el marco del despachador original; con los dos despachadores enganchados el código original no corre, y en el ejecutable propio no existen. Quedan **descartados los 44** (10 + 34, 592 bytes): `fase8-funclets-f4.csv`, `fase8-arranque-lotes.csv` (resultado `descartada-despachador-reescrito`), `plan.csv`, `cierre.csv` y el contador de `ComandosScript` lo reflejan. El lote F4 pasa de «cerrado (dueño resuelto)» a «cerrado (dueños reescritos en C++)».

`docs/ComandosScript.csv`/`.md` regenerados con `python contrib/inventario_comandos_script.py` (flujo del proyecto): `despachador_reescrito` pasa a `si` para los 93 comandos `original` de 1500–1599 y 2000–2099; el total de comandos *original* con despachador reescrito sube de 184 a 277 y los que siguen en el EXE bajan de 794 a 701. El generador pierde el bloque `RESA-CURRENT-STATUS` de `ComandosScript.md`; se restauró a mano.

## 3. Verificación

| Comprobación | Resultado |
| --- | --- |
| Compilación (`_build_VIVOF4.sh`, Debug x86, candado) | EXIT=0 (los `static_assert` de desplazamientos del EXE pasan) |
| Humo ropero (`prueba_ropero.py cal_mal.args f4`) | BIEN (puerta y marcador) |
| Humo agua (`prueba_agua.py cal_mal.args f4`) | BIEN |
| `audit_hook_catalog.py --strict` | OK |
| `audit_cpp_assembly.py` | 0 `__asm`/`naked` |
| Banco CmdBench del despachador | **no ejecutado**: `build_probe.py` usa la copia de pruebas de `E:` y `OriginalDependencies499`, fuera del checkout parcial; el modo `CMDBENCH_VIA_DISPATCHER` queda preparado |

Riesgos conocidos (sin validar comando a comando, por indicación):

* La aritmética flotante suelta (por ejemplo la del comando 2068 y las diferencias de 1540/1555) está en `float`; el EXE trabaja en x87 con precisión de 24 bits: puede haber diferencias de 1 ulp.
* 2044 (`DISPLAY_TEXT_WITH_FLOAT`): el EXE lee el segundo parámetro como entero (`fild`) y el manejador propio de `TextCommands.cpp` lo declara `float`; el despachador sigue al EXE, el manejador propio no se tocó.
* Los índices de pools, secuencias, atractores y grupos se usan como el EXE (sin más comprobación que las suyas).
* El EXE comprueba el resultado de `CTask::operator new` (tarea nula); `new` de C++ no devuelve nulo.
* No cambia ningún tipo de retorno de manejadores existentes (el ropero depende de `GET_CHAR_AREA_VISIBLE`; no se tocó).
