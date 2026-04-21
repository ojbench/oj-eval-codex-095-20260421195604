#ifndef SRC_HPP
#define SRC_HPP
#include <iostream>
#include <cstddef>

struct Permutation { // 置换
    int* mapping; // 存储 a(i)
    size_t size;  // 数组长度

    // 构造恒等置换
    Permutation(size_t n) : mapping(nullptr), size(n) {
        mapping = (size ? new int[size] : nullptr);
        for (size_t i = 0; i < size; ++i) mapping[i] = static_cast<int>(i);
    }

    // 深拷贝构造
    Permutation(const Permutation& other) : mapping(nullptr), size(other.size) {
        mapping = (size ? new int[size] : nullptr);
        for (size_t i = 0; i < size; ++i) mapping[i] = other.mapping[i];
    }

    // 移动构造
    Permutation(Permutation&& other) noexcept : mapping(other.mapping), size(other.size) {
        other.mapping = nullptr;
        other.size = 0;
    }

    // 由给定映射构造
    Permutation(const int* src_mapping, size_t n) : mapping(nullptr), size(n) {
        mapping = (size ? new int[size] : nullptr);
        for (size_t i = 0; i < size; ++i) mapping[i] = src_mapping[i];
    }

    // 深拷贝赋值
    Permutation& operator=(const Permutation& other) {
        if (this == &other) return *this;
        int* newmap = (other.size ? new int[other.size] : nullptr);
        for (size_t i = 0; i < other.size; ++i) newmap[i] = other.mapping[i];
        delete[] mapping;
        mapping = newmap;
        size = other.size;
        return *this;
    }

    // 移动赋值
    Permutation& operator=(Permutation&& other) noexcept {
        if (this == &other) return *this;
        delete[] mapping;
        mapping = other.mapping;
        size = other.size;
        other.mapping = nullptr;
        other.size = 0;
        return *this;
    }

    ~Permutation() {
        delete[] mapping;
        mapping = nullptr;
        size = 0;
    }

    // 应用到大小为 size 的数组: new[i] = old[mapping[i]]
    void apply(int* permutation) const {
        if (size == 0) return;
        int* tmp = new int[size];
        for (size_t i = 0; i < size; ++i) tmp[i] = permutation[mapping[i]];
        for (size_t i = 0; i < size; ++i) permutation[i] = tmp[i];
        delete[] tmp;
    }

    // 乘积 this * other（先应用 other，再应用 this）
    // r[i] = other.mapping[ this->mapping[i] ]
    Permutation operator*(const Permutation& other) const {
        Permutation result(size);
        for (size_t i = 0; i < size; ++i) result.mapping[i] = other.mapping[mapping[i]];
        return result;
    }

    // 逆置换：inv[mapping[i]] = i
    Permutation inverse() const {
        Permutation inv(size);
        for (size_t i = 0; i < size; ++i) inv.mapping[mapping[i]] = static_cast<int>(i);
        return inv;
    }

    friend std::ostream& operator<<(std::ostream& os, const Permutation& p) {
        os << "[";
        for (size_t i = 0; i < p.size; ++i) {
            os << p.mapping[i];
            if (i < p.size - 1) os << " ";
        }
        os << "]";
        return os;
    }
};

struct Transposition { // 对换
    int a, b;

    Transposition() : a(0), b(0) {
        // a==b 为恒等
    }

    Transposition(int aa, int bb) : a(aa), b(bb) {}

    // 在数组上交换下标 a 与 b 的元素
    void apply(int* permutation, size_t n) const {
        if (static_cast<size_t>(a) < n && static_cast<size_t>(b) < n) {
            int t = permutation[a];
            permutation[a] = permutation[b];
            permutation[b] = t;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Transposition& t) {
        os << "{" << t.a << " " << t.b << "}";
        return os;
    }

    // 转换为置换（除 a、b 互换外，其余恒等）
    Permutation toPermutation(size_t n) const {
        Permutation p(n);
        if (static_cast<size_t>(a) < n && static_cast<size_t>(b) < n) {
            p.mapping[a] = b;
            p.mapping[b] = a;
        }
        return p;
    }
};

struct Cycle { // 轮换
    int* elements; // b0, b1, ..., b_{k-1}
    size_t size;

    Cycle(const int* elems, size_t k) : elements(nullptr), size(k) {
        elements = (size ? new int[size] : nullptr);
        for (size_t i = 0; i < size; ++i) elements[i] = elems[i];
    }

    // 拷贝/赋值与移动，避免双重释放
    Cycle(const Cycle& other) : elements(nullptr), size(other.size) {
        elements = (size ? new int[size] : nullptr);
        for (size_t i = 0; i < size; ++i) elements[i] = other.elements[i];
    }
    Cycle& operator=(const Cycle& other) {
        if (this == &other) return *this;
        int* ne = (other.size ? new int[other.size] : nullptr);
        for (size_t i = 0; i < other.size; ++i) ne[i] = other.elements[i];
        delete[] elements;
        elements = ne;
        size = other.size;
        return *this;
    }
    Cycle(Cycle&& other) noexcept : elements(other.elements), size(other.size) {
        other.elements = nullptr;
        other.size = 0;
    }
    Cycle& operator=(Cycle&& other) noexcept {
        if (this == &other) return *this;
        delete[] elements;
        elements = other.elements;
        size = other.size;
        other.elements = nullptr;
        other.size = 0;
        return *this;
    }

    ~Cycle() {
        delete[] elements;
        elements = nullptr;
        size = 0;
    }

    // 应用到大小为 n 的数组：permutation[b_j] = old[b_{(j+1) mod k}]
    void apply(int* permutation, size_t n) const {
        if (size == 0) return;
        int* old = new int[n];
        for (size_t i = 0; i < n; ++i) old[i] = permutation[i];
        for (size_t j = 0; j < size; ++j) {
            int to = elements[j];
            int from = elements[(j + 1) % size];
            if (static_cast<size_t>(to) < n && static_cast<size_t>(from) < n) {
                permutation[to] = old[from];
            }
        }
        delete[] old;
    }

    friend std::ostream& operator<<(std::ostream& os, const Cycle& c) {
        os << "{";
        for (size_t i = 0; i < c.size; ++i) {
            os << c.elements[i];
            if (i < c.size - 1) os << " ";
        }
        os << "}";
        return os;
    }

    // 转换为置换：mapping[b_j] = b_{(j+1) mod k}
    Permutation toPermutation(size_t n) const {
        Permutation p(n);
        for (size_t j = 0; j < size; ++j) {
            int i = elements[j];
            int nxt = elements[(j + 1) % size];
            if (static_cast<size_t>(i) < n && static_cast<size_t>(nxt) < n) {
                p.mapping[i] = nxt;
            }
        }
        return p;
    }
};

#endif
