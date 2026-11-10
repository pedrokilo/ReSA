#pragma once

struct CKeyEntry {
    const GxtChar* string; // relative to TKEY block; need to call update after reading
    uint32      hash;      // name hash
};

// TKEY block
class CKeyArray {
public:
    CKeyEntry* m_data;
    uint32     m_size;

public:
    CKeyArray();
    ~CKeyArray();

    bool Load(uint32 length, FILESTREAM file, uint32* offset, uint8 unknown);
    void Unload();
    // 0x69FFA0 (lote 144A, banco Fn144A): cuerpo del destructor del EXE (Unload en linea: libera m_data con delete[] solo si no es nulo, lo anula y pone m_size a 0; sin llamadores directos; atribucion
    // CKeyArray/CData por orden de direcciones, los dos cuerpos del EXE (0x69FFA0 y 0x69FFD0) son identicos byte a byte)
    __declspec(noinline) void Destructor();

    void Update(char* offset);

    /*!
     * @brief Find text by it's key
     *
     * @param     key   The text's key
     * @param out found Whenever the text was found
     *
     * @return The text identified by the given key or null if not found
     */
    [[nodiscard]] const GxtChar* Search(const char* key, bool& found);

private:
    CKeyEntry* BinarySearch(uint32 key, CKeyEntry* entries, int16 firstIndex, int16 lastIndex);

private:
    friend class CText; // InjectHooks()
};
