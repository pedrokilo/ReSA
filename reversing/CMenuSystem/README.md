<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CMenuSystem — FillGridWithCarColours

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Validación del 2026-09-22: **784 comparaciones aprobadas dentro de GTA**, con igualdad exacta de los 50.176 índices de color comparados. CMenuSystem pasa de 20/21 a 21/21 funciones registradas. Este banco valida específicamente `FillGridWithCarColours` (`0x5820E0`), antes delegada al original y deshabilitada como reconstrucción.

La función llena las 64 casillas buscando en la tabla de 128 colores de vehículos. Rechaza un color si las diferencias de sus tres componentes RGB respecto a otro son estrictamente menores que 15. Conserva el comportamiento original: la primera casilla acepta el índice cero; las siguientes comparan también la casilla aún negra; la búsqueda vuelve al índice cero después de cada selección; al agotar las alternativas usa el índice 127, aunque se repita. La tabla de colores no se modifica.

## Repetir la prueba

Desde la raíz:

```powershell
.\PROBAR.cmd --case menusystem
python reversing/run.py --case menusystem
```

El runner Python termina sin pausa ni supervisión. Usa `runtime_runner.py`, abre únicamente la copia `D:/JUEGOS/GTA San Andreas - CVehicle Test`, compara dentro del motor, cierra su propio proceso y restaura ASI/PDB. Verifica los hashes del ejecutable y la sonda, y comprueba las partidas. Se mantiene un solo PROBAR.cmd y este caso también está incluido en `--all`.

`build_probe.py` añade temporalmente contadores a la función y ejecuta `LiveComparison.hpp` desde CGame::Process. Comprueba los puntos de inserción y restaura las fuentes mediante `finally`. Archiva ASI/PDB y un manifiesto de hashes en `artifacts`. Recompilar una sonda exige revisar su nuevo hash y volver a validarla; `PROBE_HASH` identifica el binario aprobado en `_baseline/gta_reversed-menusystem-probe.asi`. La compilación normal no contiene instrumentación.

## Entradas y comprobaciones

196 paletas × 2 identificadores de menú × 2 patrones iniciales de memoria = 784 pares:

- Tabla real cargada por GTA; paletas negra, blanca, escala de grises, colores separados y mezcla con colores cercanos al negro.
- 126 paletas de frontera: tres canales, bases 0/14/15/128/240/255 y diferencias −16/−15/−14/0/14/15/16, limitadas al rango de cada componente.
- 64 paletas generadas con una semilla fija, incluyendo variaciones de alpha.

El analizador comprueba las entradas sintéticas registradas contra ese diseño. Los resultados esperados de las cuadrículas proceden siempre de la función original de GTA, no de una segunda implementación del algoritmo.

Cada par utiliza dos estructuras de menú con almacenamiento independiente y una tercera como menú no seleccionado. El hook se alterna para llamar al original y al candidato. Los contadores exigen cero entradas al candidato durante la llamada original y exactamente una durante la candidata. Se comparan los 64 índices y todo el almacenamiento de ambas estructuras, incluidas guardas, y se verifica que las escrituras se limiten a los primeros 64 bytes. Se comprueba además que la tabla de colores, los punteros, los flags y el otro menú no cambien. El estado global y el hook se restauran antes de continuar el juego.

El banco observó cuadrículas con 64 colores distintos y 32.492 casillas con índice 127 por versión, incluidos los casos de agotamiento de opciones. La comparación aprobó sin diferencias ni escrituras fuera de límites. El analizador rechazó mutaciones de índices, contadores, paletas, límites de escritura, restauración y casos duplicados; un registro incompleto permanece pendiente.

## Evidencia y límites

Resumen y hashes: `validation-summary.json`. Log completo local: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-menusystem/20260922-203512-507016/comparisons.jsonl`. Desensamblado: `artifacts/FillGridWithCarColours.asm`. Las fuentes de la sonda se conservan en esta carpeta; los artefactos voluminosos están ignorados por Git.

Se prueban ambos IDs válidos con estructuras temporales; no se prueban punteros inválidos ni todos los menús de scripts. La función solo lee la tabla y escribe los índices: no se valida aquí el dibujo, navegación o aceptación de selecciones, ni las otras veinte funciones de CMenuSystem. Los hooks Audio y Global/SurfaceInfos_c siguen excluidos durante el banco. No hubo revisión visual ni se necesita para esta comparación de memoria.

21/21 expresa funciones registradas como reconstruidas, no cobertura exhaustiva ni estabilidad general. La sonda archivada valida esta versión; la campaña de integración de todas las clases sigue aplazada según lo solicitado.

El ASI normal quedó instalado únicamente en la copia de pruebas, con respaldo en `_baseline/before-menusystem-20260922`. Instalación personal conservada, partidas sin cambios, sin commit ni push.
