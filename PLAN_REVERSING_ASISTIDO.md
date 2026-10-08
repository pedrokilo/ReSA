<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Plan de reversing asistido para GTA San Andreas

Fecha: 22 de septiembre de 2026.
Carpeta de trabajo: D:\Original\gta-reversed-master
Estado: **plan histórico del 22 de septiembre**. Las observaciones iniciales se conservan como historia; los recuadros de estado vigente están actualizados al 30/09/2026. El proyecto ya dispone de Git, compilación y bancos dentro del motor. Para el estado actual consulta [el README](README.md) y [la continuidad](reversing/MEJORAS_DEL_PROCESO.md#continuidad-vigente).

## 1. Objetivo y forma de trabajar

> **Objetivo vigente (23 de septiembre de 2026):** reconstrucción funcionalmente equivalente, validada e independiente de funciones originales, seguida de un ejecutable propio. Consultar la [hoja de ruta](docs/ProjectRoadmap.md); las etapas siguientes se conservan como plan histórico, no como orden de ejecución actual.

Construir progresivamente un flujo que combine gta-reversed, Ghidra y observación del juego para identificar funciones pendientes, analizar su comportamiento, proponer C++ y comprobarlo contra el original.

Yo me encargaré del trabajo técnico: preparar herramientas, escribir y ejecutar scripts, manejar Ghidra, interpretar resultados, desarrollar instrumentación, compilar, diagnosticar errores y documentar lo comprobado. Tú no necesitas aprender Ghidra, programar ni interpretar ensamblador.

La meta inicial será completar un caso pequeño de principio a fin. No se promete recuperar automáticamente todo el C++ original ni terminar el juego completo. Las inferencias y el pseudocódigo serán candidatos hasta que exista evidencia suficiente.

Tu intervención se limitará a lo que no pueda resolver desde el entorno: indicar dónde está tu instalación si no la encuentro, proporcionar archivos propios que falten, o atender un diálogo del sistema que requiera interacción personal. Intentaré automatizar también los escenarios de ejecución; jugar manualmente será una alternativa si algún escenario no se puede controlar de forma fiable. No te delegaré el análisis técnico.

La ejecución ocurre durante sesiones de trabajo y procesos que dejemos configurados explícitamente. Este documento no instala un agente permanente ni inicia trabajo autónomo en segundo plano.

## 2. Lo comprobado en esta copia

- El README describe una DLL/ASI que sustituye funciones del juego por implementaciones reconstruidas.
- En la inspección inicial, `docs/hooks.csv` contenía 8.014 registros. **Estado vigente:** 8.442 entradas, 7.857 con `reversed=1` y 585 pendientes. Son registros del catálogo, no un inventario exhaustivo del ejecutable ni necesariamente direcciones únicas.
- Las columnas actuales son `class`, `fn_name`, `address`, `reversed`, `locked`, `validated` y `type`. Su significado y la sincronización de evidencia se describen en [HookRegistration.md](docs/HookRegistration.md#columnas-de-hookscsv).
- En la inspección inicial, `docs/ReversedClasses.md` enumeraba 701 clases. **Estado vigente:** 772 clases/grupos, 665 completas, 104 parciales y 3 sin revertir. El porcentaje del catálogo no equivale al porcentaje del juego probado ni a independencia del EXE.
- docs/HookRegistration.md exige marcar reversed=false en funciones que todavía redirigen al código original.
- El README exige GTA SA Compact exe de 5.189.632 bytes y distingue esta versión de otros ejecutables 1.0 US. El tamaño será un filtro preliminar; también comprobaré hash, cabeceras PE y bytes en direcciones relevantes.
- El README pide Visual Studio 2026, Python, Conan 2 y CMake 4.2 o superior. La compilación prevista es x86.
- La consulta inicial al PATH encontró comandos python y java, pero no verificó sus versiones ni su funcionamiento. No encontró cmake ni conan por nombre. Esto no demuestra que falten instalaciones fuera del PATH.
- No apareció AGENTS.md en la búsqueda dentro del proyecto. La carpeta parece una extracción del código; no se observó .git en la raíz.
- Todavía no se ha localizado ni verificado el ejecutable del juego o una instalación de Ghidra.

### Estado vigente del catálogo

La rama `ReSA` incorpora una auditoría bidireccional que contrasta scopes e instalaciones de hooks, llamadas de registro y `docs/hooks.csv`. El corte actual pasa:

```powershell
python contrib/audit_hook_catalog.py --strict
```

con cero inconsistencias críticas. El modo `--verbose` muestra adicionalmente hooks inactivos, registros indirectos y aliases informativos sin incorporarlos automáticamente al progreso.

## 3. Arquitectura propuesta

Fuentes C++ + catálogo de hooks + ejecutable identificado
    -> inventario con procedencia
    -> Ghidra en modo headless
    -> paquetes de análisis por función
    -> trazas de escenarios controlados
    -> correlación e hipótesis de tipos
    -> candidato C++
    -> compilación y comparación de comportamiento
    -> integración de cambios que superen las comprobaciones

Ghidra permite trabajar por consola sin abrir su interfaz. Escribiré scripts para importar el binario, aplicar símbolos y tipos compatibles y exportar resultados. No hace falta que tú abras Ghidra. Referencia oficial: https://github.com/NationalSecurityAgency/ghidra/blob/master/GhidraDocs/GhidraClass/Intermediate/HeadlessAnalyzer.html

El primer prototipo usará Ghidra y Python. angr será opcional para problemas concretos de flujo o restricciones; añadirlo desde el principio aumentaría las dependencias sin demostrar todavía su utilidad en este proyecto.

## 4. Etapas y criterios para avanzar

### Etapa A. Preparar una base reproducible

Yo localizaré la instalación, identificaré el EXE y revisaré herramientas, dependencias y guías del repositorio. Registraré hashes, versiones y configuración. Prepararé una copia de pruebas y respaldos de los archivos que deba cambiar, incluidas partidas y configuración cuando corresponda.

Primero compilaré el proyecto sin modificaciones y comprobaré que la instalación de referencia arranca. Conservaré el estado inicial aunque la carpeta no tenga Git. No cambiaré otros proyectos ni tomaré la carpeta actual gta5 como instalación de San Andreas por su ubicación.

Entrega: informe de entorno, comandos reproducibles y referencia de funcionamiento. Si faltan recursos indispensables, documentaré el bloqueo concreto y avanzaré con el análisis que sí sea posible.

### Etapa B. Crear el inventario de funciones

Yo procesaré hooks.csv y lo contrastaré con registros de hooks, declaraciones y llamadas al original en source/. Detectaré direcciones repetidas, alias, funciones virtuales, wrappers y contradicciones.

Cada entrada tendrá dirección original y RVA, nombre, clase, estado declarado, fuente de la información y dudas pendientes. Separaré funciones pendientes conocidas de regiones que aún no están catalogadas. No llamaré función desconocida confirmada a cualquier dirección observada.

Entrega: catálogo JSON/CSV y lista priorizada. Se avanza cuando los resultados sean reproducibles y las discrepancias queden explícitas.

### Etapa C. Automatizar Ghidra

Yo prepararé una versión de Ghidra y Java compatibles entre sí, crearé un proyecto separado e importaré la copia identificada del ejecutable. Aplicaré nombres, firmas y tipos verificados del proyecto, empezando por un subconjunto pequeño.

La importación de clases C++ no se asumirá directa: adaptaré definiciones o generaré descripciones compatibles, verificando tamaños, alineación, herencia y convenciones de llamada de x86. Un offset coincidente por sí solo no confirma una clase.

Exportaré por función: instrucciones, pseudocódigo, referencias, llamadas directas, llamadas indirectas sin resolver, accesos a memoria y advertencias del análisis. Cada salida quedará vinculada al hash del EXE y a la versión del catálogo.

Entrega: paquete estático de varias funciones sencillas, comprobado manualmente por mí contra sus instrucciones. Incluiré funciones ya reconstruidas como controles.

### Etapa D. Probar un tracer mínimo

Yo estudiaré primero el sistema de hooks existente y elegiré una técnica compatible. ReversedTracer.asi es un nombre propuesto, no una herramienta existente ni una decisión definitiva de arquitectura.

El prototipo observará entre una y cinco funciones verificadas, antes de ampliar cobertura. Evitaré que el tracer y gta-reversed parcheen independientemente la misma entrada. Distinguiré explícitamente ejecución original y ejecución de reemplazo.

Validaré instrucciones desplazadas, trampolines y conservación del estado requerido por la ABI: registros, pila y estado de coma flotante/SIMD cuando corresponda. La primera captura recogerá contadores y muestras limitadas; argumentos, retornos y this solo cuando se conozca cómo obtenerlos correctamente.

Usaré buffers limitados, escritura fuera de rutas críticas, protección contra reentrada y contadores de eventos perdidos. No guardaré un volcado completo por llamada. Mediré tiempo de cuadro, volumen de datos y estabilidad contra una sesión sin tracer; propondré como objetivo inicial menos del 5% de degradación en un escenario repetible y lo ajustaré según mediciones.

Entrega: una traza útil con sobrecarga medida, desactivación sencilla y restauración comprobada. No se ampliará a cientos de funciones antes de superar esta prueba.

### Etapa E. Correlacionar observaciones

Yo uniré información estática, catálogo y trazas por binario, RVA y sesión. Etiquetaré los escenarios controlados para relacionar actividad con conducir, chocar u otras acciones. La cercanía temporal a un choque será evidencia contextual, no prueba automática de causalidad.

Usaré accesos a campos, firmas conocidas, vtables verificadas y relaciones de llamadas para proponer tipos. Registraré por separado hechos, hipótesis y contradicciones. No generaré porcentajes de confianza aparentes sin calibración.

Entrega: expediente por función con evidencia, comportamiento observado, tipos propuestos y dudas. La ausencia de una llamada en las trazas no significará que la función no se use.

### Etapa F. Reconstruir y validar el primer caso

Yo elegiré una función pendiente corta, preferentemente determinista y con pocas dependencias. Prepararé el candidato C++ respetando las guías del proyecto y lo compilaré.

Para funciones puras o con estado aislable, compararé original y candidato con entradas normales, límites y casos adversos. Comprobaré retorno y efectos observables; las comparaciones numéricas tendrán criterios justificados.

Para funciones con efectos sobre el juego, usaré procesos o escenarios separados con estado inicial equivalente y control de tiempo y aleatoriedad donde sea posible. No ejecutaré original y candidato consecutivamente sobre el mismo estado vivo: eso duplicaría efectos. Audio, streaming, física y recursos externos pueden impedir una comparación completa; esas limitaciones quedarán documentadas.

Entrega: cambio C++ revisable, resultados de comparación y restauración. Compilar o jugar unos minutos sin crash no bastará para declarar equivalencia general. Una función permanecerá pendiente si la evidencia es insuficiente.

### Etapa G. Ampliar por lotes

Solo después del primer caso completo ampliaré a grupos pequeños de funciones relacionados. Priorizaré casos con buena evidencia y pruebas repetibles, y pospondré los que dependan de subsistemas todavía ambiguos.

Cada lote producirá un informe en español: qué se analizó, qué se implementó, qué pruebas pasó y qué continúa sin resolver. La capacidad de recuperar la configuración anterior se mantendrá en cada etapa.

## 5. Organización prevista

Estos nombres son una propuesta; todavía no se han creado estas herramientas:

- tools/reversing/: orquestación, inventario, scripts de Ghidra e informes.
- reversing/config/: rutas, versiones, objetivos y parámetros de captura.
- reversing/catalog/: catálogo normalizado y discrepancias.
- reversing/ghidra/: proyecto de análisis local.
- reversing/sessions/: trazas y configuración de cada escenario.
- reversing/functions/: expedientes por RVA.
- reversing/reports/: resultados y estado del trabajo.
- reversing/backups/: manifiestos y copias de los archivos afectados.

Los ejecutables propios, volcados, partidas y artefactos grandes se conservarán fuera de cualquier publicación. Los procesos reutilizarán resultados solo si coinciden hashes y configuración.

## 6. Primer hito concreto

El primer hito será: entorno comprobado, inventario generado, Ghidra ejecutado por consola y un expediente estático útil de una función pendiente. Después vendrá el tracer mínimo y, finalmente, una primera implementación contrastada.

El sistema completo se considerará demostrado cuando una función haya recorrido todas las etapas con evidencia reproducible. Esto no equivale a completar las 586 entradas pendientes ni las partes aún no catalogadas. No se fija una fecha de terminación antes de medir el primer caso.

## 7. Referencias

- Conversación proporcionada: C:\Users\pedro\.codex\attachments\0bc714c6-5847-459c-88f5-d3ddd20fb6c9\pasted-text.txt
- Archivos locales revisados: README.md, CMakePresets.json, docs/hooks.csv, docs/ReversedClasses.md y docs/HookRegistration.md.
- Ghidra, uso por consola: https://github.com/NationalSecurityAgency/ghidra/blob/master/GhidraDocs/GhidraClass/Intermediate/HeadlessAnalyzer.html
- Ghidra, referencia de AnalyzeHeadless: https://ghidra.re/ghidra_docs/api/ghidra/app/util/headless/AnalyzeHeadless.html


## Objetivo concreto elegido posteriormente

El usuario eligió inicialmente CVehicle (141/142), cuya entrada pendiente era RemoveObjectsCB-Object, 0x6D33B0. Ese trabajo ya se completó; CVehicle conserva 142/142 en el catálogo. [Evidencia y alcance](reversing/CVehicle/README.md). La entrega más reciente es [HooksActivados](reversing/HooksActivados/README.md) (39 clases registradas, sin comparaciones todavía); la anterior es [CConversationForPed, 2/2](reversing/ConversationForPed/README.md), con 4.702 pares contra el original y ASI normal comprobado en la copia de pruebas.
