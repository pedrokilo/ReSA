<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fallo 0xC0000409 al terminar la misión INTRO en el EXE independiente (22/11/2026)

Rama `crash-fin-mision` (base `745cee8725`). Evidencia del usuario: `D:\Original\_evidencia_usuario1` (partida nueva, ~590 s, fallo a las 12:38:19 en `gta_sa_standalone.exe+0x01266AA4`, dirección absoluta 0x01666AA4).

## Causa

Se enlazó el mismo código (`enlazar.cmd` sobre los `.obj` de `wv-int`, `salida\gta_sa_standalone.map`) y se resolvieron las direcciones de la pila del log, que el log nombra mal:

| Dirección | Símbolo real |
| --- | --- |
| 0x01666AA4 / 0x01666A96 | `std::array<unsigned char,208>::operator[]` (`array(525)`, comprobación de rango de la STL en Debug) |
| 0x01666C9E | lambda de `CFont::ProcessCurrentString` / `CFont::RenderFontBuffer` (0x01666C9E está dentro del bucle de impresión) |
| 0x0166C8B9 / 0x0166C942 | `CFont::PrintSegment` |
| 0x0166DAB6 | `CFont::RenderFontBuffer` |
| 0x0166D36F / 0x0166D788 / 0x0166CF15 | `ProcessCurrentString`, `ProcessStringToDisplay`, `PrintString` |
| 0x01796745 / 0x01790461 | `CHud::DrawSuccessFailedMessage` / `CHud::Draw` |
| 0x00FB6778, 0x00FB62BE | `Render2dStuff`, `Idle` |

Al terminar la misión el HUD dibuja el mensaje grande de éxito (`m_BigMessage[0]`, `INT1_GN`...) con `CFont::RenderFontBuffer`. Allí `data->m_propValues[index]` (`std::array<uint8,208>`, `tFontData`) se indexaba con comprobación de rango. El índice (`letter`, tras `FindSubFontCharacter` para los estilos distintos de 0) puede valer 208 o más (por ejemplo, 208 = `m_spaceValue`, el carácter no encontrado). El EXE original **no comprueba el rango**: lee el byte siguiente de la estructura (`m_spaceValue`/`m_unpropValue`) o de la siguiente fuente. En Debug la STL aborta con el `_STL_VERIFY` de `array(525)`: el CRT del experimento lo registra como "CRT ASSERT (sin cuadro, se continua)" y el fallo rápido posterior mata el proceso con 0xC0000409.

Es el mismo patrón que ya estaba resuelto con `.data()` en `CFont::GetCharacterSize` y `GetLetterIdPropValue` (comentados "indexa sin comprobar el rango"): quedaba este tercer acceso.

## Arreglo

`source/game_sa/Font.cpp` (`RenderFontBuffer`): `data->m_propValues[index]` pasa a `data->m_propValues.data()[index]`, como el original (lectura sin comprobar, mismo valor). Un solo cambio, C++ puro, sin hooks nuevos.

## ASI

El ASI Debug compila el mismo `Font.cpp` con la misma STL comprobada, así que el defecto es **propio del port** (no del standalone): en el ASI con el EXE original también salta el assert de `array(525)` al indexar con un valor >= 208 (en Release no se comprueba y lee memoria vecina, igual que el original). No se pudo ejecutar el ASI aquí (sin árbol de build en el worktree); es una conclusión por el código compartido.

## Verificación

* Reproducción dirigida (`reversing/FnFase8/repro_barra_fuente.py`): tras `gGameState=9` +25 s escribe en `CHud::m_BigMessage[0]` (0xBAACC0) un texto con los caracteres 0x20..0xFF, que `DrawSuccessFailedMessage` dibuja cada fotograma. Con el EXE del usuario: `SALIO exit=0xC0000409` y `array subscript out of range` en el log. Con el EXE corregido: vivo a los 220 s, 0 aserciones.
* Build: `Font.cpp` recompilado con `compilar_over.cmd` y reenlazado con `enlazar.cmd` (`_build_CRASH.sh`, candado `compile_lock`, `EXIT=0/0`). No hay árbol de build completo del ASI en el worktree, de ahí el solape `over\Font.obj` (la corrección es un cambio de una línea que no afecta al resto de `.obj`).
* `python contrib/audit_hook_catalog.py --strict`: OK.
* No se ejecutó la misión INTRO completa (~590 s con control) ni los humos del ASI de FnBIS1 (necesitan el ASI compilado); la prueba dirigida cubre exactamente la ruta de la pila.
