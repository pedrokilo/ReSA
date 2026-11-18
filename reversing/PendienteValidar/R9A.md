<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11652 con evidencia histórica y 789 pendientes de validación en 38 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote R9A: reconstruccion sin validar

Rama `reconstruccion-sin-validar` (base con R1A-R8A y R11A), worktree `wv-7`. Treinta y dos direcciones del encargo, atribuidas por vecindad a RpMatFX con confianza baja. **Ninguna es de RpMatFX ni de RenderWare**: son puentes (`jmp [ranura de la IAT]`) hacia vorbisfile.dll, WS2_32.dll y DDRAW.dll (carga retardada), y 17 puentes de QuickTime/QTML. **No hay banco**: no se ha ejecutado el juego ni se ha tocado `approvals.json`, `validation-registry` ni `gta_lock`. En `docs/hooks.csv` entran con `validated=0`. Ninguna direccion estaba enganchada ni envuelta en la rama (`git grep` y `docs/hooks.csv`).

Los bytes se leyeron del EXE (`gta-sa.exe`, sha256 72ae59e4...) y los destinos de cada `jmp` se resolvieron contra la tabla de importaciones (nombres y ordinales). Las firmas salen de los llamadores del EXE (0x5024D0 para vorbisfile, 0x7455E0 para DirectDrawCreateEx) y de las firmas publicas de vorbisfile / Winsock.

## Funciones reconstruidas (15)

| Direccion | Nombre | Bytes | Tipo | Archivo | Riesgos que debe mirar el banco |
| --- | --- | ---: | :---: | --- | --- |
| `0x0081805C` | `DirectDrawCreateExThunk` | 6 | S | `source/app/platform/win/DelayLoadThunks.cpp` | `jmp [0x8E3EBC]` (carga retardada de DDRAW.dll `DirectDrawCreateEx`; antes de la primera llamada la ranura apunta al ayudante 0x818041). stdcall de 4 argumentos `(guid, ppDD, iid, punkOuter)`; llamador 0x7455E0 con `(NULL, &out, 0x859694, NULL)` y sin limpieza posterior. Mirar el HRESULT y que el ayudante de carga retardada siga funcionando al pasar por la ranura. |
| `0x00818062` | `OvClearThunk` | 6 | S | `source/app/platform/win/ImportThunks.cpp` | `jmp [0x858360]` = vorbisfile `ov_clear(vf)`, cdecl, 1 argumento. |
| `0x00818068` | `OvReadThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x85836C]` = `ov_read(vf, buf, len, bigEndian, word, signed, &bitstream)`, cdecl, 7 argumentos, devuelve long. |
| `0x0081806E` | `OvInfoThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858370]` = `ov_info(vf, link)`, cdecl, 2 argumentos; llamador 0x50251B con `link = -1`. |
| `0x00818074` | `OvOpenCallbacksThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x85835C]` = `ov_open_callbacks(datasource, vf, initial, ibytes, ov_callbacks)`; **`ov_callbacks` (4 punteros, 16 bytes) por valor en la pila** (el llamador 0x502509 limpia 0x20 bytes). Comprobar que la estructura llega intacta. |
| `0x0081807A` | `OvTimeTotalThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858364]` = `ov_time_total(vf, i)`, cdecl, devuelve **double en ST0** (x87: el banco debe capturar ST0 completo). |
| `0x00818080` | `OvTimeTellThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858368]` = `ov_time_tell(vf)`, double en ST0. |
| `0x00818086` | `OvTimeSeekThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858374]` = `ov_time_seek(vf, double pos)`, cdecl, el double ocupa 8 bytes de pila. |
| `0x0081809E` | `WsConnectThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858334]` = WS2_32 ordinal 4 `connect(s, name, namelen)`, stdcall, 3 argumentos. |
| `0x008180A4` | `WsInetAddrThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858330]` = ordinal 11 `inet_addr(cp)`, stdcall, 1 argumento. |
| `0x008180AA` | `WsHtonsThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x85832C]` = ordinal 9 `htons(u_short)`, stdcall, 1 argumento (el retorno es de 16 bits en AX; la parte alta de EAX no esta definida por la API). |
| `0x008180B0` | `WsSocketThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858344]` = ordinal 23 `socket(af, type, protocol)`, stdcall, 3 argumentos. |
| `0x008180B6` | `WsCloseSocketThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858328]` = ordinal 3 `closesocket(s)`, stdcall, 1 argumento. |
| `0x008180BC` | `WsSendThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858324]` = ordinal 19 `send(s, buf, len, flags)`, stdcall, 4 argumentos. |
| `0x008180C2` | `WsRecvThunk` | 6 | S | `ImportThunks.cpp` | `jmp [0x858320]` = ordinal 16 `recv(s, buf, len, flags)`, stdcall, 4 argumentos. |

Clase real: `DelayLoadThunks` (la de R8A) para el de DDRAW; clase nueva `ImportThunks` (`ImportThunks.cpp/.h`, `ImportThunksInjectHooks` llamada desde `InjectHooksMain.cpp`) para los de vorbisfile y WS2_32. Todos tipo S (`RH_ScopedGlobalInstall`); ninguno V. Cada cuerpo llama a la funcion real por la ranura de la IAT del EXE (con la misma firma), igual que hace R8A con DirectInput8Create. Riesgo comun: un `jmp` puro conserva todos los registros y la pila de quien llama; el cuerpo en C++ puede dejar distintos valores en ECX/EDX (volatiles), que ningun llamador del EXE lee tras estas llamadas (los llamadores ya son codigo de AEVorbisDecoder / WinsockLink, convertidos o con el contrato de la API).

## Saltadas (17): puentes de QuickTime/QTML

`0x818430, 0x818450, 0x818490, 0x818510, 0x818570, 0x818650, 0x818670, 0x818970, 0x818D70, 0x818DB0, 0x818E70, 0x818F90, 0x818FB0, 0x8191F0, 0x819210, 0x819270, 0x8192B0` (26 bytes cada una). Todas tienen exactamente este cuerpo, que solo cambia el identificador de funcion:

```
mov eax, [0x8E2A08]        ; A1 08 2A 8E 00
or  eax, eax
jz  L
call dword ptr [0x8E2A08]  ; gancho de inicializacion de QTML, sin argumentos
L: mov eax, <ID>           ; B8 imm32
jmp dword ptr [0x8E2A0C]   ; despachador de QTML
```

Motivo: el identificador de funcion llega al despachador por **EAX** (convencion de registros propia de la biblioteca estatica de QuickTime) y los argumentos de la funcion QuickTime, de numero y tamano distintos y desconocidos por puente, pasan por la pila del llamador de forma transparente hasta el `jmp` final (el `ret` lo ejecuta el despachador y limpia sus propios argumentos). Ni EAX como entrada del salto ni un salto puro con pila ajena se pueden expresar en C++ sin `__asm`/`naked` (prohibidos). No se comprobo la firma de cada una (el encargo los describe como ayudantes de decodificacion MP3 de QuickTime usados por AEMFDecoder). Identificadores: 0x250027, 0x250028, 0x25002A, 0x25002E, 0x25003F, 0x1D0008, 0x1D0009, 0x1D002E, 0x150006, 0x150008, 0x15000F, 0x150019, 0x15001A, 0x15002E, 0x15002F, 0x150032, 0x150034 (en el orden de las direcciones).

Direcciones no alineadas a 16: todas las 15 de 6 bytes (0x81805C..0x8180C2) son puentes consecutivos de la tabla de `jmp [IAT]`, no fragmentos; se reconstruyeron. Los huecos 0x81808C, 0x818092 y 0x818098 de esa tabla no estaban en el encargo.
