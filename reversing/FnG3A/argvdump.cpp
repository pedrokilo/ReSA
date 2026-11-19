// Referencia: lo que el CRT de MSVC (UCRT, __argc/__argv) obtiene de GetCommandLineA(). Escribe en el archivo de %ARGOUT%.
// Formato: "argc" y, por cada argv[i], su contenido en hex.
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>

int main() {
    const char* out = std::getenv("ARGOUT");
    FILE* f = out ? std::fopen(out, "wb") : stdout;
    std::fprintf(f, "%d", __argc);
    for (int i = 0; i < __argc; i++) {
        std::fputc('|', f);
        for (const unsigned char* p = (const unsigned char*)__argv[i]; *p; ++p) {
            std::fprintf(f, "%02x", *p);
        }
    }
    std::fputc('\n', f);
    if (out) std::fclose(f);
    return 0;
}
