<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/10/2026 (Ciudad de México):** 818 grupos / 8901 funciones reconstruidas; 8901 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Mapa de código y direcciones de ReSA

**[Abrir el visor local](index.html)**. Página interactiva sin servidor ni conexión, compatible con navegadores modernos. Busca una clase, función, dirección o archivo; selecciona una fila para consultar su ficha y procedencia. El filtro de estado permite consultar sólo lo reconstruido, la evidencia aprobada, la vigencia o los pendientes.

Este mapa combina el **análisis del EXE de GTA SA** y el inventario de `D:\Original\gta-reversed-master`, actualizado el 10 de octubre de 2026; el análisis global del EXE se conserva del 6 de octubre. No ejecuta GTA, modifica hooks, recompila, cambia flags ni vuelve a correr bancos.

## Inventario del EXE

La vista inicial **Funciones del EXE** incluye el código detectado aunque no figure en el catálogo de ReSA. Ghidra 12.1.4 terminó el análisis global de memoria y código en 201 segundos, sin timeout; después se delimitaron tres entradas mediante revisión estática dirigida. Se aportaron 9.473 entradas candidatas de registros fuente y destinos virtuales, además de la detección automática del analizador.

| Resultado del análisis binario | Cantidad |
| --- | ---: |
| Entradas de función detectadas | **17.765** |
| Entradas con nombres cruzados del catálogo o sus registros fuente | 8.542 |
| Entradas sin nombre del catálogo | 9.223 |
| Dirección candidata dentro de un cuerpo existente, sin entrada propia | 1 |
| Direcciones de código en el inventario combinado | 17.766 |
| Tablas fuente leídas del EXE | 445 |
| Destinos de slots leídos y verificados como punteros a código | 5.506 |
| Tipos RTTI recuperados estructuralmente | 9 |
| Sitios de llamada detectados | 80.567 |
| Sitios de llamada indirecta sin resolver | 6.004 |
| Bytes de archivo en secciones ejecutables | 4.547.584 |
| Bytes decodificados como instrucciones | 4.014.768 |
| Bytes asignados a cuerpos de funciones, sin duplicados | 3.724.559 |
| Bytes ejecutables sin cuerpo de función asignado | 823.025 |
| Rangos de bytes sin cuerpo asignado | 15.639 |

**8.670 es el tamaño del catálogo de ReSA, no el número de funciones del EXE.** El análisis abarca todas las secciones del binario; el mapa conserva cada rango de bytes ejecutables no asignado a un cuerpo en «Huecos por revisar». Esos huecos pueden incluir alineación, tablas de datos, instrucciones sueltas y código pendiente de identificar. No se suman como nuevas funciones.

El análisis automático global se completó, pero **17.765 sigue siendo un número de entradas detectadas, no un total semántico exacto demostrado**. Puede haber límites imperfectos, thunks, cuerpos compartidos, saltos indirectos o funciones no reconocidas. La revisión de las cuatro entradas pendientes delimitó tres funciones adicionales. La cuarta, 0x007487CF, está dentro de la función 0x00748710: no se cuenta como una función nueva.

El EXE no conserva RTTI útil de todas las clases del juego. Los nueve tipos recuperados no representan todas sus clases. Los nombres de clases del juego se cruzan con el código y las tablas declaradas de ReSA, sin atribuir necesariamente cada implementación virtual a la clase de su tabla.

Binario analizado: `D:\Grand Theft Auto San Andreas\gta-sa.exe`, 5.189.632 bytes, x86 PE32, base `0x00400000`. SHA-256 **`72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`**, coincidente con la copia de pruebas. Ningún EXE se modifica ni se publica. Los proyectos de Ghidra y las exportaciones de trabajo permanecen en build, fuera de Git.

## Inventario captado

| Inventario | Cantidad |
| --- | ---: |
| Entradas de `docs/hooks.csv` | 8.670 |
| Clases. namespaces o grupos del catálogo | 771 |
| Entradas registradas como reconstruidas | 8.670 |
| Pendientes de reconstrucción | 0 |
| Evidencias aprobadas vinculadas. verificadas por el auditor | 8.445 |
| Validaciones con vigencia acreditada para su alcance | 0 |
| Registros con evidencia y coincidencia de fuentes probadas. sin acreditar dependencias | 423 |
| Definiciones de class/struct en source. con repeticiones | 1.950 |
| Nombres distintos de tipos captados | 1.937 |
| Declaraciones y cuerpos candidatos de función. con repeticiones | 29.102 |
| Referencias de vtable declaradas en el código | 445 |
| Referencias de globales StaticRef/StaticArray | 4.099 |
| Registros explícitos de hooks captados | 8.446 |
| Archivos fuente examinados | 2.332 |
| Extras NOTSA. fuera del catálogo principal | 4 |

El auditor estricto del catálogo devolvió **OK** durante esta generación. Sus 772 owners incluyen el grupo de los extras NOTSA; los 771 de este mapa corresponden sólo al catálogo principal. Los totales describen lo captado, no el total de funciones o clases presentes en gta_sa.exe.

## Qué está confirmado

- **Catálogo:** se preservan las identidades clase + nombre + dirección + tipo y los flags `reversed`, `locked` y `validated` de `hooks.csv`. `S` y `V` describen hooks simples y virtuales.
- **Evidencia aprobada:** se usa el auditor existente `contrib/validation_registry.py`. Comprueba resumen, SHA-256 y resultado aprobado por función. No repite el banco ni verifica de nuevo los logs referidos por ese resumen.
- **Vigencia:** exige además coincidencia de fuentes y revisión de dependencias según el registro. Coincidir sólo en las fuentes de prueba no acredita vigencia. Los cambios pendientes no se clasifican automáticamente como fallos.
- **Código:** se fichan referencias explícitas de registro de hooks, tablas virtuales y StaticRef/StaticArray con archivo y línea. Sus estados fuente se preservan. Las vistas binarias agregan la lectura real del EXE y el cruce por dirección, sin elevar la validación del comportamiento.
- **Tipos y funciones extraídos:** son candidatos sintácticos de un parser conservador, no una certificación ABI ni un análisis exhaustivo de C++. Las sobrecargas y las declaraciones/cuerpos se conservan como entradas distintas. El código auxiliar y librerías también están dentro de source.

El inventario no atribuye independencia del EXE por no tener locks. Una clase catalogada puede ser un namespace o un grupo sin definición class/struct. Tampoco se acreditan todas las funciones de una clase porque una tenga evidencia.

## Direcciones y tablas

Se conserva la VA del catálogo. **7.212 entradas tienen dirección distinta de cero y 1.458 tienen dirección cero**, principalmente hooks virtuales. Los ceros no se sustituyen ni se confunden con la base del módulo.

Las direcciones candidatas del registro fuente se guardan en otra columna. El cruce requiere el mismo owner y nombre de entrada, incluyendo etiquetas de sobrecargas. No se inventan fuentes cuando el parser no consigue enlazarlas: 175 entradas del catálogo quedaron sin localización directa de registro por este criterio.

La RVA se calcula respecto a `0x00400000`, base comprobada en la cabecera PE. El visor permite calcular otra VA mediante `base indicada + RVA`; **no observa un proceso**. «Tablas virtuales» conserva la referencia fuente y «Vtables leídas»/«Slots leídos del EXE» añaden los valores del binario para la extensión declarada por RH_ScopedVirtualClass; no se adivina una extensión mayor.

## Archivos

| Archivo | Contenido |
| --- | --- |
| [classes.csv](classes.csv) | Clases y grupos del catálogo, estados, bases, tablas y fuentes |
| [functions.csv](functions.csv) | Todas las identidades catalogadas, estados auditados y evidencias |
| [types.csv](types.csv) | Definiciones class/struct captadas en source |
| [source_functions.csv](source_functions.csv) | Declaraciones y cuerpos candidatos de funciones |
| [vtables.csv](vtables.csv) | Referencias explícitas de tablas virtuales |
| [globals.csv](globals.csv) | Referencias StaticRef/StaticArray del código |
| [registrations.csv](registrations.csv) | Registros explícitos RH_Scoped*Install |
| [notsa.csv](notsa.csv) | Adaptaciones NOTSA fuera del catálogo principal |
| [summary.json](summary.json) | Recuentos y hashes de las fuentes del inventario |
| [source_manifest.json](source_manifest.json) | SHA-256 de cada archivo fuente examinado |
| [binary_functions.csv](binary_functions.csv) | Todas las entradas de función detectadas en el EXE, nombres cruzados y referencias virtuales |
| [binary_entry_candidates.csv](binary_entry_candidates.csv) | Direcciones a código sin entrada de función delimitada |
| [binary_vtables.csv](binary_vtables.csv) | Tablas fuente leídas del binario, slots declarados y punteros comprobados |
| [binary_slots.csv](binary_slots.csv) | Destinos reales de cada slot leído |
| [binary_rtti_types.csv](binary_rtti_types.csv) | Tipos RTTI recuperados del EXE |
| [binary_calls.csv](binary_calls.csv) | Sitios de llamada directa e indirecta detectados |
| [binary_function_ranges.csv](binary_function_ranges.csv) | Rangos de bytes que Ghidra asigna a cada función |
| [binary_unassigned_ranges.csv](binary_unassigned_ranges.csv) | Todos los huecos de secciones ejecutables sin cuerpo asignado |
| [binary_sections.json](binary_sections.json) | Rangos PE y cobertura por sección |
| [binary_summary.json](binary_summary.json) | Recuentos binarios, SHA-256 del EXE y del análisis |

Las fichas del visor enlazan directamente a declaraciones, registros y evidencia del mismo repositorio. Los CSV se pueden descargar completos o exportar filtrados desde el visor.

Durante la preparación se observó que `source/game_sa/Game.cpp` alternaba entre versiones mientras se comprobaban hashes. El mapa conserva un **corte de extracción**, no una lectura en vivo del repositorio. Cada archivo se parsea y firma a partir de los mismos bytes; el generador informa en `source_files_changed_during_generation` si detecta cambios al finalizar. Que esa lista esté vacía sólo indica coincidencia en aquella comprobación, no estabilidad posterior del árbol. La vigencia sigue siendo una dimensión separada y no se elevó ninguna acreditación.

## Regenerar

Desde la raíz de este mismo repositorio:

```powershell
python contrib/build-memory-map.py
```

También acepta `--root`, `--output` y `--template`. El generador sólo escribe el inventario y el visor en la carpeta de salida; no actualiza hooks.csv ni el registro de validación. Los hashes del resumen y del manifiesto permiten identificar el corte de fuentes consultado.

Para exportar del proyecto de Ghidra existente y actualizar la vista binaria:

```powershell
powershell -ExecutionPolicy Bypass -File reversing/MemoryMap/export-binary.ps1 -GhidraDirectory 'RUTA\ghidra_12.1.4_PUBLIC'
```

Si el proyecto no existe, importa y analiza el EXE completo. `-Reanalyze` solicita repetir su análisis. Se requiere JDK compatible y Python; las exportaciones de trabajo se guardan en `build/binary-map`. `contrib/build-binary-map.py` exige el hash canónico y un log de análisis global terminado antes de publicar el inventario. Regenerar sólo la vista fuente preserva las vistas binarias existentes; no actualiza su corte automáticamente.

## Cobertura fuera del catálogo

[Auditoría de cobertura fuera del catálogo](../BinaryCoverageAudit.md): funciones, slots y tipos sin registro, con ejemplos contrastados contra el EXE. Estas listas no son automáticamente funciones pendientes de reconstrucción.
