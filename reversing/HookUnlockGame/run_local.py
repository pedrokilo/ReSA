import json
import sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer
base=analyzer({'Game::Idle':1024,'Game::FrontendIdle':1024,'Game::Render2dStuff':512})
def analyze(path):
    report=base(path)
    if report is None:return None
    rows=[json.loads(line) for line in path.read_text().splitlines()]
    idle_complete=[r for r in rows if r.get('event')=='idle_pairs_complete']
    if len(idle_complete)!=1 or not idle_complete[0]['state_restored'] or idle_complete[0]['pairs']!=2048:raise RuntimeError('Pares Idle no restaurados/completos')
    pairs=[r for r in rows if r.get('event')=='comparison']
    frames=[r for r in rows if r.get('event')=='game_dispatch_frame']
    if len(pairs)!=2560 or any(r.get('original') is None or r.get('candidate') is None for r in pairs):raise RuntimeError('Pares incompletos/nulos')
    if len(frames)!=320 or any(not(r['route_ok'] and r['returned'] and r['bytes_restored'] and r['foreground_before']) for r in frames):raise RuntimeError('Frames reales incompletos/fallidos')
    if any(r.get('event','').startswith('first_chance_av') for r in rows):raise RuntimeError('AV durante Game')
    bg=[r for r in rows if r.get('event')=='background_dispatch_guard']
    if {(r['mask'],r['param_nonnull'],r['app_active']) for r in bg}!={(mask,flag,app) for mask in range(8) for flag in (False,True) for app in (False,True)} or not all(r['passed'] and r['state_restored'] and r.get('foreground_tracking_restored') is True and r['foreground_tracking_before']==r['foreground_tracking_after'] for r in bg):raise RuntimeError('Guarda de segundo plano incompleta/fallida')
    messages=[r for r in rows if r.get('event')=='background_message_pump']
    if len(messages)!=len(bg) or not all(r['passed'] and r['frame_unchanged'] for r in messages):raise RuntimeError('Bombeo de mensajes en segundo plano fallido')
    sdl=[r for r in rows if r.get('event')=='sdl_background_focus_queue']
    if not all(r['applicable'] for r in messages):
        if len(sdl)!=len(bg) or not all(r['passed'] and r['state_restored'] and r['focus_lost_queued'] and r['focus_gained_queued'] and r['focus_lost_consumed'] and r['focus_gained_consumed'] and r['frame_unchanged'] for r in sdl):raise RuntimeError('Cola SDL durante pausa incompleta/fallida')
    for mask in range(8):
        group=[r for r in frames if r['mask']==mask]
        if len(group)!=40 or sum(r['function']=='Idle' for r in group)!=20 or sum(r['function']=='FrontendIdle' for r in group)!=20:raise RuntimeError('Máscara híbrida incompleta')
        if any(r['render_calls']<1 for r in group if r['function']=='Idle'):raise RuntimeError('Render2dStuff no observado en ruta real')
    report.update(game_frames=320,foreground_only=True,idle_background_dispatch_guard_proven=True,
        background_cases=len(bg),real_render_frames=160,native_message_pump_tested=all(r['applicable'] for r in messages),foreground_tracking_restored=True,
        sdl_focus_event_queue_tested=bool(sdl),native_focus_reactivation_tested=False)
    return report
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('hookunlockgame',manifest['binaries']['asi'],analyze,'Conmutación Game desde dispatcher real',timeout=900))
