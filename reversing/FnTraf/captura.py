# Captura la ventana del juego (cliente) por GDI y mide pixeles rojos del marcador (sin HUD ni radar). Sin dependencias externas.
import ctypes, zlib, struct, sys, json
from ctypes import wintypes
u=ctypes.WinDLL('user32',use_last_error=True); g=ctypes.WinDLL('gdi32',use_last_error=True)
WNDENUMPROC=ctypes.WINFUNCTYPE(wintypes.BOOL,wintypes.HWND,wintypes.LPARAM)
def buscar():
    r=[]
    def cb(h,l):
        c=ctypes.create_unicode_buffer(128); u.GetClassNameW(h,c,128)
        if c.value=='Grand theft auto San Andreas' and u.IsWindowVisible(h): r.append(h)
        return True
    u.EnumWindows(WNDENUMPROC(cb),0); return r[0] if r else None
class BIH(ctypes.Structure):
    _fields_=[('biSize',wintypes.DWORD),('biWidth',wintypes.LONG),('biHeight',wintypes.LONG),('biPlanes',wintypes.WORD),('biBitCount',wintypes.WORD),('biCompression',wintypes.DWORD),('biSizeImage',wintypes.DWORD),('biX',wintypes.LONG),('biY',wintypes.LONG),('biClrUsed',wintypes.DWORD),('biClrImportant',wintypes.DWORD)]
def capturar(ruta=None, traer=True):
    h=buscar()
    if not h: return None
    if traer:
        u.ShowWindow(h,9); u.SetForegroundWindow(h)
    rc=wintypes.RECT(); u.GetClientRect(h,ctypes.byref(rc)); pt=wintypes.POINT(0,0); u.ClientToScreen(h,ctypes.byref(pt))
    w,hh=rc.right-rc.left,rc.bottom-rc.top
    sdc=u.GetDC(0); mdc=g.CreateCompatibleDC(sdc); bm=g.CreateCompatibleBitmap(sdc,w,hh); g.SelectObject(mdc,bm)
    if not u.PrintWindow(h,mdc,3): g.BitBlt(mdc,0,0,w,hh,sdc,pt.x,pt.y,0x00CC0020)
    bi=BIH(ctypes.sizeof(BIH),w,-hh,1,32,0,0,0,0,0,0); buf=ctypes.create_string_buffer(w*hh*4)
    g.GetDIBits(mdc,bm,0,hh,buf,ctypes.byref(bi),0)
    g.DeleteObject(bm); g.DeleteDC(mdc); u.ReleaseDC(0,sdc)
    data=buf.raw
    if ruta:
        raw=b''.join(b'\x00'+bytes(sum(([data[i+2],data[i+1],data[i]] for i in range(y*w*4,(y+1)*w*4,4)),[])) for y in range(hh))
        def ch(t,d): return struct.pack('>I',len(d))+t+d+struct.pack('>I',zlib.crc32(t+d)&0xffffffff)
        open(ruta,'wb').write(b'\x89PNG\r\n\x1a\n'+ch(b'IHDR',struct.pack('>IIBBBBB',w,hh,8,2,0,0,0))+ch(b'IDAT',zlib.compress(raw,6))+ch(b'IEND',b''))
    x1=int(w*0.72); y0=int(hh*0.18); y1=int(hh*0.80); rojo=tot=0
    for y in range(y0,y1,2):
        o=y*w*4
        for x in range(0,x1,2):
            b,gg,r=data[o+x*4],data[o+x*4+1],data[o+x*4+2]; tot+=1
            if r>140 and gg<70 and b<70: rojo+=1
    sr=sg=sb=n=0; mx=0
    for y in range(0,hh,4):
        o=y*w*4
        for x in range(0,w,4):
            b,gg,r=data[o+x*4],data[o+x*4+1],data[o+x*4+2]; sr+=r; sg+=gg; sb+=b; n+=1; mx=max(mx,r,gg,b)
    return {'ancho':w,'alto':hh,'rojo':rojo,'muestras':tot,'media_rgb':[round(sr/n,1),round(sg/n,1),round(sb/n,1)],'max':mx}
if __name__=='__main__':
    print(json.dumps(capturar(sys.argv[1] if len(sys.argv)>1 else None)))
