"""Instrumenta el dispatcher antes/después de Game y restaura app.cpp incluso si falla."""
import json
import os
import sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
APP=ROOT/'source/app/app.cpp'
APP_GAME=ROOT/'source/app/app_game.cpp'
SOURCES=['source/app/app_game.cpp','source/app/platform/win/WinMain.cpp',
         'source/app/app.cpp','source/app/app.h','source/InjectHooksMain.cpp',
         'source/reversiblehooks/ReversibleHook/Simple.cpp','source/reversiblehooks/ReversibleHook/Base.h']
if __name__=='__main__':
    saved=APP.read_bytes()
    saved_game=APP_GAME.read_bytes()
    try:
        text=saved.decode('utf-8').replace('\r\n','\n')
        anchor='RsEventStatus AppEventHandler(RsEvent event, void* param) {'
        if text.count(anchor)!=1:raise RuntimeError('AppEventHandler anchor')
        rel=os.path.relpath(HERE/'LiveComparison.hpp',APP.parent).replace('\\','/')
        text=text.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+rel+'"',1)
        text=text.replace(anchor,anchor+'\n    HookUnlockGameLive::Before(event,param);')
        for call in ('Idle(param);','FrontendIdle();'):
            if text.count(call)!=1:raise RuntimeError(call)
            text=text.replace(call,call+'\n        HookUnlockGameLive::After(event);')
        APP.write_text(text,encoding='utf-8',newline='\n')
        game_text=saved_game.decode('utf-8').replace('\r\n','\n')
        if game_text.count('        Render2dStuff();')!=1:raise RuntimeError('Idle Render callsite')
        game_text=game_text.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+rel+'"',1)
        game_text=game_text.replace('        Render2dStuff();','        HookUnlockGameLive::RenderForward();')
        APP_GAME.write_text(game_text,encoding='utf-8',newline='\n')
        result=build(HERE,'hookunlockgame','HookUnlockGameLive',SOURCES)
        manifest=HERE/'artifacts/build-manifest.json'
        data=json.loads(manifest.read_text());data['app_cpp_original_sha256']=runtime_runner.digest_bytes(saved) if hasattr(runtime_runner,'digest_bytes') else __import__('hashlib').sha256(saved).hexdigest()
        data['dispatcher_instrumented_sha256']=data['sources']['source/app/app.cpp']
        data['render_callsite_instrumented_sha256']=data['sources']['source/app/app_game.cpp']
        data['sources']['source/app/app.cpp']=__import__('hashlib').sha256(saved).hexdigest()
        data['sources']['source/app/app_game.cpp']=__import__('hashlib').sha256(saved_game).hexdigest()
        manifest.write_text(json.dumps(data,indent=2))
        print(json.dumps(result))
    finally:
        APP.write_bytes(saved)
        APP_GAME.write_bytes(saved_game)
