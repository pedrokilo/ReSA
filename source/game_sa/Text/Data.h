#pragma once

// TDAT block
class CData {
    friend class CText; // InjectHooks()
public:
    char*  m_data;
    uint32 m_size;

public:
    CData();
    ~CData();

    bool Load(uint32 length, FILESTREAM file, uint32* offset, uint8 unknown);
    void Unload();
    // 0x69FFD0 (lote 144A, banco Fn144A): cuerpo del destructor del EXE (identico a 0x69FFA0 de CKeyArray; ver ahi)
    __declspec(noinline) void Destructor();
};
