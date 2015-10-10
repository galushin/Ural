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

#include <ural/sequence/adaptors/cartesian_product.hpp>
#include <ural/sequence/adaptors/delimit.hpp>
#include <ural/sequence/adaptors/filtered.hpp>
#include <ural/sequence/adaptors/map.hpp>
#include <ural/sequence/adaptors/moved.hpp>
#include <ural/sequence/adaptors/outdirected.hpp>
#include <ural/sequence/adaptors/partition.hpp>
#include <ural/sequence/adaptors/remove.hpp>
#include <ural/sequence/adaptors/replace.hpp>
#include <ural/sequence/adaptors/reversed.hpp>
#include <ural/sequence/adaptors/set_operations.hpp>
#include <ural/sequence/adaptors/transform.hpp>
#include <ural/sequence/adaptors/uniqued.hpp>
#include <ural/sequence/adaptors/taken.hpp>
#include <ural/sequence/adaptors/taken_while.hpp>
#include <ural/sequence/adaptors/zip.hpp>
#include <ural/sequence/adaptors/simo.hpp>

#include <ural/sequence/base.hpp>
#include <ural/sequence/by_line.hpp>
#include <ural/sequence/chunks.hpp>
#include <ural/sequence/constant.hpp>
#include <ural/sequence/function_output.hpp>
#include <ural/sequence/generator.hpp>
#include <ural/sequence/insertion.hpp>
#include <ural/sequence/iostream.hpp>
#include <ural/sequence/iterator_sequence.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/progression.hpp>
#include <ural/sequence/to.hpp>
#include <ural/sequence/sequence_iterator.hpp>
#include <ural/sequence/sink.hpp>
#include <ural/sequence/to.hpp>

namespace ural
{
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
