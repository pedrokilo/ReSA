#pragma once

#include "Route.hpp"
#include "PathFind.h" // CNodeAddress

class CNodeRoute : public notsa::Route<CNodeAddress> {
public:
    static void InjectHooks();

    void Reverse(); // 0x667900 (lote 170A, banco Fn170A): inversion en el sitio con contador con signo; oculta la plantilla notsa::Route::Reverse

    static void* operator new(uint32 size);
    static void operator delete(void* ptr, size_t sz);
};
VALIDATE_SIZE(CNodeRoute, 0x24);
