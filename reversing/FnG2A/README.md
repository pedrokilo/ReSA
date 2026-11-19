<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote G2A: revisión de las 118 funciones excluidas del código del juego

Cada fila de [docs/JuegoExe/excluidas.csv](../../docs/JuegoExe/excluidas.csv) queda **cerrada con una categoría y una evidencia** obtenida por análisis estático del EXE original (SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`). No se ejecutó el juego ni se compiló nada.

## Resumen por categoría (118)

| Categoría | Funciones | Tratamiento |
| --- | ---: | --- |
| `usercall-cubierta` | 51 | Cerrada: todas sus llamadoras directas están reconstruidas en C++ con `validated=1` (o son otras excluidas ya cubiertas, o funciones pequeñas que el mapa omite, ver abajo). Sin hook propio. |
| `usercall-renderware` | 17 | Pasan al [plan de validación de RenderWare](../../docs/RenderWareExe/plan-validacion.md) (módulos `RenderWare (rwtexdict)` y `RenderWare (D3D9 raster)`). No se trabajan aquí. |
| `usercall-parcial` | 0 | — (la única, 0x00746480, se cerró en el lote G3A como `sustituida-por-crt`) |
| `sustituida-por-crt` | 1 | 0x00746480 (separador de la línea de comandos): lo sustituye el CRT (`__argv`) en `NOTSA_WinMain`; evidencia en [reversing/FnG3A](../FnG3A/README.md) (`--check` la revalida) |
| `usercall-sin-llamadores` | 0 | — |
| `salto-puro` | 20 | Cerradas: un solo `jmp` (E9) a una función ya reconstruida y validada (destino con nombre y estado en el JSON). |
| `proteccion-descartada` | 18 | Se descartan: 17 bloques de `ExeProtectionStub` (0x4027CD–0x403D07) y el bloque anti-manipulación de `VideoPlayer` 0x746800. |
| `no-es-funcion` | 11 | Salen de la cuenta de «funciones a reconstruir»: 9 funclets de `catch(...)` de STL, 1 fragmento interior de 0x4C4910 y la cola de 0x4B5AC0. |
| `otra` | 0 | — |

Las cifras anteriores son las del CSV (`categoria`) y las del JSON (`por_categoria`); `--check` comprueba que cuadran.

Antes se decía «73 `__usercall`, 31 saltos y 14 fragmentos»: esa clasificación salía del texto del motivo (por ejemplo, los bloques del protector y 0x746800 contaban como `__usercall`). Las cifras reales por inspección son las de la tabla.

## Archivos

| Archivo | Contenido |
| --- | --- |
| `inspect_excluidas.py` | Herramienta (Python 3 + `capstone`). Genera la evidencia, la revalida (`--check`) y rellena el CSV (`--write-csv`). |
| `excluidas-evidencia.json` | Una entrada por función con toda la evidencia. |
| `check.out` | Salida de `--check` contra el EXE (código de salida 0). |

## Uso

Desde la raíz del repo (`pip install capstone`):

```
python -I reversing/FnG2A/inspect_excluidas.py <gta_sa.exe>               # genera excluidas-evidencia.json
python -I reversing/FnG2A/inspect_excluidas.py <gta_sa.exe> --write-csv   # además escribe categoria,tratamiento,evidencia en el CSV
python -I reversing/FnG2A/inspect_excluidas.py <gta_sa.exe> --check       # revalida el JSON contra el EXE; código 0 si todo coincide
```

El EXE puede ser el de `E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe` o la copia que usan los bancos (mismo SHA256). Opciones: `--input` (CSV de entrada con `va,bytes,modulo,motivo`) y `--out` (JSON).

## Formato de la evidencia

`excluidas-evidencia.json`: `generado`, `exe_sha256`, `entrada`, `total`, `por_categoria`, `funciones_no_listadas_en_el_mapa` y `funciones` (una entrada por función). Campos de cada entrada:

- Comunes: `va`, `bytes`, `modulo`, `motivo_original`, `categoria`, `tratamiento`, `hex` (primeros 64 bytes; `hex_completo` indica si son todos), `sha256_cuerpo`, `desensamblado` (capstone), `plugin`, `referencias_directas` (call/jmp rel32 con su función contenedora), `referencias_datos` (dwords en datos e inmediatos en código), `hooks_csv_relacionadas` (filas de `docs/hooks.csv` de la función, los destinos y las llamadoras), `validada_por_inspeccion` y `criterio_validacion`.
- `salto-puro`: `salto` (opcode, destino, nombre, estado de hooks.csv, registro de validación), `destinos`, `vtable` (inicio estimado y ranura si hay referencia en datos).
- `no-es-funcion`: `padre` (dirección, nombre, estado, distancia), `saltos_internos_del_padre_al_bloque`, `llega_por_continuacion_desde_el_padre`, `referencias_directas_externas_al_padre` (debe estar vacía), notas.
- `proteccion-descartada`: `marcadores` (pushal/popal, bandera de acarreo, referencias a `_TEXT_HA`, autodirección como constante...) y `referencias_fuera_del_bloque_protector_0x402000_0x403FFF`.
- `usercall-*`: `llamadoras` (sitio, tipo, función, nombre, `estado`, `validated`, `registro` de validación) y `cubierta_por_llamadoras_validadas`.

Estados de una llamadora: `validada` (fila de hooks.csv con `reversed=1` y `validated=1`; los virtuales se buscan por nombre porque tienen dirección 0), `hook-sin-validar`, `sin-hook`, `cubierta-sin-catalogo` (en `cubiertas-sin-catalogo.csv`, sin validar), `excluida-usercall` (otra excluida: vale si ella misma resulta cubierta, por punto fijo), `no-listada-en-el-mapa` y `recursiva`.

### Funciones que el mapa omite

El mapa de Ghidra no lista algunas funciones pequeñas entre dos conocidas (por ejemplo 0x5C4730, 0x5C6030, 0x745E50). Si una llamada cae en un hueco de relleno (`nop`/`int3` tras `ret`/`jmp`), la herramienta crea un nodo `sub_XXXXXXXX` y lo resuelve por sus propias referencias (`funciones_no_listadas_en_el_mapa`): una dirección tomada como inmediato dentro de una función validada (típico de los `jinit_*` de libjpeg, que instalan los métodos) cuenta como cubierta, porque esa función ya no existe en el EXE; una función sin ninguna referencia nunca se ejecuta. **Aviso:** 0x5C4730 (36 bytes, llamadora de 0x5C4640) no tiene ninguna referencia (ni llamada, ni dato, ni inmediato): es código muerto o se llama por una ruta que el análisis estático no ve.

## Cómo se decide cada categoría (plugins)

1. `proteccion-descartada`: módulo `ExeProtectionStub` o 0x746800.
2. `salto-puro`: el cuerpo entero es un `E9`, `EB` o `FF 25` y el destino es un inicio de función conocido (hooks.csv, mapa) o una importación.
3. `no-es-funcion`: el motivo dice «no es una función» y la comprobación pasa: sin llamadas externas al padre, y el padre salta al bloque, llega por continuación, lo referencia la tabla de excepciones o usa el marco `[ebp-N]` del padre.
4. `usercall-renderware`: dirección en 0x750000–0x81FFFF o módulo RenderWare/rwtexdict/D3D9.
5. `usercall-cubierta` / `-parcial` / `-sin-llamadores`: por las llamadoras (arriba). Referencias de dirección en datos (vtables) sin resolver dejan la función en `parcial`.
6. `otra`: cualquier fila que no encaje o cuya comprobación falle; no se fuerza una categoría.

## Cómo registrar una función «validada por inspección»

1. Añade la fila `va,bytes,modulo,motivo` al CSV de entrada.
2. Ejecuta la herramienta con `--write-csv`. Si el plugin la reconoce y su comprobación pasa, la entrada sale con `validada_por_inspeccion: true` y el `criterio_validacion` de su categoría; si no, queda `otra` (o `usercall-parcial`) y hay que revisarla.
3. Ejecuta `--check` y guarda la salida (`check.out`).
4. Regenera los informes que leen el CSV (`contrib/game_function_map.py`, `contrib/progress_report.py`).

La validación por inspección **no** toca `docs/hooks.csv` ni el registro de validaciones: las excluidas no están en el catálogo de hooks.

## Reutilización para los thunks de RenderWare

La herramienta es genérica: una categoría es un plugin (`match`, `inspect`, `check`) y las entradas no dependen del CSV de juego. Para ImportThunks, DelayLoadThunks y QTML: pasa `--input` con las filas (`va,bytes,modulo,motivo`) y `--out` con otro JSON; `SaltoPuro` ya resuelve `jmp [IAT]` (`FF 25`) con el nombre de la importación (`dll!función`). Para formas distintas (carga retardada con `mov eax,id; jmp`) basta añadir un plugin nuevo a `PLUGINS`. `--check` revalida bytes, SHA256 del cuerpo, desensamblado, destino del salto y las referencias contra el EXE.

Qué cubre `--check`: el EXE (SHA256), los bytes y el desensamblado de cada función, el destino de los saltos, las referencias (llamadas y datos) y que el CSV y el JSON coinciden. **No** vuelve a evaluar el estado `validated` de las llamadoras (depende de hooks.csv, que cambia): para eso se regenera el JSON.
