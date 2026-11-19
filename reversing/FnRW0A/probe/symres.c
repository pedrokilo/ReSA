/* Resuelve RVA de gta_reversed.asi a simbolo+linea con su PDB (x64, porque el PDB pesa ~450 MB). Uso: symres asi rva_hex... */
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    HANDLE p = GetCurrentProcess();
    DWORD64 base = 0x10000000ULL;
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
    if (!SymInitialize(p, NULL, FALSE)) return 1;
    DWORD64 b = SymLoadModuleEx(p, NULL, argv[1], NULL, base, 0x4000000, NULL, 0);
    if (!b) { printf("no se pudo cargar: %lu\n", GetLastError()); return 2; }
    for (int i = 2; i < argc; i++) {
        DWORD64 rva = strtoull(argv[i], NULL, 16), disp = 0;
        char buf[sizeof(SYMBOL_INFO) + 512]; SYMBOL_INFO *s = (SYMBOL_INFO *)buf;
        s->SizeOfStruct = sizeof(SYMBOL_INFO); s->MaxNameLen = 511;
        IMAGEHLP_LINE64 ln = {sizeof ln}; DWORD ld = 0;
        printf("0x%llX ", (unsigned long long)rva);
        if (SymFromAddr(p, base + rva, &disp, s)) printf("%s+0x%llX", s->Name, (unsigned long long)disp); else printf("?");
        if (SymGetLineFromAddr64(p, base + rva, &ld, &ln)) printf("  %s:%lu", ln.FileName, ln.LineNumber);
        printf("\n");
    }
    return 0;
}
