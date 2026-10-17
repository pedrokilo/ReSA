<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Seis helpers compuestos: propuesta integrada independiente

Pendiente de admision real. Combina las tres variantes exactas del estudio inicial y las tres corregidas de CppTrigCompoundStudy/final3-proof.json. Los objetos de estudio demuestran cuerpos completos, no subsecuencias ni resultados en juego. La configuracion real /Oy- puede agregar prologo EBP; verificar de nuevo todos los helpers.

El parche sustituye seis naked y seis bloques asm en cuatro consumidores: Markers, Birds, FollowNodeRoute y Swim. No llama al cuerpo GAME. Nuevo TU TrigCompound6Cpp sin PCH/Unity, /O2 /Oi /arch:IA32 /fp:fast y /Zi, sin RTC. No altera opciones de los consumidores.

Las tres correcciones integradas: NegSinDiv separa sin, negacion y division para conservar FCHS antes FDIV; CosHalf lee half dos veces mediante const volatile float* local y conserva FCOS/FMUL/FADD; SubSin usa -(sin(angle)-z) para emitir FSUBR exacto. No se conserva el pragma precise rechazado.

Bancos nuevos y separados: CppTrigCompound6Markers4608, CppTrigCompound6Birds2560, CppTrigCompound6Route1024 y CppTrigCompound6Swim1024. Total previsto9216pares. Preservan indices, semillas, fixtures y gates, snapshots completos y auditores nested. Todos los manifests apuntan al TU/header6, CMake y scripts correctos. Bancos/propuestas anteriores permanecen intactos.

Update de Birds (0x712330, 1024 pares) consume CosMulAdd. CreateNumberOfBirds permanece como acompañante y no acredita ese helper. El auditor exige llamadas water observadas después de calcular ambos componentes de spawn.

No inventar garantia rawABI ausente de snapshots historicos: requiere helper codegen completo exacto y COFF consumidores antiguos/nuevos identicos salvo destinos autorizados, con relocations e inicializadores pertinentes. Luego prueba serial dentro del juego y auditoria de pares/restauracion. Sin tolerancias nuevas ni creditos de validacion anticipados.

Propuesta offline: sin produccion, build, GTA o Git.

## Resultado final

Integracion aprobada: seis helpers, 9216 pares compartidos en cuatro bancos, cero diferencias y restauracion verificada. Registro final en ../CppOnlyMigration/approvals.json. Los trece consumidores conservan instrucciones y relocaciones salvo los destinos autorizados. Los apartados de propuesta y fallos previos son historicos.
