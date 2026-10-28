<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CMenuSystem (20 funciones)

Validadas el 02/10/2026 con **16.640 pares original/candidato dentro de GTA**, todos exactos: las 20 funciones de `CMenuSystem` que faltaban, con `validated=1`. `FillGridWithCarColours` ya estaba validada en [CMenuSystem](../CMenuSystem/README.md).

**Diferencias reales corregidas:**
- `InputStandardMenu` e `InputGridMenu`:
  - Intro en pausa solo con la tecla Intro extendida;
  - `InputGridMenu` marca la fila aceptada con -99.
- `Process` dibuja los menús no actuales con su índice (la DLL pasaba -99).
- `CreateNewMenu`:
  - no pone a cero la memoria del menú;
  - copia el título con `strcpy`, sin el relleno 0xFE de `strcpy_s` en depuración.
- `InsertMenu` solo reinicia los números de la columna insertada.
- El recuento de filas suma los huecos hasta el último título no vacío.
- `DisplayStandardMenu`:
  - escalado del original;
  - acumuladores enteros;
  - orden de las sumas;
  - alineación de cabecera;
  - colores menos la opacidad de la ventana.
- `DisplayGridMenu`: orden de las sumas de la celda coloreada.
- `MenuInUse` se indexa sin comprobar límites, como el original (lee los globales contiguos).

**Método:**
- Menús falsos con guardas.
- Estado del mando y del teclado fijado por caso.
- Se simulan y se trazan `DrawWindow`, `CFont`, `CText::Get`, `DrawRect` y el audio de interfaz.
- `operator new`/`delete` se sustituyen solo durante la llamada.

Repetir: `python reversing/MenuSystem2/build_probe.py` y `python reversing/run.py --case menusys2`. Evidencia: [validation-summary.json](validation-summary.json).
