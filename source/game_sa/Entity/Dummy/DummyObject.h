/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Dummy.h"

/**
 * Proxy de streaming capaz de intercambiar su recurso RenderWare con un CObject.
 *
 * Permite conservar una representación ligera cuando el objeto dinámico no necesita
 * permanecer materializado.
 */
class NOTSA_EXPORT_VTABLE CDummyObject : public CDummy {
public:
    CDummyObject();
    explicit CDummyObject(CObject* obj);

public:
    static void InjectHooks();

    CObject* CreateObject();
    void     UpdateFromObject(CObject* obj);

private:
    CDummyObject* ScalarDeletingDestructor(uint8 flags); // 0x59EB50 (lote 125A, banco Fn125A): destructor con borrado (ranura 0 de la vtable 0x866E78), `ret 4`
};

VALIDATE_SIZE(CDummyObject, 0x38);
