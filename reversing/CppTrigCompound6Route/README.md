<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppTrigCompound6Route: banco independiente pendiente

Derivado de Route19; consumidor CTaskComplexFollowNodeRoute::CalcBlendRatio, indice original f=11, 1024 pares previstos. Tabla completa sin compactar; mismos casos i, semillas, fixtures, colaboradores y gates. Hpp normalizado frente original identico salvo namespace/prefijo/filtro/serializacion.

Snapshot completo original/candidate en todos los pares, sin NULL pasantes ni recortes de fallos. audit_nested.py aplica analizador historico de cobertura/equal/accepted/pares unicos/restauracion; exige objetos completos, ret/trace/switches, switches true y clean true en ambos lados. Ret void NULL interno es dato legitimo. Conserva limpieza y rutas originales, sin tolerancias.

Scripts redirigen GAME solo en memoria a instalacion de prueba. Manifest incluye fuentes originales, TrigCompound6Cpp.cpp/.h, CMake, inyeccion/runtime y auditores/scripts propios. Propuesta integrada6 aun no aplicada.

No hay registros fisicos completos capturados en banco historico; no inventar ABI por snapshots. Requiere codegen completo de helper/prologo/epilogo y consumidor COFF old/new exacto salvo destinos autorizados, con relocations/initializers. Status FP adicional no certificado por este corpus. Root debe demostrar ambos objetos antes de GTA.

Sin build, GTA, Git ni edicion de produccion o banco anterior.

Hash hpp original:35b44ed15de6d6004c0bee08a18c67d9ed1f95a58d34167f1fde83ce64b1fe86.
