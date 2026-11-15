#pragma once

class CEntityScanner;
class CPed;
class CObject;

class CObjectScanner : public CEntityScanner {
public:
    CObjectScanner() : CEntityScanner() { } // 0x606010
    ~CObjectScanner() = default;            // 0x606050

    static void InjectHooks();
    void ScanForObjectsInRange(const CPed& ped); // 0x5FFF30
    CObject* GetClosestObjectInRange() const;
};
