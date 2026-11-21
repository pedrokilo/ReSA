# Genera prueba_humo.py a partir de FnBIS1/prueba_ropero.py (que se deja intacto): prueba la DLL de ESTE arbol (jugar.ps1 -Variante X -AsiPath) y no lanza nada si el juego del usuario sigue activo tras 900 s.
s = open(r'D:\Original\wv-8\reversing\FnBIS1\prueba_ropero.py', encoding='utf-8').read()
s = s.replace("aqui=os.path.dirname(os.path.abspath(__file__))",
              "aqui=r'D:\\Original\\wv-8\\reversing\\FnBIS1'\nsys.path.insert(0,aqui)\nASI=r'D:\\Original\\wv-31\\bin\\Debug\\gta_reversed.asi'")
assert "'-Variante','A'," in s
s = s.replace("'-Variante','A',", "'-Variante','X','-AsiPath',ASI,")
old = "while (pid() or os.path.exists('D:/Original/gta_lock') or os.path.exists(os.path.join(aqui,'respaldo_pendiente'))) and time.time()-t<900: time.sleep(3)"
assert old in s
s = s.replace(old, old + "\n    if pid() or os.path.exists('D:/Original/gta_lock') or os.path.exists(os.path.join(aqui,'respaldo_pendiente')): print('NO LANZADO: el juego del usuario sigue activo tras 900 s'); return 4")
head0 = "import sys; sys.path.insert(0, r'D:\\Original\\wv-8\\reversing\\FnBIS1')\n"
head = "# Copia de FnBIS1/prueba_ropero.py (mk_humo.py) que prueba la DLL de ESTE arbol y no lanza nada si el juego del usuario sigue activo tras 900 s.\n"
open(r'D:\Original\wv-31\reversing\FnS31AFILE\prueba_humo.py', 'w', encoding='utf-8').write(head + head0 + s)
