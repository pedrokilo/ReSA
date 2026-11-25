// FnD3DX9C: ver D3DX9NativeAssembler.h. Formato del bytecode de Direct3D 9 (vertex shaders 2.0 / 2.x).
#include <cctype>
#include <cstring>

#include "D3DX9NativeAssembler.h"

namespace D3DX9Native {
namespace {
// Tipos de registro (D3DSHADER_PARAM_REGISTER_TYPE).
enum RegType : uint32_t { kTemp = 0, kInput = 1, kConst = 2, kAddr = 3, kRastOut = 4, kAttrOut = 5, kTexCrdOut = 6 };

struct Param {
    RegType type = kTemp;
    uint32_t num = 0;
    uint32_t mask = 0xF;        // destino: bits x=1 y=2 z=4 w=8
    uint32_t swizzle = 0xE4;    // origen: 2 bits por componente (x=0 y=1 z=2 w=3), el componente 0 en los bits bajos
    uint32_t srcMod = 0;        // 1 = negacion
    bool relative = false;
    uint32_t relComp = 0;       // componente de a0 (0..3)
};

struct Op { const char* name; uint32_t code; int sources; };
constexpr Op kOps[] = {
    {"mov", 0x01, 1}, {"add", 0x02, 2}, {"mad", 0x04, 3}, {"mul", 0x05, 2}, {"rcp", 0x06, 1}, {"rsq", 0x07, 1}, {"dp3", 0x08, 2}, {"dp4", 0x09, 2},
    {"min", 0x0A, 2}, {"max", 0x0B, 2}, {"slt", 0x0C, 2}, {"sge", 0x0D, 2}, {"m4x4", 0x14, 2}, {"m4x3", 0x15, 2}, {"m3x3", 0x17, 2}, {"mova", 0x2E, 1}, {"expp", 0x4E, 1},
};

struct Usage { const char* name; uint32_t code; };
constexpr Usage kUsages[] = {
    {"position", 0}, {"blendweight", 1}, {"blendindices", 2}, {"normal", 3}, {"psize", 4}, {"texcoord", 5}, {"tangent", 6}, {"binormal", 7}, {"color", 10},
};

struct Cursor {
    const char* p;
    const char* end;
    bool Eof() const { return p >= end; }
    void SkipSpace() { while (p < end && (*p == ' ' || *p == '\t')) ++p; }
    bool Eat(char c) { SkipSpace(); if (p < end && *p == c) { ++p; return true; } return false; }
    bool Number(uint32_t& v) {
        SkipSpace();
        if (p >= end || !std::isdigit(static_cast<unsigned char>(*p))) return false;
        uint64_t x = 0;
        while (p < end && std::isdigit(static_cast<unsigned char>(*p))) {
            x = x * 10 + static_cast<uint32_t>(*p - '0');
            if (x > 0xFFFFF) return false;
            ++p;
        }
        v = static_cast<uint32_t>(x);
        return true;
    }
};

int Component(char c) {
    switch (c) { case 'x': return 0; case 'y': return 1; case 'z': return 2; case 'w': return 3; default: return -1; }
}

// ".xyzw" -> mascara de escritura (componentes en orden estricto x<y<z<w, sin repetir)
bool ParseMask(Cursor& c, uint32_t& mask) {
    if (c.p >= c.end || *c.p != '.') { mask = 0xF; return true; }
    ++c.p;
    mask = 0;
    int last = -1;
    int count = 0;
    while (c.p < c.end && Component(*c.p) >= 0) {
        const int comp = Component(*c.p);
        if (comp <= last) return false;
        mask |= 1u << comp;
        last = comp;
        ++c.p;
        ++count;
    }
    return count > 0;
}

// ".xyzw", ".x", ".xy", ".zxyw" -> permutacion (se rellena repitiendo el ultimo componente)
bool ParseSwizzle(Cursor& c, uint32_t& swz) {
    if (c.p >= c.end || *c.p != '.') { swz = 0xE4; return true; }
    ++c.p;
    int comps[4];
    int count = 0;
    while (c.p < c.end && Component(*c.p) >= 0 && count < 4) comps[count++] = Component(*c.p++);
    if (count == 0) return false;
    for (int i = count; i < 4; ++i) comps[i] = comps[count - 1];
    swz = static_cast<uint32_t>(comps[0] | comps[1] << 2 | comps[2] << 4 | comps[3] << 6);
    return true;
}

// Registro: rN, vN, cN, c[expr], a0, oPos, oFog, oDN, oTN. Devuelve false si no es de la gramatica soportada.
bool ParseRegister(Cursor& c, Param& r, bool destination, std::string& why) {
    c.SkipSpace();
    if (c.Eof()) { why = "registro vacio"; return false; }
    const char ch = *c.p;
    uint32_t n = 0;
    if (ch == 'r' || ch == 'v' || (ch == 'c' && c.p + 1 < c.end && c.p[1] != '[')) {
        ++c.p;
        if (!c.Number(n)) { why = "numero de registro"; return false; }
        r.type = ch == 'r' ? kTemp : ch == 'v' ? kInput : kConst;
        if (r.type == kTemp && n > 11) { why = "temporal fuera de rango"; return false; }
        if (r.type == kInput && n > 15) { why = "entrada fuera de rango"; return false; }
        if (r.type == kConst && n > 255) { why = "constante > 255"; return false; }
        r.num = n;
    } else if (ch == 'c') { // c[...]
        c.p += 2;
        r.type = kConst;
        uint32_t total = 0;
        for (;;) {
            c.SkipSpace();
            if (c.p + 1 < c.end && c.p[0] == 'a' && c.p[1] == '0') { // a0.x
                if (r.relative) { why = "dos registros de direccion"; return false; }
                c.p += 2;
                if (c.p >= c.end || *c.p != '.') { why = "a0 sin componente"; return false; }
                ++c.p;
                const int comp = c.p < c.end ? Component(*c.p) : -1;
                if (comp < 0) { why = "componente de a0"; return false; }
                ++c.p;
                r.relative = true;
                r.relComp = static_cast<uint32_t>(comp);
            } else {
                uint32_t v;
                if (!c.Number(v)) { why = "indice de constante"; return false; }
                total += v;
            }
            if (c.Eat('+')) continue;
            if (c.Eat(']')) break;
            why = "sintaxis de c[...]";
            return false;
        }
        if (total > 255) { why = "constante > 255"; return false; }
        r.num = total;
    } else if (ch == 'a' && c.p + 1 < c.end && c.p[1] == '0') {
        c.p += 2;
        r.type = kAddr;
        r.num = 0;
    } else if (ch == 'o') {
        if (c.end - c.p >= 4 && std::strncmp(c.p, "oPos", 4) == 0) { c.p += 4; r.type = kRastOut; r.num = 0; }
        else if (c.end - c.p >= 4 && std::strncmp(c.p, "oFog", 4) == 0) { c.p += 4; r.type = kRastOut; r.num = 1; }
        else if (c.end - c.p >= 3 && c.p[1] == 'D') { c.p += 2; if (!c.Number(n) || n > 1) { why = "oD fuera de rango"; return false; } r.type = kAttrOut; r.num = n; }
        else if (c.end - c.p >= 3 && c.p[1] == 'T') { c.p += 2; if (!c.Number(n) || n > 7) { why = "oT fuera de rango"; return false; } r.type = kTexCrdOut; r.num = n; }
        else { why = "registro de salida no soportado"; return false; }
        if (!destination) { why = "registro de salida como origen"; return false; }
        if (r.type == kRastOut && r.num == 1 && c.p < c.end && *c.p == '.') { why = "oFog con mascara (D3DX lo rechaza)"; return false; }
    } else {
        why = "registro no soportado";
        return false;
    }
    if (destination) {
        if (!ParseMask(c, r.mask)) { why = "mascara de escritura"; return false; }
    } else {
        if (!ParseSwizzle(c, r.swizzle)) { why = "permutacion"; return false; }
    }
    return true;
}

uint32_t DestToken(const Param& r) {
    return 0x80000000u | r.num | (r.type & 7u) << 28 | (r.type & 0x18u) << 8 | r.mask << 16;
}
uint32_t SrcToken(const Param& r) {
    return 0x80000000u | r.num | (r.type & 7u) << 28 | (r.type & 0x18u) << 8 | (r.relative ? 1u << 13 : 0u) | r.swizzle << 16 | r.srcMod << 24;
}

bool IsIdent(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }
} // namespace

AssembleResult AssembleVertexShader(const char* source, size_t length, std::vector<uint32_t>& out, std::string& reason) {
    out.clear();
    std::string lineText;
    const auto unsupported = [&](const std::string& why) { reason = why + " [" + lineText + "]"; return AssembleResult::Unsupported; };
    const char* p = source;
    const char* const end = source + length;
    bool haveVersion = false;
    int lineNo = 0;
    while (p < end) {
        const char* eol = static_cast<const char*>(std::memchr(p, '\n', static_cast<size_t>(end - p)));
        const char* lineEnd = eol ? eol : end;
        const char* next = eol ? eol + 1 : end;
        ++lineNo;
        Cursor c{p, lineEnd};
        while (c.end > c.p && (c.end[-1] == '\r' || c.end[-1] == ' ' || c.end[-1] == '\t')) --c.end;
        c.SkipSpace();
        lineText.assign(c.p, c.end);
        p = next;
        if (c.Eof()) continue; // linea vacia
        const std::string where = "linea " + std::to_string(lineNo) + ": ";
        const char* ms = c.p;
        while (c.p < c.end && IsIdent(*c.p)) ++c.p;
        const std::string mn(ms, c.p);
        if (mn.empty()) return unsupported(where + "sin mnemonico");
        if (!haveVersion) {
            if (mn == "vs_2_0") out.push_back(0xFFFE0200u);
            else if (mn == "vs_2_x") out.push_back(0xFFFE0201u);
            else return unsupported(where + "version no soportada '" + mn + "'");
            haveVersion = true;
            c.SkipSpace();
            if (!c.Eof()) return unsupported(where + "texto tras la version");
            continue;
        }
        std::string why;
        if (mn.rfind("dcl_", 0) == 0) {
            const Usage* usage = nullptr;
            size_t nameLen = 0;
            for (const auto& u : kUsages) {
                const size_t l = std::strlen(u.name);
                if (mn.compare(4, l, u.name) == 0 && (usage == nullptr || l > nameLen)) { usage = &u; nameLen = l; }
            }
            if (!usage) return unsupported(where + "uso de dcl no soportado '" + mn + "'");
            uint32_t index = 0;
            const std::string digits = mn.substr(4 + nameLen);
            if (!digits.empty()) {
                for (char d : digits) if (!std::isdigit(static_cast<unsigned char>(d))) return unsupported(where + "indice de dcl");
                if (digits.size() > 2) return unsupported(where + "indice de dcl grande");
                index = static_cast<uint32_t>(std::stoul(digits));
            }
            Param r;
            if (!ParseRegister(c, r, true, why)) return unsupported(where + "dcl: " + why);
            if (r.type != kInput) return unsupported(where + "dcl: el registro no es de entrada");
            r.mask = 0xF;
            out.push_back(0x0000001Fu | 2u << 24);
            out.push_back(0x80000000u | usage->code | index << 16);
            out.push_back(DestToken(r));
            c.SkipSpace();
            if (!c.Eof()) return unsupported(where + "texto tras dcl");
            continue;
        }
        const Op* op = nullptr;
        for (const auto& o : kOps) if (mn == o.name) { op = &o; break; }
        if (!op) return unsupported(where + "instruccion no soportada '" + mn + "'");
        Param dst;
        if (!ParseRegister(c, dst, true, why)) return unsupported(where + why);
        if (dst.type == kConst || dst.type == kInput) return unsupported(where + "destino no escribible");
        if (op->code == 0x2E) { if (dst.type != kAddr) return unsupported(where + "mova solo a a0"); }
        else if (dst.type == kAddr) return unsupported(where + "escritura a a0 (solo mova)");
        Param src[3];
        for (int i = 0; i < op->sources; ++i) {
            if (!c.Eat(',')) return unsupported(where + "faltan operandos");
            c.SkipSpace();
            if (c.p < c.end && *c.p == '-') { ++c.p; src[i].srcMod = 1; }
            if (!ParseRegister(c, src[i], false, why)) return unsupported(where + why);
            if (src[i].type == kAddr) return unsupported(where + "a0 como origen");
        }
        c.SkipSpace();
        if (!c.Eof()) return unsupported(where + "texto sobrante");
        uint32_t tokens = 1 + static_cast<uint32_t>(op->sources);
        for (int i = 0; i < op->sources; ++i) if (src[i].relative) ++tokens;
        out.push_back(op->code | tokens << 24);
        out.push_back(DestToken(dst));
        for (int i = 0; i < op->sources; ++i) {
            out.push_back(SrcToken(src[i]));
            if (src[i].relative) out.push_back(0x80000000u | kAddr << 28 | (src[i].relComp * 0x55u) << 16); // token de la direccion relativa: a0.<componente>
        }
    }
    if (!haveVersion) return unsupported("sin cabecera");
    out.push_back(0x0000FFFFu);
    return AssembleResult::Ok;
}
} // namespace D3DX9Native
