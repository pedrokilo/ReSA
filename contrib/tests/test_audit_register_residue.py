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


if __name__ == '__main__':
    unittest.main()
