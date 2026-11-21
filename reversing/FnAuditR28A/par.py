# Muestra lado a lado el cuerpo C++ y el desensamblado del EXE de una funcion: python par.py Nombre [Nombre...]
import sys, re
import triage_lib as T
for n in sys.argv[1:]:
    a = T.name2addr.get(n)
    b = T.body_of(n)
    print('#' * 20, n, '%08X' % a if a else '?')
    print(b if b else 'SIN CUERPO')
    if a:
        T.X.dis(a, T.X.SIZES.get(a) or None)
