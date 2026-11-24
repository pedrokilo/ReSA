# Genera el .rsp para enlazar la ASI a mano: los .obj del ASI Debug de wv-int (lista de link.command.1.tlog) con los .obj de reversing\FnFase8\over\ (copias recompiladas de este arbol) por encima.
import os, sys
tlog = r'D:\Original\wv-int\build\source\gta_reversed.dir\Debug\gta_reversed.tlog\link.command.1.tlog'
b = open(tlog, 'rb').read(); s = b.decode('utf-16' if b[:2] == b'\xff\xfe' else 'utf-8', 'replace')
objs = [o.strip() for o in s.split('\n')[0].lstrip('^').split('|') if o.strip()]
over = r'D:\Original\wv-datos3\reversing\FnFase8\over'
base = r'D:\Original\wv-int\build\source\gta_reversed.dir\Debug'
real = {f.lower(): f for f in os.listdir(base)}
out = sys.argv[1]; n_over = 0; L = []
for o in objs:
    nm = os.path.basename(o); nm = real.get(nm.lower(), nm)
    p_over = os.path.join(over, nm)
    if os.path.exists(p_over) and nm.lower() != 'cmake_pch.obj': L.append('"%s"' % p_over); n_over += 1
    else: L.append('"%s"' % os.path.join(base, nm))
conan = r'D:\Original\wv-int\reversing\CVehicle\conan-cache\p\b'
libs = [r'ogg21219f4658423\p\lib\ogg.lib', r'spdlo8029646db848c\p\lib\spdlogd.lib', r'tracy2fefc9668738f\p\lib\TracyClient.lib', r'imgui5396f0f7f8179\p\lib\imgui.lib']
L += ['"' + conan + chr(92) + x + '"' for x in libs]
L += ['ddraw.lib', 'Winmm.lib', 'dxguid.lib', 'strmiids.lib', 'd3d9.lib', '"' + conan + r'\sdlc3cf2c66533bd\p\lib\SDL3-static.lib"']
L += [r'"D:\Original\wv-int\build\libs\zlib-1.1.3\Debug\zlib113.lib"', r'"D:\Original\wv-int\build\libs\libpng-1.0.12\Debug\libpng1012.lib"', r'"D:\Original\wv-int\build\libs\libjpeg-6b\Debug\libjpeg6b.lib"', 'dsound.lib', 'dbghelp.lib', 'ws2_32.lib']
L += ['"' + conan + r'\vorbi9990e9d508d57\p\lib' + chr(92) + x + '"' for x in ('vorbisenc.lib', 'vorbisfile.lib', 'vorbis.lib')]
L += ['"' + conan + r'\libjpbe5e6c22fbdf6\p\lib' + chr(92) + x + '"' for x in ('jpeg-static.lib', 'turbojpeg-static.lib')]
L += 'opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib imm32.lib ole32.lib oleaut32.lib version.lib uuid.lib advapi32.lib setupapi.lib shell32.lib comdlg32.lib winspool.lib'.split()
open(out, 'w').write('\n'.join(L) + '\n'); print(len(objs), 'objs;', n_over, 'sustituidos por over')
