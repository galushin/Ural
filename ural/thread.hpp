#ifndef Z_URAL_THREAD_HPP_INCLUDED
#define Z_URAL_THREAD_HPP_INCLUDED

/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @file ural/thread.hpp
 @brief Классы и функции для многопточного программирования
*/

#include <cstddef>

namespace ural
{
    /** @brief Стратегия многопотоночти: многопоточная программа, использовать
    средства стандартной библиотеки C++11
    @todo Реализовать
    */
    class cpp11_thread_policy
    {
    public:
    };

    /** @brief Класс, реализующий синтаксические требования к мьютексу, но не
    выполняющий никакой синхронизации
    @todo Реализовать
    */
    class single_thread_mutex
    {
    public:
        void lock(){}
        void unlock(){}
    };

    /// @brief Стратегия многопотоночти: однопоточная программа
    class single_thread_policy
    {
    public:
        /// @brief Тип потоково-безопасного счётчика
        typedef std::size_t atomic_counter_type;

        /// @brief Тип мьютекса
        typedef single_thread_mutex mutex_type;
    };
}
// namespace ural

#endif
// Z_URAL_THREAD_HPP_INCLUDED
