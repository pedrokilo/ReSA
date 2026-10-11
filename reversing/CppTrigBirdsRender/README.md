# CppTrigBirdsRender: regresión de helpers trig simples

Preparado offline. No aplicado, compilado, ejecutado, aprobado ni registrado. Derivado independiente de Misc6; el banco histórico permanece intacto. Total previsto: **512 pares**, únicamente consumidores seleccionados.

| Consumidor | Índice histórico | Pares |
|---|---:|---:|
| CBirds::Render | 5 | 512 |

Se conserva la tabla completa original, no se compacta ni reordena. El filtro se aplica en el bucle de funciones después de obtener full y antes de generar casos. Así f e i mantienen sus valores originales: mismas semillas, secuencias aleatorias, fixtures y número de casos por consumidor. Se conservan todos los colaboradores/EntryPatch/hooks, su conmutación y restauración; funciones fuera del filtro no aportan pares ni crédito de conversión.

Gates idénticos a Misc6: snapshots completos original==candidate y switches verdaderos de ambos lados. Se conserva toda memoria, retornos, trazas ordenadas, fixtures y restauración final. Solo se amplía serialización de filas: snapshots completos original/candidate para pasantes void y todos los fallos; ningún placeholder nulo. Sin tolerancias ni normalización nueva de resultados.

El analizador exige exactamente el reparto señalado, secuencia de pares completa, igualdad/accepted y state_restored. El archivo opcional cpptrigbirdsrender-only.txt no permite aprobar cobertura parcial.

## Configuración y límites

Ambos scripts redirigen GAME en memoria a E:/JUEGOS/GTA San Andreas - CVehicle Test. No se modifica runtime_runner.py ni instalación personal. El manifiesto conserva fuentes históricas y añade TrigSimpleCpp.cpp/.h, CMakeLists raíz/source, inyección y ProbeSupport/ProbeRuntime, script de ejecución y este README. Las fuentes TrigSimpleCpp aún son propuestas en ../CppTrigSimple10Proposal: no se han copiado a producción; el build solo procede cuando el coordinador aplique su parche.

Estos gates históricos no demuestran status/control x87 completo, flags CPU ni raw registros por sí solos. Antes de GTA el coordinador exige objeto caller antiguo/nuevo equivalente fuera de las llamadas sustituidas y prueba del código emitido de helpers (FLD; FSIN/FCOS; FMUL cuando corresponda; RET sin redondeos/store intermedio/CRT/SSE/prologos). No se concede aprobación FP adicional por aprobar snapshots.

## Verificación offline

Scripts ast.parse válidos sin importar runner. Comparación normalizada del hpp frente al origen: idéntico salvo namespace/prefijo, filtro consumidor y serialización completa. Tablas y bucles de fixtures/semillas permanecen iguales. SHA256 de hpp origen: 8146649abb23b8f0ac41674a8383c26a7966151216d92fd35610147daad37961.
