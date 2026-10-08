<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
