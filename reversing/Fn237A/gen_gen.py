"""Genera GenTables.hpp (tablas, solo C++ estandar) y GenStubs.hpp (falsos con __asm) del banco Fn237A a partir de bank_spec.py y fixture_types.py."""
import sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import bank_spec as S
import fixture_types as T

IDS = {c['name']: i for i, c in enumerate(S.COLLABS)}
KIND = {'A': 0, 'B': 1, 'C': 2, 'D': 3, 'G': 4}
SETS = list(S.PTRSETS.keys())


def val(v):
    if v is None:
        return '{0,0,0}'
    if isinstance(v, int):
        return '{0,0,0x%XU}' % (v & 0xFFFFFFFF)
    if isinstance(v, tuple):
        if v[0] == 'F':
            return '{2,0,%dU}' % IDS[v[1]]
        return '{1,%d,0x%XU}' % (KIND[v[0]], v[1])
    if isinstance(v, str) and v.startswith('KEY:'):
        return '{3,0,%dU}' % int(v[4:])
    raise ValueError(v)


def gen_tables():
    o = []
    o.append('#pragma once\n// GENERADO por gen_gen.py a partir de bank_spec.py / fixture_types.py. No editar.\n#include <cstdint>\nnamespace F237AGen {\nusing u8 = std::uint8_t; using u16 = std::uint16_t; using u32 = std::uint32_t;\n')
    o.append('struct Val { u8 kind; u8 arena; u32 v; };   // kind: 0 entero, 1 puntero a arena (arena, desplazamiento), 2 falso virtual (id), 3 clave de etiqueta (indice)\n')
    o.append('inline constexpr u32 kGuard = 0x%X;\ninline constexpr u32 kArenaSize[4] = {%s};\n' % (S.GUARD, ','.join('0x%X' % x for x in S.ARENA_SIZE)))
    o.append('inline constexpr u32 kScript = 0x%X, kSlotC = 0x%X, kNSlotC = %d;\n' % (S.SCRIPT, S.SLOT_C, S.NSLOT_C))
    o.append('struct Collab { const char* name; u32 exe; char conv; const char* args; char ret; int set; u8 mode; };   // mode: F falso, R real\n')
    o.append('inline constexpr unsigned kCollabs = %d;\ninline constexpr Collab kCollab[kCollabs] = {\n' % len(S.COLLABS))
    for c in S.COLLABS:
        rk = c['ret']
        ch = rk[0]
        st = SETS.index(rk[2:]) if rk.startswith('o:') else -1
        o.append('    {"%s", 0x%X, \'%s\', "%s", \'%s\', %d, \'%s\'},\n' % (c['name'], c['exe'] or 0, c['conv'], c['args'], ch, st, c['mode']))
    o.append('};\n')
    for c in S.COLLABS:
        if c['mode'] == 'F' and c['conv'] in 'TS':
            pass
    # conjuntos de punteros
    o.append('struct PtrSet { u8 n; Val v[8]; };\ninline constexpr PtrSet kPtrSets[%d] = {\n' % len(SETS))
    for name in SETS:
        items = S.PTRSETS[name]
        assert len(items) <= 8, name
        if name == 'TASKMEM':
            items = [0]
        o.append('    {%d, {%s}},   // %s\n' % (len(items), ','.join(val(x) for x in items), name))
    o.append('};\n')
    o.append('inline constexpr unsigned kTaskMemSet = %d;\n' % SETS.index('TASKMEM'))
    o.append('struct Fill { u8 arena; u32 off; u32 n; u8 kind; };   // kind 0 bytes aleatorios, 1 floats de FSET' + chr(10) + 'inline constexpr Fill kFills[%d] = {%s};' % (len(S.FILLS), ','.join('{%d,0x%XU,0x%XU,%d}' % (KIND[f[0]], f[1], f[2], 1 if len(f) > 3 else 0) for f in S.FILLS)) + chr(10))
    o.append('inline constexpr u32 kFSet[%d] = {%s};' % (len(S.FSET), ','.join('0x%XU' % x for x in S.FSET)) + chr(10))
    o.append('struct Poke { u8 arena; u32 off; u8 size; Val val; };\ninline constexpr Poke kStruct[%d] = {%s};\n' % (len(S.STRUCT), ','.join('{%d,0x%XU,%d,%s}' % (KIND[a], off, sz, val(v)) for a, off, sz, v in S.STRUCT)))
    for a, off, sz, vals in S.CHOICES:
        assert len(vals) <= 8, (a, off, vals)
    o.append('struct Choice { u8 arena; u32 off; u8 size; u8 n; Val v[8]; };\ninline constexpr Choice kChoices[%d] = {\n' % len(S.CHOICES))
    for a, off, sz, vals in S.CHOICES:
        o.append('    {%d,0x%XU,%d,%d,{%s}},\n' % (KIND[a], off, sz, len(vals), ','.join(val(x) for x in vals)))
    o.append('};\n')
    o.append('struct Global { u32 at; u32 n; };\ninline constexpr Global kGlobals[%d] = {%s};\n' % (len(S.GLOBALS), ','.join('{0x%XU,0x%XU}' % (a, n) for a, n in S.GLOBALS)))
    o.append('inline constexpr const char* kLabels[%d] = {%s};\n' % (len(S.LABELS), ','.join('"%s"' % x for x in S.LABELS)))
    o.append('inline constexpr u32 kThings[%d] = {%s};\n' % (len(S.THINGS), ','.join('0x%XU' % x for x in S.THINGS)))
    o.append('inline constexpr u32 kZones[%d] = {%s};\n' % (len(S.ZONES), ','.join('0x%XU' % x for x in S.ZONES)))
    o.append('inline constexpr u32 kNodes[%d] = {%s};\n' % (len(S.NODES), ','.join('0x%XU' % x for x in S.NODES)))
    o.append('inline constexpr u32 kFloats[%d] = {%s};\n' % (len(S.FLOATS), ','.join('0x%XU' % x for x in S.FLOATS)))
    o.append('inline constexpr u32 kIdReadLabel = %d;' % IDS['ReadTextLabelFromScript'] + chr(10))
    o.append('inline constexpr u32 kEntries[2] = {0x%X, 0x%X};\ninline constexpr int kBases[2] = {%d, %d};\n' % (S.ENTRIES[0], S.ENTRIES[1], S.BASES[0], S.BASES[1]))
    o.append('inline constexpr const char* kFnNames[2] = {"%s", "%s"};\n' % S.NAMES)
    o.append('inline constexpr const char* kCmdTypes[2][100] = {\n')
    for fn in (0, 1):
        o.append('  {%s},\n' % ','.join('"%s"' % T.CMDTYPES[S.BASES[fn] + k] for k in range(100)))
    o.append('};\n')
    o.append('inline constexpr u32 kNullAllocOk[] = {%s};\n' % ','.join(str(x) for x in sorted(T.NULL_ALLOC_OK)))
    o.append('}\n')
    return ''.join(o)


def gen_stubs():
    o = ['#pragma once\n// GENERADO por gen_gen.py. Falsos de los colaboradores (naked): registran la llamada en FakeCore y devuelven como el original.\n']
    for i, c in enumerate(S.COLLABS):
        if c['mode'] != 'F':
            continue
        pop = 4 * len(c['args']) if c['conv'] in 'TS' else 0
        flt = c['ret'] == 'f'
        o.append('__declspec(naked) inline void Fk_%d() {\n    __asm {\n        push ecx\n        lea eax, [esp+8]\n        push eax\n        push ecx\n        push %d\n        call FakeCore\n        add esp, 12\n        pop ecx\n' % (i, i))
        if flt:
            o.append('        fld dword ptr [gFloatBits]\n')
        o.append('        ret %d\n    }\n}\n' % pop if pop else '        ret\n    }\n}\n')
    o.append('inline void* const kFakeFn[%d] = {%s};\n' % (len(S.COLLABS), ','.join(('reinterpret_cast<void*>(&Fk_%d)' % i) if c['mode'] == 'F' else 'nullptr' for i, c in enumerate(S.COLLABS))))
    return ''.join(o)


if __name__ == '__main__':
    (HERE / 'GenTables.hpp').write_text(gen_tables(), encoding='utf-8')
    (HERE / 'GenStubs.hpp').write_text(gen_stubs(), encoding='utf-8')
    print('ok', len(S.COLLABS), 'colaboradores')
