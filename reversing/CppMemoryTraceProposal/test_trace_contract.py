"""Offline negative controls for the proposed trace model; never launches GTA."""
import copy
from trace_contract import expected, validate, MALLOC, REALLOC, FREE


def make_events(sequence):
    return [dict(entry=entry, eip=entry, args=args, owner_thread=37, scope_serial=1,
                 sequence=index, dr6=1 << {MALLOC: 0, REALLOC: 1, FREE: 2}[entry],
                 stack_words_valid=True, context_preserved=True, caller_origin_valid=True)
            for index, (entry, args) in enumerate(sequence)]


def reject(function, fixture, events):
    try:
        validate(function, fixture, events, 37, 1)
    except ValueError:
        return
    raise AssertionError('A negative trace control was accepted')


def main():
    tested = 0
    rejected = 0
    fixtures = [
        ('Malloc', dict(size=0)), ('Malloc', dict(size=4096)),
        ('Realloc', dict(size=0)), ('Realloc', dict(size=17)),
        ('Realloc', dict(size=0, input_pointer=0x1020)),
        ('Realloc', dict(size=65, input_pointer=0x1020)),
        ('MallocAlign', dict(size=0, alignment=4)),
        ('MallocAlign', dict(size=4096, alignment=256)),
        ('FreeAlign', dict(size=0, alignment=16, input_pointer=0x1030, allocation_base=0x1021)),
        ('FreeAlign', dict(size=4096, alignment=256, input_pointer=0x1100, allocation_base=0x10FF - 3)),
    ]
    for function, fixture in fixtures:
        events = make_events(expected(function, **fixture))
        validate(function, fixture, events, 37, 1)
        tested += 1
        reject(function, fixture, events[:-1]); rejected += 1
        reject(function, fixture, events + events[-1:]); rejected += 1
        for field, replacement in [('entry', 0x400000), ('eip', 0x400000),
                                   ('owner_thread', 38), ('scope_serial', 2),
                                   ('sequence', 17), ('dr6', 0x4000),
                                   ('stack_words_valid', False), ('context_preserved', False),
                                   ('caller_origin_valid', False)]:
            broken = copy.deepcopy(events)
            broken[0][field] = replacement
            reject(function, fixture, broken); rejected += 1
        for index in range(len(events)):
            for argument in range(len(events[index]['args'])):
                broken = copy.deepcopy(events)
                broken[index]['args'][argument] += 1
                reject(function, fixture, broken); rejected += 1
    print(f'{tested} valid semantic fixtures; {rejected} negative trace controls rejected; in-game validation: false')


if __name__ == '__main__':
    main()
