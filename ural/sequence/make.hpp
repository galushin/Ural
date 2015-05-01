#ifndef Z_URAL_SEQUENCE_MAKE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_MAKE_HPP_INCLUDED

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

/** @file ural/sequence/make.hpp
 @brief Функции создания последовательностей
*/

#include <iterator>
#include <valarray>

#include <ural/sequence/cargo.hpp>
#include <ural/sequence/iostream.hpp>
#include <ural/sequence/iterator_sequence.hpp>
#include <ural/sequence/insertion.hpp>

namespace ural
{
    /** @brief Создание последовательности на основе контейнера
    @param c контейнер
    @return <tt> iterator_sequence<decltype(c.begin())>(c.begin(), c.end())</tt>
    */
    template <class Container>
    auto sequence(Container && c)
    -> typename std::enable_if<std::is_reference<Container>::value,
                               ::ural::iterator_sequence<decltype(c.begin())>>::type
    {
        return iterator_sequence<decltype(c.begin())>(c.begin(), c.end());
    }

    template <class Container>
    auto sequence(Container && c)
    -> typename std::enable_if<!std::is_reference<Container>::value,
                               ::ural::cargo_sequence<::ural::iterator_sequence<decltype(c.begin())>,
                                                      Container>>::type
    {
        typedef ::ural::cargo_sequence<::ural::iterator_sequence<decltype(c.begin())>, Container>
            Result;
        auto seq = ::ural::iterator_sequence<decltype(c.begin())>(c.begin(), c.end());
        return Result(std::move(seq), std::move(c));
    }

    /** @brief Создание последовательности на основе массива фиксированной длины
    @param x массив
    @return <tt> iterator_sequence<T *>{x, x + N} </tt>
    */
    template <class T, size_t N>
    iterator_sequence<T *>
    sequence(T (&x)[N])
    {
        return iterator_sequence<T *>{x, x + N};
    }

    /** @brief Создание последовательности на основе итератора вставки в конец
    контейнера
    @param i итератор-вставка
    @return <tt> output_iterator_sequence<decltype(i)>(std::move(i)) </tt>
    */
    template <class Container>
    output_iterator_sequence<std::back_insert_iterator<Container>>
    sequence(std::back_insert_iterator<Container> i)
    {
        typedef std::back_insert_iterator<Container> Iterator;
        return output_iterator_sequence<Iterator>(std::move(i));
    }

    /** @brief Создание последовательности на основе итератора вставки в начало
    контейнера
    @param i итератор-вставка
    @return <tt> output_iterator_sequence<decltype(i)>(std::move(i)) </tt>
    */
    template <class Container>
    output_iterator_sequence<std::front_insert_iterator<Container>>
    sequence(std::front_insert_iterator<Container> i)
    {
        typedef std::front_insert_iterator<Container> Iterator;
        return output_iterator_sequence<Iterator>(std::move(i));
    }

    //@{
    /** @brief Создание последовательности на основе <tt> std::valarray </tt>
    @param c вектор для которого создаётся последовательность
    @return <tt> iterator_sequence<T*>(begin(c), end(c)) </tt>
    */
    template <class T>
    iterator_sequence<T*>
    sequence(std::valarray<T> & c)
    {
        if(c.size() == 0)
        {
            return iterator_sequence<T*>(nullptr, nullptr);
        }
        else
        {
            auto first = std::addressof(c[0]);
            return iterator_sequence<T*>(first, first + c.size());
        }
    }

    template <class T>
    iterator_sequence<T const*>
    sequence(std::valarray<T> const & c)
    {
        if(c.size() == 0)
        {
            return iterator_sequence<const T*>(nullptr, nullptr);
        }
        else
        {
            auto first = std::addressof(c[0]);
            return iterator_sequence<const T*>(first, first + c.size());
        }
    }
    //@}

    /** @brief Преобразование потока ввода в последовательность символов
    @param is поток ввода
    @return <tt> ::ural::make_istream_sequence<Char>(is) </tt>
    */
    template <class Char, class Traits>
    auto sequence(std::basic_istream<Char, Traits> & is)
    -> decltype(::ural::make_istream_sequence<Char>(is))
    {
        return ::ural::make_istream_sequence<Char>(is);
    }

    /** @brief Преобразование потока вывода в последовательность символов
    @param is поток ввода
    @return <tt> ::ural::make_ostream_sequence(os) </tt>
    */
    template <class Char, class Traits>
    auto sequence(std::basic_ostream<Char, Traits> & os)
    -> decltype(::ural::make_ostream_sequence(os))
    {
        return ::ural::make_ostream_sequence(os);
    }

    //@{
    /** @brief Функция, комбинирующая @c sequence и <tt> std::forward </tt>
    @param t аргумент
    @return <tt> sequence(std::forward<Traversable>(t)) </tt>
    @todo Преобразовать в функциональный объект/шаблон переменной
    */
    template <class Traversable>
    auto sequence_fwd(typename std::remove_reference<Traversable>::type & t)
    -> decltype(sequence(std::forward<Traversable>(t)))
    {
        return sequence(std::forward<Traversable>(t));
    }

    template <class Traversable>
    auto sequence_fwd(typename std::remove_reference<Traversable>::type && t)
    -> decltype(sequence(std::forward<Traversable>(t)))
    {
        return sequence(std::forward<Traversable>(t));
    }
    //@}
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MAKE_HPP_INCLUDED
