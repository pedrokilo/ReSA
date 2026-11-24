import sys, os, time, struct, ctypes
sys.path.insert(0, r'D:\Original\wv-comp\reversing\FnTraf'); sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sonda_traf as S, teclas, capt2, png_util
captura = S.captura
sal = sys.argv[1]; pid = int(sys.argv[sys.argv.index('--pid')+1])
m = S.M(pid); t0 = time.time(); n = 0; ult = 0; t7 = None; k = 0
seq = ['', 'enter', 'down', 'enter', 'enter']
while time.time()-t0 < 170 and S.pid('gta_sa'):
    gs = m.u32(0xC8D4C0)
    h = captura.buscar()
    if h and gs in (2, 4):
        S.u.ShowWindow(h, 9); S.u.SetForegroundWindow(h); teclas.tecla('enter')
    if gs == 7 and t7 is None: t7 = time.time()
    if time.time()-ult > (3 if gs != 7 else 4):
        ult = time.time()
        if gs == 7 and h:
            S.u.ShowWindow(h, 9); S.u.SetForegroundWindow(h); time.sleep(0.3)
            im = capt2.bitblt(h); png_util.escribir(os.path.join(sal, 'b%03d.png' % n), *im)
            print('bitblt', n, [round(x,1) for x in png_util.media(*im)], flush=True)
            if k < len(seq) and time.time()-t7 > 6:
                if seq[k]: teclas.tecla(seq[k]); print('tecla', seq[k], flush=True)
                k += 1
        r = captura.capturar(os.path.join(sal, 'e%03d.png' % n), traer=False); n += 1
        print('%.0f gs=%d' % (time.time()-t0, gs), (r or {}).get('media_rgb'), flush=True)
    time.sleep(0.4)
