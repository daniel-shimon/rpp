from collections import namedtuple
from dataclasses import dataclass
from glob import glob
from importlib import import_module
from os import chdir, name
from os.path import abspath, dirname, join
from subprocess import Popen, PIPE
from typing import List, Union, Callable

if name == 'nt':
    rpp = r'cmake-build-debug\rpp.exe'
else:
    rpp = '/build/rpp'

bench_metrics = ['time', 'instructions', 'cycles', 'cache_misses', 'branch_misses']
BenchResult = namedtuple('BenchResult', bench_metrics)
chdir(dirname(dirname(abspath(__file__))))


@dataclass
class Test:
    code: str
    expected: Union[List[str], str]
    input: str = ''
    return_code: int = 0
    timeout: int = 1
    validation_func: Callable = None
    is_path: bool = False

    def run(self):
        self.code = self.code.replace('"', "'")
        out, err, _return_code = run(self.code, self.input, self.timeout, self.is_path)
        expected = self.expected if isinstance(self.expected, list) else [self.expected]
        if not err and _return_code == self.return_code and all(exp in out for exp in expected):
            if not self.validation_func or self.validation_func(out):
                return True
        print('return code', _return_code)
        print('output', repr(out))
        return False

    def bench(self, reps) -> BenchResult:
        self.code = self.code.replace('"', "'")
        p = Popen(list('perf stat -e instructions:u -e cycles:u -e cache-misses:u -e branch-misses:u -r'.split()) +
                  [str(reps), rpp] + (['-c'] if not self.is_path else []) + [self.code],
                  stdin=PIPE, stdout=PIPE, stderr=PIPE, universal_newlines=True)
        _, out = p.communicate(self.input * reps, self.timeout * reps)
        lines = out.split('\n')

        def value(i): return lines[-i].strip().split()[0]

        try:
            return BenchResult(float(value(3)), int(value(8)), int(value(7)), int(value(6)), int(value(5)))
        except (ValueError, IndexError) as e:
            print('bad perf stat output:')
            print('\n'.join(lines[-10:]))
            raise e


def run(code, inputs, timeout, is_path):
    if is_path:
        cmd = [rpp, code]
    else:
        cmd = [rpp, '-c', code]
    p = Popen(cmd, stdin=PIPE, stdout=PIPE,
              universal_newlines=True)
    out, err = p.communicate(inputs, timeout)
    p.wait(timeout)
    return out, err, p.returncode


def run_cmd(code):
    return rpp + ' -c "%s"' % code.replace('"', "'")


def main(test: Test):
    if test.run():
        print('Passed')
        return True
    else:
        print('Failed :(')
        return False


def get_tests():
    tests_dir = join(dirname(abspath(__file__)), '')
    module_names = [p.split(tests_dir)[-1][:-3] for p in glob(join(dirname(abspath(__file__)), 'test_*.py'))]
    return {test_name[5:]: import_module(f'tests.{test_name}').test for test_name in module_names}
