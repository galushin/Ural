#ifndef Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ALL_HPP_INCLUDED

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

/** @file ural/sequence/all.hpp
 @brief Все последовательности
*/

/**
ОБОСНОВАНИЕ

Алгоритмы реализованы в виде двуслойной схемы. Первый слой взаимодействует с
пользователем и преобразует аргументы, переданные пользователем: контейнеры
в последовательности, указатели на функции-члены в функциональные объекты и т.д.
Второй слой не должен использоваться клиентами непосредственно, в нём считается,
что все аргументы преобразованы в готовую к использованию форму.

Такой подход выбран, чтобы избежать комбинаторного возрастания количества
алгоритмов. Можно было бы определить одну версию алгоритма для
последовательностей, а другую --- для контейнеров, но этот подход не
масшабируется: поддержка возможности передавать в качестве функциональных
объектов указатели на функции-члены потребует либо преобразования на стороне
клиента, либо удваивания количества алгоритмов.
*/

#include <iterator>

#include <ural/sequence/all_tuples.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/moved.hpp>
#include <ural/sequence/filtered.hpp>
#include <ural/sequence/iostream.hpp>
#include <ural/sequence/iterator_sequence.hpp>
#include <ural/sequence/insertion.hpp>
#include <ural/sequence/map.hpp>
#include <ural/sequence/remove.hpp>
#include <ural/sequence/replace.hpp>
#include <ural/sequence/reversed.hpp>
#include <ural/sequence/set_operations.hpp>
#include <ural/sequence/taken.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/sequence/uniqued.hpp>

namespace ural
{
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
