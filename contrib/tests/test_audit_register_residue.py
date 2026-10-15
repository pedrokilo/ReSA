"""Comprueba el detector de contrato de registros de contrib/audit_register_residue.py con desensamblados de ejemplo."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import audit_register_residue as audit


def disassembly(*functions):
    """Cada función se escribe como lista de instrucciones; las direcciones se numeran desde 00000000."""
    lines = []
    for function in functions:
        for index, text in enumerate(function):
            lines.append(f'  {index * 3:08X}: {text}')
        lines.append('')
    return lines


CALL = 'call        ?ResidueRegsCpp@@YIIIII@Z'
GET = 'call        ?Get@ResidueEcx@@QAEII@Z'
ENTRY = 'call        ?EntryEaxCpp@@YIIXZ'


class RegisterResidueAuditTest(unittest.TestCase):
    def run_audit(self, *functions):
        return audit.audit(audit.split_functions(disassembly(*functions)))

    def test_epilogue_after_the_helper_call_is_accepted(self):
        checked, bad = self.run_audit(['push        ebp', 'mov         ebp,esp', CALL, 'pop         edi', 'pop         esi', 'pop         ebx',
                                       'mov         esp,ebp', 'pop         ebp', 'ret         4'])
        self.assertEqual((checked, bad), (1, []))

    def test_early_exit_follows_the_jump_to_the_epilogue(self):
        # Como FindGarageForObjectRegs: la llamada de salida anticipada salta al epílogo; el cuerpo del bucle va entre medias.
        function = ['push        ebp', 'mov         ebp,esp', GET, 'jmp         0000000F', 'mov         ecx,eax', 'call        foo',
                    'pop         edi', 'pop         ebp', 'ret']
        function[3] = 'jmp         00000018'
        checked, bad = self.run_audit(function)
        self.assertEqual((checked, bad), (1, []))

    def test_runtime_check_after_the_call_is_rejected(self):
        checked, bad = self.run_audit(['push        ebp', CALL, 'cmp         ebp,esp', 'call        __RTC_CheckEsp', 'pop         ebp', 'ret'])
        self.assertEqual(checked, 1)
        self.assertEqual([text for _, text in bad], ['cmp         ebp,esp'.replace('         ', ' ')])

    def test_pop_ecx_after_the_call_is_rejected(self):
        # El epílogo de __try/__finally del compilador restablece fs:[0] y hace pop ecx.
        checked, bad = self.run_audit(['push        ebp', CALL, 'mov         ecx,dword ptr [ebp-10h]', 'pop         ecx', 'ret'])
        self.assertEqual(checked, 1)
        self.assertEqual(len(bad), 1)

    def test_code_without_helper_calls_is_ignored(self):
        checked, bad = self.run_audit(['push        ebp', 'mov         ecx,eax', 'pop         ebp', 'ret'])
        self.assertEqual((checked, bad), (0, []))

    def test_entry_eax_read_as_first_instruction_is_accepted(self):
        checked, bad = self.run_audit(['push        ebp', 'mov         ebp,esp', 'sub         esp,8', 'mov         dword ptr [ebp-4],ecx', ENTRY, 'pop         ebp', 'ret'])
        self.assertEqual((checked, bad), (1, []))

    def test_write_to_eax_before_entry_read_is_rejected(self):
        checked, bad = self.run_audit(['push        ebp', 'mov         ebp,esp', 'mov         eax,dword ptr [ebp+8]', ENTRY, 'pop         ebp', 'ret'])
        self.assertEqual(checked, 1)
        self.assertEqual(len(bad), 1)

    def test_runtime_check_prologue_before_entry_read_is_rejected(self):
        checked, bad = self.run_audit(['push        ebp', 'mov         ebp,esp', 'mov         eax,0CCCCCCCCh', ENTRY, 'pop         ebp', 'ret'])
        self.assertEqual(len(bad), 1)

    def test_call_before_entry_read_is_rejected(self):
        checked, bad = self.run_audit(['push        ebp', 'call        foo', ENTRY, 'pop         ebp', 'ret'])
        self.assertEqual(len(bad), 1)

    def run_every_path(self, *functions):
        return audit.audit(audit.split_functions(disassembly(*functions)), every_path=True)

    def test_every_path_accepts_branches_that_all_reach_the_helper(self):
        checked, bad = self.run_every_path(['push        ebp', 'test        al,al', 'je          0000000F', 'mov         ecx,1', 'jmp         00000012', 'mov         ecx,2',
                                            CALL, 'pop         ebp', 'ret'])
        self.assertEqual(bad, [])

    def test_every_path_rejects_a_return_that_skips_the_helper(self):
        checked, bad = self.run_every_path(['push        ebp', 'test        al,al', 'jne         0000000F', 'pop         ebp', 'ret', 'mov         ecx,2', CALL, 'pop         ebp', 'ret'])
        self.assertEqual(len(bad), 1)
        self.assertIn('alcanzable', bad[0][1])

    # PedGroups: las envolturas reciben ECX/EDX de entrada como parametros y deben exigir al menos tres ayudantes de residuo.
    def test_jmc_prologue_call_is_rejected_when_entry_params_matter(self):
        function = ['push        ebp', 'mov         ebp,esp', 'call        @__CheckForDebuggerJustMyCode@4', CALL, 'pop         ebp', 'ret']
        checked, bad = audit.audit(audit.split_functions(disassembly(function)), entry_params=True)
        self.assertEqual(len(bad), 1)
        self.assertEqual(audit.audit(audit.split_functions(disassembly(function)))[1], [])  # sin la opcion no cambia el comportamiento previo

    def test_object_with_fewer_residue_calls_than_expected_is_rejected(self):
        function = ['push        ebp', 'mov         ebp,esp', CALL, 'pop         ebp', 'ret']
        checked, bad = audit.audit(audit.split_functions(disassembly(function, function)), entry_params=True, min_calls=3)
        self.assertEqual(len(bad), 1)
        checked, bad = audit.audit(audit.split_functions(disassembly(function, function, function)), entry_params=True, min_calls=3)
        self.assertEqual(bad, [])

    def test_pedgroups_is_audited(self):
        self.assertIn('PedGroups', audit.OBJECTS)
        self.assertEqual(audit.MIN_RESIDUE_CALLS['PedGroups'], 3)

    # Ped (CPed::GetLocalDirection, 0x5DEF60): thiscall con un argumento en la pila (ret 4); toda salida pasa por el ayudante y nada lo sigue salvo el epilogo.
    def test_ped_is_audited_on_every_path(self):
        self.assertIn('Ped', audit.OBJECTS)
        self.assertIn('Ped', audit.EVERY_PATH_OBJECTS)
        self.assertEqual(audit.MIN_RESIDUE_CALLS['Ped'], 1)

    def test_thiscall_function_with_stack_argument_is_accepted(self):
        function = ['push        ebp', 'mov         ebp,esp', 'sub         esp,18h', 'mov         dword ptr [ebp-4],ecx', 'cmp         eax,3', 'jle         00000015',
                    'mov         edx,0FFFFFFFCh', CALL, 'mov         esp,ebp', 'pop         ebp', 'ret         4']
        checked, bad = audit.audit(audit.split_functions(disassembly(function)), every_path=True, min_calls=1)
        self.assertEqual(bad, [])

    def test_ped_rejects_a_check_after_the_helper_and_a_return_that_skips_it(self):
        after = ['push        ebp', 'mov         ebp,esp', CALL, 'cmp         ebp,esp', 'call        __RTC_CheckEsp', 'pop         ebp', 'ret         4']
        self.assertEqual(len(audit.audit(audit.split_functions(disassembly(after)), every_path=True)[1]), 1)
        skip = ['push        ebp', 'mov         ebp,esp', 'test        al,al', 'je          00000015', CALL, 'pop         ebp', 'ret         4', 'pop         ebp', 'ret         4']
        bad = audit.audit(audit.split_functions(disassembly(skip)), every_path=True)[1]
        self.assertTrue(any('alcanzable' in text for _, text in bad))


if __name__ == '__main__':
    unittest.main()
