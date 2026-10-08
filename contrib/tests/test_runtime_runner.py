"""Analyzer verdicts must survive the shared runner result merge."""
import importlib.util
from pathlib import Path
import unittest

spec = importlib.util.spec_from_file_location('runtime_runner', Path(__file__).resolve().parents[2] / 'reversing/runtime_runner.py')
runner = importlib.util.module_from_spec(spec)
spec.loader.exec_module(runner)

class AnalysisVerdictTests(unittest.TestCase):
    def test_explicit_failure_preserves_differences_and_natural_exit(self):
        result = {'passed': False, 'natural_exit': True, 'exit_code': 0}
        runner.merge_analysis_result(result, {'passed': False, 'differences': 590, 'pairs': 201552})
        self.assertIs(result['passed'], False)
        self.assertEqual(result['differences'], 590)
        self.assertEqual(result['pairs'], 201552)
        self.assertTrue(result['natural_exit'])
        self.assertEqual(result['exit_code'], 0)

    def test_explicit_success_is_approved(self):
        result = {'passed': False}
        runner.merge_analysis_result(result, {'passed': True, 'differences': 0})
        self.assertIs(result['passed'], True)
        self.assertEqual(result['differences'], 0)

    def test_legacy_analyzer_without_verdict_is_approved(self):
        result = {'passed': False}
        runner.merge_analysis_result(result, {'pairs': 128})
        self.assertIs(result['passed'], True)

    def test_non_boolean_verdict_is_not_approval(self):
        result = {}
        runner.merge_analysis_result(result, {'passed': 'false'})
        self.assertIs(result['passed'], False)

if __name__ == '__main__':
    unittest.main()
