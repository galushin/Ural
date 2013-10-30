#ifndef Z_URAL_UTILITY_TRACERS_HPP_INCLUDED
#define Z_URAL_UTILITY_TRACERS_HPP_INCLUDED

namespace ural
{
    /** @todo настраиваемый тип счётчика, возможно --- потоково-безопасный
    */
    template <class T>
    class regular_tracer
    {
    public:
        typedef size_t counter_type;

        static counter_type active_objects()
        {
            return constructed_objects() - destroyed_objects();
        }

        static counter_type constructed_objects()
        {
            return constructed_ref();
        }

        static counter_type destroyed_objects()
        {
            return destroyed_ref();
        }

        explicit regular_tracer(T init_value)
         : value_{std::move(init_value)}
        {
            ++ constructed_ref();
        }

        ~regular_tracer()
        {
            try
            {
                ++ destroyed_ref();
            }
            catch(...)
            {}
        }

    private:
        static counter_type & constructed_ref()
        {
            static counter_type inst;
            return inst;
        }

        static counter_type & destroyed_ref()
        {
            static counter_type inst;
            return inst;
        }

    private:
        T value_;
    };
}
// namespace ural

#endif
// Z_URAL_UTILITY_TRACERS_HPP_INCLUDED
