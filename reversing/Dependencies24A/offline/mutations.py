"""Pruebas de mutacion del ensayo sin juego: cada mutacion altera UNA construccion de __MainWndProc (obj/WndProc_h.cpp, generado por build.bat) o del ayudante x87 (X87Math.cpp) y el ensayo
(obj/hmain.obj ya compilado) debe detectarla (diferencias > 0). Las que sigan en 0 se explican abajo.
Uso: python mutations.py [indices...]   (tras ejecutar build.bat una vez)
Resultado de la entrega (256 casos por ruta): todas detectadas salvo
  - 'sin & 31 en el desplazamiento': `shl` de x86 ya enmascara el contador a 5 bits, el codigo generado es identico (el & 31 solo evita el comportamiento indefinido de C++);
"""
import subprocess
import sys
from pathlib import Path

W = Path(__file__).resolve().parent
ROOT = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 and Path(sys.argv[1]).is_dir() else W.parents[2]
IDX = [int(a) for a in sys.argv[1:] if a.isdigit()]
VC = r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
GAME_EXE = r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
src = (W / 'obj/WndProc_h.cpp').read_text(encoding='utf-8')
MUT = [
    ('SWP_NOMOVE solo -> NOMOVE|NOSIZE', '            SWP_NOMOVE\n', '            SWP_NOMOVE | SWP_NOSIZE\n'),
    ('sin WM_TIMER', 'case WM_TIMER:', 'case 0x7FFFFFF1:'),
    ('WM_ACTIVATE usa wParam completo', 'if (LOWORD(wParam) == 0) {\n            CAudioEngine::PauseAllSounds();', 'if (wParam == 0) {\n            CAudioEngine::PauseAllSounds();'),
    ('ForegroundApp antes del evento', '            RsEventHandler(rsACTIVATE, (void*)1);\n            ForegroundAppDword = 1;', '            ForegroundAppDword = 1;\n            RsEventHandler(rsACTIVATE, (void*)1);'),
    ('letra con signo', 'const auto dvletter = (uint8)AEAudioHardware.GetDVDDriveLetter();', 'const auto dvletter = (int8)AEAudioHardware.GetDVDDriveLetter();'),
    ('sin & 31 en el desplazamiento', "(1u << ((dvletter - 'A') & 31))", "(1u << ((dvletter - 'A') & 255))"),
    ('state inicial 0', 'OAFilterState state = (OAFilterState)lParam;', 'OAFilterState state = 0;'),
    ('WM_SIZE && en vez de ||', 'if (wndsz.w == width || wndsz.h == height) {', 'if (wndsz.w == width && wndsz.h == height) {'),
    ('GetWindowPlacement con PSGLOBAL', 'GetWindowPlacement(hWnd, &wndpl);', 'GetWindowPlacement(PSGLOBAL(window), &wndpl);'),
    ('WM_SETFOCUS devuelve 0', '        ForegroundAppDword = 1;\n        break;\n    }\n    case WM_KILLFOCUS', '        ForegroundAppDword = 1;\n        return 0;\n    }\n    case WM_KILLFOCUS'),
    ('MOUSEMOVE break', '        FrontEndMenuManager.m_nMousePosWinY = GET_Y_LPARAM(lParam);\n        return 0;', '        FrontEndMenuManager.m_nMousePosWinY = GET_Y_LPARAM(lParam);\n        break;'),
    ('sin NoDisk inicial', '        FrontEndMenuManager.NoDiskInDriveMessage();\n        bool inserted = false;', '        bool inserted = false;'),
    ('CodePause se limpia al salir sin disco', '            OutputDebugStringA("Exiting game as Audio CD was not inserted");', '            OutputDebugStringA("Exiting game as Audio CD was not inserted");\n            CTimer::SetCodePause(false);'),
    ('RwInitialized como bool', 'if (!RwInitializedDword || height <= 0 || width <= 0) {', 'if (!RwInitialized || height <= 0 || width <= 0) {'),
    ('Windowed como bool', '                WindowedDword = 1;', '                Windowed = true;'),
    ('Pause != 0 -> FAILED', 'if (mc->Pause() != 0) {', 'if (FAILED(mc->Pause())) {'),
    ('UpdateWindow tras Run', '                VideoPlayer::UpdateWindow();\n                if (state != State_Running) {', '                if (state != State_Running) {\n                VideoPlayer::UpdateWindow();'),
    ('timer con PSGLOBAL(window)', 'SetTimer(hWnd, 1, WndProcTimerPeriodCpp', 'SetTimer(PSGLOBAL(window), 1, WndProcTimerPeriodCpp'),
    ('gamma ramps invertidas', 'wParam ? &CGamma::ms_GammaTable : &CGamma::ms_SavedGamma', 'wParam ? &CGamma::ms_SavedGamma : &CGamma::ms_GammaTable'),
    ('SetGammaRamp flags 0', 'dev->SetGammaRamp(0, D3DSGR_CALIBRATE,', 'dev->SetGammaRamp(0, 0,'),
    ('mascara de bandera con 2', 'if ((idev->dbcv_flags & DBTF_MEDIA) == 0) {', 'if ((idev->dbcv_flags & 3) == 0) {'),
    ('desplazamiento >= 32 sale', "(idev->dbcv_unitmask & (1u << ((dvletter - 'A') & 31))) == 0)", "(dvletter - 'A') >= 32 || (idev->dbcv_unitmask & (1u << (dvletter - 'A'))) == 0)"),
    ('IsStreaming antes de m_bInitialised', 'if (!AEAudioHardware.m_bInitialised || !AEAudioHardware.IsStreamingFromDVD()) {', 'if (!AEAudioHardware.IsStreamingFromDVD() || !AEAudioHardware.m_bInitialised) {'),
    ('ms_running como bool', 'if (CCutsceneMgr::ms_running == 1) {', 'if (CCutsceneMgr::ms_running) {'),
    ('INITIAL sin comprobar activacion', 'if (!wndBeingActivated && PSGLOBAL(fullScreen)) {', 'if (PSGLOBAL(fullScreen)) {'),
    ('fullScreen como bool', 'if (!wndBeingActivated && PSGLOBAL(fullScreen)) {', 'if (!wndBeingActivated && (bool)(uint8)PSGLOBAL(fullScreen)) {'),
    ('Pause sin WaitState', '                            VideoPlayer::WaitState(State_Paused);', '                            (void)0;'),
    ('Run: SetFocus con hWnd', 'SetFocus(PSGLOBAL(window));', 'SetFocus(hWnd);'),
    ('KEYUP no distingue', '(uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)', '(uMsg == WM_KEYDOWN)'),
    ('shift tras traducir solo si true', '        if (wParam == VK_SHIFT) {\n            GTATranslateShiftKey(&ck);', '        if (wParam == VK_SHIFT && ck != rsNULL) {\n            GTATranslateShiftKey(&ck);'),
    ('CodePause no se activa', '        CTimer::SetCodePause(true);\n        if (CCutsceneMgr::ms_running == 1) {', '        if (CCutsceneMgr::ms_running == 1) {'),
    ('SIZING sin condicion de estado', 'if (RwInitializedDword && gGameState == GAME_STATE_IDLE) {', 'if (RwInitializedDword) {'),
    ('MOUSEMOVE Y sin signo', 'GET_Y_LPARAM(lParam)', '(int)HIWORD(lParam)'),
    ('GTATranslateKey con EAX completo', 'if (static_cast<uint8>(GTATranslateKey(&ck, lParam, wParam))) {', 'if (GTATranslateKey(&ck, lParam, wParam)) {'),
    ('Foreground=0 despues del SetTimer', '            ForegroundAppDword = 0;\n            // 0x747FAA', '            // 0x747FAA'),
    ('SetTimer id 2', 'SetTimer(hWnd, 1, WndProcTimerPeriodCpp', 'SetTimer(hWnd, 2, WndProcTimerPeriodCpp'),
]
XMUT = [  # ayudante x87 (se aplica sobre el final de X87Math.cpp)
    ('x87: sin PE del fistp', '        if (!integral) {', '        if (false) {'),
    ('x87: sin IE con frameLimit 0', '        volatile double invalid  = sqrt(minusOne);', '        volatile double invalid  = minusOne;'),
    ('x87: division en entero', 'volatile float quotient = float(1000.0 / double(frameLimit)); // fild; fdivr [1000.0f]', 'volatile float quotient = frameLimit ? float(1000 / frameLimit) : 0.0f;'),
    ('x87: registros muertos sin plantar', 'return Ftol2RegsCpp(v, 0).eax;\n}\n#pragma float_control(pop)\n', 'return unsigned(static_cast<long long>(v));\n}\n#pragma float_control(pop)\n'),
]


def run(cmd):
    return subprocess.run(cmd, shell=True, capture_output=True, text=True, encoding='utf-8', errors='replace')


def link_and_run(wndproc_obj, x87_obj):
    bat = W / 'mutation.bat'
    bat.write_text('@echo off\ncall "%s" >nul 2>&1\ncd /d %%~dp0\n'
                   'link /nologo /OUT:obj\\hm.exe obj\\hmain.obj %s %s /OPT:REF /FORCE:UNRESOLVED /DEBUG /BASE:0x10000000 /FIXED /DYNAMICBASE:NO /LARGEADDRESSAWARE:NO user32.lib kernel32.lib strmiids.lib dxguid.lib > obj\\hmlink.log 2>&1\n'
                   'obj\\hm.exe "%s" 256 > obj\\hm.out 2>&1\n' % (VC, wndproc_obj, x87_obj, GAME_EXE), encoding='utf-8')
    run(str(bat))
    out = (W / 'obj/hm.out').read_text(encoding='utf-8', errors='replace') if (W / 'obj/hm.out').exists() else ''
    (W / 'obj/hm.out').unlink(missing_ok=True)
    tot = [l for l in out.splitlines() if l.startswith('TOTAL')]
    bad = [l.split()[1] + ':' + l.split()[-1] for l in out.splitlines() if l.startswith('  ruta') and not l.endswith('diferencias 0')]
    return (tot[0] if tot else 'EXC ' + out.strip()[-200:]), '; '.join(bad)


for i, (name, old, new) in enumerate(MUT):
    if IDX and i not in IDX:
        continue
    if src.count(old) != 1:
        print(i, name, 'PATRON NO ENCONTRADO', src.count(old)); continue
    (W / 'obj/WndProc_m.cpp').write_text(src.replace(old, new), encoding='utf-8', newline='\n')
    c = run('call "%s" >nul 2>&1 && cd /d "%s" && cl @proj.rsp /Fo"obj\\WndProc_m.obj" obj\\WndProc_m.cpp > obj\\wpm.log 2>&1' % (VC, W))
    print(i, name, '=>', *link_and_run('obj\\WndProc_m.obj', 'obj\\X87Math.obj'))

x87 = (ROOT / 'source/game_sa/X87Math.cpp').read_text(encoding='utf-8')
for j, (name, old, new) in enumerate(XMUT):
    if IDX and 1000 + j not in IDX:
        continue
    a = x87.rfind('WndProcTimerPeriodCpp')
    head, tail = x87[:a], x87[a:]
    if tail.count(old) != 1:
        print(1000 + j, name, 'PATRON NO ENCONTRADO'); continue
    (W / 'obj/X87Math_m.cpp').write_text(head + tail.replace(old, new), encoding='utf-8', newline='\n')
    run('call "%s" >nul 2>&1 && cd /d "%s" && cl /nologo /c /std:c++latest /EHsc /utf-8 /O2 /Oi /arch:IA32 /Zi /fp:fast /MTd /DNOMINMAX /I"%s" obj\\X87Math_m.cpp /Foobj\\X87Math_m.obj /Fdobj\\xm.pdb > obj\\xm.log 2>&1'
        % (VC, W, ROOT / 'source/game_sa'))
    print(1000 + j, name, '=>', *link_and_run('obj\\WndProc_h.obj', 'obj\\X87Math_m.obj'))
