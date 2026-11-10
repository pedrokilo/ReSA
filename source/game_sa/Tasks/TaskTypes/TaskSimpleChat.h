#pragma once

#include "TaskSimpleRunTimedAnim.h"

class CPed;

class NOTSA_EXPORT_VTABLE  CTaskSimpleChat : public CTaskSimpleRunTimedAnim {
public:
    static constexpr auto Type = TASK_SIMPLE_CHAT;

    static void InjectHooks();

    CTaskSimpleChat(uint32 duration);
    CTaskSimpleChat(const CTaskSimpleChat& o);
    ~CTaskSimpleChat();

    virtual CTask* Clone() const { return new CTaskSimpleChat{ *this }; }
    virtual bool   IsInterruptable(CPed const* ped) { return false; }
 
private:
    // 0x5F7FD0 (sin expansion en linea desde el lote 148A: el banco sustituye su cuerpo)
    __declspec(noinline) CTaskSimpleChat* Destructor() {
        this->CTaskSimpleChat::~CTaskSimpleChat();
        return this;
    }
    CTaskSimpleChat* ScalarDeletingDestructor(uint8 flags); // 0x5F7FB0 (lote 148A, banco Fn148A): destructor con borrado (ranura 0 de la vtable 0x86C78C), `ret 4`
    static CTaskSimpleChat* __fastcall ConstructorExe(CTaskSimpleChat* self, void* edx, uint32 duration); // 0x5F65A0 (lote 151A, banco Fn151A): constructor del EXE, vtable 0x86C78C del EXE
};
