p = r'D:\Original\wv-32\source\game_sa\Crt\CrtLocale.cpp'
s = open(p, encoding='utf-8').read()
a = '''                    const int written = WideCharToMultiByte(dstCodePage, 0, static_cast<LPCWSTR>(wbuf), wide, result, n, nullptr, nullptr);'''
b = '''                    // El EXE pasa aqui n (el tamaño en bytes) como numero de caracteres anchos, no wide. Con n > wide la conversion lee mas alla del bufer y
                    // siempre falla (cada caracter de mas necesita al menos un byte y el destino mide justo n): se resuelve sin leer fuera del bufer.
                    const int written = n > wide ? 0 : WideCharToMultiByte(dstCodePage, 0, static_cast<LPCWSTR>(wbuf), n, result, n, nullptr, nullptr);'''
assert a in s
s = s.replace(a, b)
open(p, 'w', encoding='utf-8', newline='').write(s)
print('ok')
