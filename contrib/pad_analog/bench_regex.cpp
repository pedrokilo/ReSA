// Banco del motor de expresiones regulares de las sustituciones del GXT de GInputSA (docs/CodigoVivoExe/lectura-mando-ginput.md, seccion 3): aplica cada una de las 124 entradas con la misma
// secuencia que PadButtonIcons::ApplyGxtFix (std::regex_search y despues std::regex_replace, ECMAScript por defecto) sobre cada texto de entrada de regex_vectores.tsv y compara con el
// resultado esperado calculado por `re` de Python (otro motor). Se repite con std::regex::icase para comprobar que la bandera no cambia nada en esta tabla.
// Compilar: cl /nologo /std:c++20 /EHsc /O2 bench_regex.cpp ; uso: bench_regex regex_vectores.tsv
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdio>

static std::vector<std::string> Split(const std::string& l) {
    std::vector<std::string> r; std::string cur;
    for (char c : l) { if (c == '\t') { r.push_back(cur); cur.clear(); } else { cur += c; } }
    r.push_back(cur); return r;
}

static std::string Apply(const std::string& pattern, const std::string& replacement, const std::string& text, std::regex::flag_type flags, bool* matched) {
    if (pattern.empty()) { *matched = true; return replacement; }
    const std::regex rx(pattern, flags);
    if (!std::regex_search(text, rx)) { *matched = false; return text; }
    *matched = true;
    return std::regex_replace(text, rx, replacement);
}

int main(int argc, char** argv) {
    if (argc < 2) { puts("uso: bench_regex regex_vectores.tsv"); return 2; }
    std::ifstream in(argv[1], std::ios::binary);
    std::string line;
    int total = 0, fail = 0, failIcase = 0, real = 0, matchedReal = 0, claves = 0;
    std::string lastKey;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        const auto f = Split(line);
        if (f.size() < 6) continue;
        if (f[0] != lastKey) { lastKey = f[0]; claves++; }
        bool m1 = false, m2 = false;
        const std::string r1 = Apply(f[1], f[2], f[4], std::regex::ECMAScript, &m1);
        const std::string r2 = Apply(f[1], f[2], f[4], std::regex::ECMAScript | std::regex::icase, &m2);
        const bool expectNoMatch = f[5] == "<<SIN COINCIDENCIA>>";
        const bool ok = expectNoMatch ? (!m1 && r1 == f[4]) : (m1 && r1 == f[5]);
        const bool ok2 = expectNoMatch ? (!m2 && r2 == f[4]) : (m2 && r2 == f[5]);
        total++;
        if (f[3].rfind("gxt:", 0) == 0) { real++; matchedReal += m1; }
        if (!ok) { fail++; if (fail < 15) printf("FALLO %s (%s): entrada '%s' -> C++ '%s' (coincide=%d), esperado '%s'\n", f[0].c_str(), f[3].c_str(), f[4].c_str(), r1.c_str(), m1, f[5].c_str()); }
        if (!ok2) { failIcase++; }
    }
    printf("claves: %d, casos: %d (textos reales del GXT: %d, de ellos con coincidencia: %d), fallos ECMAScript: %d, fallos con icase: %d\n", claves, total, real, matchedReal, fail, failIcase);
    printf("BANCO REGEX: %s\n", (fail == 0 && failIcase == 0) ? "OK" : "FALLO");
    return fail ? 1 : 0;
}
