from string import ascii_letters
from tests.utils import Test, main

code = '''
פעולה א(מ):
    ''' + '\n\t'.join(f'{c} = {i}' for i, c in enumerate(ascii_letters)) + '''
    גיל = 7
    פלוט גיל
    כ = 0
    ''' + '\n\t'.join(f'כ = כ + {c}' for c in ascii_letters) + '''
    החזר מ כפול כ

גיל = 10
פלוט א(גיל)
פלוט א(-1)
פלוט גיל
פלוט a
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
