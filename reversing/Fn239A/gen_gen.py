"""Genera GenTables.hpp (tablas, solo C++ estandar) y GenStubs.hpp (falsos con __asm) del banco Fn239A a partir de bank_spec.py y fixture_types.py."""
import sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import bank_spec as S
import fixture_types as T

NL = chr(10)
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


def table(name, vals, typ='u32'):
    return 'inline constexpr %s %s[%d] = {%s};%s' % (typ, name, len(vals), ','.join('0x%XU' % x for x in vals), NL)


def gen_tables():
    o = []
    o.append('#pragma once' + NL + '// GENERADO por gen_gen.py a partir de bank_spec.py / fixture_types.py. No editar.' + NL + '#include <cstdint>' + NL + 'namespace F239AGen {' + NL + 'using u8 = std::uint8_t; using u16 = std::uint16_t; using u32 = std::uint32_t;' + NL)
    o.append('struct Val { u8 kind; u8 arena; u32 v; };   // kind: 0 entero, 1 puntero a arena (arena, desplazamiento), 2 falso virtual (id), 3 clave de etiqueta (indice)' + NL)
    o.append('inline constexpr u32 kGuard = 0x%X;' % S.GUARD + NL + 'inline constexpr u32 kArenaSize[4] = {%s};' % ','.join('0x%X' % x for x in S.ARENA_SIZE) + NL)
    o.append('inline constexpr u32 kScript = 0x%X, kSlotC = 0x%X, kNSlotC = %d;' % (S.SCRIPT, S.SLOT_C, S.NSLOT_C) + NL)
    o.append('struct Collab { const char* name; u32 exe; char conv; const char* args; char ret; int set; u8 mode; };   // mode: F falso, R real' + NL)
    o.append('inline constexpr unsigned kCollabs = %d;' % len(S.COLLABS) + NL + 'inline constexpr Collab kCollab[kCollabs] = {' + NL)
    for c in S.COLLABS:
        rk = c['ret']
        ch = rk[0]
        st = SETS.index(rk[2:]) if rk.startswith('o:') else -1
        o.append('    {"%s", 0x%X, \'%s\', "%s", \'%s\', %d, \'%s\'},' % (c['name'], c['exe'] or 0, c['conv'], c['args'], ch, st, c['mode']) + NL)
    o.append('};' + NL)
    o.append('struct PtrSet { u8 n; Val v[8]; };' + NL + 'inline constexpr PtrSet kPtrSets[%d] = {' % len(SETS) + NL)
    for name in SETS:
        items = S.PTRSETS[name]
        assert len(items) <= 8, name
        if name == 'TASKMEM':
            items = [0]
        o.append('    {%d, {%s}},   // %s' % (len(items), ','.join(val(x) for x in items), name) + NL)
    o.append('};' + NL)
    o.append('inline constexpr unsigned kTaskMemSet = %d;' % SETS.index('TASKMEM') + NL + 'inline constexpr unsigned kEntlSet = %d;' % SETS.index('ENTL') + NL)
    o.append('struct Fill { u8 arena; u32 off; u32 n; u8 kind; };   // kind 0 bytes aleatorios, 1 floats de FSET' + NL + 'inline constexpr Fill kFills[%d] = {%s};' % (len(S.FILLS), ','.join('{%d,0x%XU,0x%XU,%d}' % (KIND[f[0]], f[1], f[2], 1 if len(f) > 3 else 0) for f in S.FILLS)) + NL)
    o.append(table('kFSet', S.FSET))
    o.append('struct Poke { u8 arena; u32 off; u8 size; Val val; };' + NL + 'inline constexpr Poke kStruct[%d] = {%s};' % (len(S.STRUCT), ','.join('{%d,0x%XU,%d,%s}' % (KIND[a], off, sz, val(v)) for a, off, sz, v in S.STRUCT)) + NL)
    for a, off, sz, vals, cmd in S.CHOICES:
        assert len(vals) <= 8, (a, off, vals)
    o.append('struct Choice { u8 arena; u32 off; u8 size; u8 n; Val v[8]; u32 cmd; };   // cmd: 0 = todos los comandos' + NL + 'inline constexpr Choice kChoices[%d] = {' % len(S.CHOICES) + NL)
    for a, off, sz, vals, cmd in S.CHOICES:
        o.append('    {%d,0x%XU,%d,%d,{%s},%dU},' % (KIND[a], off, sz, len(vals), ','.join(val(x) for x in vals), cmd) + NL)
    o.append('};' + NL)
    o.append('struct Global { u32 at; u32 n; };' + NL + 'inline constexpr Global kGlobals[%d] = {%s};' % (len(S.GLOBALS), ','.join('{0x%XU,0x%XU}' % (a, n) for a, n in S.GLOBALS)) + NL)
    o.append('inline constexpr const char* kLabels[%d] = {%s};' % (len(S.LABELS), ','.join('"%s"' % x for x in S.LABELS)) + NL)
    o.append(table('kThings', S.THINGS))
    o.append(table('kZones', S.ZONES))
    o.append(table('kNodes', S.NODES))
    o.append(table('kFloats', S.FLOATS))
    o.append(table('kCounts', S.COUNTS))
    o.append(table('kAngles', S.ANGLES))
    o.append(table('kSpeeds', S.SPEEDS))
    o.append(table('kOutOfRange', S.OOR))
    o.append(table('kSchedule', S.SCHEDULE))
    o.append('inline constexpr u32 kIdReadLabel = %d;' % IDS['ReadTextLabelFromScript'] + NL)
    o.append('inline constexpr u32 kEntries[1] = {0x%X};' % S.ENTRIES[0] + NL + 'inline constexpr int kBases[1] = {%d};' % S.BASES[0] + NL)
    o.append('inline constexpr const char* kFnNames[1] = {"%s"};' % S.NAMES[0] + NL)
    o.append('inline constexpr unsigned kNCmd = %d;' % S.NCMD + NL + 'inline constexpr const char* kCmdTypes[1][%d] = {' % S.NCMD + NL)
    o.append('  {%s},' % ','.join('"%s"' % T.CMDTYPES[S.BASES[0] + k] for k in range(S.NCMD)) + NL)
    o.append('};' + NL)
    o.append('inline constexpr u32 kNullAllocOk[] = {%s};' % ','.join(str(x) for x in sorted(T.NULL_ALLOC_OK)) + NL)
    o.append('}' + NL)
    return ''.join(o)


def gen_stubs():
    o = ['#pragma once' + NL + '// GENERADO por gen_gen.py. Falsos de los colaboradores (naked): registran la llamada en FakeCore y devuelven como el original.' + NL]
    for i, c in enumerate(S.COLLABS):
        if c['mode'] != 'F':
            continue
        pop = 4 * len(c['args']) if c['conv'] in 'TS' else 0
        flt = c['ret'] == 'f'
        o.append('__declspec(naked) inline void Fk_%d() {' % i + NL + '    __asm {' + NL + '        push ecx' + NL + '        lea eax, [esp+8]' + NL + '        push eax' + NL + '        push ecx' + NL + '        push %d' % i + NL + '        call FakeCore' + NL + '        add esp, 12' + NL + '        pop ecx' + NL)
        if flt:
            o.append('        fld dword ptr [gFloatBits]' + NL)
        o.append(('        ret %d' % pop + NL + '    }' + NL + '}' + NL) if pop else ('        ret' + NL + '    }' + NL + '}' + NL))
    o.append('inline void* const kFakeFn[%d] = {%s};' % (len(S.COLLABS), ','.join(('reinterpret_cast<void*>(&Fk_%d)' % i) if c['mode'] == 'F' else 'nullptr' for i, c in enumerate(S.COLLABS))) + NL)
    return ''.join(o)


if __name__ == '__main__':
    (HERE / 'GenTables.hpp').write_text(gen_tables(), encoding='utf-8')
    (HERE / 'GenStubs.hpp').write_text(gen_stubs(), encoding='utf-8')
    print('ok', len(S.COLLABS), 'colaboradores')
