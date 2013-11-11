#ifndef Z_URAL_THREAD_HPP_INCLUDED
#define Z_URAL_THREAD_HPP_INCLUDED

namespace ural
{
    class cpp11_thread_policy
    {};

    class single_thread_policy
    {
    public:
        typedef size_t atomic_counter_type;
    };
}
// namespace ural

#endif
// Z_URAL_THREAD_HPP_INCLUDED
