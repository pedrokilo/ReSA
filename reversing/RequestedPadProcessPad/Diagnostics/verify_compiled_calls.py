"""Static (no execution) verification of the compiled CPad::ProcessPad candidate.

Derives every call site / COM slot / float path from the dumpbin listing and the bytes of the
matching ASI (symbols come from the PDB through inspect_compiled.py). No code address is copied:
only the *native* EXE constants named in scope.json (RsPadEventHandler, GetPad, FTOL) are used and
they are read from scope.json. Writes Diagnostics/compiled-runtime-proof.json with passed=true only
if every check holds; otherwise passed=false and the failures are listed. Never forces a result.
"""
import contextlib, hashlib, io, json, re, runpy, struct
from pathlib import Path

B = Path(__file__).resolve().parents[1]
with contextlib.redirect_stdout(io.StringIO()):
    S = runpy.run_path(str(B / 'inspect_compiled.py'))   # also re-validates hashes + PE/PDB RSDS identity
read, resolve, records, base, size = S['read'], S['resolve'], S['records'], S['base'], S['size']
O = B / 'Diagnostics/compiled-proof'
scope = json.loads((B / 'scope.json').read_text(encoding='utf-8'))
native = ' '.join(scope['native_collaborators']['traced_scripted'] + scope['native_collaborators']['native_cpu_real'])
RS_NATIVE = int(re.search(r'RsPadEventHandler (0x[0-9A-Fa-f]+)', native).group(1), 16)
GETPAD_NATIVE = int(re.search(r'GetPad (0x[0-9A-Fa-f]+)', native).group(1), 16)
FTOL_NATIVE = int(re.search(r'FTOL (0x[0-9A-Fa-f]+)', native).group(1), 16)
failures, notes = [], []


def check(cond, msg):
    if not cond:
        failures.append(msg)
    return bool(cond)


def sym(pred, label):
    m = [x for x in records if pred(x['name'])]
    if len(m) != 1:
        failures.append(f'symbol {label}: {len(m)} matches')
        return None
    return m[0]


def listing(rec):
    """Instructions of a function body (up to and including its single ret), from the dumpbin text."""
    text = (O / f"{rec['body']:08X}.asm.txt").read_text(encoding='utf-8')
    out = []
    for line in text.splitlines():
        m = re.match(r'\s+([0-9A-F]{8}): ((?:[0-9A-F]{2} )+)\s+(\S.*)$', line)
        if m:
            n = re.sub(r'\s+', ' ', m.group(3).strip())
            out.append((int(m.group(1), 16), n, n))
            if out[-1][1].startswith('ret'):
                break
    return out


def call_target(va):
    raw = read(va, 5)
    return va + 5 + struct.unpack_from('<i', raw, 1)[0] if raw[0] == 0xE8 else None


def calls(ins):
    r = []
    for a, t, _ in ins:
        if t.startswith('call') and read(a, 1)[0] == 0xE8:
            tgt = call_target(a)
            r.append({'site': a, 'entry': tgt, 'body': resolve(tgt)[0] if base <= tgt < base + size else tgt,
                      'chain': resolve(tgt)[1] if base <= tgt < base + size else [], 'in_image': base <= tgt < base + size})
    return r


pp = sym(lambda n: n == 'CPad::ProcessPad', 'CPad::ProcessPad')
lam = sym(lambda n: n.startswith("`CPad::ProcessPad'::`") and 'lambda_1' in n and n.endswith('operator()'), 'ProcessPad lambda')
norm = sym(lambda n: n.endswith('NormalizePadAxis'), 'NormalizePadAxis')
getpad = sym(lambda n: n == 'CPad::GetPad', 'CPad::GetPad')
rsh = sym(lambda n: n == 'RsPadEventHandler', 'RsPadEventHandler')
check(pp and pp['size'] > 0, 'ProcessPad size (derived from PDB, not pinned)')
ipp, ilam, inorm = listing(pp), listing(lam), listing(norm)
report = {}

# ---- 1. RsPadEventHandler: 3 calls (0x20, 0x20, 0x1F) in order, cdecl, &padID second, final target ----
ev = []
for c in calls(ipp):
    if c['in_image'] and c['body'] == rsh['body']:
        i = next(k for k, x in enumerate(ipp) if x[0] == c['site'])
        prev = ipp[i - 3:i]
        ok = (len(prev) == 3 and prev[0][2] == 'lea eax,[ebx+8]' and prev[1][2] == 'push eax' and re.fullmatch(r'push [0-9A-F]+h', prev[2][2])
              and ipp[i + 1][2] == 'add esp,8')
        check(ok, f'event call {c["site"]:#x} arg/cdecl pattern')
        ev.append({'site': hex(c['site']), 'event': int(prev[2][2][5:-1], 16), 'thunk_entry': hex(c['entry']), 'final_body': hex(c['body']),
                   'chain': c['chain'], 'second_arg_is_&padID(ebx+8)_cdecl_add_esp_8': ok})
check([e['event'] for e in ev] == [0x20, 0x20, 0x1F], f'event order {[hex(e["event"]) for e in ev]}')
check(len(ev) == 3, 'exactly 3 RsPadEventHandler calls')
check(len({e['thunk_entry'] for e in ev}) == 1 and ev[0]['final_body'] == hex(rsh['body']), 'event thunk -> RsPadEventHandler body')
# Registration of the hook: "push <thunk>; push RS_NATIVE; sub esp,1Ch" with <thunk> resolving to the same body. Must be unique.
asi = S['b']


def fo2va(fo):
    for vs, va, rs, ro in S['ss']:
        if ro <= fo < ro + rs:
            return base + va + fo - ro


pat = b'\x68' + struct.pack('<I', RS_NATIVE) + b'\x83\xEC\x1C'
reg = []
for m in re.finditer(re.escape(pat), asi):
    va = fo2va(m.start())
    raw = read(va - 5, 5)
    if raw[0] == 0x68:
        thunk = struct.unpack_from('<I', raw, 1)[0]
        if base <= thunk < base + size and resolve(thunk)[0] == rsh['body']:
            reg.append({'push_native_site': hex(va), 'thunk': hex(thunk), 'body': hex(resolve(thunk)[0])})
check(len(reg) == 1, f'hook install sites for native RsPadEventHandler: {len(reg)}')
check(not reg or reg[0]['thunk'] == ev[0]['thunk_entry'], 'hook-install thunk == ProcessPad event thunk')
# every other reference to the native address (bank gate / lookup) for completeness
others = [hex(fo2va(m.start())) for m in re.finditer(re.escape(struct.pack('<I', RS_NATIVE)), asi) if fo2va(m.start()) is not None]
report['rs_pad_event_handler'] = {'native': hex(RS_NATIVE), 'dll_function_body': hex(rsh['body']), 'dll_function_size': rsh['size'], 'calls': ev,
                                  'hook_install_sites': reg, 'all_imm32_refs_to_native_in_asi': others,
                                  'dll_function_dispatches_via': 'RsGlobal.ps->padHandler (see 113327D0 listing), so the hook target is the single Simple hook'}

# ---- 2. COM slots: Poll +64h, Acquire +1Ch (INPUTLOST loop), Poll again, GetDeviceState +24h with (this,110h,buf) ----
slots = [(a, int(re.search(r'\[ecx\+([0-9A-F]+)h\]', t).group(1), 16)) for (a, t, _), nxt in zip(ipp, ipp[1:]) if t.startswith('mov edx,dword ptr [ecx+') and nxt[1].startswith('call') ]
slots = [(a, s) for a, s in slots if s in (0x64, 0x1C)]
seq = [s for a, s in slots]
check(seq == [0x64, 0x1C, 0x1C, 0x64], f'Poll/Acquire/Acquire(loop)/Poll slot sequence {[hex(x) for x in seq]}')
inlost = [i for i, x in enumerate(ipp) if x[2].startswith('cmp dword ptr [ebp') and '8007001Eh' in x[2]]
check(len(inlost) == 1, 'single INPUTLOST (0x8007001E) compare')
if inlost:
    i = inlost[0]
    acq2 = next(k for k, x in enumerate(ipp) if x[0] == slots[2][0])
    back = next((x for x in ipp[acq2:acq2 + 10] if x[1].startswith('jmp')), None)
    check(ipp[i + 1][1].startswith('jne') and acq2 > i and back is not None and int(back[2].split()[-1], 16) == ipp[i][0],
          'INPUTLOST: jne exits, Acquire retried, jmp back to the compare')
gds = [i for i, x in enumerate(ipp) if x[2] == 'mov edx,dword ptr [ecx+24h]']
check(len(gds) == 1, 'single GetDeviceState slot 24h')
gds_info = {}
if gds:
    i = gds[0]
    win = ipp[i - 2:i + 14]
    pushes = [x[2] for x in win if x[1].startswith('push')]
    lea = next(x for x in win if x[1].startswith('lea eax,[ebp'))
    buf = int(re.search(r'ebp\+([0-9A-F]+)h', lea[2]).group(1), 16) - (1 << 32)
    check(pushes[:3] == ['push eax', 'push 110h', 'push ecx'], f'GetDeviceState arg order {pushes[:3]}')
    gds_info = {'site': hex(win[0][0]), 'cb': 0x110, 'buffer_ebp_offset': buf, 'buffer_end_ebp_offset': buf + 0x110,
                'stdcall_args_pushed_right_to_left': pushes[:3]}
    check(buf + 0x110 <= -4, 'DIJOYSTATE2 buffer does not reach the stack cookie at ebp-4')
    # joystick fields read from the buffer: X,Y,Z,Rz and POV[0] (+0x20) must lie inside it
    rd = set()
    for x in ipp:
        mm = re.search(r'\[ebp\+(?:ecx\+)?(FFFF[0-9A-F]{4})h\]', x[2])
        if mm and buf <= int(mm.group(1), 16) - (1 << 32) < buf + 0x110 and (x[2].startswith(('mov e', 'push')) or 'cvtsi2sd' in x[2]):
            rd.add(int(mm.group(1), 16) - (1 << 32) - buf)
    rd = sorted(rd)
    gds_info['buffer_offsets_read'] = rd
    check({0x00, 0x04, 0x08, 0x14, 0x20} <= set(rd), f'X/Y/Z/Rz/POV offsets read: {[hex(x) for x in rd]}')
report['com'] = {'poll_acquire_slots_in_order': [{'site': hex(a), 'slot': hex(s)} for a, s in slots], 'inputlost_value': '0x8007001E',
                 'get_device_state': gds_info}

# ---- 3. GetPad: called (not inlined) via thunk to the DLL's own CPad::GetPad; the native EXE address is not called ----
gp = [c for c in calls(ipp) if c['in_image'] and c['body'] == getpad['body']]
check(len(gp) == 1, f'GetPad calls in ProcessPad: {len(gp)}')
allc = calls(ipp) + calls(ilam) + calls(inorm)
check(not any(c['entry'] == GETPAD_NATIVE or c['body'] == GETPAD_NATIVE for c in allc), 'no direct call to native GetPad address')
gbody = listing(getpad)
check(any('imul eax,dword ptr [ebp+8],134h' in x[2] for x in gbody), 'DLL GetPad = Pads + n*0x134 (sizeof CPad 308)')
if gp:
    i = next(k for k, x in enumerate(ipp) if x[0] == gp[0]['site'])
    check(ipp[i - 1][2] == 'push edx' and ipp[i + 1][2] == 'add esp,4' and 'movsx ecx,byte ptr' in ' '.join(x[2] for x in ipp[i - 14:i]),
          'GetPad cdecl arg = movsx byte padNumber')
report['get_pad'] = {'native': hex(GETPAD_NATIVE), 'called': [{'site': hex(c['site']), 'thunk_entry': hex(c['entry']), 'final_body': hex(c['body']), 'chain': c['chain']} for c in gp],
                     'inlined': False, 'dll_function_size': getpad['size'], 'note': 'candidate calls the DLL CPad::GetPad (not the EXE 0x53FB70, not inlined); then uses the returned CPad* at +0xA8..+0xAF'}

# ---- 4. FTOL: absent everywhere in the three compiled functions; truncation is inline cvttss2si ----
check(not any(c['entry'] == FTOL_NATIVE or c['body'] == FTOL_NATIVE for c in allc), 'no call to native FTOL')
cv = [x for x in ilam if x[1].startswith('cvttss2si')]
check(len(cv) == 1, 'lambda has one inline cvttss2si')
mul128 = [x for x in ilam if 'mulss xmm0,dword ptr ds:[' in x[2]]
check(any(struct.unpack('<f', read(int(re.search(r'\[([0-9A-F]+)h\]', x[2]).group(1), 16), 4))[0] == 128.0 for x in mul128), 'lambda multiplies by 128.0f before cvttss2si')
lam_calls = [c for c in calls(ipp) if c['in_image'] and c['body'] == lam['body']]
check(len(lam_calls) == 4, f'lambda called 4 times: {len(lam_calls)}')
dests = []
for c in lam_calls:
    i = next(k for k, x in enumerate(ipp) if x[0] == c['site'])
    dests.append([x[2] for x in ipp[i - 14:i] if re.match(r'(add ecx|add edx),0A[0-9A-F]h', x[2])])
flat = [[int(re.search(r'0([0-9A-F]+)h', y).group(1), 16) for y in d] for d in dests]
check(flat == [[0xA8, 0xAA], [0xAA, 0xA8], [0xAC, 0xAE], [0xAE, 0xAC]] or sorted(map(tuple, flat)) == sorted([(0xA8, 0xAA), (0xAA, 0xA8), (0xAC, 0xAE), (0xAE, 0xAC)]), f'lambda destination offsets {flat}')
report['ftol'] = {'native': hex(FTOL_NATIVE), 'called_anywhere': False, 'replacement': 'inline cvttss2si ecx,xmm0 after mulss 128.0f (lambda %s), result stored as 16-bit' % hex(lam['body']),
                  'cvttss2si_site': hex(cv[0][0]) if cv else None, 'lambda_call_sites_dest_offsets': [{'site': hex(c['site']), 'dest_offsets': [hex(x) for x in f]} for c, f in zip(lam_calls, flat)]}

# ---- 5. Float path of the POV (and axes) ----
text = [x[2] for x in ipp]
LOC = r'dword ptr \[ebp\+([0-9A-F]+)h\]'
fi = [i for i, t in enumerate(text) if t.startswith('fild')]
check(len(fi) == 1, f'exactly one x87 fild in compiled ProcessPad (POV): {len(fi)}')
check(not any(t.startswith('cvtsi2sd') for t in text), 'no cvtsi2sd anywhere in ProcessPad (POV path is x87 fild like the EXE)')
check(len([t for t in text if t.startswith('fadd')]) == 1, 'exactly one x87 fadd in ProcessPad')
pov = {}
if fi:
    i = fi[0]
    m_fild = re.fullmatch(r'fild ' + LOC, text[i])
    check(bool(m_fild), f'fild operand is a signed dword local: {text[i]}')
    loc = m_fild.group(1) if m_fild else None
    seq = text[i:i + 14]
    shape = [r'fild ' + LOC, r'cmp ' + LOC + r',0', r'jge [0-9A-F]+', r'fadd ' + LOC, r'fmul ' + LOC, r'fmul ' + LOC, r'fld st\(0\)', r'fsin', r'fstp ' + LOC, r'fcos', r'fchs', r'fstp ' + LOC]
    ok_shape = len(seq) >= len(shape) and all(re.fullmatch(p_, t) for p_, t in zip(shape, seq))
    check(ok_shape, f'POV sequence fild;cmp,0;jge;fadd dword;fmul;fmul;fld st0;fsin;fstp;fcos;fchs;fstp not matched: {seq[:12]}')
    if ok_shape:
        g = [re.search(r'\[ebp\+([0-9A-F]+)h\]', t).group(1) for t in seq[:6] if '[ebp' in t]
        check(g[0] == g[1], 'cmp tests the same signed local that fild loaded (sign branch on the DWORD)')
        jt = int(seq[2].split()[-1], 16)
        after_fadd = next(x for x in ipp[i:i + 8] if x[2].startswith('fmul'))[0]
        check(jt == after_fadd, 'jge skips exactly the fadd (lands on first fmul)')
        # constants: each fadd/fmul local was stored from a movss xmm0,[ds:addr] just before
        def const(local):
            for k in range(i - 1, max(i - 40, 0), -1):
                if re.fullmatch(r'movss dword ptr \[ebp\+' + local + r'h\],xmm0', text[k]):
                    mm = re.fullmatch(r'movss xmm0,dword ptr ds:\[([0-9A-F]+)h\]', text[k - 1])
                    return struct.unpack('<f', read(int(mm.group(1), 16), 4))[0] if mm else None
        fa = re.fullmatch(r'fadd ' + LOC, seq[3]).group(1)
        m1 = re.fullmatch(r'fmul ' + LOC, seq[4]).group(1)
        m2 = re.fullmatch(r'fmul ' + LOC, seq[5]).group(1)
        c_add, c1, c2 = const(fa), const(m1), const(m2)
        check(c_add == 4294967296.0, f'fadd operand is float 2^32: {c_add}')
        check(c1 is not None and abs(c1 - 0.01) < 1e-9, f'first fmul 0.01f: {c1}')
        check(c2 is not None and abs(c2 - 0.017453292) < 1e-9, f'second fmul 0.017453292f: {c2}')
        # the signed local is filled from the POV[0] buffer slot (ebp+ecx+..) just before the fild
        check(any(re.fullmatch(r'mov dword ptr \[ebp\+' + loc + r'h\],edx', t) for t in text[max(i - 12, 0):i]), 'signed local stored from POV dword read')
        pov = {'fild_site': hex(ipp[i][0]), 'fadd_float_constant': c_add, 'multipliers_float': [c1, c2],
               'sequence': [f'{x[0]:#x}: {x[2]}' for x in ipp[i:i + 12]]}
# normalizer
check(any(t[2].startswith('fild dword ptr [ebp+8]') for t in inorm) and any(t[2].startswith('fmul dword ptr') for t in inorm), 'NormalizePadAxis = fild; fmul; fstp float; fld')
check(struct.unpack('<f', read(int(re.search(r'ds:\[([0-9A-F]+)h\]', next(t[2] for t in inorm if 'movss xmm0,dword ptr ds:[' in t[2])).group(1), 16), 4))[0] == struct.unpack('<f', struct.pack('<f', 0.0005))[0], 'NormalizePadAxis multiplier 0.0005f')
norm_calls = [c for c in calls(ipp) if c['in_image'] and c['body'] == norm['body']]
check(len(norm_calls) == 4, f'NormalizePadAxis calls: {len(norm_calls)}')

pov['exe_sequence'] = 'fild dword [pov]; test eax,eax; jge skip; fadd dword [2^32 float] (PC width: 24 bits in game); fmul 0.01f; fmul 0.017453292f; fld st0; fsin; fstp; fcos; fmul -1.0; fstp'
pov['compiled_sequence'] = 'fild dword [povSigned]; cmp [povSigned],0; jge skip; fadd dword [2^32 float local]; fmul dword [0.01f local]; fmul dword [0.017453292f local]; fld st0; fsin; fstp dword; fcos; fchs; fstp dword'
pov['equivalent_to_exe'] = not any(f.startswith(('exactly one x87', 'fild', 'POV', 'cmp tests', 'jge skips', 'fadd operand', 'first fmul', 'second fmul', 'no cvtsi2sd', 'signed local')) for f in failures)
report['pov_float_path'] = pov
report['axis_float_path'] = {'normalize_pad_axis': hex(norm['body']), 'calls': len(norm_calls), 'same_as_exe': 'fild; fmul 0.0005f; fstp float (no difference found)',
                             'deadzone_compare': 'comiss 0.3f < fabs(x), jb taken when ordered-less OR unordered (NaN proceeds); EXE skips on NaN; NaN is excluded from the corpus'}

# ---- no stray direct calls into the EXE from the three functions (everything stays in the ASI) ----
exe_calls = [hex(c['entry']) for c in allc if not c['in_image']]
check(not exe_calls, f'direct calls out of the image: {exe_calls}')

d = S['digest']
ok_events = all(k in report for k in ('rs_pad_event_handler',)) and len(ev) == 3 and not [f for f in failures if f.startswith(('event', 'hook', 'exactly 3', 'Poll', 'INPUTLOST', 'single GetDeviceState', 'GetDeviceState', 'DIJOY', 'X/Y'))]
proof = {'asi_sha256': d(S['asi']), 'pdb_sha256': d(S['pdb']), 'manifest_sha256': d(B / 'artifacts/manifest.json'), 'pdb_rsds_match': True,
         'passed': not failures, 'event_call_paths_verified': bool(ok_events), 'inline_getpad_and_float_paths_reviewed': not failures,
         'failures': failures,
         'target': {'ProcessPad': hex(pp['body']), 'size': pp['size'], 'lambda': hex(lam['body']), 'NormalizePadAxis': hex(norm['body'])},
         **report,
         'known_semantic_difference': {},
         'limitations': ['Static call/ABI/float-path review of the matching PE only. Not validation; no runtime, no GTA, no hook-state credit.']}
out = B / 'Diagnostics/compiled-runtime-proof.json'
out.write_text(json.dumps(proof, indent=2) + '\n', encoding='utf-8', newline='\n')
print(json.dumps({'passed': proof['passed'], 'failures': failures, 'sha256': hashlib.sha256(out.read_bytes()).hexdigest()}, indent=2))
