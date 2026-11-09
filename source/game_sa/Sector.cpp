#include "StdInc.h"

#include "Sector.h"

// 0x563130 (lote 131A, banco Fn131A; el listado decia CWanted): destructor de CSector del EXE (thiscall sin argumentos, `ret`, 76 bytes, marco SEH que solo desenrolla y aqui se omite). Es el
// destructor de los 14400 sectores de ms_aSectors (0xB7D0B8, 8 bytes cada uno: lista simple de edificios en +0 y lista doble de dummies en +4) que registra el `atexit` del EXE (0x84E9B0 /
// 0x856520, con `push 0x563130`). Como el original: primero Flush de la lista doble de dummies (+4, 0x552470) y despues Flush de la lista simple de edificios (+0, 0x552400), sin tocar nada mas.
// Llama a las funciones propias de la DLL de esos dos Flush (no a sus direcciones del EXE) para que el banco las ejecute reales en el lado candidato.
static void __fastcall SectorDestructorExe(CSector* self, void* /*edx*/) {
    details::CPtrListDoubleLink__FlushExe(&self->Dummies, nullptr);
    details::CPtrListSingleLink__FlushExe(&self->Buildings, nullptr);
}

void CSector::InjectHooks() {
    RH_ScopedClass(CSector);
    RH_ScopedCategory("Core");

    RH_ScopedNamedGlobalInstall(SectorDestructorExe, "Destructor", 0x563130);
}
