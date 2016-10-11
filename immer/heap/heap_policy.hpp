
#pragma once

#include <immer/heap/free_list_heap.hpp>
#include <immer/heap/malloc_heap.hpp>
#include <immer/heap/thread_local_free_list_heap.hpp>
#include <immer/config.hpp>

#include <cstdlib>
#include <algorithm>

namespace immer {

template <typename Heap>
struct heap_policy
{
    template <std::size_t...>
    struct apply
    {
        using type = Heap;
    };
};

template <typename Deriv, typename HeapPolicy>
struct enable_heap_policy
{
    static void* operator new (std::size_t size)
    {
        using heap_type = typename HeapPolicy
            ::template apply<sizeof(Deriv)>::type;

        return heap_type::allocate(size);
    }

    static void operator delete (void* data)
    {
        using heap_type = typename HeapPolicy
            ::template apply<sizeof(Deriv)>::type;

        heap_type::deallocate(data);
    }
};

struct default_heap_policy
{
    template <std::size_t... Sizes>
    struct apply
    {
#if IMMER_FREE_LIST
        static constexpr auto node_size = std::max({Sizes...});

        using type = with_free_list_node<
            thread_local_free_list_heap<
                node_size,
                malloc_heap>>;
#else
        using type = malloc_heap;
#endif
    };
};

} // namespace immer