#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <span>
#include <string_view>
#include <ranges>

static long long qs_comparisons = 0;
static long long ms_comparisons = 0;

// partition: pivot p, L={x<p}, R={x>=p} (Lomuto)
auto lomuto_partition(std::span<int> a, int lo, int hi) -> int {
    const int pivot = a[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; ++j) {
        ++qs_comparisons;
        if (a[j] < pivot) {
            ++i;
            std::swap(a[i], a[j]);
        }
    }
    std::swap(a[i + 1], a[hi]);
    return i + 1;
}

// T(n) = O(n log n) avg, O(n^2) worst
void quicksort(std::span<int> a, int lo, int hi) {
    if (lo < hi) {
        const int p = lomuto_partition(a, lo, hi);
        quicksort(a, lo, p - 1);
        quicksort(a, p + 1, hi);
    }
}

void quicksort(std::span<int> a) {
    if (!a.empty()) {
        quicksort(a, 0, static_cast<int>(a.size()) - 1);
    }
}

// merge: T(n) = 2T(n/2) + O(n)
void merge(std::span<int> a, std::span<int> tmp, int lo, int mid, int hi) {
    int i = lo, j = mid + 1, k = lo;
    while (i <= mid && j <= hi) {
        ++ms_comparisons;
        if (a[i] <= a[j])
            tmp[k++] = a[i++];
        else
            tmp[k++] = a[j++];
    }
    while (i <= mid) tmp[k++] = a[i++];
    while (j <= hi) tmp[k++] = a[j++];
    for (int x = lo; x <= hi; ++x) a[x] = tmp[x];
}

// T(n) = O(n log n)
void mergesort(std::span<int> a, std::span<int> tmp, int lo, int hi) {
    if (lo < hi) {
        const int mid = lo + (hi - lo) / 2;
        mergesort(a, tmp, lo, mid);
        mergesort(a, tmp, mid + 1, hi);
        merge(a, tmp, lo, mid, hi);
    }
}

// T(n) = O(n + k), k = max(a) - min(a) + 1
void counting_sort(std::span<int> a) {
    if (a.empty()) return;
    const auto [mn_it, mx_it] = std::ranges::minmax_element(a);
    const int mn = *mn_it;
    const int mx = *mx_it;
    const int range = mx - mn + 1;
    // count[v-mn] = |{i : a[i]=v}|
    std::vector<int> count(range, 0);
    for (const int x : a) ++count[x - mn];
    int idx = 0;
    for (int i = 0; i < range; ++i) {
        while (count[i]-- > 0)
            a[idx++] = i + mn;
    }
}

void print_vec(std::string_view label, std::span<const int> v, int limit = -1) {
    std::cout << label << ": [";
    const int n = (limit < 0) ? static_cast<int>(v.size())
                              : std::min(limit, static_cast<int>(v.size()));
    for (int i = 0; i < n; ++i) {
        if (i) std::cout << ", ";
        std::cout << v[i];
    }
    if (limit >= 0 && limit < static_cast<int>(v.size())) std::cout << ", ...";
    std::cout << "]\n";
}

int main() {
    constexpr int N = 25;
    std::mt19937 rng(123);
    std::uniform_int_distribution<int> dist(0, 200);

    std::vector<int> original(N);
    std::ranges::generate(original, [&] { return dist(rng); });

    print_vec("original", original);
    std::cout << "\n";

    // T(n) = O(n log n) avg, O(n^2) worst
    {
        auto a = original;
        qs_comparisons = 0;
        quicksort(std::span{a});
        print_vec("quicksort", a);
        std::cout << "  comparisons = " << qs_comparisons << "\n\n";
    }

    // T(n) = O(n log n)
    {
        auto a = original;
        std::vector<int> tmp(N);
        ms_comparisons = 0;
        mergesort(std::span{a}, std::span{tmp}, 0, N - 1);
        print_vec("mergesort", a);
        std::cout << "  comparisons = " << ms_comparisons << "\n\n";
    }

    // T(n) = O(n + k), k = range
    {
        auto a = original;
        const auto t0 = std::chrono::high_resolution_clock::now();
        counting_sort(std::span{a});
        const auto t1 = std::chrono::high_resolution_clock::now();
        const double us = std::chrono::duration<double, std::micro>(t1 - t0).count();
        print_vec("counting_sort", a);
        std::cout << "  time = " << us << " us\n\n";
    }

    // O(n log n) vs O(n+k)
    constexpr int BIG = 1000000;
    std::cout << "=== benchmark N=" << BIG << " ===\n";
    std::vector<int> big(BIG);
    std::uniform_int_distribution<int> bdist(0, 100000);
    std::ranges::generate(big, [&] { return bdist(rng); });

    {
        auto a = big;
        const auto t0 = std::chrono::high_resolution_clock::now();
        std::ranges::sort(a);
        const auto t1 = std::chrono::high_resolution_clock::now();
        const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        std::cout << "std::sort O(n log n): " << ms << " ms\n";
    }

    {
        auto a = big;
        const auto t0 = std::chrono::high_resolution_clock::now();
        counting_sort(std::span{a});
        const auto t1 = std::chrono::high_resolution_clock::now();
        const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        std::cout << "counting_sort O(n+k): " << ms << " ms\n";
    }

    return 0;
}
