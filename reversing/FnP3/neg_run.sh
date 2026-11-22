#!/bin/sh
# Prueba negativa P3: aplica una mutacion de una linea, recompila el banco y comprueba que detecta la diferencia.
# Uso: sh neg_run.sh <ruta del EXE>  (desde el worktree: reversing/FnP3)
EXE="${1:-D:\Original\_crt1a_work\gta_sa.exe}"
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
SRCDIR=$ROOT/source/game_sa
n=0
run() { # carpeta, archivo, original, reemplazo, filtro
  n=$((n+1)); T=/d/Original/_p3_neg$n; rm -rf $T; mkdir -p $T/src/Crt $T/src/RenderWare
  cp $SRCDIR/Crt/*.cpp $SRCDIR/Crt/*.h $SRCDIR/Crt/*.inc $T/src/Crt/; cp $SRCDIR/RenderWare/RwMathX87.* $T/src/RenderWare/
  python - "$T/src/$1/$2" "$3" "$4" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8').write(s.replace(a,b,1))
PY
  W=$(cygpath -w "$ROOT/reversing/FnP3/build_bench.cmd" 2>/dev/null || echo "$ROOT/reversing/FnP3/build_bench.cmd")
  cmd.exe //c "$W D:\Original\_p3_neg$n\out D:\Original\_p3_neg$n\src" >/dev/null 2>&1
  echo "== mutacion $n: $2: '$3' -> '$4'"
  $T/out/bench_p3.exe "$EXE" --casos 30000 --solo $5 2>&1 | grep -E "DIFERENCIA|RESULTADO" | head -2
  rm -rf $T
}
run RenderWare RwMathX87.cpp "float v = Add(Add(Mul(m[j], x), Mul(m[4 + j], y)), Mul(m[8 + j], z));
        if (translate)" "float v = Add(Mul(m[j], x), Add(Mul(m[4 + j], y), Mul(m[8 + j], z)));
        if (translate)" vec
run RenderWare RwMathX87.cpp "v = Add(v, m[12 + j]);
        }
        r[j] = v;" "v = Add(v, m[12 + j] + 0.0f * 0);
        }
        r[j] = v * 1.0000001f;" vec
run RenderWare RwMathX87.cpp "if (row == 3) {" "if (row == 2) {" mat
run RenderWare RwMathX87.cpp "return const_cast<RwV3d*>(pointIn);" "return pointOut;" vec
run RenderWare RwMathX87.cpp "uint32_t remaining = static_cast<uint32_t>(count);" "uint32_t remaining = static_cast<uint32_t>(count) - 1 + (count == 1);" vec
run RenderWare RwMathX87.cpp "value = (value & 0xFFFFFCFFu) | 0x3Fu;" "value = (value & 0xFFFFFDFFu) | 0x3Fu;" cw
run RenderWare RwMathX87.cpp "value = (value & 0xFFFFFCFFu) | 0x3Fu;" "value = (value & 0xFFFFFCFFu) | 0x3Eu;" cw
run RenderWare RwMathX87.cpp "SavedControlWordSlot() = value;" "SavedControlWordSlot() = value | 1;" cw
run Crt CrtMath.cpp "*ErrnoPtr() = 0x22;" "*ErrnoPtr() = 0x21;" err
run Crt CrtMath.cpp "case 0x0A6: return { MathPath::Generic, 3, \"exp10\" };" "case 0x0A6: return { MathPath::Generic, 2, \"exp10\" };" err
run Crt CrtMath.cpp "case 0x3EF: return { MathPath::DomainCopy, 1, \"modf\" };" "case 0x3EF: return { MathPath::Domain, 1, \"modf\" };" err
run Crt CrtMath.cpp "const uint64_t sum = static_cast<uint64_t>(fractionBits ^ 0x80000000u) + 0x7FFFFFFFull;" "const uint64_t sum = static_cast<uint64_t>(fractionBits) + 0x7FFFFFFFull;" ftol
run Crt CrtMath.cpp "if (low == 0 && (high & 0x7FFFFFFFu) == 0) {" "if (low == 0 && high == 0) {" ftol
run Crt CrtMath.cpp "r = static_cast<int64_t>(std::nearbyint(x));" "r = static_cast<int64_t>(std::floor(x));" ftol
