<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies24B (grupo g24b de BRIEF_21): WinInput::diPadInit

PREPARADO, sin compilar en el arbol real ni ejecutar dentro del juego: la sonda debe construirla y correrla el coordinador (en serie, clon de pruebas). No constituye validacion hasta entonces.
Evidencia previa sin juego (sin tocar el directorio `build\`): `offline/` mapea los bytes REALES del EXE en un proceso hijo y los compara con el texto de la funcion extraido de `WinInput.cpp`:
2048 pares, 0 diferencias en tabla de pads, punteros de dispositivo, traza completa de colaboradores y contrato de pila/no volatiles; `run_local.py --analyze` valida esa corrida contra el oraculo
independiente. Con dos errores sembrados a proposito (dwData de residuo a cero y 0x21 en lugar de 0x20) el mismo arnes da 1706 diferencias: el banco tiene dientes.

| Funcion | Entrada | Convencion | Pares |
| --- | --- | --- | --- |
| WinInput::diPadInit (`_InputInitialiseJoys`) | 0x7485C0 | cdecl, sin argumentos, void; el EXE guarda/restaura EBX; usa EAX/ECX/EDX | 2048 |

Hook: `RH_ScopedGlobalInstall(diPadInit, 0x7485C0)`, categoria `Win`, espacio `Input` (fila `Input,diPadInit` del catalogo; `registrations.csv` ya la lista con la clase `Input`). Hook apagado para el lado
original (EXE) y encendido para el candidato (`OriginalDependencies499/HookRoute.hpp`, ruta `("Input","diPadInit",0x7485C0,&WinInput::diPadInit)`).

IMPORTANTE (SDL3): la configuracion actual de `build\` tiene `GTASA_USE_SDL3=ON`, y con ella `Win32InjectHooks` no llama a `WinInput::InjectHooks` (todo WinInput queda sin enganchar). El parche `g24b.patch`
registra `diPadInit` en una funcion aparte, `WinInput::InjectPadHooks`, que `Win32InjectHooks` llama siempre; sin ese cambio `Registered()` seria falso y el banco no podria correr.

## Que hace la funcion (desensamblado en `diPadInit.dis.txt`)
1. Borra los dos registros de `AllValidWinJoys` (0xC92144, 2 x 0x10): dword +0 (wDeviceID) y los bytes +4, +5, +6 (conectado, eje Z, eje RZ). NO toca wVendorID/wProductID (+8, +0xC) ni el byte +7.
2. `call 0x7477C0` (`_InputAddJoys`, EXE sin reconstruir: EnumDevices con el callback 0x747020 y 0x746D80 por pad); el resultado se ignora.
3. Pad 1 (`RsGlobal.ps->diDevice1`, +0x20): conectado = 1; `GetCapabilities` (vtable +0xC) con `dwSize = 0x2C` (resultado ignorado); `GetProperty(DIPROP_VIDPID = 0x18)` (vtable +0x14) con un DIPROPDWORD
   {0x14, 0x10, 0, 0} en la pila; guarda LOWORD(dwData) en wVendorID y HIWORD en wProductID; `InitDefaultControlConfigJoyPad(0x20)` sobre `ControlsManager` (0xB70198). La constante 0x20 NO es dwButtons.
4. Pad 2 (`diDevice2`, +0x24): `GetProperty` con el MISMO DIPROPDWORD de la pila (cabecera y dwData tal como los dejo el pad 1, o restos de pila si no hay pad 1); vendor/producto; conectado = 1.
5. psGlobal se relee donde lo hace el EXE: una vez al entrar, otra tras GetCapabilities y otra tras InitDefaultControlConfigJoyPad; el puntero de cada dispositivo se relee antes de cada llamada.
HRESULT de `GetCapabilities`/`GetProperty` no se examinan: si `GetProperty` falla o no escribe, el EXE guarda en la tabla el resto de pila que habia en `dwData` (y, sin pad 1, usa una cabecera de restos).
La reconstruccion copia esos cinco dwords de [R-0x40, R-0x2C) (R = direccion de retorno del EXE) al entrar, con `_AddressOfReturnAddress()`, antes de que el marco de la DLL los pise; ver `g24b-classification.md`.

## Que se compara (en bits, original == candidato == oraculo)
- Por lado: tabla final de 0x20 bytes, ids de diDevice1/diDevice2 (0 nulo, 1 = A, 2 = B), numero de trampas de vtable (debe ser 0) y la traza completa de colaboradores: 0x7477C0 (tipo 1), GetCapabilities
  (tipo 2: id, dwSize, puntero en pila), GetProperty (tipo 3: id, puntero GUID = 0x18, puntero en pila, los cinco dwords del DIPROPDWORD tal como los ve el falso), InitDefaultControlConfigJoyPad (tipo 4: ECX = 0xB70198,
  argumento 0x20); cada entrada lleva el hash FNV de la tabla en ese instante (fija el orden de las escrituras respecto de las llamadas).
- ABI: ESP restaurado, EBX/ESI/EDI/EBP conservados (`stack_restored`, `nonvolatile_preserved`), ruta (`route_ok`: candidato salta a la DLL, original no), x87 completo (FNSAVE de 108 bytes con los ocho registros,
  incluidos los vacios, salvo FIP; MXCSR). `kCompareDeadX87Registers = true` y NO se relaja: diPadInit no ejecuta ninguna instruccion x87 (ni el EXE ni el C++ SSE de la DLL), asi que el estado x87 debe llegar intacto
  (los registros ST se cargan antes de cada llamada con un residuo conocido). No hay helpers X87Plant*/Ftol2RegsCore que plantar: se documenta en `g24b-classification.md`.
- EAX/ECX/EDX al volver solo se registran (`original_residue`, `candidate_residue`): el unico llamador del EXE (0x748813) los sobrescribe antes de leerlos.

## Colaboradores y falsos (sin abrir dispositivos reales)
- `0x7477C0`: funcion del EXE sin reconstruir que la DLL llama por direccion -> un solo EntryPatch (`StubEnum`). Deja en diDevice1/diDevice2 (A, B, nulo, A=B, B/A...) segun `dev_mode`.
- `0x530B00` `CControllerConfigManager::InitDefaultControlConfigJoyPad` (hook RH): EntryPatch en la direccion del EXE y en el cuerpo de la DLL (`Resolve`, que salta los `E9`), mismo falso (`StubInit`, `__fastcall`).
  El cuerpo de la DLL se calcula ANTES de crear los parches. No se llama a la funcion real: `ControlsManager` no se modifica.
- IDirectInputDevice8 falsos (A y B con vtable propia de 40 ranuras): solo GetCapabilities y GetProperty responden; el resto cuenta una trampa. HRESULT por caso entre S_OK, S_FALSE, E_FAIL, E_NOTIMPL, E_OUTOFMEMORY, E_INVALIDARG,
  DIERR_NOTACQUIRED, DIERR_INPUTLOST, DIERR_OBJECTNOTFOUND, 0x7FFFFFFF, -1 y 0x80000000. Los falsos no leen el buffer de GetCapabilities salvo `dwSize`.
- Mutadores (8 variantes): un metodo falso cambia diDevice1/diDevice2 en pleno vuelo (p. ej. GetCapabilities del pad A pone diDevice1 = B; GetProperty de A pone diDevice2 = nulo/B; InitDefaultControlConfigJoyPad pone
  diDevice2 = nulo/A) para comprobar que la DLL relee los punteros exactamente donde el EXE.
- `GetProperty` modo 0 (escribe dwData si SUCCEEDED), 1 (siempre), 2 (nunca), 3 (SUCCEEDED y corrompe dwObj de la cabecera, que hereda el pad 2).

## Estado guardado y restaurado
Tabla `AllValidWinJoys` (0x20 bytes) y los punteros `diDevice1`/`diDevice2` de `RsGlobal.ps` se guardan al empezar, se fijan por caso y se restauran tras cada lado y al final (`globals_restored`, evento `global_state_final`
con tabla y punteros iguales a los iniciales). `diInterface`, `diMouse` y `RsGlobal.ps` no se tocan; nunca se llama a Release/Acquire. Vigilados 16 bytes de las entradas 0x7485C0, 0x7477C0, 0x530B00 y del cuerpo de la DLL de
InitDefaultControlConfigJoyPad (deben quedar intactos) y los hooks (`HookRoute::Restore`).

## Fixtures (Fixtures.hpp; fixture_oracle.py las repite)
2048 casos: `dev_mode` (6) x `mutator` (8) con periodo 48, HRESULT, valores VID/PID (0, 0xFFFFFFFF, 0x80000000, 0x0001FFFF, 0xFFFF0000...), 5 tipos de tabla inicial (ceros, 0xFF, aleatoria, configuracion plausible con
wDeviceID/vendor/producto previos y byte de relleno +7, 0xA5) y semilla de pila por caso (las 8 primeras: 0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000, 0x9E3779B1, 0x55555555). La semilla rellena 64 dwords bajo el ESP
(`SlotValue = semilla ^ k*0x9E3779B1`) antes de la llamada; el oraculo calcula los cinco dwords de residuo que debe ver GetProperty (ranuras 17..13).

## Compuertas de run_local.py
Igualdad fila a fila (probe_case.analyzer), hook registrado y cuerpo en el ASI, colaboradores sustituidos, ABI/ruta/x87 en ambos lados, `fp_difference` vacio, fixture == oraculo, salida de CADA lado == oraculo (tabla, ids,
traza), cero trampas, cobertura minima (modos, mutadores, tablas, modos de GetProperty, los 12 HRESULT en ambos metodos, >= 150 restos de pila guardados por pad, >= 250 casos con cabecera de restos, >= 300 mutaciones
efectivas) y restauracion final. `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.

## Ejecucion
```
python -B reversing/Dependencies24B/build_probe.py
python -B reversing/Dependencies24B/run_local.py
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies24B
```
Sin juego (compila contra los bytes del EXE; no toca `build\`): `reversing/Dependencies24B/offline/bld.bat` y despues `offline\obj\dipad.exe 2048 salida.jsonl` y `python -B run_local.py --analyze salida.jsonl`.
Antes: aplicar `g24b.patch` (y, opcionalmente, `g24b-status.patch`, que actualiza los contadores generados de las cabeceras de `WinInput.cpp` y `app_input.cpp`), anadir la fila de `g24b-hooks.csv.txt` a docs/hooks.csv y compilar.
