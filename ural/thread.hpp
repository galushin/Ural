#ifndef Z_URAL_THREAD_HPP_INCLUDED
#define Z_URAL_THREAD_HPP_INCLUDED

/** @file ural/thread
 @brief Классы и функции для многопточного программирования
*/

namespace ural
{
    /** @brief Стратегия многопотоночти: многопоточная программа, использовать
    средства стандартной библиотеки C++11
    */
    class cpp11_thread_policy
    {};

    /// @brief Стратегия многопотоночти: однопоточная программа
    class single_thread_policy
    {
    public:
        /// @brief Тип потоково-безопасного счётчика
        typedef size_t atomic_counter_type;
    };
}
// namespace ural

#endif
// Z_URAL_THREAD_HPP_INCLUDED
