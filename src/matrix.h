#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <tuple>

template <typename T, T DefaultValue>
class Matrix
{
public:
    explicit Matrix(T defaultValue = DefaultValue) : m_default(defaultValue) {}

    static inline long long keyOf(int r, int c) noexcept {
        return (static_cast<long long>(r) << 32) | static_cast<unsigned int>(c);
    }

    class CellProxy;
    class RowProxy {
    public:
        RowProxy(Matrix& m, int r) : m_(m), r_(r) {}
        CellProxy operator[](size_t c);
    private:
        Matrix& m_;
        int r_;
    };

    RowProxy operator[](size_t r) { return RowProxy(*this, static_cast<int>(r)); }
    T operator()(size_t r, size_t c) const { return get(static_cast<int>(r), static_cast<int>(c)); }

    std::size_t size() const noexcept { return m_store.size(); }

    class iterator {
    public:
        using inner_it = typename std::unordered_map<long long, T>::const_iterator;
        iterator(inner_it it) : it_(it) {}
        iterator& operator++() { ++it_; return *this; }
        bool operator!=(iterator const& o) const { return it_ != o.it_; }
        std::tuple<int,int,T> operator*() const {
            long long k = it_->first;
            int r = static_cast<int>(k >> 32);
            int c = static_cast<int>(k & 0xffffffff);
            return std::make_tuple(r, c, it_->second);
        }
    private:
        inner_it it_;
    };
    iterator begin() const { return iterator(m_store.cbegin()); }
    iterator end() const { return iterator(m_store.cend()); }

private:
    T get(int r, int c) const {
        auto it = m_store.find(keyOf(r,c));
        if (it == m_store.end()) return m_default;
        return it->second;
    }
    void set(int r, int c, T v) {
        auto k = keyOf(r,c);
        if (v == m_default) m_store.erase(k);
        else m_store[k] = v;
    }

    T m_default{DefaultValue};
    std::unordered_map<long long,T> m_store;

public:
    // Cell proxy allows `matrix[row][col] = val` and reading as `int`
    class CellProxy {
    public:
        CellProxy(Matrix& m, int r, int c) : m_(m), r_(r), c_(c) {}
        CellProxy& operator=(T v) { m_.set(r_, c_, v); return *this; }
        operator T() const { return m_.get(r_, c_); }
    private:
        Matrix& m_;
        int r_, c_;
    };
};

template <typename T, T DefaultValue>
inline typename Matrix<T, DefaultValue>::CellProxy Matrix<T, DefaultValue>::RowProxy::operator[](size_t c) {
    return CellProxy(m_, r_, static_cast<int>(c));
}