<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 08/11/2026 (Ciudad de México):** 869 grupos / 9484 funciones reconstruidas; 9484 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn89D: `CFont::PrintSegment` (lote 89D)

Estado: **APROBADO dentro del juego** (03/11/2026, worktree `wh-1`, rama de trabajo wip-4-reconstrucciones + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn89d/20261103-234308-184068` (archivada en `reversing/CppTwelveLevel6Final/runs/`). Banco par a par dentro del juego, 16384 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).
Una funcion de codigo de juego reconstruida en C++ puro y enganchada (tipo S); `CFont::ProcessCurrentString` (0x71A220) ya no la llama por direccion.

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `CFont::PrintSegment` (antes `plugin::Call<0x719B40>` en `ProcessCurrentString`) | 0x719B40 | `CFont` / `PrintSegment` / S | `Font.cpp` |

cdecl, cinco argumentos en pila (`left`, `top`, `begin`, `end`, `spacing`; los limpia el llamador), `void`. Se llama a si misma: la sombra (`m_nFontShadow`, byte con signo) tiene prioridad sobre el contorno (`m_nFontOutlineSize`);
ambos recursan con el color de sombra y `m_bFontIsBlip = 1`, con el valor a cero durante la llamada, y al volver restauran el valor y el color previo y dejan `m_bFontIsBlip = 0`.

## Revision contra el EXE (0x719B40..0x71A0DF)
- 0x719B43: color guardado (dword 0xC71A60) antes de vaciar el buffer; si la textura de estado (word 0xC71ACC) difiere de la de la fuente (byte 0xC71A94) se llama a `RenderFontBuffer` (0x719840) y se iguala. El color que usan los caminos de sombra y contorno se vuelve a leer DESPUES del vaciado.
- Desplazamientos: `dx = (int(0xC17044) * 0.0015625f) * o`, `dy = (int(0xC17048) * 0.0022321429f) * o` (0x859520 / 0x859524): `fild` del entero, `fmul` y `fmul` con la palabra de control del juego (24 bits). **Correccion hallada por el banco:** el fuente original de la reconstruccion pasaba el entero por `float` antes
  de multiplicar; con |ancho| o |alto| > 2^24 el EXE (que carga el entero exacto) redondea distinto (141 pares de 16384 fallaban, todos de las familias 10 y 11). Ahora `FontOffsetCpp` (X87Math.cpp, compilado con x87 y `float_control(precise)`) hace `(double(n) * k) * o` con la x87 y un solo redondeo por operacion.
- Con inclinacion distinta de cero (ordenada: NaN cuenta como distinta) la referencia de inclinacion (0xC71A70/74) suma el desplazamiento y se guarda como float (no se restaura).
- Sombra: una llamada `(left + dx, top + dy)`. Contorno: ocho llamadas `(+dx,-dy) (-dx,-dy) (+dx,+dy) (-dx,+dy) (+dx,0) (-dx,0) (0,+dy) (0,-dy)`.
- 0x719F30: si `pEmptyChar >= 0xC7187C - (end - begin)` (sin signo) se vacia el buffer; cabecera de 0x30 bytes (no se escriben `m_cLetter`, `m_dLetter`, +0x2B ni +0x2D); texto copiado byte a byte, con `ParseToken` (0x718F00) en cada `~`; terminador y alineacion a 4; el color se restaura solo si no es blip.

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (ambos colaboradores tienen hook RH, apagado durante el banco): `RenderFontBuffer` 0x719840 (registra `pEmptyChar`, ejecuta la mutacion del caso y lo reinicia a 0xC716B0 como el original) y `ParseToken` 0x718F00 (registra texto, color, blip y etiqueta; avanza, escribe el color y el simbolo segun el caso).
Memoria real del juego (se rellena y se restaura tras cada lado): `pEmptyChar`, el buffer (relleno de 0xA5) y los estaticos de `CFont` (0xC716A8..0xC71AD0) y los enteros 0xC17044 / 0xC17048.
Se observa: los eventos de los falsos con todos sus argumentos, los bytes cambiados de toda la imagen (en tramos) y los de la arena del texto. La recursion corre de verdad (EXE en el original, DLL en el candidato).
16 familias de 1024 casos: aleatorio, sin efectos, sombra, sombra con inclinacion, contorno, contorno con inclinacion, sombra y contorno, etiquetas `~`, vaciado por textura, vaciado por buffer lleno, valores con signo y enteros enormes, floats especiales (NaN, infinitos, desnormales), mutaciones (en el primer vaciado: color, color de sombra, sombra, contorno, inclinacion, ancho de pantalla), texto vacio o `end < begin`, blip inicial y simbolo, ABI con registros sembrados.

## Fuera de los fixtures
Texto largo que desborde el buffer tras el vaciado, `ParseToken` con avance cero o que devuelva una posicion anterior (el EXE entra en bucle infinito), punteros de texto no accesibles, `begin` y `end` en arenas distintas, NaN con cargas distintas de 0xFFC00000, y los valores de retorno de los falsos.

## Bancos repetidos
`FontPending` y `RadioFont1` citaban 0x719B40 (el llamador pasa a usar el metodo nuevo): adaptados, con el mismo falso en la direccion del EXE y en el cuerpo de la DLL (`PrintSegment`).

## Prueba negativa
Produccion mutada en cinco puntos (`Font.cpp`: llamada 5 del contorno `+dx` -> `-dx`, spacing de la cabecera -> left, alineacion a 4 -> a 2, limite 0xC7187C -> 0xC7187D; `X87Math.cpp` sin tocar en el banco D): 16378 de 16384 pares con diferencias. Restaurado con `cmp`.

## Hallazgos del banco
1) `FontOffsetCpp`: el entero de pantalla no debe pasar por `float` (141 pares). 2) `FontAddCpp`/`FontSubCpp`: la suma/resta se hace en la x87 porque la SSE de la DLL, con el MXCSR del juego (flush-to-zero), vacia a cero un resultado desnormal que la x87 conserva (1 par). 3) dx y dy se recalculan en cada llamada del contorno, como el EXE.
