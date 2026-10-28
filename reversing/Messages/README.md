<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CMessages

Validadas el 02/10/2026 con **25.152 pares original/candidato dentro de GTA**, todos exactos: las 28 funciones de `CMessages` con `validated=1`.

**Diferencias reales corregidas:**
- Limpiezas (`ClearMessages`, `ClearSmallMessagesOnly`, `ClearPreviousBriefArray`, `Init`): el original solo pone a cero el texto y la cadena insertada de cada entrada. `Init` limpia los 7 estilos grandes sin excepciones.
- `StringCopy` con origen nulo rellena con ceros `len` bytes.
- `CutString` estaba sin implementar: hasta 8 trozos, contadores de 16 bits con signo.
- `AddToPreviousBriefArray` perdía la entrada `i-1` al desplazar.
- `InsertNumberInString`: `sprintf("%d")` + `AsciiToGxtChar` por número; del séptimo `~1~` en adelante reutiliza el último número.
- `InsertStringInString`: el marcador es `~a~` (no `~1~`); copia y rellena con ceros hasta 400 bytes como el original.
- `InsertPlayerControlKeysInString`: búsqueda de los 59 nombres de acción del EXE con `~k~~NOMBRE~`.
- Las funciones `Add*` hacen el preludio del original (copia, inserciones y teclas sobre un búfer local, sin usar el resultado).
- Los mensajes grandes no escriben `PreviousBrief`; los campos se escriben uno a uno.
- `ClearThisPrint`, `ClearThisBigPrint` y `Process`: desplazamiento limitado como el `rep movs` del original. `Process` desplazaba en sentido contrario y añadía al resumen anterior cada fotograma.

**Método:**
- Las colas (resúmenes anteriores, mensajes y mensajes grandes) y los textos de `CHud` se aleatorizan por caso con punteros a un depósito de cadenas, y se restauran.
- Se simulan y se trazan `CHud::SetBigMessage`/`SetMessage`/`GetRidOfAllHudMessages` y `GetGxtStringOfCommandKeys`.
- Los búferes del llamador se comparan enteros (512 bytes).

Repetir: `python reversing/Messages/build_probe.py` y `python reversing/run.py --case messages`. Evidencia: [validation-summary.json](validation-summary.json).
