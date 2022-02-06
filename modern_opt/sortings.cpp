#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <utility>
#include <iostream>
#include <cmath>
#include <climits>
#include <mkl.h>

using namespace std;

typedef long long unsigned int uint64;

void swapKeysAndValues(vector<uint64>& a, vector<uint64>& k, const uint64 i, const uint j)
{
    swap(a[i], a[j]);
    swap(k[i], k[j]);
}

// a - values, k - indices
pair<vector<uint64>, vector<uint64>> bubbleSort(const vector<uint64>& a, const vector<uint64>& k)
{
    uint64 n = a.size();
    uint64 res[n];
    uint64 idx[n];
    for (uint64 i = 0; i < n; ++i)
    {
        res[i] = a[i];
        idx[i] = k[i];
    }
    for (uint64 i = 0; i < n - 1; ++i)
        for (uint64 j = 0; j < n - i - 1; ++j)
        {
            if (res[j] > res[j + 1])
            {
                uint64 tmp = res[j];
                res[j] = res[j + 1];
                res[j + 1] = tmp;
                tmp = idx[j];
                idx[j] = idx[j + 1];
                idx[j + 1] = tmp;
            }
        }

    return pair<vector<uint64>, vector<uint64>>(
        vector<uint64>(idx, idx + sizeof(idx) / sizeof(idx[0])),
        vector<uint64>(res, res + sizeof(res) / sizeof(res[0]))
    );
}

pair<vector<uint64>, vector<uint64>> countingSort(const vector<uint64>& a, const vector<uint64>& k)
{
    uint64 n = a.size();
    uint64 alpha = 0;
    for (uint64 i = 0; i < n; ++i)
        if (alpha < a[i])
            alpha = a[i];

    uint64 c[alpha + 1];
    for (uint64 i = 0; i < alpha + 1; ++i)
        c[i] = 0;

    for (uint64 i = 0; i < n; ++i)
        ++c[a[i]];

    for (uint64 i = 1; i < alpha + 1; ++i)
        c[i] = c[i] + c[i - 1];

    uint64 idx[n];
    uint64 res[n];
    for (int i = n - 1; i >= 0; --i)
    {
        res[c[a[i]] - 1] = a[i];
        idx[c[a[i]] - 1] = k[i];
        --c[a[i]];
    }

    return pair<vector<uint64>, vector<uint64>>(
        vector<uint64>(idx, idx + sizeof(idx) / sizeof(idx[0])),
        vector<uint64>(res, res + sizeof(res) / sizeof(res[0]))
    );
}

pair<vector<uint64>, vector<uint64>> sortOnBitMask(const vector<uint64>& a, const vector<uint64>& k, unsigned int bitShift)
{
    const uint64 mask = 0x000000000000ffffULL;
    vector<uint64> x;
    for (uint64 i = 0; i < a.size(); ++i)
        x.push_back(((a[k[i]]) >> bitShift) & mask);
    return countingSort(x, k);
}

vector<uint64> radixArgSort(const vector<uint64>& a)
{
    vector<uint64> idx;
    for (uint64 i = 0; i < a.size(); ++i)
        idx.push_back(i);
    // sort by forth 16 bits
    auto p3 = sortOnBitMask(a, idx, 0);
    // sort by third 16 bits
    auto p2 = sortOnBitMask(a, get<0>(p3), 16);
    // sort by second 16 bits
    auto p1 = sortOnBitMask(a, get<0>(p2), 32);
    // sort by first 16 bits
    auto p0 = sortOnBitMask(a, get<0>(p1), 48);
    return get<0>(p0);
}

vector<uint64> bucketArgSort(const vector<uint64>& a)
{
    uint64 n = a.size();
    uint64 max = 0;
    uint64 min = 0xffffffffffffffffULL;
    vector<pair<vector<uint64>, vector<uint64>>> b;
    for (uint64 i = 0; i < n; ++i)
    {
        if (max < a[i])
            max = a[i];
        if (min > a[i])
            min = a[i];
        b.push_back((pair<vector<uint64>, vector<uint64>>(vector<uint64>(), vector<uint64>())));
    }
    for (uint64 i = 0; i < n; ++i)
    {
        uint64 pos = floor(float(a[i] - min) * n / (max - min));
        if (pos == n)
            pos--;
        get<0>(b[pos]).push_back(i); // index
        get<1>(b[pos]).push_back(a[i]); // value
    }
    vector<uint64> r;
    for (uint64 i = 0; i < n; ++i)
        if (get<1>(b[i]).size() > 0)
        {
            auto p = bubbleSort(get<1>(b[i]), get<0>(b[i]));
            for (uint64 j = 0; j < get<0>(p).size(); ++j)
            {
                r.push_back(get<0>(p)[j]);
            }
        }
    return r;
}

void insertionSort(vector<uint64>& a, vector<uint64>& k, int start, int end)
{
    for (int i = start; i <= end; ++i)
    {
        uint64 x = a[i];
        uint64 kx = k[i];
        int j = i - 1;
        while (j >= 0 && a[j] > x)
        {
            a[j + 1] = a[j];
            k[j + 1] = k[j];
            --j;
        }
        a[j + 1] = x;
        k[j + 1] = kx;
    }
}

uint64 generateSwapIndex(uint64 i, VSLStreamStatePtr & stream)
{
    uint64 bitMask = i;
    for (int p = 1; p <= 64; p *= 2)
        bitMask |= bitMask >> p;

    uint64 r = i + 1;
    while (r > i)
    {
        viRngUniformBits64(VSL_RNG_METHOD_UNIFORMBITS64_STD, stream, 1, &r);
        r = r & bitMask;
    }

    return r;
}

void shuffle(vector<uint64>& a, vector<uint64>& k)
{
    uint64 n = a.size();
    VSLStreamStatePtr rngStream;
    vslNewStream(&rngStream, VSL_BRNG_MT19937, 42);
    for (uint64 i = n - 1; i > 0; --i)
    {
        uint64 j = generateSwapIndex(i, rngStream);
        swapKeysAndValues(a, k, i, j);
    }
}

uint64 partition(vector<uint64>& a, vector<uint64>& k, int start, int end)
{
    uint64 c = a[end];
    int i = start;

    for (int j = start; j < end; ++j)
        if (a[j] <= c)
        {
            swapKeysAndValues(a, k, i, j);
            ++i;
        }

    swapKeysAndValues(a, k, i, end);
    return i;
}

void quickSort(vector<uint64>& a, vector<uint64>& k, int start, int end)
{
    const int limit = 32;
    while (end - start >= limit)
    {
        int pivot = partition(a, k, start, end);
        if (pivot - start <= end - pivot - 1)
        {
            quickSort(a, k, start, pivot - 1);
            start = pivot + 1;
        }
        else
        {
            quickSort(a, k, pivot + 1, end);
            end = pivot - 1;
        }
    }
    insertionSort(a, k, start, end);
}

vector<uint64> quickArgSort(const vector<uint64>& a)
{
    vector<uint64> val, idx;
    for (uint64 i = 0; i < a.size(); ++i)
    {
        val.push_back(a[i]);
        idx.push_back(i);
    }

    shuffle(val, idx);
    quickSort(val, idx, 0, val.size() - 1);

    return idx;
}

PYBIND11_MODULE(sortings, m)
{
    m.doc() = "pybind11 sortings plugin";
    m.def("radix_argsort", &radixArgSort, "Radix ArgSort");
    m.def("bucket_argsort", &bucketArgSort, "Bucket ArgSort");
    m.def("quick_argsort", &quickArgSort, "Quick ArgSort");
}
