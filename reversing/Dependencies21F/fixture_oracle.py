"""Oraculo independiente de TidyUpD3DTextures (0x7305E0) y TidyUpD3DIndexBuffers (0x730740).

Repite, instruccion a instruccion, el desensamblado del EXE (ver *.dis.txt) con enteros de Python y 32 bits explicitos, a partir de la fixture registrada por el banco
(no comparte codigo con la reconstruccion C++ ni con el generador de fixtures de LiveComparison.hpp). Devuelve la traza esperada de Release (indices de objeto), el
estado final esperado de los contadores y las rutas recorridas.
"""
M = 0xFFFFFFFF
REGION = 0xC87C60
TEX_AT, TEX_SIZE = 0xC87C68 - REGION, 0x1C          # num en +0x10, size en +0x14
IB_AT, IB_SIZE = 0xC87E48 - REGION, 0x18            # num en +0x0C, size en +0x10; el buffer generico (0xC87FC8) es la ranura 16
FREE_TEX, FREE_IB, NUM_TEX = 0xC87FE0 - REGION, 0xC87FE4 - REGION, 0
REGION_SIZE = 0xC87FE8 - REGION
COUNTS = (0, 1, 2, 3, 4, 7, 100, 200, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFE, 5, 17, 33, 64)
NUM_EXTREMES = (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF)


def s32(v):
    v &= M
    return v - (1 << 32) if v & 0x80000000 else v


def tdiv_rem(a, b):
    """Resto de idiv (signo del dividendo)."""
    r = abs(a) % abs(b)
    return -r if a < 0 else r


class Slot:
    def __init__(self, raw):
        self.num, self.size, self.ids = raw[0], raw[1], list(raw[2])


class State:
    def __init__(self, fx):
        self.n = fx['n']
        self.free_tex = fx['free_tex']
        self.free_ib = fx['free_ib']
        self.tex = [Slot(s) for s in fx['tex']]
        self.ib = [Slot(s) for s in fx['ib']] + [Slot(fx['gen'])]          # ranura 16 = buffer generico
        self.mut = fx['mutation']
        self.release_no = 0
        self.trace = []
        self.mutation_applied = False

    def release(self, object_id):
        self.trace.append(object_id)
        self.release_no += 1
        if self.mut['at'] and self.release_no == self.mut['at']:
            self.mutation_applied = True
            if self.mut['n']:
                self.n = self.mut['n']
            if self.mut['tex_index'] >= 0:
                self.free_tex = self.mut['tex_index']
            if self.mut['ib_index'] >= 0:
                self.free_ib = self.mut['ib_index']


def simulate_textures(fx):
    st, paths = State(fx), set()
    esi = fx['count']
    if esi == 0:
        paths.add('zero_count')
        return st, paths
    edi, edx, ebx = 0, s32(st.free_tex), s32(st.n)
    if edx >= ebx:
        paths.add('index_not_below_count')
    while True:
        if edi >= ebx:                                   # jge 0x730652
            paths.add('scan_exhausted')
            break
        slot = st.tex[edx]                               # IndexError = fixture fuera del dominio probado
        ecx = slot.size
        released = False
        if ecx != 0:
            slot.num = (slot.num - 1) & M
            ecx = (ecx - 1) & M
            slot.size = ecx
            pointer = slot.ids[ecx]
            if pointer != -1:
                st.release(pointer)
                ebx, edx, edi = s32(st.n), s32(st.free_tex), 0
                esi = (esi - 1) & M
                released = True
                if st.mutation_applied and st.release_no == st.mut['at']:
                    paths.add('reload_after_release')
            else:
                paths.add('null_entry')
        else:
            paths.add('empty_buffer')
        if not released:
            edi += 1
        if ebx <= 0:
            raise RuntimeError('NumTextureBuffers <= 0 tras Release: fuera del dominio')
        new = tdiv_rem(s32(edx + 1), ebx)
        if new == 0 and s32(edx + 1) == ebx:
            paths.add('index_wrap')
        edx = new
        st.free_tex = edx & M
        if esi == 0:
            paths.add('count_reached')
            break
    return st, paths


def simulate_index_buffers(fx):
    st, paths = State(fx), set()
    esi = fx['count']
    if esi == 0:
        paths.add('zero_count')
        return st, paths
    gen = st.ib[16]
    while True:                                          # fase 1: buffer generico
        if gen.size == 0:
            paths.add('generic_empty')
            break
        gen.num = (gen.num - 1) & M
        gen.size = (gen.size - 1) & M
        pointer = gen.ids[gen.size]
        if pointer == -1:
            paths.add('generic_null_break')
            break
        st.release(pointer)
        paths.add('generic_release')
        esi = (esi - 1) & M
        if esi == 0:
            paths.add('count_reached_generic')
            return st, paths
    edx, edi = s32(st.free_ib), 0
    while True:                                          # fase 2: 16 buffers
        if edi >= 16:
            paths.add('scan_exhausted')
            break
        if edx == 16:
            paths.add('generic_used_as_slot')
        slot = st.ib[edx]
        ecx = slot.size
        released = False
        if ecx != 0:
            slot.num = (slot.num - 1) & M
            ecx = (ecx - 1) & M
            slot.size = ecx
            pointer = slot.ids[ecx]
            if pointer != -1:
                st.release(pointer)
                paths.add('slot_release')
                edx, edi = s32(st.free_ib), 0
                esi = (esi - 1) & M
                released = True
                if st.mutation_applied and st.release_no == st.mut['at']:
                    paths.add('reload_after_release')
            else:
                paths.add('null_entry')
        else:
            paths.add('empty_buffer')
        if not released:
            edi += 1
        edx = tdiv_rem(s32(edx + 1), 16)
        if edx == 0:
            paths.add('index_wrap')
        st.free_ib = edx & M
        if esi == 0:
            paths.add('count_reached_slot')
            break
    return st, paths


def word(region_hex, at):
    return int.from_bytes(bytes.fromhex(region_hex[at * 2:at * 2 + 8]), 'little')


def expected_words(st):
    """Palabras de la region que cambian o que el oraculo conoce, como {desplazamiento: valor}."""
    words = {NUM_TEX: st.n, FREE_TEX: st.free_tex, FREE_IB: st.free_ib}
    for i, slot in enumerate(st.tex):
        words[TEX_AT + i * TEX_SIZE + 0x10] = slot.num
        words[TEX_AT + i * TEX_SIZE + 0x14] = slot.size
    for i, slot in enumerate(st.ib):
        words[IB_AT + i * IB_SIZE + 0xC] = slot.num
        words[IB_AT + i * IB_SIZE + 0x10] = slot.size
    return words


def check(function, fx, observed):
    """Compara un lado observado con el oraculo; devuelve las rutas. observed = dict del lado (globals, trace)."""
    simulate = simulate_textures if function.endswith('Textures') else simulate_index_buffers
    st, paths = simulate(fx)
    if [t[0] for t in observed['trace']] != st.trace:
        raise RuntimeError('Traza de Release distinta de la del oraculo: %s != %s' % ([t[0] for t in observed['trace']][:12], st.trace[:12]))
    if observed['releases'] != len(st.trace):
        raise RuntimeError('Numero de Release distinto del oraculo')
    for at, value in expected_words(st).items():
        if word(observed['globals'], at) != value:
            raise RuntimeError('Palabra %#x de la region: %#x, esperado %#x' % (REGION + at, word(observed['globals'], at), value))
    return paths
