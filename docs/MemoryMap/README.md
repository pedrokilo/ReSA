# Mapa de código y direcciones de ReSA

**[Abrir el visor local](index.html)**. Página interactiva sin servidor ni conexión, compatible con navegadores modernos. Busca una clase, función, dirección o archivo; selecciona una fila para consultar su ficha y procedencia. El filtro de estado permite consultar sólo lo reconstruido, la evidencia aprobada, la vigencia o los pendientes.

Este mapa se generó desde `D:\Original\gta-reversed-master` el 6 de octubre de 2026. No copia clases de ReIV ni convierte sus direcciones a GTA SA. No ejecuta GTA, modifica hooks, recompila, cambia flags ni vuelve a correr bancos.

## Inventario captado

| Inventario | Cantidad |
| --- | ---: |
| Entradas de `docs/hooks.csv` | 8.441 |
| Clases, namespaces o grupos del catálogo | 771 |
| Entradas registradas como reconstruidas | 7.947 |
| Pendientes de reconstrucción | 494 |
| Evidencias aprobadas vinculadas, verificadas por el auditor | 6.003 |
| Validaciones con vigencia acreditada para su alcance | 0 |
| Registros con evidencia y coincidencia de fuentes probadas, sin acreditar dependencias | 858 |
| Definiciones de class/struct en source, con repeticiones | 1.942 |
| Nombres distintos de tipos captados | 1.929 |
| Declaraciones y cuerpos candidatos de función, con repeticiones | 27.228 |
| Referencias de vtable declaradas en el código | 445 |
| Referencias de globales StaticRef/StaticArray | 2.582 |
| Registros explícitos de hooks captados | 8.441 |
| Archivos fuente examinados | 2.332 |
| Extras NOTSA, fuera del catálogo principal | 3 |

El auditor estricto del catálogo devolvió **OK** durante esta generación. Sus 772 owners incluyen el grupo de los extras NOTSA; los 771 de este mapa corresponden sólo al catálogo principal. Los totales describen lo captado, no el total de funciones o clases presentes en gta_sa.exe.

## Qué está confirmado

- **Catálogo:** se preservan las identidades clase + nombre + dirección + tipo y los flags `reversed`, `locked` y `validated` de `hooks.csv`. `S` y `V` describen hooks simples y virtuales.
- **Evidencia aprobada:** se usa el auditor existente `contrib/validation_registry.py`. Comprueba resumen, SHA-256 y resultado aprobado por función. No repite el banco ni verifica de nuevo los logs referidos por ese resumen.
- **Vigencia:** exige además coincidencia de fuentes y revisión de dependencias según el registro. Coincidir sólo en las fuentes de prueba no acredita vigencia. Los cambios pendientes no se clasifican automáticamente como fallos.
- **Código:** se fichan referencias explícitas de registro de hooks, tablas virtuales y StaticRef/StaticArray con archivo y línea. Son referencias fuente, sin nueva confirmación del EXE.
- **Tipos y funciones extraídos:** son candidatos sintácticos de un parser conservador, no una certificación ABI ni un análisis exhaustivo de C++. Las sobrecargas y las declaraciones/cuerpos se conservan como entradas distintas. El código auxiliar y librerías también están dentro de source.

El inventario no atribuye independencia del EXE por no tener locks. Una clase catalogada puede ser un namespace o un grupo sin definición class/struct. Tampoco se acreditan todas las funciones de una clase porque una tenga evidencia.

## Direcciones y tablas

Se conserva la VA del catálogo. **6.982 entradas tienen dirección distinta de cero y 1.459 tienen dirección cero**, principalmente hooks virtuales. Los ceros no se sustituyen ni se confunden con la base del módulo.

Las direcciones candidatas del registro fuente se guardan en otra columna. El cruce requiere el mismo owner y nombre de entrada, incluyendo etiquetas de sobrecargas. No se inventan fuentes cuando el parser no consigue enlazarlas: 175 entradas del catálogo quedaron sin localización directa de registro por este criterio.

La RVA se calcula respecto a `0x00400000`, base de referencia del proyecto. El visor permite calcular otra VA mediante `base indicada + RVA`; **no observa un proceso ni verifica la base leyendo un EXE**. Las vtables incluyen el número de slots declarado en RH_ScopedVirtualClass, pero no sus destinos individuales: generar este mapa desde el código no equivale a leer la tabla del binario.

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
| [notsa.csv](notsa.csv) | Extras propios sin equivalente en el EXE |
| [summary.json](summary.json) | Recuentos y hashes de las fuentes del inventario |
| [source_manifest.json](source_manifest.json) | SHA-256 de cada archivo fuente examinado |

Las fichas del visor enlazan directamente a declaraciones, registros y evidencia del mismo repositorio. Los CSV se pueden descargar completos o exportar filtrados desde el visor.

Durante la preparación se observó que `source/game_sa/Game.cpp` alternaba entre versiones mientras se comprobaban hashes. El mapa conserva un **corte de extracción**, no una lectura en vivo del repositorio. Cada archivo se parsea y firma a partir de los mismos bytes; el generador informa en `source_files_changed_during_generation` si detecta cambios al finalizar. Que esa lista esté vacía sólo indica coincidencia en aquella comprobación, no estabilidad posterior del árbol. La vigencia sigue siendo una dimensión separada y no se elevó ninguna acreditación.

## Regenerar

Desde la raíz de este mismo repositorio:

```powershell
python contrib/build-memory-map.py
```

También acepta `--root`, `--output` y `--template`. El generador sólo escribe el inventario y el visor en la carpeta de salida; no actualiza hooks.csv ni el registro de validación. Los hashes del resumen y del manifiesto permiten identificar el corte de fuentes consultado.
