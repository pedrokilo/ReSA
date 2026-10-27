<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPad::ProcessPad (0x00746A10): banco por pares preparado

Estado: preparado, sin compilar y sin ejecutar GTA. No se aprueba ni registra nada. No se ha modificado `source/game_sa/Pad.cpp` (SHA-256 `89badf36b91db868c0a9d41408db7b595bc116ce941a7864cd697af8a4bbaa4d`) ni `Pad.h` (`d18adb99...9889e2`), ni `runtime_runner.py`.

EXE canónico `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Función de 872 bytes (0x746A10..0x746D77, `ret` simple, cdecl). Único llamador: 0x541DDE (`CPad::UpdatePads`), que descarta `eax` (`add esp,4` y sigue).

## Análisis estático (REA combo, dumpbin.asm y bytes)

Evidencia en `evidence/`: `ProcessPad.dumpbin.asm.txt` (combo REA de esta preparación), `ProcessPad-fresh-combo.txt` (volcado previo), `ProcessPad.bytes.bin` (872 bytes), `ProcessPad-constants.json` (constantes de .rdata).

Colaboradores:

| Destino | Llamada | Tratamiento en el banco |
|---|---|---|
| `IDirectInputDevice8::Poll` | `call [vtbl+0x64]` stdcall(this) | dispositivo COM privado con guion de HRESULT, traza |
| `::Acquire` | `call [vtbl+0x1C]`, bucle mientras `eax==0x8007001E` (746A60) | idem, secuencias acotadas |
| `::GetDeviceState` | `call [vtbl+0x24]`(this, 0x110, buf) | copia 272 bytes del estado sembrado a la pila; verifica tamaño y buffer en pila |
| `RsPadEventHandler` 0x6195A0 | cdecl(evento, &padID) x3: 0x20, 0x20, 0x1F | adaptador común EXE/DLL, traza (evento, valor de *param); puede mutar estado entre llamadas |
| `GetPad` 0x53FB70 | cdecl(n) | EXE: código nativo real; candidata: `&Pads[n]` en línea. Ambos sobre el `CPad[2]` global (616 bytes), sembrado, comparado y restaurado |
| FTOL 0x821B40 | `call` tras `fmul 128.0` | código nativo real |

Globals: `RsGlobal.ps` (C17054, redirigido a una arena privada de `psGlobalType` con guardas; diDevice1/2 en +0x20/+0x24), `ControlsManager` B70198 (flag, Old, New: 548 bytes), sticks C8CFB0..BF, `AllValidWinJoys` C92144 (+5 Z, +6 ZRot por entrada de 0x10), menú BA67F9..FE (invertir X1,Y1,X2,Y2, intercambiar 1,2), `CPad::padNumber` B73400, `PCTempJoyState` en pad+0xA8..AF.

## Comparación EXE contra fuente actual

Equivalentes comprobados en el ensamblador: ranuras COM 0x64/0x1C/0x24; bucle de INPUTLOST; reintento Poll tras Acquire; copia Old/New según `bJoyJustInitialised`; constantes 0.0005f (859978), 2^32 (858C54), 0.01f (858C58), 0.017453292f (8595EC), -1.0 (858C1C), 128.0 (858BF4) y 0.3 (874AF8 = double de 0.3F exacto, por lo que `<= 0.3F` equivale); orden de eventos 0x20,0x20,0x1F; relectura de `padNumber` antes de GetPad; relectura de `*pDiDevice` tras el primer evento; índice de `JoyStickNum` leído tras el primer evento; destinos de los 4 ejes y su intercambio (A8/AA, AC/AE).

Diferencias o límites (no se ha tocado la fuente):

1. 746A1F..746A3A: `padID` distinto de 0/1 deja `ebx=0` y `mov eax,[ebx]` desreferencia NULL (fallo del EXE); la fuente devuelve sin hacer nada. Fuera de dominio (llamador pasa PAD1).
2. Valor de retorno: 746D6E/746D70 devuelve 0 o el HRESULT de la llamada fallida (746A6F, 746A81, 746A9B); la fuente es `void`. Irrelevante para el único llamador; se registra solo como informativo. `WIN_FCHECK` añade además un log de depuración.
3. 746AA1 `cmp byte [B70198],1`: el EXE compara con exactamente 1; la fuente usa `bool`. Iguales para booleanos canónicos (el banco sólo siembra 0/1).
4. **POV 746B32..746B44**: el EXE hace `fild` con signo y, si es negativo, `fadd` de 2^32 **bajo la precisión de control x87 (24 bits en el juego)**, y después multiplica. La fuente carga `double pov = DWORD` y `fld pov`, exacto. Para POV >= 0x80000000 con palabra baja distinta de 0xFFFF (nunca ocurre con hardware real: 0..35999 o 0xFFFFFFFF), el argumento de `fsin/fcos` puede cambiar. Con el modelo independiente se estima que ~23% de esos valores aleatorios cambian el seno bajo PC24; el corpus incluye 8 valores concretos y se predicen 128 pares divergentes si la candidata compila como indica la fuente. Corrección mínima propuesta (no aplicada): leer el POV como `LONG` y hacer en `__asm` `fild`, `test/jge` y `fadd dword ptr [2^32 float]` antes de los dos `fmul`.
5. 746C09 y equivalentes: el EXE salta cuando `fcomp` es no ordenado (NaN); `fabs(pos) <= 0.3F` en la fuente no retorna con NaN. Inalcanzable porque los sticks proceden de `fild*0.0005` (finitos); excluido del corpus (floats finitos).

6. **Puerta `*pDiDevice` 746B00..746BA6 (hallazgo de la corrida 20261009-010223-123803, 622 pares, todos `flip==4`)**: en el EXE, `mov eax,[ebx]; test eax,eax; je 746BA6` solo salta el bloque de sticks (POV/Z/RZ, 746B08..746BA0); los eventos 0x20 y 0x1F, `GetPad` y el procesado de ejes (746BA6..) siguen ejecutandose con el dispositivo anulado (3 eventos, escrituras en `PCTempJoyState` con los sticks sin tocar). La fuente envuelve todo ese tramo dentro de `if (*pDiDevice)` (Pad.cpp ~371) y no emite los eventos 2 y 3 ni escribe `PCTempJoyState`. `model.py` modelaba la fuente (retorno inmediato) y se corrigio a lo que hace el EXE: con el modelo corregido el original coincide en 17.256/17.256 y la candidata falla exactamente esos 622. Correccion minima propuesta (no aplicada): cerrar el `if (*pDiDevice)` justo tras el bloque Z/RZ (antes de `padID = padNumber ? PAD2 : PAD1;`) y dejar el resto incondicional.

Los puntos 4 y 5 requieren confirmación contra el código compilado (el compilador puede dar una secuencia distinta de la leída en la fuente).

## Corpus

17.256 pares (17.280 filas, 24 duplicados exactos eliminados), generados por `generate_fixtures.py` desde cero: rutas de dispositivo (11), flips de efectos laterales (4), PC24/PC53, 64 combinaciones de menú, 4 máscaras de joystick, 12 vectores curados, 22 valores frontera por eje (599/600/601, INT_MIN...), 41 valores POV, 1.536 vectores pseudo-aleatorios (LCG documentado en `corpus-plan.json`). `model.py` es un modelo independiente de la candidata (aritmética racional con redondeo a 24/53 bits) que calcula el payload completo y `eax` esperados; el analizador compara original, candidata y modelo (tolerancia de 1 ulp de seno/coseno solo en filas con POV activo, contada aparte).

## Ejecución (solo coordinador, en serie)

```
python -B reversing/RequestedPadProcessPad/generate_fixtures.py   # ya ejecutado; determinista
python -B reversing/RequestedPadProcessPad/make_freeze.py         # tras cualquier edición
python -B reversing/RequestedPadProcessPad/build_probe.py         # compila; guarda y restaura WinMain y binarios
python -B reversing/RequestedPadProcessPad/inspect_compiled.py    # PE/PDB RSDS y ASM
# revisar a mano y escribir Diagnostics/compiled-runtime-proof.json (ver scope.json)
python -B reversing/RequestedPadProcessPad/run_local.py
```

`run_local.py` exige el informe externo con `passed`, hashes ASI/PDB, `event_call_paths_verified` (la candidata llama a RsPadEventHandler por la entrada/cuerpo parcheados) e `inline_getpad_and_float_paths_reviewed`. No crear ese informe con valores literales sin revisar el binario real.

## Supuestos a revisar al compilar

- `static_assert` de disposición: `sizeof(psGlobalType)==0x28`, `diDevice1/2` en 0x20/0x24, `PCTempJoyState` en 0xA8, `CMenuManager::m_bInvertPadX1` en 0xB1 y `m_bSwapPadAxis2` en 0xB6, `JoyStruct` +5/+6, `m_OldJoyState` +4 y `m_NewJoyState` +0x114.
- `RsPadEventHandler` debe tener hook Simple único en 0x6195A0 (si no, `helpers_known=false` y no se ejecuta ningún par).
- Si MSVC integra `RsPadEventHandler` en `ProcessPad`, el parche del cuerpo no intercepta y el informe de llamadas lo debe detectar.
- `offsetof` sobre clases no estándar produce avisos, no errores, en MSVC.
- El número de rangos del preflight (`snapshot_ranges`) no se fija; el analizador exige solo `changed_ranges==[]`.
- Comprobar que `Pads` globales sembradas con patrón no dispara código ajeno durante el banco (se ejecuta al inicio de WinMain, hooks inactivos).
- Alcance: ningún crédito para DirectInput/COM, RsPadEventHandler, GetPad, FTOL, integración, jugabilidad ni vigencia de dependencias.
