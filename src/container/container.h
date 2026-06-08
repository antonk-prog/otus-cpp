#pragma once

#include <memory>
#include <cstddef>
#include <utility>

template <typename T, typename Allocator = std::allocator<T>>
class CustomContainer
{
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;

private:
    Allocator m_alloc;
    T* m_data{nullptr};
    size_type m_size{0};
    size_type m_capacity{0};

    using traits = std::allocator_traits<Allocator>;

    void reallocate(size_type new_cap)
    {
        T* new_data = traits::allocate(m_alloc, new_cap);
        size_type i = 0;
        try
        {
            for (; i < m_size; ++i)
            {
                traits::construct(m_alloc, new_data + i, std::move(m_data[i]));
                traits::destroy(m_alloc, m_data + i);
            }
        }
        catch (...)
        {
            for (size_type j = 0; j < i; ++j)
                traits::destroy(m_alloc, new_data + j);
            traits::deallocate(m_alloc, new_data, new_cap);
            throw;
        }
        if (m_data)
            traits::deallocate(m_alloc, m_data, m_capacity);
        m_data = new_data;
        m_capacity = new_cap;
    }

public:
    CustomContainer() = default;

    explicit CustomContainer(Allocator alloc)
        : m_alloc(std::move(alloc))
    {
    }

    ~CustomContainer()
    {
        clear();
        if (m_data)
            traits::deallocate(m_alloc, m_data, m_capacity);
    }

    void push(const T& value)
    {
        if (m_size == m_capacity)
        {
            size_type new_cap = m_capacity == 0 ? 1 : m_capacity * 2;
            reallocate(new_cap);
        }
        traits::construct(m_alloc, m_data + m_size, value);
        ++m_size;
    }

    void reserve(size_type new_cap)
    {
        if (new_cap > m_capacity)
            reallocate(new_cap);
    }

    void push(T&& value)
    {
        if (m_size == m_capacity)
        {
            size_type new_cap = m_capacity == 0 ? 1 : m_capacity * 2;
            reallocate(new_cap);
        }
        traits::construct(m_alloc, m_data + m_size, std::move(value));
        ++m_size;
    }

    void clear() noexcept
    {
        for (size_type i = 0; i < m_size; ++i)
            traits::destroy(m_alloc, m_data + i);
        m_size = 0;
    }

    size_type size() const noexcept { return m_size; }
    size_type capacity() const noexcept { return m_capacity; }
    bool empty() const noexcept { return m_size == 0; }

    T& operator[](size_type i) { return m_data[i]; }
    const T& operator[](size_type i) const { return m_data[i]; }

    T* data() noexcept { return m_data; }
    const T* data() const noexcept { return m_data; }

    iterator begin() noexcept { return m_data; }
    iterator end() noexcept { return m_data + m_size; }
    const_iterator begin() const noexcept { return m_data; }
    const_iterator end() const noexcept { return m_data + m_size; }
    const_iterator cbegin() const noexcept { return m_data; }
    const_iterator cend() const noexcept { return m_data + m_size; }
};
