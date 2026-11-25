#pragma once

/*
 * Banco aislado DENTRO del proceso para los arreglos de SilentPatch pendientes 7a (MIT, Copyright (c) 2024 Adrian Zdanowicz (Silent); docs/THIRD_PARTY_NOTICES.md):
 * escaleras mecanicas (EscalatorsCrashFix), liberacion de objetos temporales (TempObjectsFreeFix) y paracaidas skinned (SkinBuildingPipelineFix).
 * Solo actua con la variable de entorno GTASA_SILENTPATCH_SELFTEST_PEND7A=1. Escribe logs/silentpatch_pend7a_selftest.log (termina con "SELFTEST FIN").
 */
namespace SilentPatchPend7aSelfTest {
void OnFrame();
} // namespace SilentPatchPend7aSelfTest
