# CppTrigBrightPlane: regresión de helpers trig simples

Preparado offline. No aplicado, compilado, ejecutado, aprobado ni registrado. Derivado independiente de Render23; el banco histórico permanece intacto. Total previsto: **1536 pares**, únicamente consumidores seleccionados.

| Consumidor | Índice histórico | Pares |
|---|---:|---:|
| CBrightLights::Render | 2 | 512 |
| CPlaneTrails::Update | 11 | 1024 |

Se conserva la tabla completa original, no se compacta ni reordena. El filtro se aplica en el bucle de funciones después de obtener full y antes de generar casos. Así f e i mantienen sus valores originales: mismas semillas, secuencias aleatorias, fixtures y número de casos por consumidor. Se conservan todos los colaboradores/EntryPatch/hooks, su conmutación y restauración; funciones fuera del filtro no aportan pares ni crédito de conversión.

Gates idénticos a Render23: snapshots completos original==candidate y switches verdaderos de ambos lados. Se conserva toda memoria, retornos, trazas ordenadas, fixtures y restauración final. Solo se amplía serialización de filas: snapshots completos original/candidate para pasantes void y todos los fallos; ningún placeholder nulo. Sin tolerancias ni normalización nueva de resultados.

El analizador exige exactamente el reparto señalado, secuencia de pares completa, igualdad/accepted y state_restored. El archivo opcional cpptrigbrightplane-only.txt no permite aprobar cobertura parcial.

## Configuración y límites

Ambos scripts redirigen GAME en memoria a E:/JUEGOS/GTA San Andreas - CVehicle Test. No se modifica runtime_runner.py ni instalación personal. El manifiesto conserva fuentes históricas y añade TrigSimpleCpp.cpp/.h, CMakeLists raíz/source, inyección y ProbeSupport/ProbeRuntime, script de ejecución y este README. Las fuentes TrigSimpleCpp aún son propuestas en ../CppTrigSimple10Proposal: no se han copiado a producción; el build solo procede cuando el coordinador aplique su parche.

Estos gates históricos no demuestran status/control x87 completo, flags CPU ni raw registros por sí solos. Antes de GTA el coordinador exige objeto caller antiguo/nuevo equivalente fuera de las llamadas sustituidas y prueba del código emitido de helpers (FLD; FSIN/FCOS; FMUL cuando corresponda; RET sin redondeos/store intermedio/CRT/SSE/prologos). No se concede aprobación FP adicional por aprobar snapshots.

## Verificación offline

Scripts ast.parse válidos sin importar runner. Comparación normalizada del hpp frente al origen: idéntico salvo namespace/prefijo, filtro consumidor y serialización completa. Tablas y bucles de fixtures/semillas permanecen iguales. SHA256 de hpp origen: f168f9e003be12bc6e28a7def3d991c5a93b429bf157c4266feb860892383295.
