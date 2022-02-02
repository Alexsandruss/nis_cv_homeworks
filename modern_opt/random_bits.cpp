#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <mkl.h>

using namespace std;

typedef long long unsigned int uint64;

vector<uint64> getRandomNumbers(const uint n)
{
    VSLStreamStatePtr stream;
    vslNewStream(&stream, VSL_BRNG_MT19937, 42);
    uint64 * arr = new uint64[n];
    viRngUniformBits64(VSL_RNG_METHOD_UNIFORMBITS64_STD, stream, n, arr);

    vector<uint64> r;
    for (uint i = 0; i < n; ++i)
    {
        r.push_back(arr[i]);
    }
    delete arr;
    return r;
}

uint64 cxor(const uint64 a, const uint64 b)
{
    return a ^ b;
}

PYBIND11_MODULE(random_bits, m)
{
    m.doc() = "pybind11 random_bits plugin";
    m.def("get_random_numbers", &getRandomNumbers, "UINT64 generator");
    m.def("cxor", &cxor, "XOR");
}
