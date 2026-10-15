#pragma once

class CPed;

struct PlayerRelationship {
    const CPed* Ped;
    int32       Relationship;

    PlayerRelationship() { Flush(); }

    void Flush() {
        Ped = nullptr;
        Relationship = 0;
    }
};

// useless class (unfinished)
class CPlayerRelationshipRecorder {
public:
    std::array<PlayerRelationship, 16> m_Relationships;

public:
    static void InjectHooks();
    CPlayerRelationshipRecorder();
    ~CPlayerRelationshipRecorder();

    void Flush();
    void ClearRelationshipWithPlayer(const CPed* ped);
    void AddRelationship(const CPed* ped, int32 value);
    int32 GetRelationshipWithPlayer(const CPed* ped);
    void RecordRelationshipWithPlayer(const CPed* ped);
    // Forma del EXE (0x61A1D0): this en ECX, EDX de entrada y ped en pila; deja EAX/ECX/EDX como el EXE (ver el .cpp). Es la que engancha el hook.
    static uint32 __fastcall RecordRelationshipWithPlayerRegs(CPlayerRelationshipRecorder* self, uint32 entryEdx, const CPed* ped);
};

CPlayerRelationshipRecorder& GetPlayerRelationshipRecorder();

VALIDATE_SIZE(CPlayerRelationshipRecorder, 0x80);
