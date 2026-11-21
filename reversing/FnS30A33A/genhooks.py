# Regenera el bloque de ganchos (RpWorldInjectHooksS30A31A) al final de source/game_sa/RenderWare/rw/rpworld_s30a31a.cpp a partir de los comentarios `// 0xDIR (lote S3xA ...` de cada funcion.
import re,sys
p='source/game_sa/RenderWare/rw/rpworld_s30a31a.cpp'
s=open(p,encoding='utf-8').read()
m=s.find('// Lotes S30A y S31A (sin validar): ganchos')
if m>=0: s=s[:m]
res=re.findall(r'// 0x([0-9A-F]{6}) \(lote (S3[01]A)[^\n]*\n(?://[^\n]*\n)*?(?:static )?[\w\*:<> ]+?\s+\*?(\w+)\(',s)
res=sorted(set(res),key=lambda r:r[0])
out=['// Lotes S30A y S31A (sin validar): ganchos de las raices de RpWorld reconstruidas en este archivo.\nvoid RpWorldInjectHooksS30A31A() {\n    RH_ScopedCategory("Plugins");\n    RH_ScopedNamespaceName("RpWorld");\n']
for a,l,n in res: out.append(f'    RH_ScopedGlobalInstall({n}, 0x{a});'+' '*max(1,56-len(n))+f'// lote {l} (sin validar)')
out.append('}\n')
open(p,'w',encoding='utf-8').write(s+'\n'.join(out))
print(len(res),'ganchos')
