p='contrib/inventory_implementations.py'
s=open(p,encoding='utf-8').read()
old="""            else:
                entry.update(function='', definition_line='')
                unresolved.append(entry)
            occurrences.append(entry)
"""
new="""            else:
                # Assembly inside a #define belongs to every definition of this file that expands the macro (one count per expansion).
                macro = next((m for m in DEFINE.finditer(lexical) if m.start() <= match.start() < m.end()), None)
                users = [f for f in definitions if macro and re.search(r'\b' + re.escape(macro[1]) + r'\b', f['body'])]
                if users:
                    for user in users:
                        user['naked_declarations' if kind == 'naked' else 'asm_occurrences'] += 1
                        occurrences.append(dict(entry, function=user['function'], definition_line=user['line']))
                    continue
                entry.update(function='', definition_line='')
                unresolved.append(entry)
            occurrences.append(entry)
"""
assert s.count(old)==1
s=s.replace(old,new)
old2="# NOTSA file with real inline assembly in macros (outside any definition); its hooks are not part of hooks.csv.\nSKIPPED_FILES = {'source/app/platform/win/WindowedMode.cpp'}\n"
new2="# Nothing is hidden: NOTSA files with assembly (WindowedMode.cpp) are scanned and reported, flagged as NOTSA exceptions.\nSKIPPED_FILES = set()\n# Preprocessor macro (with backslash continuations): its assembly is attributed to the definitions that expand it.\nDEFINE = re.compile(r'^[ \t]*#[ \t]*define[ \t]+(\w+)(?:[^\n]*\\\n)*[^\n]*', re.M)\n"
assert old2 in s
s=s.replace(old2,new2)
open(p,'w',encoding='utf-8').write(s)
