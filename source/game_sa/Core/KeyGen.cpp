/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CKeyGen
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "KeyGen.h"

/**
 * Registra los hooks de las rutinas de generación de claves CRC usadas por el juego.
 *
 * Los cuerpos reales de las funciones son constexpr y viven en KeyGen.h; aquí sólo
 * se asocian esas implementaciones con las direcciones conocidas del ejecutable.
 */
void CKeyGen::InjectHooks() {
    RH_ScopedClass(CKeyGen);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(AppendStringToKey, 0x53CF70);
    RH_ScopedOverloadedInstall(GetKey, "null-terminated", 0x53CF00, uint32(*)(const char*));
    RH_ScopedOverloadedInstall(GetKey, "sized", 0x53CED0, uint32(*)(const char*, int32));
    RH_ScopedOverloadedInstall(GetUppercaseKey, "", 0x53CF30, uint32(*)(const char*));
}
