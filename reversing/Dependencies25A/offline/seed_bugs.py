"""Siembra errores en los .inc extraidos (los restaura al terminar), recompila el arnes sin juego y cuenta las diferencias (el banco debe tener dientes).
Uso: ejecutar antes bld.bat; despues  python -B seed_bugs.py [filtro]   (cada caso recompila: ~10 s)."""
import json, os, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
OFF = HERE   # reversing/Dependencies25A/offline (tras bld.bat: existen inc/ y obj/)
BUGS = [
    ('F0 vtable equivocada', 'task.inc', '0x86D48C;', '0x86D48D;'),
    ('F0 sin optimizar (clobbera EAX)', 'task.inc', '#pragma optimize("gty", on)', ''),
    ('F1 bit del estado', 'pool_members.inc', 'return m_SlotState[idx].IsEmpty;', 'return !m_SlotState[idx].IsEmpty;'),
    ('F2 contador cacheado', 'autopilot.inc', ('    int16 i = 0;', '} while (i < *count);'), ('    int16 i = 0; const int16 cached = *count;', '} while (i < cached);')),
    ('F2 sin dec', 'autopilot.inc', '--m_nPathFindNodesCount;', 'm_nPathFindNodesCount = m_nPathFindNodesCount;'),
    ('F2 zancada de 2 bytes', 'autopilot.inc', 'base + 0x68 + 4 * slot', 'base + 0x68 + 2 * slot'),
    ('F3 sin comprobar activo', 'pedgroups.inc', 'if (!ms_activeGroups.data()[groupId]) {', 'if (false) {'),
    ('F3 grupo equivocado', 'pedgroups.inc', '(ms_groups.data() + groupId)->', '(ms_groups.data() + (groupId & 7))->'),
    ('F3 indice acotado', 'pedgroups.inc', 'ms_activeGroups.data()[groupId]', 'ms_activeGroups.data()[groupId & 7]'),
    ('F4 zancada', 'decision.inc', 'kDecisionStride = sizeof(CDecision);', 'kDecisionStride = sizeof(CDecision) - 1;'),
    ('F4 evento', 'decision.inc', 'static_cast<int32>(eventId)]', 'static_cast<int32>(eventId) + 1]'),
    ('F4 sin +4', 'decision.inc', 'self + offset + 4u', 'self + offset'),
    ('F5 mascara de modelo', 'garages.inc', 'slots[i].m_wModelIndex != 0', 'slots[i].m_wModelIndex > 1'),
    ('F5 modelo de 8 bits', 'garages.inc', 'slots[i].m_wModelIndex != 0', '(slots[i].m_wModelIndex & 0xFF) != 0'),
    ('F5 3 ranuras', 'garages.inc', 'i < MAX_CARS_IN_SAFEHOUSE', 'i < 3'),
    ('F6 bit equivocado', 'upgrade.inc', 'GetAtomicId(atomic) & ATOMIC_UPGRADE', 'GetAtomicId(atomic) & 0x400'),
    ('F6 indice', 'upgrade.inc', '!= -1', '> 0'),
    ('F6 data[0]', 'upgrade.inc', ')[1] = object;', ')[0] = object;'),
    ('F7 sin parada', 'hierarchy.inc', 'return nullptr;', 'return frame;'),
    ('F7 sin recorrer hijos', 'hierarchy.inc', 'RwFrameForAllChildren(frame, GetAnimHierarchyFromFrameCallback, data);', ''),
    ('F8 flag', 'decoder.inc', 'if (flags & 1) {', 'if (flags & 2) {'),
    ('F8 sin anular', 'decoder.inc', 'm_dataStream = nullptr;', ''),
    ('F8 vtable', 'decoder.inc', '0x85F418', '0x85F419'),
    ('F8 delete this siempre', 'decoder.inc', 'if (flags & 1) {', 'if (true) {'),
]
which = sys.argv[1:]
results = []
for label, name, old, new in BUGS:
    if which and not any(w in label for w in which):
        continue
    path = os.path.join(OFF, 'inc', name)
    original = open(path, encoding='utf-8').read()
    olds = old if isinstance(old, tuple) else (old,)
    news = new if isinstance(new, tuple) else (new,)
    if any(o not in original for o in olds):
        print('NO ENCONTRADO', label); continue
    mutated = original
    for o, n2 in zip(olds, news):
        mutated = mutated.replace(o, n2, 1)
    open(path, 'w', encoding='utf-8', newline='\n').write(mutated)
    try:
        bat = os.path.join(OFF, 'bug_build.bat')
        lines = ['@echo off',
                 'call "C:\\Program Files\\Microsoft Visual Studio\\18\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" >nul 2>&1',
                 'cd /d "%~dp0"',
                 'cl /nologo /std:c++latest /EHsc /utf-8 /Od /Zi /MT /DNOMINMAX /I"D:\\Original\\gta-reversed-master\\reversing\\CVehicle\\conan-cache\\p\\nlohmd014ef7748f4b\\p\\include" main.cpp /Fe:obj/bug.exe /Fdobj/b.pdb /Foobj/ '
                 '/link /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO /INCREMENTAL:NO user32.lib']
        open(bat, 'w').write('\r\n'.join(lines) + '\r\n')
        r = subprocess.run(['cmd', '/c', bat], capture_output=True, text=True)
        if r.returncode:
            print('COMPILACION FALLO', label, (r.stdout + r.stderr)[-600:]); continue
        out = os.path.join(OFF, 'bug.jsonl')
        subprocess.run([os.path.join(OFF, 'obj', 'bug.exe'), '256', out], capture_output=True, text=True, timeout=600)
        fails = {}
        for line in open(out, encoding='utf-8'):
            row = json.loads(line)
            if row['event'] == 'function_summary' and row['failures']:
                fails[row['function']] = row['failures']
        print('%-28s -> %s' % (label, fails or 'SIN DETECTAR'))
    finally:
        open(path, 'w', encoding='utf-8', newline='\n').write(original)
