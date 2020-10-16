from string import ascii_letters
from tests.utils import Test, main

code = '''
פעולה א(מ):
    ''' + '\n\t'.join(f'{c} = {i}' for i, c in enumerate(ascii_letters)) + '''
    גיל = 7
    הדפס גיל
    כ = 0
    ''' + '\n\t'.join(f'כ = כ + {c}' for c in ascii_letters) + '''
    החזר מ כפול כ

גיל = 10
הדפס א(גיל)
הדפס א(-1)
הדפס גיל
הדפס a
'''

s = sum(range(len(ascii_letters)))
expected = [f'''7
{s * 10}
7
{-s}
10
Name error ''', 'a is not defined']

test = Test(code, expected, return_code=1)

if __name__ == '__main__':
    main(test)
