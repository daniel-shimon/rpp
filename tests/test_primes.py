from pathlib import Path

from tests.utils import main, Test

expected = '2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97'

test = Test(str(Path(__file__).absolute().parent.parent / 'examples' / 'primes.rpp'), expected, input='100\n', is_path=True)


if __name__ == '__main__':
    main(test)
