#ifndef Z_URAL_UTILITY_TRACERS_HPP_INCLUDED
#define Z_URAL_UTILITY_TRACERS_HPP_INCLUDED

namespace ural
{
    template <class T>
    class regular_tracer
    {
    public:
        typedef size_t counter_type;

        static counter_type active_objects();
        static counter_type destroyed_objects();

        explicit regular_tracer(T init_value);

    private:
        T value_;
    };
}
// namespace ural

#endif
// Z_URAL_UTILITY_TRACERS_HPP_INCLUDED
