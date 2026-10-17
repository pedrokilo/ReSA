import glob
parts = sorted(glob.glob('parts/p*.inc'))
out = []
for p in parts:
    out.append(open(p, 'rb').read().decode().replace('\r\n', '\n'))
open('flc_vehicle.inc', 'wb').write(''.join(out).encode())
print(parts)
