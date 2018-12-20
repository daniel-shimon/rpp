from tests.utils import get_tests, BenchResult, bench_metrics

total = 0
results = [0] * (len(bench_metrics) + 1)
reps = 100

tests = get_tests()
print(f'running {len(tests)} tests with {reps} repetitions\n')
print(''.join(n.ljust(20) for n in ['test'] + bench_metrics))
print()
for name, test in tests.items():
    result: BenchResult = test.bench(reps)
    for i, v in enumerate(result):
        results[i] += v
    total += result.time * reps

    print(name.ljust(20) + ''.join(format(n, ',').ljust(20) for n in result))

print()
for i in range(len(bench_metrics) + 1):
    results[i] /= len(tests)
print('average'.ljust(20) + ''.join(format(n, ',')[:15].ljust(20) for n in results))
print('\n' + 'total benchmark time:'.ljust(40) + format(total, '.6f') + ' seconds')
