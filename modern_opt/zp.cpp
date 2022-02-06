#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <stack>

using namespace std;

int gcp(int a, int b)
{
    if (a < b)
    {
        int tmp = a;
        a = b;
        b = tmp;
    }
    if (b == 0)
        return a;
    else
        return gcp(b, a % b);
}

int invertInZpEuclidean(int p, int x)
{
    if (p == 0 || x == 0 || gcp(p, x) != 1)
        return 0;
    int a = p;
    int b = x;
    if (a < b)
    {
        int tmp = a;
        a = b;
        b = tmp;
    }
    int p0 = a;
    int u = 1;
    int v = 0;
    int m = 0;
    int k = 1;

    while (true)
    {
        int bn = a % b;
        if (bn == 0)
            break;
        int d = a / b;
        int vn = u - v * d;
        u = v;
        v = vn;
        int kn = m - k * d;
        m = k;
        k = kn;
        a = b;
        b = bn;
    }

    if (b == 1)
    {
        if (k < 0)
            k += p0;
        return k;
    }
    return 0;
}

int invertInZpFerma(int p, int x)
{
    if (p == 0 || x == 0 || gcp(p, x) != 1)
        return 0;
    stack<int> operationsStack; // 1 - x^n = (x^2)^k * x, 2 - x^n = (x^2)^k
    int k = p - 2;
    while (k > 1)
    {
        if (k % 2 == 1)
        {
            operationsStack.push(1);
            k = (k - 1) / 2;
        }
        else
        {
            operationsStack.push(2);
            k = k / 2;
        }
    }
    int r = x;
    while (!operationsStack.empty())
    {
        if (operationsStack.top() == 1)
            r = (((r * r) % p) * x) % p;
        else
            r = (r * r) % p;
        operationsStack.pop();
    }
    return r;
}

PYBIND11_MODULE(zp, m)
{
    m.doc() = "pybind11 zp plugin";
    m.def("invert_in_Zp_Euclidean", &invertInZpEuclidean, "invertInZpEuclidean");
    m.def("invert_in_Zp_Ferma", &invertInZpFerma, "invertInZpFerma");
}
