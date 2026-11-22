# Ronda 4: direcciones con gancho segun el CODIGO FUENTE (RH_Scoped*Install...), no segun docs/hooks.csv (que va retrasado).
# Escribe docs/CodigoVivoExe/fase8-experimento/ganchos-fuente.csv (analizar.py de FnIND0A lo suma a H si existe).
# Uso: python -I ganchos_fuente.py [raiz del repo]
import re, sys, pathlib
raiz = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else '.')
rx_call = re.compile(r'RH_Scoped\w*\(([^;]*)')
rx_addr = re.compile(r'0x([0-9A-Fa-f]{6})\b')
dirs = set()
for p in list((raiz / 'source').rglob('*.cpp')) + list((raiz / 'source').rglob('*.h')) + list((raiz / 'source').rglob('*.hpp')):
    t = p.read_text(encoding='utf-8', errors='replace')
    for m in rx_call.finditer(t):
        for a in rx_addr.findall(m.group(1)):
            v = int(a, 16)
            if 0x401000 <= v < 0xCB1000: dirs.add(v)
out = raiz / 'docs/CodigoVivoExe/fase8-experimento/ganchos-fuente.csv'
out.write_text('address\n' + ''.join('0x%08X\n' % v for v in sorted(dirs)), encoding='utf-8')
print(len(dirs), 'direcciones ->', out)
