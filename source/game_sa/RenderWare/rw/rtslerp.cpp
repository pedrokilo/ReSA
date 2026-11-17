/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RtSlerp
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rtslerp.h"
#include "X87Math.h"

static_assert(sizeof(RtQuat) == 0x10 && sizeof(RtQuatSlerpCache) == 0x28 && offsetof(RtQuatSlerpCache, raTo) == 0x10 && offsetof(RtQuatSlerpCache, omega) == 0x20 && offsetof(RtQuatSlerpCache, nearlyZeroOm) == 0x24);

void RtSlerpInjectHooks() {
    RH_ScopedNamespaceName("RtSlerp");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RtQuatSetupSlerpCache, 0x7EC220);
}

// 0x7EC220: prepara la cache de slerp (raFrom, raTo, omega, nearlyZeroOm). Las matematicas (producto escalar, arcocoseno de RW, escalado por 1/sen aproximado) estan en
// RtSlerpSetupCacheCpp (X87Math.cpp, compilado con x87 a 24 bits): ver su comentario en X87Math.h. El EXE solo toca EAX, ECX y EDX (conserva ESI/EDI) y reutiliza los tres huecos de los argumentos de pila
// como variables locales; sus llamadores (0x616ECA, 0x651E42, 0x6522BD, cdecl: limpian ellos) no releen esos huecos, asi que no hace falta reproducirlo.
void RtQuatSetupSlerpCache(RtQuat* qpFrom, RtQuat* qpTo, RtQuatSlerpCache* sCache) {
    RtSlerpSetupCacheCpp(reinterpret_cast<const float*>(qpFrom), reinterpret_cast<const float*>(qpTo), reinterpret_cast<float*>(sCache));
}
