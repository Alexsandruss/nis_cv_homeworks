import numpy as np
from sortings import radix_argsort, bucket_argsort, quick_argsort
from random_bits import get_random_numbers, cxor

def argsort(s):
    return sorted(range(len(s)), key=s.__getitem__)

N_TESTS = 100
LOW, HIGH, N = 0, 512, 512

radix_works, bucket_works, quick_works = True, True, True
np.random.seed(42)
for i in range(N_TESTS):
    a = list(np.random.randint(LOW, HIGH, size=(N,)))
    radix_res = radix_argsort(a)
    bucket_res = bucket_argsort(a)
    quick_res = quick_argsort(a)
    numpy_stable_res =  list(np.argsort(a, kind='stable'))
    stock_res = argsort(a)

    radix_works = radix_works and (radix_res == numpy_stable_res)
    bucket_works = bucket_works and (bucket_res == numpy_stable_res)
    quick_res = np.array(a)[quick_res]
    for i in range(1, N):
        if quick_res[i] < quick_res[i - 1]:
            quick_works = False
            break

print('radix sort is correct and stable:', radix_works)
print('bucket sort is correct and stable:', bucket_works)
print('quick sort is correct:', quick_works)

print('\nRandom uints:', get_random_numbers(4))
print('\nCXOR(1111, 0101) == 1010:', cxor(1 + 2 + 4 + 8, 1 + 4) == 2 + 8)
