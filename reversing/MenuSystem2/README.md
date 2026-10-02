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
