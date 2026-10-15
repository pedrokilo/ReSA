#!/usr/bin/env python
"""Comprueba que las funciones que reproducen el residuo de registros del EXE (ver reversing/CppFourNakedStudy/README.md) siguen
generando el código que esa técnica necesita.

Cada una termina con una llamada a ``ResidueRegsCpp`` o ``ResidueEcx::Get`` (X87Math.cpp), que dejan ECX, EDX y EAX tal como los
necesita el llamador del EXE. Tras esa llamada no puede ejecutarse nada que cambie ECX, EDX o EAX: sólo ``pop`` de registros guardados,
``mov esp, ebp``, saltos hacia el epílogo y ``ret``. Si un cambio de opciones o de compilador introduce otra cosa (por ejemplo
``call __RTC_CheckEsp`` o ``mov ecx, ...``), este script falla.

Uso:  python contrib/audit_register_residue.py [--obj-dir build/source/gta_reversed.dir/Debug] [--dumpbin RUTA]
Sale con 0 si todo está bien (o si no hay objetos compilados que revisar), con 1 si hay una violación.
"""
import argparse
import glob
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OBJECTS = ['FxFrustumInfo', 'Garages', 'EventScriptCommand', 'BaseModelInfo', 'ClumpModelInfo', 'PedGroupIntelligence', 'Pools', 'RwHelper', 'TaskInteriorSitInChair',
           'TaskComplexGoToCarDoorAndStandStill', 'TaskSimpleGoToPointFine', 'PedIntelligence', 'TaskComplexSeekEntityXYOffset', 'PlantLocTri', 'Pickup', 'EntryExit', 'PlayerPed', 'CarEnterExit', 'TaskComplexWaitForPed', 'PlayerRelationshipRecorder', 'EventSource']
HELPER_CALL = re.compile(r'^call\s+\?(ResidueRegsCpp@@YIIIII@Z|Get@ResidueEcx@@QAEII@Z|PlayerPedResidueFloatCpp@@YIMIIM@Z)\b')
ENTRY_CALL = re.compile(r'^call\s+\?EntryEaxCpp@@YIIXZ$')
ENTRY_EDX_CALL = re.compile(r'^call\s+\?CarEnterExitEntryEdxCpp@@YI_KXZ$')
# Antes de leer el EAX de entrada (EntryEaxCpp) sólo puede ejecutarse código lineal que no escriba EAX: prólogo, guardado de registros y
# copias de ECX/EDX/argumentos. Cualquier llamada, salto condicional o instrucción cuyo destino sea EAX/AX/AL/AH lo estropearía.
ENTRY_FORBIDDEN = [
    re.compile(r'^(call|xchg|cdq|cwde|mul|div|idiv|cpuid|rep|j(?!mp\b)\w+|loop\w*)\b'),
    re.compile(r'^(?!push |cmp |test )\w+\s+(eax|ax|al|ah)\b'),
    re.compile(r'^pop\s+(eax|ax)$'),
]
# Igual para el EDX de entrada (CarEnterExitEntryEdxCpp): antes de leerlo no puede escribirse EDX/DX/DL/DH ni desviarse el flujo.
ENTRY_EDX_FORBIDDEN = [
    re.compile(r'^(call|xchg|cdq|cwd|mul|div|idiv|cpuid|rep|j(?!mp\b)\w+|loop\w*)\b'),
    re.compile(r'^(?!push |cmp |test )\w+\s+(edx|dx|dl|dh)\b'),
    re.compile(r'^pop\s+(edx|dx)$'),
]
ALLOWED = [
    re.compile(r'^pop\s+(ebx|esi|edi|ebp)$'),
    re.compile(r'^mov\s+esp,ebp$'),
    re.compile(r'^ret(\s+[0-9A-Fa-f]+h?)?$'),
    re.compile(r'^jmp\s+[0-9A-Fa-f]{8}$'),
    re.compile(r'^int\s+3$'),
]
# Llamadas cuyo residuo de ECX/EDX debe llegar intacto a la salida (CEventSource::ComputeEventSourceType): los tres colaboradores del EXE
# (se cargan por direccion en un registro justo antes de `call reg`) y la primera llamada indirecta tras leer el EDX de entrada
# (GetSourceEntity). Por cada camino que sale a `ret` sin otra llamada de por medio no puede escribirse ECX, EDX ni sus partes.
EXE_COLLABORATOR_LOAD = re.compile(r'^mov\s+(eax|ecx|edx),(601C30h|601BC0h|5DF8F0h)$')
INDIRECT_CALL = re.compile(r'^call\s+(eax|ecx|edx)$')
CLOBBERS_ECX_EDX = [
    re.compile(r'^(?!push |cmp |test |j\w+ )\w+\s+(ecx|cx|cl|ch|edx|dx|dl|dh)\b'),
    re.compile(r'^(cdq|cwd|mul|div|idiv|rep|xchg)\b'),
]
JUMP_TARGET = re.compile(r'^(j\w+)\s+([0-9A-Fa-f]{8})$')

INSTRUCTION = re.compile(r'^\s*([0-9A-F]{8}):\s+(.*\S)\s*$')


def find_dumpbin(explicit=None):
    if explicit:
        return explicit
    for pattern in ('C:/Program Files/Microsoft Visual Studio/*/*/VC/Tools/MSVC/*/bin/Hostx64/x86/dumpbin.exe',
                    'C:/Program Files (x86)/Microsoft Visual Studio/*/*/VC/Tools/MSVC/*/bin/Hostx64/x86/dumpbin.exe'):
        found = sorted(glob.glob(pattern))
        if found:
            return found[-1]
    return None


def disassemble(dumpbin, obj):
    out = subprocess.run([dumpbin, '/disasm:nobytes', str(obj)], capture_output=True, text=True, encoding='utf-8', errors='replace').stdout
    return out.splitlines()


def split_functions(lines):
    """El desensamblado de un objeto con /Gy reinicia la dirección en 00000000 en cada función."""
    functions, current = [], []
    for line in lines:
        m = INSTRUCTION.match(line)
        if not m:
            continue
        address, text = int(m.group(1), 16), re.sub(r'\s+', ' ', m.group(2)).strip()
        if address == 0 and current:
            functions.append(current)
            current = []
        current.append((address, text))
    if current:
        functions.append(current)
    return functions


def violations_after_calls(function):
    """Instrucciones ejecutables tras cada llamada a un ayudante hasta el ``ret`` que no estén permitidas."""
    found = []
    by_address = {address: index for index, (address, _) in enumerate(function)}
    for index, (_, text) in enumerate(function):
        if not HELPER_CALL.match(text):
            continue
        cursor, visited = index + 1, set()
        while cursor < len(function) and cursor not in visited:
            visited.add(cursor)
            address, following = function[cursor]
            if not any(pattern.match(following) for pattern in ALLOWED):
                found.append((address, following))
                break
            if following.startswith('ret'):
                break
            if following.startswith('jmp'):
                target = int(following.split()[1], 16)
                if target not in by_address:
                    found.append((address, following + ' (destino fuera de la función)'))
                    break
                cursor = by_address[target]
                continue
            cursor += 1
    return found


def monitored_calls(function):
    """Indices de las llamadas `call reg` cuyo residuo debe conservarse: colaboradores del EXE y GetSourceEntity (primera tras el EDX de entrada)."""
    found = []
    entry_edx = next((i for i, (_, text) in enumerate(function) if ENTRY_EDX_CALL.match(text)), None)
    first_indirect_done = False
    for index, (_, text) in enumerate(function):
        m = INDIRECT_CALL.match(text)
        if not m:
            continue
        register = m.group(1)
        if any((load := EXE_COLLABORATOR_LOAD.match(function[j][1])) and load.group(1) == register for j in range(max(0, index - 8), index)):
            found.append(index)
        elif entry_edx is not None and index > entry_edx and not first_indirect_done:
            found.append(index)
        if entry_edx is not None and index > entry_edx:
            first_indirect_done = True
    return found


def violations_clobbering_residue(function):
    """Escrituras de ECX/EDX entre una llamada vigilada y un `ret` alcanzable sin otra llamada de por medio (otra llamada reinicia el residuo)."""
    found = []
    by_address = {address: index for index, (address, _) in enumerate(function)}
    for call_index in monitored_calls(function):
        seen, stack = set(), [(call_index + 1, None)]
        while stack:
            cursor, tainted = stack.pop()
            if cursor >= len(function) or (cursor, tainted) in seen:
                continue
            seen.add((cursor, tainted))
            address, text = function[cursor]
            if text.startswith('call'):
                continue
            if tainted is None and any(pattern.match(text) for pattern in CLOBBERS_ECX_EDX):
                tainted = (address, text)
            if text.startswith('ret'):
                if tainted is not None:
                    found.append((tainted[0], tainted[1] + ' (pisa ECX/EDX antes de la salida)'))
                continue
            jump = JUMP_TARGET.match(text)
            if jump and jump.group(2) and int(jump.group(2), 16) in by_address:
                stack.append((by_address[int(jump.group(2), 16)], tainted))
                if jump.group(1) == 'jmp':
                    continue
            stack.append((cursor + 1, tainted))
    return sorted(set(found))


def violations_before_entry_read(function):
    """Instrucciones anteriores a cada llamada a EntryEaxCpp que pueden cambiar EAX (o desviar el flujo)."""
    found = []
    for index, (_, text) in enumerate(function):
        if not ENTRY_CALL.match(text):
            continue
        for address, before in function[:index]:
            if any(pattern.match(before) for pattern in ENTRY_FORBIDDEN):
                found.append((address, before + ' (antes de leer el EAX de entrada)'))
    return found


def violations_before_entry_edx_read(function):
    """Instrucciones anteriores a cada llamada a CarEnterExitEntryEdxCpp que pueden cambiar EDX (o desviar el flujo)."""
    found = []
    for index, (_, text) in enumerate(function):
        if not ENTRY_EDX_CALL.match(text):
            continue
        for address, before in function[:index]:
            if any(pattern.match(before) for pattern in ENTRY_EDX_FORBIDDEN):
                found.append((address, before + ' (antes de leer el EDX de entrada)'))
    return found


def audit(functions):
    """Devuelve (llamadas_revisadas, violaciones) para una lista de funciones ya separadas."""
    checked, bad = 0, []
    for function in functions:
        calls = sum(1 for _, text in function if HELPER_CALL.match(text))
        entry_reads = sum(1 for _, text in function if ENTRY_CALL.match(text) or ENTRY_EDX_CALL.match(text))
        watched = len(monitored_calls(function))
        if not calls and not entry_reads and not watched:
            continue
        checked += calls + entry_reads + watched
        bad.extend(violations_after_calls(function))
        bad.extend(violations_clobbering_residue(function))
        bad.extend(violations_before_entry_read(function))
        bad.extend(violations_before_entry_edx_read(function))
    return checked, bad


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--obj-dir', default=str(ROOT / 'build/source/gta_reversed.dir/Debug'))
    parser.add_argument('--dumpbin')
    args = parser.parse_args(argv)
    objects = [Path(args.obj_dir) / f'{name}.obj' for name in OBJECTS]
    present = [o for o in objects if o.exists()]
    dumpbin = find_dumpbin(args.dumpbin)
    if not present or not dumpbin:
        print('Sin objetos compilados o sin dumpbin: nada que revisar.')
        return 0
    total, failed = 0, False
    for obj in present:
        checked, bad = audit(split_functions(disassemble(dumpbin, obj)))
        total += checked
        print(f'{obj.name}: {checked} llamadas a ayudantes de residuo revisadas, {len(bad)} violaciones')
        for address, text in bad:
            failed = True
            print(f'  {address:08X}: {text}')
    if total == 0:
        print('No se encontró ninguna llamada a los ayudantes: la técnica ya no se aplica o cambió el nombre.')
        return 1
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
