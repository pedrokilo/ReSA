p = r'D:\Original\wv-32\reversing\FnCRT4A\bench_locale.cpp'
s = open(p, encoding='latin-1').read()
a = 'hex(r1, strlen(r1) + 1)'
b = 'hex(r1, HeapSize(GetProcessHeap(), 0, r1))'
assert a in s
s = s.replace(a, b)
a = 'hex(r2, strlen(r2) + 1)'
b = 'hex(r2, HeapSize(GetProcessHeap(), 0, r2))'
assert a in s
s = s.replace(a, b)
open(p, 'w', encoding='latin-1', newline='').write(s)
print('ok')
