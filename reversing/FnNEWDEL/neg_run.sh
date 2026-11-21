#!/bin/sh
# Prueba negativa NEWDEL: aplica una mutacion de una linea a CrtHeap.cpp, recompila el banco y comprueba que detecta la diferencia.
ROOT=${ROOT:-/d/Original/wv-nd}
EXE="E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe"
WORK=${WORK:-/d/Original/_newdel_neg}
WORKW=${WORKW:-'D:\Original\_newdel_neg'}
rm -rf $WORK; mkdir -p $WORK
n=0; det=0
run() { # original, reemplazo
  n=$((n+1)); T=$WORK/m$n; mkdir -p $T/src; cp $ROOT/source/game_sa/Crt/*.cpp $ROOT/source/game_sa/Crt/*.h $ROOT/source/game_sa/Crt/*.inc $T/src/
  python - "$T/src/CrtHeap.cpp" "$1" "$2" <<'PY' || { echo "mutacion $n: no se pudo aplicar"; return; }
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8',newline='').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8',newline='').write(s.replace(a,b,1))
PY
  cmd //c "$(cygpath -w $ROOT/reversing/FnNEWDEL/build_bench.cmd) $WORKW\m$n $WORKW\m$n\src" > $T/build.txt 2>&1
  out=$($T/bench_newdel.exe "$EXE" --casos 20000 2>&1 | tail -4)
  if ! echo "$out" | grep -q "SIN DIFERENCIAS"; then det=$((det+1)); echo "mutacion $n DETECTADA: $1  ->  $2"; else echo "mutacion $n NO DETECTADA: $1  ->  $2"; fi
}
run 'NewHandlerPtr()           = handler;' 'NewHandlerPtr()           = nullptr;'
run '    return previous;' '    return handler;'
run 'return handler(size) ? 1 : 0;' 'return handler(size);'
run '    Free(block);
}

// 0x8213AE' '    (void)block;
}

// 0x8213AE'
run '    return NewHandlerPtr();' '    return nullptr;'
run '    return OperatorNew(size);' '    return Malloc(size);'
run '        if (CallNewHandler(size) == 0) {
            reinterpret_cast' '        if (CallNewHandler(size) != 0) {
            reinterpret_cast'
echo "NEGATIVA: $det de $n detectadas"
