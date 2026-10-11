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
