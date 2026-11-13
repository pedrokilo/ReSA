<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10424 funciones reconstruidas; 10424 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn174A: 20 funciones diminutas del EXE (lote 174A)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-4` desde main 7f2cac1f99 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn174a/20261112-043729-662081` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn174A-20261112-043729-662081`, `comparisons.jsonl.gz` de 54,8 MB). 20 x 16384 = **327680 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `842e1b7730d2541a6a0077e9b59e5376482b805b4b15cec82f1606a0157f8115`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo listaba 21 direcciones y **ninguna tenia hook** (codigo, `docs/hooks.csv` y `git worktree list`). Viables en C++ puro: 20. **Saltada: 0x6F55C0** (5 bytes, `E9 ..` = `jmp 0x6D0E60`; es la ranura 18 (Render) de la vtable 0x872370 de `CTrain` y salta a `CVehicle::Render`: salto puro a otra direccion, sin trabajo propio). Las clases del listado eran en parte erroneas; los nombres salen del desensamblado, de las vtables leidas del PE y de los llamadores.

| # | Funcion real (el listado decia) | Entrada | Bytes | Fuente (`source/game_sa/`) | Tipo |
|---|---|---|---|---|---|
| 0-3 | `CPad::IsMouseWheelUpPressed / WheelDown / Bmx1 / Bmx2` (CMenuManager) | 0x57C400, 0x57C420, 0x57C440, 0x57C460 | 24 | `Pad.cpp` (+ `Pad.h`: de inline a declarada) | S |
| 4 | `CMenuManager::NoDiskInDriveMessage` | 0x57C5E0 | 118 | `MenuManager.cpp` | S |
| 5 | `CHeli` cuerpo del destructor (vtable 0x871680, marco SEH) | 0x6C4340 | 169 | `Entity/Vehicle/Heli.cpp` | S |
| 6 | `CHeli::SetupDamageAfterLoad` (ranura 51) | 0x6C4550 | 8 | idem | V |
| 7 | `CHeli` destructor con borrado (ranura 0) | 0x6C4810 | 30 | idem | S |
| 8 | `CMonsterTruck` cuerpo del destructor (vtable 0x8717D8; el listado decia CHeli) | 0x6C7D10 | 11 | `Entity/Vehicle/MonsterTruck.cpp` | S |
| 9 | `CFontChar::Set` (copia de 47 bytes) | 0x718E50 | 117 | `Font.cpp` (+ `Font.h`) | S |
| 10 | `CFont::StripTildeTags` (quita las etiquetas `~..~`) | 0x719240 | 115 | `Font.cpp` (+ `Font.h`) | S |
| 11 | `CFont::MapExtendedChar` (tabla 0x8D5AD8) | 0x719360 | 19 | `Font.cpp` (+ `Font.h`) | S |
| 12 | `Common::TransformPointsSimple` (el listado decia Common) | 0x54ED20 | 105 | `common.cpp` + `X87Math.cpp` | S |
| 13 | `Common::TransformVectors` con `CMatrix` | 0x54EEA0 | 79 | `common.cpp` | S |
| 14 | `Common::DoesInfiniteLineCrossFiniteLine` | 0x71DB30 | 76 | `common.cpp` + `X87Math.cpp` | S |
| 15 | `tHandlingData` constructor (el listado decia cHandlingDataMgr) | 0x6F4FA0 | 15 | `cHandlingDataMgr.cpp` (+ `cTransmission.cpp/.h`) | S |
| 16 | `CTrainNode::GetPosn` | 0x6F5440 | 70 | `TrainNode.cpp` | S |
| 17 | `CTrain` cuerpo del destructor (vtable 0x872370, marco SEH) | 0x6F54D0 | 85 | `Entity/Vehicle/Train.cpp` | S |
| 18 | `CTrain::SetModelIndex` (ranura 5) | 0x6F5530 | 49 | idem (+ `Train.h`) | V |
| 19 | `CTrain::PreRender` (ranura 17) | 0x6F5570 | 75 | idem (+ `Train.h`) | V |

V = `RH_ScopedVMTInstall` (fila `0x00000000`); S = `RH_ScopedInstall` / `RH_ScopedNamedGlobalInstall`. 20 filas nuevas en `docs/hooks.csv`.

## Envolturas sustituidas
- `Stats.cpp`: `StatsStripTags` llamaba a 0x719240 por direccion (`reinterpret_cast<...>(0x719240)`); ahora llama a `CFont::StripTildeTags`.
- `ControllerConfigManager.cpp`: la funcion estatica `KeyCharToGxtChar` (copia de 0x719360) pasa a llamar a `CFont::MapExtendedChar`.
- `Heli.cpp`: `~CHeli` y el cuerpo del EXE comparten `HeliReleaseGunflashFx`; el cuerpo del EXE libera la tabla con `operator delete[]` por direccion (0x8213AE, la reserva el EXE).

## Revision contra el EXE (desensamblado propio con `dumpbin /disasm`)
- **0x57C400/20/40/60** (cdecl, AL): byte del boton en `NewMouseControllerState` (0xB73418 + 3..6) distinto de 0 y byte de `OldMouseControllerState` (0xB7342C + 3..6) igual a 0.
- **0x57C5E0** (thiscall): `DoRWStuffStartOfFrame` con siete ceros; si `RsGlobal.quit` (dword 0xC17050) es 0: `MessageLoop`, `CPad::UpdatePads`, `MessageScreen("NO_PCCD", 1, 0)`, `DrawFonts`, `DoRWStuffEndOfFrame`, `CPad::GetPad(0)` (sin usar; se anade la llamada al C++); con Esc recien pulsado (palabras 0xB733A8 != 0 y 0xB73138 == 0) pone el byte `+0x58` y llama a `RsEventHandler(0x1E, 0)`.
- **0x6C4340**: vtable 0x871680; con la tabla de fogonazos (+0xA0C) no nula recorre `GetPlaneNumGuns` (con signo, releida en cada pasada) llamando a `FxSystem_c::Kill` y `DestroyFxSystem` (ranura releida) de cada entrada no nula; `operator delete[]` de la tabla y puntero a 0; `CAEVehicleAudioEntity::Terminate` (+0x138) y cuerpo de `~CAutomobile` (0x6A61E0). 0x6C4810: cuerpo 0x6C4340 y `CVehicle::operator delete` 0x6E2D90 con el bit 0 del BYTE de banderas. 0x6C4550: `and byte [ecx+42Ah],0FEh`.
- **0x6C7D10**: `[ecx]=0x8717D8; jmp 0x6A61E0`. Ya tenia funcion propia (lote 125A) que usaba el destructor generado (vtable de la DLL); ahora deja la vtable del EXE y llama al cuerpo de `~CAutomobile`.
- **0x718E50**: dword +0, +4, +8, +0xC, +0x10; bytes +0x14..+0x17; dwords +0x18..+0x24; bytes +0x28..+0x2A; word +0x2C; byte +0x2E (cada lectura seguida de su escritura; el byte +0x2F no se toca; el dword +0 copia los dos bytes de relleno); devuelve `this`. Antes era una copia campo a campo (no copiaba el relleno) y devolvia void.
- **0x719240**: copia el texto sin limite a un buffer local de 0x100 y lo reescribe en el sitio sin lo que hay entre pares de `~`; una `~` sin pareja recorre memoria (como el EXE; el texto del juego trae las marcas por pares). **0x719360**: `ch < 0x80` tal cual; si no el byte de 0x8D5AD8 + 2 * ch.
- **0x54ED20**: `c`, `s` = `fcos`/`fsin` del rumbo (+0xC) guardados como float; sin puntos (`test/je`) no hace nada; por punto `x' = ((c*x) - (s*y)) + pos.x`, `y' = ((s*x) + (c*y)) + pos.y` (con `in.x` releido tras escribir `x'`) y `z' = z + pos.z`; contador sin signo `dec/jne`. El bucle esta en `X87Math.cpp` **sin optimizar por pragma**: con `/O2` el compilador carga los operandos de memoria en registros y un NaN senalizante de la entrada dejaba de ceder ante el NaN silencioso del coseno (tabla de Intel: con un QNaN en el registro y un SNaN en memoria gana el QNaN); el banco lo detecto en 125 casos por funcion y el oraculo se corrigio al medir el EXE (`mulm`/`addm`).
- **0x54EEA0**: `CopyToRwMatrix` (0x59B8B0) a la matriz de trabajo (0xB72920), `RwMatrixUpdate` (0x7F18A0) y `RwV3dTransformVector` (0x7EDDC0) por vector con contador sin signo. **0x71DB30**: `((Ax-ox)*dy - (Ay-oy)*dx) * ((Bx-ox)*dy - (By-oy)*dx) < 0.0f` (0x858B50 = 0) con la x87 sin redondeos intermedios (`X87Math.cpp`).
- **0x6F4FA0**: `cTransmission` (0x6D0450) sobre `this + 0x2C`; devuelve `this` (`TransmissionConstructorBody` pasa de `static` a funcion con declaracion en `cTransmission.h`). **0x6F5440**: tres words con signo por 0.125f; devuelve el puntero de salida.
- **0x6F54D0**: vtable 0x872370, `Terminate` (+0x138) y `~CVehicle` (0x6E2B40, con hook, por una funcion propia sin expansion en linea). **0x6F5530**: `CVehicle::SetModelIndex`, 17 dwords a cero desde +0x668 y `CClumpModelInfo::FillFrameArray` (0x4C5440). **0x6F5570**: `CVehicle::PreRender` y `DoVehicleLights` con 5 (modelo 449 y bit 3 de +0x5B8), 0x25 (bit 3) o 0x15 (bit 4).

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Arenas propias A, B y C (guardas de 0x40). Plantilla `Fn152A` (con el modelo x87 de `Fn116A`). Falsos identicos en la direccion del EXE y en el cuerpo de la DLL; lista completa en `scope_note.txt`. Casos particulares:
- `CPad::UpdatePads` (0x541DD0) esta enganchada con `HookInstall` (fuera del registro RH): su entrada del EXE salta siempre a la DLL, asi que el falso se parchea sobre ese salto (`Pair::raw`) y tambien en la funcion de la DLL.
- `CFont::DrawFonts` (0x71A210) es en el EXE un `jmp` a `RenderFontBuffer` (0x719840, con hook): se le da ruta (hook apagado) como colaborador real para que la entrada resuelva a codigo del EXE.
- Memoria global con regiones guardadas y restauradas tras cada lado: estado del raton (0xB73418, 0x28 bytes), 0xC17050 y las palabras 0xB733A8 / 0xB73138.
- `CTrain::SetModelIndex` usa un clump nulo o en la arena con el byte de tipo `rpCLUMP` (el aserto de `GetRpClump` lo lee).
Se observan bytes cambiados de las arenas y de la memoria global, eventos de los falsos con todos sus argumentos, valor devuelto (etiqueta de arena, EAX o solo AL) y x87 final vacia. 16 familias de 1024 casos por funcion (ver `Fixtures.hpp`); `python -I fixture_selfcheck.py` pasa los 327680 casos por el oraculo y mide la cobertura de ramas.

## Prueba negativa
Dos rondas (`mutate.py`, una mutacion por funcion y ronda, condicionadas a `F174ALive::gBenchThread != 0`; `FN174A_CASES=2048`): **las 20 funciones fallan en ambas** (`negative-runs/`): ronda 1 `20261112-050440-788225` con 27820 de 40960 pares con diferencias, ronda 2 `20261112-050823-864954` con 22014; el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas (SHA256 igual a las copias buenas). Copias buenas: `Heli.cpp` 0a196428..., `MonsterTruck.cpp` 9b22cc22..., `Train.cpp` 78c39123..., `Font.cpp` 7b85be08..., `MenuManager.cpp` 2102ecdb..., `Pad.cpp` d5cdbfad..., `TrainNode.cpp` a2a58910..., `X87Math.cpp` 66670407..., `cHandlingDataMgr.cpp` d2027c69..., `common.cpp` 049fc492...

## Historia de la aprobacion
Tres corridas previas con el banco corregido en sus compuertas, sin tocar lo ya correcto: la 1.a y la 2.a fallaron en `NoDiskInDriveMessage` (la entrada de `DrawFonts` resuelve por un `jmp` a una funcion con hook y `UpdatePads` salta siempre a la DLL) y en `TransformPointsSimple` (125 casos con NaN senalizante); la 3.a dejo solo `TransformPointsSimple`; la 4.a (aprobada) tiene el bucle sin optimizar.

## Bancos antiguos que citan estas direcciones (NO repetidos; pase final del coordinador)
`Dependencies24A` (parchea 0x57C5E0 y la funcion de la DLL), `Fn125A` (0x6C7D10 como colaborador "sin hook": su compuerta exige que no tenga hook), `StatsWrappers60` (0x719240 solo EXE, "no enganchado"), `CtrlConfig58` (0x719360), `CameraFrustum355` (parchea 0x54EEA0 y `TransformVectors`), `VehicleAudio355Evidence` y `World355Evidence` (solo texto), `FontPending` (texto de 0x718E50), `RequestedPadDistanceTrainNop`, `RequestedPadDistanceTrainNopHex`, `RequestedPadDistanceTrainNopHexV2` y `RequestedValidation20261008` (copias de `Pad.h`). Ningun banco cita 0x6C4340, 0x6C4550, 0x6C4810, 0x54ED20, 0x71DB30, 0x6F4FA0, 0x6F5440, 0x6F54D0, 0x6F5530, 0x6F5570 ni 0x6F55C0. Ademas `Fn152A`, `Fn149A` y los bancos de `CPad`/`CHeli`/`CTrain`/`CFont`/`common` recorren codigo que este lote cambia (por ejemplo `TransformPoints`, `DoesInfiniteLineTouchScreen`).

## Tiempo
Una corrida de los 327680 pares: unos 17 min (mas la compilacion incremental de la sonda, de 1 a 10 min segun el reparto de los candados).
