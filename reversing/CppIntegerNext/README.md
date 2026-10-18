<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

> Actualización del coordinador: Texture aplicada y aprobada en 20261010-235425-798187, 2048 pares, cero diferencias, cero registros nulos, rutas y restauración verificadas mediante auditoría independiente. [Auditoría](Texture/runs/20261010-235425-798187/approval-audit.json). Hierarchy sigue sin aplicar ni aprobar. Los párrafos siguientes describen la preparación offline original.

# CppIntegerNext — texturas aprobadas; jerarquías pendientes

| Función | EXE | Banco derivado | Pares previstos |
| --- | --- | --- | --- |
| RwTexDictionaryFindHashNamedTexture | 0x734E50..0x734E8F | Texture, de DependenciesWorld11 | 2048 |
| GetAnimHierarchyFromFrame | 0x734AB0..0x734B03 | Hierarchy, de DependenciesWorld12 | 2048 |

No se compiló ni ejecutó el juego. La propuesta no constituye validación. `source-proposal.patch` aplica exclusivamente sobre RwHelper.cpp; `candidate.cpp.txt` facilita revisión. Instrumentos derivados viven por separado en Texture/ e Hierarchy/ para ejecutar serialmente y conservar exactamente los analizadores, corpus y gates de cada banco admitido. No se editan bancos históricos.

## Texturas

EXE 734E56 add EBX,8 obtiene sentinel; 734E65 LEA EDI,[ESI-8] recupera RwTexture desde lInDictionary; 734E68 name en offset16; 734E70 llama GetUppercaseKey53CF30; comparación DWORD y primer match. C++ usa miembros RW/offsetof y misma lista intrusiva, sin asignaciones ni strings nuevos. Retorno puntero completo EAX, cdecl RET sin limpieza de argumentos. Dominio: diccionarios legítimos y vacíos constructor/destructor RW reales, nombres terminados válidos. El test EXE del puntero name=0 sólo sería alcanzable por puntero ilegítimo con wrap: no pertenece al corpus ni se atribuye cobertura. No alias de CRT ni delegación al cuerpo734E50.

## Jerarquías

Callback734A70 se reconstruye también en C++, nunca se invoca su cuerpo GAME original. Biblioteca RpHAnimFrameGetHierarchy y RwFrameForAllChildren permanecen colaboradores RW permitidos. Callback: jerarquía propia escribe output y retornaNULL; sin propia recorre hijos y retorna frame, incluso si un descendiente ya escribió output. Esa distinción conserva continuación por hermanos. Main734AB0 hace GetHierarchy; primer paseo siNULL; segundo paseo si sigueNULL. No optimizar eliminando el segundo paseo. Frames/punteros/plugin y jerarquías reales prestados; frame vacío creado/destruido por biblioteca como en banco histórico. No se crean actores falsos.

Ambos bancos conservan EAX completo, ESP/EBX/ESI/EDI/EBP, snapshots completos, rutas OFF/ON y colaboradores sin stubs; las jerarquías conservan FP108/MXCSR restaurado y recursos creados/destruidos. Ningún gate binario de identidad ensamblador se ha eliminado. El callback original734A70 permanece intacto y comprobado en el EXE como referencia; la candidata usa callback fuente propio.

Riesgo pendiente antes de aplicar: el EXE de GetAnimHierarchyFromFrame preserva ECX mediante PUSH/POP, algo que C++ cdecl no garantiza. El banco histórico verifica no volátiles, no ECX; consumidores fuente visibles son ClumpModelInfo y GetAnimHierarchyFromClump y reciben el puntero de retorno por ABI normal. No se ha demostrado que TODOS los callers del EXE ignoren ECX. Requiere revisar callers binarios antes de dar aprobación universal. Texturas tiene consumidores visibles Hud y HandObject; sin dependencia de registros crudos observable en fuente. Tampoco se afirma cobertura binaria universal.

Sintaxis de cuatro scripts correcta y parche comprobado aplicable. Root decide orden, aplica y ejecuta ambos bancos en serie. Los resultados futuros acreditarán sólo corpus y fuente probados.


## Actualizacion g25a/g25b (Hierarchy): nueva definicion de la ruta
- La ruta del candidato es ahora la del hook REGISTRADO (`HookRoute::RegisteredTarget`): `GetAnimHierarchyFromFrame` se engancha con la envoltura `GetAnimHierarchyFromFrameRegs`, asi que
  `Resolve(0x734AB0)` nunca fue `&GetAnimHierarchyFromFrame` (el cuerpo que comparaba `route_ok`). `route_ok` sigue exigiendo salto a la DLL, cuerpo == destino vivo del hook y modulo del ASI en el candidato, y ausencia de salto en el original.
- g25a: la DLL ya no pasa a `RwFrameForAllChildren` la direccion nativa 0x734A70 sino `GetAnimHierarchyFromFrameCallback` (reconstruido y validado en Dependencies25A, sin hook). El original sigue usando el callback NATIVO
  (la compuerta `library_route_ok` exige 0x734A70 intacta antes y despues de cada lado) y el candidato el de la DLL; el evento `begin` lleva ahora `callback_dll_ok` (el callback de la DLL esta en el ASI y no es 0x734A70) y run_local.py lo exige.
  La comparacion de retorno, jerarquia, memoria, ABI y x87 no cambia. g25b: el hook de 0x7C5160 se apaga durante la corrida (`libraryRoute`), la biblioteca nativa se sigue exigiendo.
