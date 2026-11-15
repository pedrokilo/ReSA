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
