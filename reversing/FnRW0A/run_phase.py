"""RW0A: prueba de juego con la DLL completa (RenderWare reconstruido enganchado) en la copia de pruebas.

Uso:  python reversing/FnRW0A/run_phase.py F1|F2|F3 [--minutes N] [--unhook=lista] [--label texto] [--wander]

  F1  arranque hasta el menu principal y 3 min en reposo
  F2  partida (ranura 2) cargada en el mundo libre, 10 min
  F3  igual, 30 min

Por cada ejecucion:
  - toma el candado D:\\Original\\gta_lock (mkdir atomico; espera si lo tiene otro, nunca lo borra ajeno);
  - respalda los archivos que cambia de la copia de pruebas (gta_reversed.asi/.pdb, los dos INI) y los restaura
    siempre al terminar, comprobando hashes (tambien que las partidas y el EXE no cambian);
  - instala la DLL completa (bin/Debug/gta_reversed.asi del worktree) y la sonda zz_rwprobe.asi (solo del arnes);
  - lanza el juego, vigila dialogos, "No responde", memoria y salida; toma capturas periodicas;
  - pide a la sonda volcado de contadores y, si hay fallo, la pila; cierra su propio proceso; restaura.
Todo el registro es en flujo (lectura de la cola de los archivos) y con tamano limitado.
"""
import argparse
import csv
import ctypes
import hashlib
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import time
import zlib
from ctypes import wintypes as w
from datetime import datetime
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'reversing'))
import runtime_runner as rr  # noqa: E402  (GAME, game_dialog, game_hung, digest, saves)

GAME = rr.GAME
LOCK = Path(r'D:\Original\gta_lock')
OWNER = LOCK / 'owner_wv6_rw0a'
ASI_BUILD = ROOT / 'bin' / 'Debug'
PROBE = HERE / 'probe' / 'zz_rwprobe.asi'
RW_CLASSES = set('RwCore RpWorld RwPlCore RwD3D9 RpSkin RpMatFX ImportThunks RpUVAnim ZlibInflate RtDict D3DX9 RtAnim RpHAnim RtQuat QTMLRuntime DelayLoadThunks RtBMP RtSlerp'.split())

user32 = ctypes.windll.user32
gdi32 = ctypes.windll.gdi32
kernel32 = ctypes.windll.kernel32
psapi = ctypes.windll.psapi


def now_s():
    return time.monotonic()


def sha(p):
    return rr.digest(Path(p))


# ------------------------------------------------------------------ capturas
class BITMAPINFOHEADER(ctypes.Structure):
    _fields_ = [('biSize', w.DWORD), ('biWidth', w.LONG), ('biHeight', w.LONG), ('biPlanes', w.WORD), ('biBitCount', w.WORD),
                ('biCompression', w.DWORD), ('biSizeImage', w.DWORD), ('biXPelsPerMeter', w.LONG), ('biYPelsPerMeter', w.LONG),
                ('biClrUsed', w.DWORD), ('biClrImportant', w.DWORD)]


def write_png(path, width, height, rgb):
    def chunk(tag, data):
        c = struct.pack('>I', len(data)) + tag + data
        return c + struct.pack('>I', zlib.crc32(tag + data) & 0xFFFFFFFF)
    stride = width * 3
    raw = b''.join(b'\x00' + rgb[y * stride:(y + 1) * stride] for y in range(height))
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0)) + chunk(b'IDAT', zlib.compress(raw, 6)) + chunk(b'IEND', b'')
    Path(path).write_bytes(png)


def game_windows(pid):
    found = []
    proc = ctypes.WINFUNCTYPE(w.BOOL, w.HWND, w.LPARAM)

    def top(h, _):
        owner = w.DWORD()
        user32.GetWindowThreadProcessId(h, ctypes.byref(owner))
        if owner.value == pid and user32.IsWindowVisible(h):
            found.append(h)
        return True
    user32.EnumWindows(proc(top), 0)
    return found


def win_title(h):
    buf = ctypes.create_unicode_buffer(256)
    user32.GetWindowTextW(h, buf, 256)
    return buf.value


def screenshot(pid, path, which='game'):
    """Captura de pantalla de una ventana del juego (lo que ve el usuario): which='game' (ventana de render) o 'console'. Devuelve (ancho, alto) o None."""
    try:
        wins = game_windows(pid)
        if which == 'game':
            wins = [h for h in wins if win_title(h).startswith('GTA: San Andreas')] or wins
        else:
            wins = [h for h in wins if not win_title(h).startswith('GTA: San Andreas')]
        if not wins:
            return None
        rect = w.RECT()
        h = wins[0]
        if user32.IsIconic(h):
            return None
        user32.GetWindowRect(h, ctypes.byref(rect))
        x, y, ww, hh = rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top
        if ww <= 0 or hh <= 0 or ww * hh > 8000 * 5000:
            return None
        hdc = user32.GetDC(0)
        mem = gdi32.CreateCompatibleDC(hdc)
        bmp = gdi32.CreateCompatibleBitmap(hdc, ww, hh)
        gdi32.SelectObject(mem, bmp)
        gdi32.BitBlt(mem, 0, 0, ww, hh, hdc, x, y, 0x00CC0020 | 0x40000000)  # SRCCOPY | CAPTUREBLT
        bi = BITMAPINFOHEADER()
        bi.biSize = ctypes.sizeof(bi)
        bi.biWidth, bi.biHeight, bi.biPlanes, bi.biBitCount = ww, -hh, 1, 32
        buf = ctypes.create_string_buffer(ww * hh * 4)
        gdi32.GetDIBits(mem, bmp, 0, hh, buf, ctypes.byref(bi), 0)
        gdi32.DeleteObject(bmp)
        gdi32.DeleteDC(mem)
        user32.ReleaseDC(0, hdc)
        raw = bytearray(buf.raw)
        n = ww * hh
        rgb = bytearray(n * 3)
        rgb[0::3] = raw[2::4]
        rgb[1::3] = raw[1::4]
        rgb[2::3] = raw[0::4]
        if ww > 1000:  # reduce a la mitad para que cada captura pese poco
            nw, nh = ww // 2, hh // 2
            out = bytearray(nw * nh * 3)
            for yy in range(nh):
                row = rgb[(2 * yy) * ww * 3:(2 * yy) * ww * 3 + ww * 3]
                for c in range(3):
                    out[yy * nw * 3 + c:(yy + 1) * nw * 3:3] = row[c:nw * 6:6][:nw]
            write_png(path, nw, nh, bytes(out))
            return nw, nh
        write_png(path, ww, hh, bytes(rgb))
        return ww, hh
    except Exception as e:  # una captura fallida nunca debe tumbar la prueba
        return None


# ------------------------------------------------------------------ utilidades
def ping(pid, ms=1500):
    """True si la ventana responde a WM_NULL; None si no hay ventana."""
    wins = game_windows(pid)
    if not wins:
        return None
    res = w.DWORD()
    r = user32.SendMessageTimeoutW(wins[0], 0, 0, 0, 0x0002 | 0x0001, ms, ctypes.byref(res))  # SMTO_ABORTIFHUNG|SMTO_BLOCK
    return bool(r)


class PMC(ctypes.Structure):
    _fields_ = [('cb', w.DWORD), ('PageFaultCount', w.DWORD), ('PeakWorkingSetSize', ctypes.c_size_t), ('WorkingSetSize', ctypes.c_size_t),
                ('QuotaPeakPagedPoolUsage', ctypes.c_size_t), ('QuotaPagedPoolUsage', ctypes.c_size_t),
                ('QuotaPeakNonPagedPoolUsage', ctypes.c_size_t), ('QuotaNonPagedPoolUsage', ctypes.c_size_t),
                ('PagefileUsage', ctypes.c_size_t), ('PeakPagefileUsage', ctypes.c_size_t), ('PrivateUsage', ctypes.c_size_t)]


def mem_mb(pid):
    h = kernel32.OpenProcess(0x1000 | 0x0400, False, pid)
    if not h:
        return None
    pmc = PMC()
    pmc.cb = ctypes.sizeof(pmc)
    ok = psapi.GetProcessMemoryInfo(h, ctypes.byref(pmc), pmc.cb)
    kernel32.CloseHandle(h)
    return round(pmc.PrivateUsage / 1048576, 1) if ok else None


def tail_lines(path, n=1, block=8192):
    p = Path(path)
    if not p.exists():
        return []
    with p.open('rb') as f:
        f.seek(max(0, p.stat().st_size - block))
        data = f.read()
    return data.decode('utf-8', 'replace').splitlines()[-n:]


def last_state(run):
    for line in reversed(tail_lines(run / 'state.jsonl', 3)):
        try:
            return json.loads(re.sub(r':(-?)nan', ':null', line).replace(':-inf', ':null').replace(':inf', ':null'))
        except ValueError:
            continue
    return None


def game_running():
    rows = csv.reader(subprocess.check_output(['tasklist', '/FO', 'CSV', '/NH'], text=True, errors='replace').splitlines())
    return any(r and r[0].lower() in ('gta_sa.exe', 'gta-sa.exe') for r in rows)


GAMELOG = GAME / 'scripts' / 'logs' / 'log.log'
LOG_ABORT_MB = 1500  # la DLL escribe aqui; si crece mas de esto durante una ejecucion se aborta (evita llenar el disco)


def log_size():
    try:
        return GAMELOG.stat().st_size
    except OSError:
        return 0


def scan_game_log(start, out_path, limit_lines=400):
    """Lee EN FLUJO el tramo nuevo de log.log y guarda: recuento por tipo, lineas distintas por plantilla (maximo limit_lines) y la cola."""
    from collections import Counter, deque
    kinds = Counter()
    seen = {}
    tail = deque(maxlen=60)
    with GAMELOG.open('rb') as f:
        f.seek(start)
        for raw in f:
            line = raw.decode('utf-8', 'replace').rstrip()
            m = re.match(r'\[(\w+)\]', line)
            kinds[m.group(1) if m else '?'] += 1
            tail.append(line[:300])
            if m and m.group(1) in ('error', 'aviso', 'critico', 'fatal') or re.search(r'assert|RTC|Run-Time Check', line, re.I):
                tpl = re.sub(r'[-+]?\d+(\.\d+)?', '#', re.sub(r'\[\d\d:\d\d:\d\d\.\d+\]', '', line))[:200]
                e = seen.setdefault(tpl, [0, line[:300]])
                e[0] += 1
    with open(out_path, 'w', encoding='utf-8') as o:
        o.write('tramo analizado: %d bytes desde %d\n' % (GAMELOG.stat().st_size - start, start))
        o.write('lineas por nivel: %s\n\n' % dict(kinds))
        o.write('--- plantillas de error/aviso/assert (veces, ejemplo) ---\n')
        for tpl, (n, ex) in sorted(seen.items(), key=lambda kv: -kv[1][0])[:limit_lines]:
            o.write('%8d  %s\n' % (n, ex))
        o.write('\n--- cola (60 lineas) ---\n' + '\n'.join(tail) + '\n')
    return dict(kinds)


def take_lock(wait_max=3600):
    t0 = now_s()
    while True:
        try:
            LOCK.mkdir()
            OWNER.write_text(f'wv-6 RW0A {datetime.now():%H:%M:%S}\n')
            return True
        except FileExistsError:
            if now_s() - t0 > wait_max:
                raise RuntimeError('gta_lock ocupado: no se pudo tomar')
            time.sleep(30)


def release_lock():
    try:
        OWNER.unlink(missing_ok=True)
        LOCK.rmdir()
    except OSError:
        pass


def catalog_addresses():
    """Direcciones de las entradas de RenderWare/D3D del catalogo (hooks.csv): las 822 (790 direcciones distintas)."""
    out = []
    with open(ROOT / 'docs' / 'hooks.csv', encoding='utf-8-sig', newline='') as f:
        for r in csv.DictReader(f):
            if r['class'] in RW_CLASSES and r['reversed'] == '1':
                a = int(r['address'], 16)
                if a:
                    out.append(a)
    return sorted(set(out))


def ini_for(original_text, slot):
    cfg = dict(Enable='true', SaveGameToLoad=str(slot), NoEAX='true', NoNVidia='true', NoLogo='true', NoTitleOrIntro='true',
               NoCopyright='true', NoFading='true', NoLoadingTune='true', RenderAtAllTimes='true')
    text = original_text
    sec = re.search(r'(?ms)^\[FastLoader\][^\r\n]*\r?\n.*?(?=^\[|\Z)', text)
    rep = '[FastLoader]\n' + ''.join(f'{k} = {v}\n' for k, v in cfg.items())
    text = text[:sec.start()] + rep + text[sec.end():] if sec else rep + text
    return text


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('phase', choices=['F1', 'F2', 'F3'])
    ap.add_argument('--minutes', type=float, default=None)
    ap.add_argument('--unhook', default='')
    ap.add_argument('--unhook-except', default='')
    ap.add_argument('--unhook-all', action='store_true')
    ap.add_argument('--label', default='')
    ap.add_argument('--slot', type=int, default=2)
    ap.add_argument('--shot-every', type=int, default=60)
    ap.add_argument('--asi', default=str(ASI_BUILD / 'gta_reversed.asi'))
    ap.add_argument('--no-probe', action='store_true')
    ap.add_argument('--hour', type=int, default=None, help='fija la hora del juego (0-23) con la sonda')
    ap.add_argument('--fptrap', action='store_true', help='la sonda desenmascara la operacion invalida SSE y registra cada sitio')
    ap.add_argument('--wander', action='store_true', help='pulsa W/Mayus en la ventana del juego solo si esta en primer plano')
    a = ap.parse_args()
    phase = a.phase
    minutes = a.minutes if a.minutes is not None else {'F1': 3, 'F2': 10, 'F3': 30}[phase]
    slot = -2 if phase == 'F1' else a.slot
    target_wait = 480 if phase == 'F1' else 1200
    label = (a.label or phase) + datetime.now().strftime('-%Y%m%d-%H%M%S')
    run = HERE / 'runs' / label
    (run / 'shots').mkdir(parents=True)
    (run / 'backup').mkdir()
    H = (run / 'harness.jsonl').open('a', encoding='utf-8')
    hwritten = [0]

    def log(ev, **kw):
        if hwritten[0] > 1_500_000:
            return
        line = json.dumps(dict(t=round(now_s() - T0, 1), ev=ev, **kw), ensure_ascii=False)
        hwritten[0] += len(line)
        H.write(line + '\n')
        H.flush()
    T0 = now_s()
    res = dict(phase=phase, label=label, minutes=minutes, slot=slot, unhook=a.unhook, unhook_except=a.unhook_except, unhook_all=a.unhook_all, passed=False, events=[])
    backups = []
    proc = None
    locked = False
    try:
        if game_running():
            raise RuntimeError('Hay un GTA abierto; no se toca.')
        asi = Path(a.asi)
        pdb = asi.with_suffix('.pdb')
        res['asi'] = dict(path=str(asi), sha256=sha(asi), size=asi.stat().st_size, mtime=datetime.fromtimestamp(asi.stat().st_mtime).isoformat(timespec='seconds'), config='Debug (/Od, /RTC1 EnableFastChecks, CRT debug estatico, sin NDEBUG: asserts activos, EXTRA_DEBUG_FEATURES)')
        res['exe_sha256'] = sha(GAME / 'gta_sa.exe')
        if res['exe_sha256'] != rr.EXE_HASH:
            raise RuntimeError('EXE distinto del aprobado')
        locked = take_lock()
        log('lock', ok=True)
        if (GAME / 'reversed-tests.lock').exists():
            raise RuntimeError('reversed-tests.lock presente en la copia de pruebas')
        saves_before = rr.saves()
        log_start = log_size()
        res['game_log_bytes_before'] = log_start
        # respaldo
        targets = [GAME / 'scripts/gta_reversed.asi', GAME / 'scripts/gta_reversed.pdb', GAME / 'gta-reversed.ini', GAME / 'scripts/gta-reversed.ini']
        for t in targets:
            if t.exists():
                b = run / 'backup' / (t.parent.name + '__' + t.name)
                shutil.copy2(t, b)
                backups.append((t, b, sha(b)))
            else:
                backups.append((t, None, None))
        res['backup_hashes'] = {str(t.relative_to(GAME)): h for t, _, h in backups}
        # instalar
        ini_orig = (GAME / 'gta-reversed.ini').read_text(encoding='utf-8-sig')
        for rel in ('gta-reversed.ini', 'scripts/gta-reversed.ini'):
            (GAME / rel).write_text(ini_for(ini_orig, slot), encoding='utf-8')
        shutil.copy2(asi, GAME / 'scripts/gta_reversed.asi')
        if pdb.exists():
            shutil.copy2(pdb, GAME / 'scripts/gta_reversed.pdb')
        if sha(GAME / 'scripts/gta_reversed.asi') != res['asi']['sha256']:
            raise RuntimeError('la DLL instalada no coincide')
        probe_target = GAME / 'scripts/zz_rwprobe.asi'
        env = dict(os.environ)
        if not a.no_probe:
            shutil.copy2(PROBE, probe_target)
            addrs = run / 'catalog_addresses.txt'
            addrs.write_text(''.join(f'0x{x:08X}\n' for x in catalog_addresses()))
            env['RWPROBE_OUT'] = str(run)
            env['RWPROBE_ADDRS'] = str(addrs)
            env['RWPROBE_PDBDIR'] = str(GAME / 'scripts')
            if a.hour is not None:
                env['RWPROBE_HOUR'] = str(a.hour)
            if a.fptrap:
                env['RWPROBE_FPTRAP'] = '1'
        args = [str(GAME / 'gta_sa.exe'), f'--dump-hooks-to={run / "hooks_dump.csv"}']
        if a.unhook_all:
            args.append('--unhook-all')
        elif a.unhook_except:
            args.append('--unhook-except=' + a.unhook_except)
        elif a.unhook:
            args.append('--unhook=' + a.unhook)
        res['args'] = args[1:]
        proc = subprocess.Popen(args, cwd=GAME, env=env)
        pid = proc.pid
        log('launch', pid=pid, args=args[1:])

        reached = None
        last_ping_ok = now_s()
        last_shot = -1e9
        shots = 0
        peak_mem = 0
        last_frame = None
        last_frame_change = now_s()
        verdict = None
        next_state = 0
        states = []
        wander_next = 0
        while True:
            t = now_s()
            if proc.poll() is not None:
                verdict = dict(kind='exit', code=proc.returncode)
                break
            dlg = rr.game_dialog(pid)
            if dlg:
                verdict = dict(kind='dialog', text=dlg)
                break
            if t >= next_state:
                next_state = t + 5
                p = ping(pid)
                if p or p is None:
                    last_ping_ok = t
                m = mem_mb(pid)
                if m:
                    peak_mem = max(peak_mem, m)
                    if m > 3800:
                        verdict = dict(kind='memoria', mb=m)
                        break
                st = last_state(run)
                if st:
                    states.append(dict(t=round(t - T0, 1), gs=st['game_state'], menu=st['menu'], frame=st['frame'], ped=st['ped'], hooks=st['hooks_called'], mem=m))
                    if len(states) > 4000:
                        states = states[::2]
                    if st['frame'] != last_frame:
                        last_frame = st['frame']
                        last_frame_change = t
                    if reached is None:
                        ok = (st['game_state'] == 7 and st['menu']) if phase == 'F1' else (st['game_state'] == 9 and st['ped'] and st['frame'] > 0)
                        if ok:
                            reached = t
                            log('objetivo alcanzado', state=st)
                            res['time_to_target_s'] = round(t - T0, 1)
                    log('estado', st=dict(gs=st['game_state'], menu=st['menu'], frame=st['frame'], ped=st['ped'], x=st['x'], y=st['y'], hooks=st['hooks_called']), mem_mb=m, ping=bool(p))
                if (log_size() - log_start) / 1048576 > LOG_ABORT_MB:
                    verdict = dict(kind='log_desbordado', mb=round((log_size() - log_start) / 1048576))
                    break
                if t - last_ping_ok > 120:
                    verdict = dict(kind='no_responde', segundos=int(t - last_ping_ok))
                    break
                if reached is None and t - T0 > target_wait:
                    verdict = dict(kind='tiempo_objetivo_agotado', segundos=target_wait)
                    break
                if reached is not None and phase != 'F1' and t - last_frame_change > 120:
                    verdict = dict(kind='fotogramas_detenidos', segundos=int(t - last_frame_change))
                    break
            if t - last_shot >= a.shot_every:
                last_shot = t
                size = screenshot(pid, run / 'shots' / f'{int(t - T0):05d}s_juego.png')
                size2 = screenshot(pid, run / 'shots' / f'{int(t - T0):05d}s_consola.png', 'console')
                if size:
                    shots += 1
                log('captura', size=size, consola=size2)
            if a.wander and reached is not None and phase != 'F1' and t >= wander_next:
                wander_next = t + 25
                wander(pid, log)
            if reached is not None and t - reached >= minutes * 60:
                verdict = dict(kind='completada')
                break
            time.sleep(1)
        res['verdict'] = verdict
        log('veredicto', **verdict)
        # fallo: pedir pila antes de cerrar
        if verdict['kind'] in ('dialog', 'no_responde', 'fotogramas_detenidos', 'tiempo_objetivo_agotado', 'memoria', 'log_desbordado') and proc.poll() is None:
            screenshot(pid, run / 'shots' / 'fallo_juego.png')
            ack = run / 'rwprobe.ack'
            ack.unlink(missing_ok=True)
            (run / 'rwprobe.cmd').write_text('stack counters')
            t1 = now_s()
            while now_s() - t1 < 40 and not ack.exists():
                time.sleep(1)
        elif verdict['kind'] == 'completada':
            screenshot(pid, run / 'shots' / 'final_juego.png')
            ack = run / 'rwprobe.ack'
            ack.unlink(missing_ok=True)
            (run / 'rwprobe.cmd').write_text('counters')
            t1 = now_s()
            while now_s() - t1 < 20 and not ack.exists():
                time.sleep(1)
        res.update(shots=shots, peak_private_mb=peak_mem, reached_target=reached is not None, duration_s=round(now_s() - T0, 1))
        res['passed'] = verdict['kind'] == 'completada'
        (run / 'states.json').write_text(json.dumps(states[-1500:]))
    except Exception as e:
        res['error'] = repr(e)
        res['passed'] = False
        log('error', error=repr(e))
    finally:
        if proc is not None:
            try:
                if proc.poll() is None:
                    proc.terminate()  # solo el proceso lanzado por este arnes
                proc.wait(timeout=30)
                res['exit_code'] = proc.returncode
            except Exception as e:
                res['error_close'] = repr(e)
        probe_t = GAME / 'scripts/zz_rwprobe.asi'
        for t in [probe_t]:
            dl = now_s() + 15
            while t.exists():
                try:
                    t.unlink()
                except PermissionError:
                    if now_s() > dl:
                        res['restore_error'] = f'no se pudo borrar {t}'
                        break
                    time.sleep(.3)
        for t, b, h in reversed(backups):
            try:
                dl = now_s() + 20
                while True:
                    try:
                        if b is None:
                            t.unlink(missing_ok=True)
                        else:
                            shutil.copy2(b, t)
                        break
                    except PermissionError:
                        if now_s() > dl:
                            raise
                        time.sleep(.3)
                if b is not None and sha(t) != h:
                    raise RuntimeError('hash al restaurar no coincide: ' + str(t))
            except Exception as e:
                res['restore_error'] = res.get('restore_error', '') + repr(e)
        try:
            res['saves_unchanged'] = rr.saves() == saves_before
        except Exception:
            res['saves_unchanged'] = None
        try:
            res['game_log_levels'] = scan_game_log(log_start, run / 'game_log_summary.txt')
            res['game_log_bytes_added'] = log_size() - log_start
        except Exception as e:
            res['game_log_error'] = repr(e)
        res['exe_unchanged'] = sha(GAME / 'gta_sa.exe') == rr.EXE_HASH
        if locked:
            release_lock()
        (run / 'phase_result.json').write_text(json.dumps(res, indent=2, ensure_ascii=False), encoding='utf-8')
        H.close()
        print(json.dumps({k: res.get(k) for k in ('phase', 'passed', 'verdict', 'duration_s', 'time_to_target_s', 'shots', 'peak_private_mb', 'restore_error', 'saves_unchanged', 'exe_unchanged', 'error')}, ensure_ascii=False))
        print('Carpeta:', run)


def wander(pid, log):
    """Pulsaciones minimas (W y Mayus) SOLO si la ventana del juego esta en primer plano; si no, no hace nada."""
    fg = user32.GetForegroundWindow()
    owner = w.DWORD()
    user32.GetWindowThreadProcessId(fg, ctypes.byref(owner))
    if owner.value != pid:
        log('wander_omitido', motivo='ventana del juego no esta en primer plano')
        return
    for sc in (0x11, 0x2A):  # W, Mayus izquierda (escaneo)
        user32.keybd_event(0, sc, 0x0008, 0)
    time.sleep(6)
    for sc in (0x2A, 0x11):
        user32.keybd_event(0, sc, 0x0008 | 0x0002, 0)
    log('wander', teclas='W+Mayus 6 s')


if __name__ == '__main__':
    main()
