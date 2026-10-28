<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# D3DResource62: CancelBuffering, CreateIndexBuffer, CreateTexture, DestroyIndexBuffer, DestroyTexture, Init y Shutdown (D3DResourceSystem)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\we-5`, HEAD 0ac292a76 + fuente del lote 62). 28672 pares (7 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados y salida de cada lado igual a la del oráculo independiente; el estado real de las caches D3D y de `_RwD3DDevice` quedó idéntico al inicial. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-d3dresource6261020-002841-524708`. La primera corrida (`20261020-001301-815647`) dio FALLO por dos defectos reales de la fuente (abajo, 7 y 8), corregidos.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `D3DResourceSystem::CancelBuffering` | 0x730900 | Simple | 4096 | COM falso (`Release`), `free` del EXE (0x8214BD) |
| `D3DResourceSystem::CreateIndexBuffer` | 0x7306A0 | Simple | 4096 | COM falso (`GetDesc`), dispositivo falso (`CreateIndexBuffer`), `D3DIndexDataBuffer::Pop` (0x730270) |
| `D3DResourceSystem::CreateTexture` | 0x730510 | Simple | 4096 | dispositivo falso (`CreateTexture`), `D3DTextureBuffer::Pop` (0x72FF60) |
| `D3DResourceSystem::DestroyIndexBuffer` | 0x730D30 | Simple | 4096 | COM falso (`Release`, `GetDesc`), `D3DIndexDataBuffer::Resize` (0x730330) |
| `D3DResourceSystem::DestroyTexture` | 0x730B70 | Simple | 4096 | COM falso (`Release`, `GetLevelCount`, `GetLevelDesc`), `D3DTextureBuffer::Setup` (0x72FE80), `Push` (0x72FFF0), `PushWithoutIncreasingCounter` (0x730AD0), `Resize` (0x730020) |
| `D3DResourceSystem::Init` | 0x730830 | Simple | 4096 | `new` del EXE (0x82119A) |
| `D3DResourceSystem::Shutdown` | 0x730A00 | Simple | 4096 | COM falso (`Release`) |

Los nueve colaboradores del EXE se parchean en las siete funciones (cualquier llamada inesperada queda registrada y rompe la igualdad con el oráculo). Ninguno tiene hook en la DLL: la DLL los llama por la dirección del EXE
(`plugin::CallMethod`, `operator new`/`delete` del proyecto), así que basta el parche de entrada del EXE.

## Qué se sustituye y qué se ejecuta de verdad

- **El dispositivo D3D real no se toca.** Durante cada llamada `_RwD3DDevice` (0xC97C28) apunta a un dispositivo falso (tabla de métodos con `CreateTexture`, ranura 23, y `CreateIndexBuffer`, ranura 27, y trampas en las demás).
  Las texturas y los index buffers de las caches son objetos COM falsos con su propia tabla de métodos (`Release` 2; textura: `GetLevelCount` 13, `GetLevelDesc` 17; index buffer: `GetDesc` 13). Cualquier otra ranura es una trampa registrada.
- **Las caches reales del juego se respetan.** El bloque de globales `0xC87C60..0xC87FE8` (NumTextureBuffers, 17 `D3DTextureBuffer`, 16 `D3DIndexDataBuffer`, el genérico y los dos índices libres), el byte `0x8D6084` y `_RwD3DDevice` se
  guardan justo antes de cada lado, se sustituyen por el estado del caso y se restauran inmediatamente después y **antes** de cualquier `Write`/`PumpMessages` (compuerta `globals_restored` incluye que la restauración coincide byte a byte; el
  evento final `d3d_state_restored` exige que el estado real al terminar sea el del principio). Las pilas de punteros de los buffers viven en la arena del banco (con 8 palabras de guarda a cada lado), nunca en el montón del juego.
- **`new`/`delete` del EXE (Init, CancelBuffering).** Los parches de 0x82119A / 0x8214BD solo actúan con la «ventana armada» (alrededor de la llamada de `Gate`) y en el hilo del banco; fuera de ella (y en cualquier otro hilo) reenvían al
  código real del EXE (`0x824257`/`0x826A57`/`0x8212ED` para `new`, `0x82413F` para `delete`), de modo que ni el propio banco ni otros hilos del juego ven la sustitución. Dentro de la ventana `new` devuelve bloques de la arena (o un valor crudo
  si el caso lo pide: 0, 0xFFFFFFFF, 0xDEAD00xx) y `delete` solo registra.
- **Falsos con estado**, modelados igual en el oráculo: los de `Push`/`PushWithoutIncreasingCounter` replican el cuerpo del EXE (contador, pila, tamaño); `Setup` escribe los siete campos y apunta la pila al bloque 19; los `Resize` cambian la
  capacidad según `rszMode` (0 nada, 1 = nueva capacidad, 2 = 0, 3 = nueva − 1); los `Pop` devuelven el recurso del caso sin sacarlo de la pila; `Release` puede mutar el estado de las caches en el N-ésimo `Release` (reentrada: NumTextureBuffers
  o el tamaño de un buffer), porque el EXE vuelve a leer longitudes y contadores tras cada `Release`.
- **No se prueba** `CreateIndexBuffer` con `numIndices >= 0x80000001` (grupo negativo: el EXE indexa fuera del arreglo de buffers) ni `NumTextureBuffers > 17` con ancho cuadrado (el bucle de búsqueda saldría de `TextureBuffers`).

## Qué compara

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con **solo** ese hook encendido. Por lado: `observed` = eventos de los falsos EN ORDEN con argumentos (punteros como fichas simbólicas `R12`, `B3`, `S5`, `T2`, `I7`,
`IG`, `N0`, `O1.0`, `D`), `changes` (palabras del bloque de globales, de las pilas, de los bloques del `new` falso, de `*pp` y de los objetos COM que cambiaron respecto al estado inicial del caso), byte `0x8D6084` y, en
`CreateIndexBuffer`/`CreateTexture`, EAX (el HRESULT: los llamadores del EXE `0x4CB935`, `0x4CD9FE` y `0x4C997C` lo leen). Compuertas de ABI en ambos lados: ESP, EBX/ESI/EDI/EBP, DF, cadena SEH, x87 normalizado (FCW 24 bits, pila vacía),
ruta del hook, entradas de los colaboradores sin tocar. **ECX/EDX de salida solo se registran**: ningún llamador del EXE los lee (comprobado con `residue_scan.py` y desensamblando los llamadores: `0x53BC21`→`jmp 0x7010C0`, `0x5BD60B`,
`0x53CAEC`, `0x4C99E0`, `0x4CBC1D` no leen EAX/ECX/EDX tras la llamada; `0x4C997C`, `0x4CB935` y `0x4CD9FE` solo leen EAX).

`fixture_oracle.py` modela cada función desde el desensamblado con una sola entrada: la especificación del caso (`input` de cada fila). Lee SOLO palabras que el caso especifica (si una función leyera basura, el oráculo falla) y emite las mismas
fichas. `python -I fixture_oracle.py` ejecuta su autoprueba. `fixture_mirror.py` compila `Fixtures.hpp` con `cl` (sin el juego), pasa las 28672 especificaciones por el oráculo (comprueba que todas son coherentes) y fija las constantes de
disposición del C++ frente a las del oráculo/desensamblado (TextureBuffers en la palabra 2 + 7·i, IndexDataBuffers en la 122 + 6·i, genérico en la 218, 34 pilas de 48 palabras...). Cobertura mínima calibrada en `run_local.py` (≈50 % de lo medido).

## Revisión propia contra el EXE (fase 2)

Se desensambló de nuevo cada función (`dis.sh`) y se contrastó línea a línea con el C++ del borrador del lote 27. Discrepancias halladas y corregidas:

1. `CancelBuffering`: tras `free` el EXE pone a 0 el puntero de la pila (`mov [esi+4],ebx`, `mov [0xC87FDC],ebx`); el borrador no lo anulaba (también en los 16 buffers por tamaño y en el genérico). Corregido.
2. `CreateTexture`: el EXE hace `fild width` + `fstp dword` (ancho redondeado a float) y compara con `fild [buffer+4]` + `fcomp` **sin redondear** el ancho del buffer; el borrador redondeaba ambos a float (coincidencia falsa con anchos > 2^24).
   Corregido: se compara en `double` (`(double)(int32)buffer.width == (double)(float)width`). El banco incluye anchos 0x1000001/0x1000003/0x2000001 con buffers a ±1.
3. `DestroyTexture`: el ancho sin signo se convierte con `fild` + `fadd 2^32` (0x858C54) + un único `fstp dword`; el borrador redondeaba a float antes de sumar (doble redondeo). Corregido: `(float)(double)(uint32)width`. Misma comparación exacta.
4. `CreateIndexBuffer`: `dec ecx`/`imul esi,esi,100` son aritmética de 32 bits con envoltura; el borrador usaba `int` con desbordamiento (comportamiento indefinido para `numIndices` grande). Corregido con `uint32` y conversión.
5. Cabecera: `CreateTexture` recibe cinco argumentos (`width, height, levels, format, ppTexture`; el llamador limpia 0x14 bytes); el borrador de la cabecera original tenía cuatro.
6. Orden de llamadas, relecturas tras cada `Release`/`Resize`, signo de cada comparación (`jl/jge` con signo, `jb/ja` sin signo en los anchos), el `dec` del contador antes de sacar el puntero y los argumentos de `CreateIndexBuffer`/`CreateTexture` del
   dispositivo (Length = índices·2, Usage 8, Format 0x65, Pool 1; Usage 0, Pool 1) coinciden con el EXE.
7. **Defecto hallado por el banco** (`CreateIndexBuffer`): con un buffer devuelto por `D3DIndexDataBuffer::Pop` (grupos de 16 o más, buffer genérico) el EXE salta el `GetDesc` (`0x730714: jne 0x730700`, `xor eax,eax; ret`); el borrador lo llamaba también en esa ruta. Solo la ruta de la cache por tamaño llama a `GetDesc`. Corregido.
8. **Defecto hallado por el banco** (`CreateTexture`, `DestroyTexture`): con `/O2 /fp:fast` el compilador eliminaba el redondeo a float del ancho (`fild` + `fstp dword`) y comparaba el entero exacto (anchos > 2^24 con buffer de ancho `float(ancho)` no coincidían). Corregido con una variable `volatile float` (el `fstp dword` real) y comparación en `double` contra el `fild` exacto del ancho del buffer.

## Comandos

```
pwsh -NoProfile -File D:\Original\we-5\reversing\CVehicle\build-project.ps1
bash reversing/_handoff/bench_here.sh D3DResource62        # compila la sonda y corre en el juego (candado global)
python -I reversing/D3DResource62/fixture_oracle.py        # autoprueba del oráculo
python -I reversing/D3DResource62/fixture_mirror.py        # coherencia de las fixtures + cobertura mínima (sin el juego)
python -B reversing/D3DResource62/run_local.py --analyze <comparisons.jsonl>
python reversing/D3DResource62/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp` (generadores y JSON de los casos, solo C++ estándar), `Core.hpp` (arena, falsos, Gate de ABI, caso), `LiveComparison.hpp` (enganche en `CGame::Process`), `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `run_local.py`,
`build_probe.py`, `register_hook.py`, `source.patch`.
