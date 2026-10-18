p = 'D:/Original/gta-reversed-master/contrib/tests/test_inventory_implementations.py'
s = open(p, encoding='utf-8').read()
new = '''class TemplateAndParserLinkTest(unittest.TestCase):
    def definitions(self, source):
        raw = source.encode()
        masked = inventory.sanitized(raw)
        tree = Parser(Language(tree_sitter_cpp.language())).parse(masked)
        return raw, tree, [n for n in inventory.walk(tree.root_node) if n.type in {'function_definition', 'field_declaration'}]

    def fn(self, name, signature, file='source/game_sa/X.h', templated=False, header='', **extra):
        return dict({'function': name, 'signature': signature, 'file': file, 'catalog_match': None,
                     'templated': templated, 'template_header': header}, **extra)

    def cat(self, cls, name, address, catalog='hooks.csv'):
        return {'class': cls, 'fn_name': name, 'address': address, 'reversed': '1', 'validated': '1', 'catalog': catalog}

    def link(self, functions, catalog, source, bases=None, aliases=None, file='source/game_sa/X.cpp'):
        regs = inventory.parse_registrations(source, file)
        inventory.link_by_registration(functions, catalog, regs, set(), {file}, bases, aliases)

    def test_strip_templates(self):
        self.assertEqual(inventory.strip_templates('CWorld::F<CPtrListDoubleLink<CPhysical*>>'), 'CWorld::F')
        self.assertEqual(inventory.strip_templates('CQuadTreeNode<T>::Get'), 'CQuadTreeNode::Get')
        self.assertEqual(inventory.strip_templates('operator<'), 'operator<')

    def test_instantiation_in_name_links_to_the_unique_generic_template_body(self):
        generic = self.fn('CWorld::Sector', 'CWorld::Sector(PtrListType& list)', templated=True, header='<typename PtrListType>')
        plain = self.fn('CWorld::Sector', 'CWorld::Sector()')
        catalog = [self.cat('CWorld', 'Sector<CPtrListDoubleLink<CPhysical*>>-', '0x00563950')]
        source = (b'void W::InjectHooks() { RH_ScopedClass(CWorld);' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Sector<CPtrListDoubleLink<CPhysical*>>, "", 0x563950, void(*)(CPtrListDoubleLink<CPhysical*>&)); }')
        self.link([generic, plain], catalog, source)
        self.assertIs(generic['catalog_match'], catalog[0])
        self.assertIsNone(plain['catalog_match'])

    def test_template_constraint_selects_between_overloads(self):
        integral = self.fn('U::Get', 'U::Get(T min, T max)', templated=True, header='<std::integral T>')
        floating = self.fn('U::Get', 'U::Get(T min, T max)', templated=True, header='<typename T> requires std::is_floating_point_v<T>')
        catalog = [self.cat('U', 'Get<int32>-int', '0x4D9C10'), self.cat('U', 'Get<float>-float', '0x4D9C50')]
        source = (b'void U::InjectHooks() { RH_ScopedClass(U);' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Get<int32>, "int", 0x4d9c10, int32(*)(int32, int32));' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Get<float>, "float", 0x4d9c50, float(*)(float, float)); }')
        self.link([integral, floating], catalog, source)
        self.assertIs(integral['catalog_match'], catalog[0])
        self.assertIs(floating['catalog_match'], catalog[1])

    def test_template_parameter_wildcard_and_typedef_alias_in_casts(self):
        wide = self.fn('S::Run', 'S::Run(L& list, float a, int32 flags)', templated=True, header='<typename L>')
        narrow = self.fn('S::Run', 'S::Run(L& list, int32 flags)', templated=True, header='<typename L>')
        catalog = [self.cat('S', 'Run<Single<E*>>-', '0x40C270')]
        source = (b'void S::InjectHooks() { RH_ScopedClass(S);' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Run<Single<E*>>, "", 0x40C270, void(*)(Single<E*>&, float, int32)); }')
        self.link([wide, narrow], catalog, source)
        self.assertIs(wide['catalog_match'], catalog[0])
        self.assertIsNone(narrow['catalog_match'])
        a = self.fn('C::Set', 'C::Set(RwIm2DVertex* v, const CRect& r)')
        b = self.fn('C::Set', 'C::Set(const CRect& r)')
        catalog = [self.cat('C', 'Set-Vertex', '0x727A00')]
        source = (b'void C::InjectHooks() { RH_ScopedClass(C);' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Set, "Vertex", 0x727A00, void(*)(RwD3D9Vertex*, const CRect&)); }')
        self.link([a, b], catalog, source, aliases={'RwIm2DVertex': 'RwD3D9Vertex'})
        self.assertIs(a['catalog_match'], catalog[0])

    def test_inherited_template_member_serves_every_derived_entry_through_the_shared_file(self):
        body = self.fn('Seek::Clone', 'Clone() const override', templated=True, header='<typename T>')
        catalog = [self.cat('Seek<A>', 'Clone', '0x00000000'), self.cat('SeekStd', 'Clone', '0x00000000')]
        source = (b'void Seek::InjectHooks() { RH_ScopedVirtualClass(Seek<A>, 0x1, 1); RH_ScopedVMTInstall(Clone, 0x10); }' + bytes([10]) +
                  b'void SeekStd::InjectHooks() { RH_ScopedVirtualClass(SeekStd, 0x2, 1); RH_ScopedVMTInstall(Clone, 0x20); }')
        self.link([body], catalog, source, bases={'SeekStd': ('Seek',)})
        self.assertIs(body['catalog_match'], catalog[0])
        self.assertEqual([c for c, _ in body['shared']], [catalog[1]])
        self.assertEqual(body['shared'][0][1], 'plantilla heredada')

    def test_shared_body_requires_a_template(self):
        body = self.fn('Plain::Clone', 'Clone() const override')
        catalog = [self.cat('Plain', 'Clone', '0x00000000'), self.cat('Kid', 'Clone', '0x00000000')]
        source = (b'void P::InjectHooks() { RH_ScopedVirtualClass(Plain, 0x1, 1); RH_ScopedVMTInstall(Clone, 0x10); }' + bytes([10]) +
                  b'void K::InjectHooks() { RH_ScopedVirtualClass(Kid, 0x2, 1); RH_ScopedVMTInstall(Clone, 0x20); }')
        self.link([body], catalog, source, bases={'Kid': ('Plain',)})
        self.assertIsNone(body['catalog_match'])
        self.assertNotIn('shared', body)

    def test_catalog_entry_without_address_accepts_its_single_registration(self):
        body = self.fn('T::Cb', 'T::Cb(int a)', 'source/game_sa/T.cpp')
        catalog = [self.cat('T', 'Cb', '0x00000000')]
        self.link([body], catalog, b'void T::InjectHooks() { RH_ScopedClass(T); RH_ScopedInstall(Cb, 0x675FC0); }')
        self.assertIs(body['catalog_match'], catalog[0])

    def test_parser_workarounds_keep_definitions_visible(self):
        # Casts inside registrations used to swallow every following definition.
        source = ('void P::InjectHooks() { RH_ScopedOverloadedInstall(Get, "ped", 0x1, int16 (CPad::*)(CPed*) const); }\\n'
                  'void P::Clear() { x = 1; }\\n'
                  'C2dEffect* __stdcall P::Compute(CPed const& ped) { return nullptr; }\\n'
                  'void* P::Plugin(void* o) { *RWPLUGINOFFSET(Data*, o, off) = &fake; return o; }\\n'
                  'class NOTSA_EXPORT_VTABLE Base { public: virtual void Update(int a) { /* empty */ }; };\\n')
        raw, tree, nodes = self.definitions(source)
        names = {inventory.declarator_name(n.child_by_field_name('declarator'), raw) for n in nodes if n.child_by_field_name('declarator')}
        for expected in ('P::InjectHooks', 'P::Clear', 'P::Compute', 'P::Plugin', 'Update'):
            self.assertIn(expected, names)
        update = [n for n in nodes if n.type == 'field_declaration'][0]
        self.assertEqual(inventory.owner_name(update, raw, 'Update', 'x.h')[1], 'Base::Update')


'''
s = s.replace("\nif __name__ == '__main__':", "\n" + new + "if __name__ == '__main__':", 1)
open(p, 'w', encoding='utf-8').write(s)
