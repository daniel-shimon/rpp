from tests.utils import get_tests

total = 0
results = [0] * 4
reps = 100

tests = get_tests()
print(f'running {len(tests)} tests with {reps} repetitions\n')
print(''.join(n.ljust(20) for n in ['test', 'time', 'instructions', 'cache_misses', 'branch_misses']))
print()
for name, test in tests.items():
    result: BenchResult = test.bench(reps)
    for i, v in enumerate(result):
        results[i] += v
    total += result.time * reps

    print(name.ljust(20) + ''.join(str(n).ljust(20) for n in result))

print()
for i in range(4):
    results[i] /= len(tests)
print('average'.ljust(20) + ''.join(str(n)[:15].ljust(20) for n in results))
print('\n' + 'total benchmark time:'.ljust(40) + format(total, '.6f') + ' seconds')
