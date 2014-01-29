#ifndef Z_URAL_MEMORY_HPP_INCLUDED
#define Z_URAL_MEMORY_HPP_INCLUDED

/** @file ural/memory.hpp
 @brief Средства для работы с динамической памятью
*/

#include <memory>

namespace ural
{
    /** @todo Перегрузки для встроенных массивов
    */
    template <class T, class... Args>
    std::unique_ptr<T> make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
// namespace ural

#endif
// Z_URAL_MEMORY_HPP_INCLUDED
