#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <utility>
#include <iostream>
#include <cmath>
#include <climits>

using namespace std;

typedef long long unsigned int uint64;

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

vector<uint64> radixArgSort(const vector<uint64>& a)
{
    uint64 n = a.size();
    vector<uint64> idx;
    for (uint64 i = 0; i < n; ++i)
        idx.push_back(i);
    vector<uint64> x0, x1, x2, x3;
    // sort by forth 16 bits
    uint64 mask = 0x000000000000ffffULL;
    for (uint64 i = 0; i < n; ++i)
        x3.push_back(a[i] & mask);
    auto p3 = countingSort(x3, idx);
    // sort by third 16 bits
    for (uint64 i = 0; i < n; ++i)
        x2.push_back(((a[get<0>(p3)[i]]) >> 16) & mask);
    auto p2 = countingSort(x2, get<0>(p3));
    // sort by second 16 bits
    for (uint64 i = 0; i < n; ++i)
        x1.push_back(((a[get<0>(p2)[i]]) >> 32) & mask);
    auto p1 = countingSort(x1, get<0>(p2));
    // sort by first 16 bits
    for (uint64 i = 0; i < n; ++i)
        x0.push_back(((a[get<0>(p1)[i]]) >> 48) & mask);
    auto p0 = countingSort(x0, get<0>(p1));
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

long int partition(vector<uint64>& a, vector<uint64>& k, long int start, long int end)
{
    uint64 p = a[end];
    long int s = start - 1;
    for (long int i = start; i < end; ++i)
        if (a[i] <= p)
        {
            s++;
            swap(a[s], a[i]);
            swap(k[s], k[i]);
        }
    swap(a[s + 1], a[end]);
    swap(k[s + 1], k[end]);
    return s + 1;
}

void quickSort(vector<uint64>& a, vector<uint64>& k, long int start, long int end)
{
    if (start < end)
    {
        long int i = partition(a, k, start, end);
        quickSort(a, k, start, i - 1);
        quickSort(a, k, i + 1, end);
    }
}

vector<uint64> quickArgSort(const vector<uint64>& a)
{
    vector<uint64> val, idx;
    for (uint64 i = 0; i < a.size(); ++i)
    {
        val.push_back(a[i]);
        idx.push_back(i);
    }

    quickSort(val, idx, 0, a.size() - 1);

    return idx;
}

PYBIND11_MODULE(sortings, m)
{
    m.doc() = "pybind11 sortings plugin";
    m.def("radix_argsort", &radixArgSort, "Radix ArgSort");
    m.def("bucket_argsort", &bucketArgSort, "Bucket ArgSort");
    m.def("quick_argsort", &quickArgSort, "Quick ArgSort");
}
