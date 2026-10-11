"""Modelo semántico offline de las llamadas CRT obligatorias.

No observador hardware, no reemplazo del analizador integral, no validación.
Entradas/argumentos crudos se contrastan contra identidades del propio lado.
"""
MALLOC, REALLOC, FREE = 0x824257, 0x824269, 0x82413F


def expected(function, size, alignment=0, input_pointer=0, allocation_base=0):
    if not 0 <= size <= 4096:
        raise ValueError('Size outside the initial safe campaign')
    if not 0 <= input_pointer <= 0xFFFFFFFF or not 0 <= allocation_base <= 0xFFFFFFFF:
        raise ValueError('Pointer is not x86')
    if function == 'Malloc':
        return [(MALLOC, [size])]
    if function == 'Realloc':
        result = [(REALLOC, [input_pointer, size])]
        if not input_pointer:
            result.append((MALLOC, [size]))
        elif not size:
            result.append((FREE, [input_pointer]))
        return result
    if function == 'MallocAlign':
        if alignment not in (4, 8, 16, 32, 64, 128, 256):
            raise ValueError('Alignment outside the initial safe campaign')
        return [(MALLOC, [size + alignment])]
    if function == 'FreeAlign':
        if not allocation_base or not input_pointer:
            raise ValueError('FreeAlign requires real aligned/base identities')
        if alignment not in (4, 8, 16, 32, 64, 128, 256):
            raise ValueError('Invalid alignment')
        if input_pointer % alignment or not allocation_base + 4 <= input_pointer <= allocation_base + alignment:
            raise ValueError('Invalid real aligned pointer/backpointer contract')
        return [(FREE, [allocation_base])]
    raise ValueError('Unknown target')


def validate(function, fixture, events, owner_thread, scope_serial):
    if owner_thread <= 0 or scope_serial <= 0:
        raise ValueError('Missing scope identity')
    sequence = expected(function, **fixture)
    if len(events) != len(sequence):
        raise ValueError('Missing/extra allocator event')
    normalized = []
    for index, (event, (entry, args)) in enumerate(zip(events, sequence)):
        if event.get('entry') != entry or event.get('eip') != entry:
            raise ValueError('Wrong actual CRT entry')
        if event.get('owner_thread') != owner_thread or event.get('scope_serial') != scope_serial:
            raise ValueError('Foreign thread/scope')
        if event.get('sequence') != index or event.get('args') != args:
            raise ValueError('Wrong order/physical allocator arguments')
        slot = {MALLOC: 0, REALLOC: 1, FREE: 2}[entry]
        dr6 = event.get('dr6', -1)
        if dr6 < 0 or dr6 & 0xF != 1 << slot or dr6 & 0xE000:
            raise ValueError('Missing/foreign hardware trigger')
        if event.get('stack_words_valid') is not True or event.get('context_preserved') is not True:
            raise ValueError('Unsafe read/context mutation')
        # This boolean must be established from raw caller/body/module evidence
        # by the complete runtime analyzer. The model cannot fabricate it.
        if event.get('caller_origin_valid') is not True:
            raise ValueError('Unproven caller origin')
        if entry == MALLOC:
            normalized.append(('malloc', args[0]))
        elif entry == REALLOC:
            normalized.append(('realloc', 'input' if args[0] else 'null', args[1]))
        else:
            normalized.append(('free', 'base' if function == 'FreeAlign' else 'input'))
    return normalized
