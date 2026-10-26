<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# D3DResource63: GetTotalIndexDataSize y GetTotalPixelsSize (D3DResourceSystem)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wf-1`, HEAD 35e7b9d4b + fuente del lote 63). 8192 pares (2 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados y salida de cada lado igual a la del oráculo independiente; el estado real del bloque de caches D3D (0xC87C60..0xC87FE8) quedó idéntico al inicial. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-d3dresource63\20261020-015047-286873`.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `D3DResourceSystem::GetTotalPixelsSize` | 0x730660 | Simple | 4096 | `D3DTextureBuffer::GetTotalDataSize` (0x7300A0) |
| `D3DResourceSystem::GetTotalIndexDataSize` | 0x7307F0 | Simple | 4096 | `D3DIndexDataBuffer::GetTotalDataSize` (0x7303B0) |

Ninguno de los dos colaboradores tiene hook en la DLL (la DLL los llama por la dirección del EXE con `plugin::CallMethodAndReturn`), así que basta el parche de entrada del EXE.

## Fuente (D3DResourceSystem.cpp)

- `GetTotalPixelsSize` (0x730660..0x730694): `ebx` = `NumTextureBuffers` (una sola lectura), `esi` = 0xC87C68 + n·0x1C con aritmética de 32 bits (sin comprobar el rango); si n == 0 no hay llamadas; si no, bucle `sub esi,0x1C` / `call 0x7300A0` / `add edi,eax` / `dec ebx` / `jne` (contador sin signo). Devuelve la suma (32 bits con vuelta).
- `GetTotalIndexDataSize` (0x7307F0..0x730822): `esi` = 0xC87FC8; cuerpo `sub esi,0x18` / `call 0x7303B0` / `add edi,eax` hasta `esi == 0xC87E48` (16 buffers, del último 0xC87FB0 al primero), y después el genérico (0xC87FC8). Devuelve `genérico + suma`.
- Nada más escribe el EXE: ni globales ni argumentos. **Ningún llamador directo** (ni `call`/`jmp` rel32 ni la dirección como dato en el EXE): código muerto en el binario; por tanto EAX se compara (es la salida de la función) y ECX/EDX solo se registran.

## Qué compara

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con **solo** ese hook encendido. Por lado: eventos de los falsos EN ORDEN (`[tipo, ECX, retorno]`), `nevents`, cambios en el bloque de globales respecto de la imagen del caso (la imagen es basura salvo la palabra 0 = contador; el falso de la llamada k puede reescribir la palabra 0, el campo size del genérico o la palabra 100) y EAX. Compuertas de ABI: ESP, EBX/ESI/EDI/EBP, DF, cadena SEH, x87 normalizado (FCW 24 bits, pila vacía), ruta del hook, entradas de los colaboradores sin tocar, estado real del bloque de globales restaurado antes de cada `Write`.

Fixtures (`Fixtures.hpp`, 12 familias): contador 0, 1, 2..16, 17, 18..64, 65..512; valores del falso en 8 modos (0, 1, pequeños, 32 bits aleatorios, todo unos, 0x80000000, alternado, mezcla) con sumas que dan la vuelta; reentrada (el falso k reescribe el contador / el size del genérico / otra palabra, para comprobar que no se relee). `fixture_oracle.py` modela las dos funciones desde el desensamblado (autoprueba con `python -I fixture_oracle.py`); `fixture_mirror.py` compila `Fixtures.hpp` con `cl` sin el juego, pasa los 8192 casos por el oráculo, fija las constantes de disposición y comprueba que `RetVal` de C++ coincide con `ret_val` de Python. Cobertura mínima calibrada en `run_local.py` (≈50 % de lo medido).

## Revisión propia contra el EXE (fase 2)

Se desensambló de nuevo `0x730660..0x730694`, `0x7307F0..0x730822` y los colaboradores `0x7300A0` / `0x7303B0` y se contrastó línea a línea con el C++ nuevo. Discrepancias: ninguna en la fuente. Detalles contrastados: orden de llamadas (último buffer primero), el contador de texturas leído una vez, `dec`/`jne` sin signo, puntero inicial `0xC87C68 + n·0x1C` en 32 bits, 16 buffers del 15 al 0 y luego el genérico, `ret` sin limpieza de pila en ambos colaboradores (thiscall con ECX = buffer). Los colaboradores se declaran como ayudantes locales (`TextureBufferGetTotalDataSize`, `IndexDataBufferGetTotalDataSize`) porque `D3DTextureBuffer::GetTotalDataSize` / `D3DIndexDataBuffer::GetTotalDataSize` están declarados pero sin definición en C++.

## Suposiciones y límites

- Los cuerpos reales de los dos colaboradores (que llaman a `GetLevelDesc`/`GetDesc` por la vtable COM) no se validan aquí.
- No se prueba un contador ≥ 2^31 ni valores que obliguen a recorrer miles de millones de buffers (el bucle es `dec`/`jne` sin signo; con n > 512 solo se llega a 512).
- El dispositivo y las texturas D3D reales no se tocan: solo se escriben palabras del bloque de caches, restauradas justo después.

## Comandos

```
pwsh -NoProfile -File D:\Original\wf-1\reversing\CVehicle\build-project.ps1
bash reversing/_handoff/bench_here.sh D3DResource63        # compila la sonda y corre en el juego (candado global)
python -I reversing/D3DResource63/fixture_oracle.py        # autoprueba del oráculo
python -I reversing/D3DResource63/fixture_mirror.py        # coherencia de las fixtures + cobertura mínima (sin el juego)
python -B reversing/D3DResource63/run_local.py --analyze <comparisons.jsonl>
python reversing/D3DResource63/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
