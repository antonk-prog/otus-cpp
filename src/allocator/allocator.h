#pragma once
#include <cstddef>
#include <stdexcept>
#include <cstdint>
#include <memory>

struct AllocatorPool
{
    std::size_t limit;
    std::size_t allocated{0};
    explicit AllocatorPool(std::size_t lim) : limit(lim) {}
};

template <typename T, std::size_t N>
class CustomAllocator
{
private:
    std::shared_ptr<AllocatorPool> m_pool;

    template <typename U, std::size_t M>
    friend class CustomAllocator;

public:
    using value_type = T;

    CustomAllocator()
        : m_pool(std::make_shared<AllocatorPool>(N))
    {
    }

    template <typename U>
    CustomAllocator(const CustomAllocator<U, N>& other) noexcept
        : m_pool(other.m_pool)
    {
    }

    template <typename U>
    struct rebind
    {
        using other = CustomAllocator<U, N>;
    };

    T* allocate(std::size_t n)
    {
        if (n == 0)
            throw std::invalid_argument("n must be > 0");
        if (!m_pool || m_pool->allocated + n > m_pool->limit)
            throw std::bad_alloc();
        m_pool->allocated += n;
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) noexcept
    {
        if (m_pool)
            m_pool->allocated -= n;
        ::operator delete(p);
    }

    friend bool operator==(const CustomAllocator& a, const CustomAllocator& b)
    {
        return a.m_pool == b.m_pool;
    }

    friend bool operator!=(const CustomAllocator& a, const CustomAllocator& b)
    {
        return a.m_pool != b.m_pool;
    }
};
