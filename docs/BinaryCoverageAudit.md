<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cobertura fuera del catálogo de ReSA

Revisión estática del 10 de octubre de 2026. **El 100% corresponde a las 8.445 entradas del catálogo principal, no a todo el código del EXE ni a todos los métodos de cada clase.** Hay funciones de juego identificadas que siguen delegando al original y no figuran en ese catálogo. No se ejecutó un nuevo banco, no se modificaron hooks ni flags de validación.

El texto recibido propone una dirección útil: auditar dependencias e integración antes de afirmar independencia del EXE. Sus cifras del catálogo son anteriores a la exclusión de `Win::NOTSA_WinMain`. Esa adaptación conserva su registro separado; no se considera una función original validada. La evidencia histórica del catálogo tampoco equivale a vigencia acreditada: el mapa registra cero validaciones con revisión actual de dependencias.

## Inventario actual

| Medida | Cantidad | Interpretación |
| --- | ---: | --- |
| Entradas del catálogo principal con evidencia histórica | 8.445 / 8.445 | Alcance del 100% publicado |
| Entradas de función detectadas en el EXE | 17.765 | Límites detectados, no total semántico exacto |
| Entradas enlazadas con nombres del catálogo | 8.437 | Direcciones binarias, no número de filas del CSV |
| Entradas sin nombre del catálogo | 9.328 | Incluye juego, bibliotecas, helpers y código generado |
| Slots fuente leídos del EXE | 5.506 | Extensión declarada por 445 tablas |
| Slots con destino sin nombre del catálogo | 2.593 | Se repiten destinos por herencia |
| Destinos únicos de esos slots | 961 | No son 961 funciones por reconstruir demostradas |
| Destinos originales explícitos encontrados en source sin nombre catalogado | 924 | Cruce limitado de llamadas plugin y casts invocados |
| De esos destinos, con aristas desde funciones catalogadas | 499 | Prioridad útil para investigar dependencias |
| Identidades de registros fuente no enlazadas por owner/nombre | 181 | Candidatos; incluye diferencias de etiquetas de sobrecarga |
| Nombres de tipos fuente fuera de los owners del catálogo | 1.190 | Incluye POD, plantillas, auxiliares y bibliotecas |
| Llamadas indirectas sin resolver | 6.004 | Impiden afirmar un grafo de dependencias completo |

Los conjuntos se solapan y **no se suman**. La cifra anterior de 9.327 cambió a 9.328 al separar WinMain del catálogo principal; no apareció una función nueva. Los nueve tipos RTTI recuperados tampoco representan todas las clases. El total exacto de funciones de juego faltantes sigue sin determinarse.

## Omisiones concretas que merecen trabajo

| Clase o función | Dirección original | Estado observado |
| --- | --- | --- |
| CPed::ProcessControl | 0x005E8CD0 | Wrapper al EXE; instalación comentada |
| CPed::SpecialEntityPreCollisionStuff | 0x005E3C30 | Wrapper al EXE; destino heredado por varios tipos de ped |
| CPed::SpecialEntityCalcCollisionSteps | 0x005E3E90 | Wrapper al EXE; instalación comentada |
| CPed::ProcessEntityCollision | 0x005E2530 | Wrapper al EXE; instalación comentada |
| CAutomobile::PreRender / Render | 0x006AAB50 / 0x006A2B10 | Wrappers al EXE sin registro |
| CTaskSimpleFight::ProcessPed | 0x00629920 | Wrapper; InjectHooks declara tabla pero no instala métodos |
| CTaskComplexLeaveCar | Tabla 0x0086E828 | Clase sin entradas propias en el catálogo; revisar cada método |
| CTaskComplexAvoidEntity | Tabla 0x0086FF00 | Plantilla marcada como no reconstruida; uso en juego no demostrado |
| CAutomobile::SetUpWheelColModel | 0x006A3060 | Cuerpo C++ presente, sin registro ni equivalencia acreditada aquí |
| CPedGroups::GetPedsGroup | 0x005F7E80 | Wrapper; 60 aristas desde funciones catalogadas |
| CStats::GetFatAndMuscleModifier | 0x00559AF0 | Wrapper; 33 aristas desde funciones catalogadas |
| CCarCtrl::JoinCarWithRoadSystem | 0x0042F5A0 | Wrapper; modifica estado de vehículo y caminos |
| CPointLights::AddLight | 0x007000E0 | Wrapper de iluminación; dependencia de render |
| CDecisionMakerTypes::GetInstance | 0x004684F0 | Singleton del juego; InjectHooks vacío |
| CVector, CVector2D, CQuaternion y CMatrix | Varias | Registros presentes en fuente; llamadas de instalación desactivadas en InjectHooksMain por rendimiento |

Las dos revisiones enlazadas abajo conservan archivo/línea, slot o llamador y dirección/instrucción del EXE. Distinguen wrappers pendientes de fuente ya implementada. No se añadieron estas entradas al porcentaje ni se concedió validación por inspección.

Por contraste, `rand`, `operator new` y `RwTextureRead` son dependencias identificadas de CRT/RenderWare; requieren decidir cómo sustituir o conservar la biblioteca, pero no deben presentarse como funciones desconocidas de gameplay. Los destructores de borrado y los cuerpos base compartidos tampoco se cuentan una vez por cada slot.

## Qué procede

1. Crear un catálogo complementario de dependencias por identidad original: nombre, dirección, ABI, cuerpo compartido, fuente real o wrapper y uso. Empezar por los 499 destinos con referencias desde funciones ya catalogadas y los virtuales de control/render de ped y vehículo.
2. Reconstruir los wrappers necesarios; registrar y probar aparte la fuente existente sin hook. Una entrada nueva sólo recibe validación después del banco original/candidato dentro del juego con estados, retornos y efectos comparables.
3. Resolver llamadas indirectas y separar CRT, RenderWare, código generado y helpers. Mantener explícitos los casos todavía desconocidos.
4. Revisar vigencia de fuentes y dependencias, y ejecutar pruebas de integración de arranque, partida, vehículos, ped, tareas, audio, render y cierre. Sólo después plantear un ejecutable independiente y medir qué servicios/datos del original siguen siendo necesarios.

El siguiente objetivo verificable es **cubrir las dependencias reales del juego fuera del catálogo**, sin convertir automáticamente las 9.328 entradas en una lista de funciones faltantes. La reconstrucción y validación de esas dependencias es trabajo posterior a esta auditoría estática.

## Listas completas y reproducción

- [9.328 entradas sin nombre catalogado](../reversing/BinaryCoverageAudit/unmatched-functions.csv).
- [Slots sin nombre catalogado](../reversing/BinaryCoverageAudit/virtual-slots-outside-catalogue.csv).
- [Registros fuente candidatos](../reversing/BinaryCoverageAudit/source-registration-candidates.csv).
- [Tipos fuente fuera del catálogo](../reversing/BinaryCoverageAudit/source-types-outside-catalogue.csv).
- [924 destinos explícitos originales y referencias](../reversing/BinaryCoverageAudit/review-source-calls.csv).
- [Revisión de virtuales con evidencia](../reversing/BinaryCoverageAudit/review-virtuals.md).
- [Revisión de dependencias con evidencia](../reversing/BinaryCoverageAudit/review-source-calls.md).
- [Resumen, hashes y limitaciones](../reversing/BinaryCoverageAudit/summary.json).

`python -B contrib/audit_binary_coverage.py` regenera las cuatro listas de inventario a partir de MemoryMap, verifica recuentos y conserva hashes de entradas. `python -B contrib/sync_memory_map_readme.py` sincroniza las cifras del README del mapa con sus resúmenes. La extracción de llamadas explícitas se reproduce con `python -B reversing/BinaryCoverageAudit/audit_source_calls.py`; es un análisis sintáctico limitado, no un parser completo de C++.

EXE original de investigación: SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Se reutilizó el análisis global terminado de Ghidra y se contrastaron instrucciones mediante `reversing/_handoff/dis.sh`. No se incluyeron binarios, proyectos de Ghidra ni artifacts de compilación.

La consulta puntual adicional mediante el proveedor REA/Ghidra falló al analizar CVector::Normalise; no se utilizó como evidencia. La revisión conserva como respaldo las exportaciones globales existentes y el desensamblado directo. Se cerró la sesión de REA.
