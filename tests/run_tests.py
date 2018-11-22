from tests.utils import get_tests, main

passed = True
for name, test in get_tests().items():
    print('checking', name)
    if not main(test):
        passed = False

if passed:
    print('*'*20 + ' all tests passed     ' + '*'*20)
else:
    print('*'*20 + ' not all tests passed ' + '*'*20)
