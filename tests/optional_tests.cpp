/*  Этот файл --- часть библиотеки URAL

URAL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

URAL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with URAL.  If not, see <http://www.gnu.org/licenses/>.
*/
/* Основано на
https://github.com/akrzemi1/Optional/blob/master/test_optional.cpp
*/

#include <ural/algorithm.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/concept/assert.hpp>

#include <ostream>
#include <sstream>
#include <utility>
#include <string>
#include <cassert>
#include <complex>
#include <vector>
#include <functional>

#include <ural/optional.hpp>
#include <ural/concepts.hpp>

/// @cond false
enum  State
{
    sDefaultConstructed,
    sValueCopyConstructed,
    sValueMoveConstructed,
    sCopyConstructed,
    sMoveConstructed,
    sMoveAssigned,
    sCopyAssigned,
    sValueCopyAssigned,
    sValueMoveAssigned,
    sMovedFrom,
    sValueConstructed
};

struct OracleVal
{
    State s;
    int i;
    OracleVal(int i = 0) : s(sValueConstructed), i(i) {}
};

struct Oracle
{
    friend bool operator==( Oracle const& a, Oracle const& b )
    { return a.val.i == b.val.i; }

    friend bool operator!=( Oracle const& a, Oracle const& b )
    { return a.val.i != b.val.i; }

    State s;
    OracleVal val;

    Oracle() : s(sDefaultConstructed) {}
    Oracle(const OracleVal& v) : s(sValueCopyConstructed), val(v) {}
    Oracle(OracleVal&& v)
     : s(sValueMoveConstructed), val(std::move(v)) {v.s = sMovedFrom;}
    Oracle(const Oracle& o) : s(sCopyConstructed), val(o.val) {}
    Oracle(Oracle&& o)
     : s(sMoveConstructed), val(std::move(o.val)) {o.s = sMovedFrom;}

    Oracle& operator=(const OracleVal& v)
    { s = sValueCopyConstructed; val = v; return *this; }
    Oracle& operator=(OracleVal&& v)
    {
        s = sValueMoveConstructed; val = std::move(v); v.s = sMovedFrom;
        return *this;
    }
    Oracle& operator=(const Oracle& o)
    { s = sCopyConstructed; val = o.val; return *this; }
    Oracle& operator=(Oracle&& o)
    {
        s = sMoveConstructed; val = std::move(o.val); o.s = sMovedFrom;
        return *this;
    }
};

struct Guard
{
    std::string val;
    Guard() : val{} {}
    explicit Guard(std::string s, int = 0) : val(s) {}
    Guard(const Guard&) = delete;
    Guard(Guard&&) = delete;
    void operator=(const Guard&) = delete;
    void operator=(Guard&&) = delete;
};

struct ExplicitStr
{
    std::string s;
    explicit ExplicitStr(const char* chp) : s(chp) {};
};

struct Date
{
    int i;
    Date() = delete;
    Date(int i) : i{i} {};
    Date(Date&& d) : i(d.i) { d.i = 0; }
    Date(const Date&) = delete;
    Date& operator=(const Date&) = delete;
    Date& operator=(Date&& d) { i = d.i; d.i = 0; return *this;};
};

namespace tr2 = ural;
/// @endcond

BOOST_AUTO_TEST_CASE(empty_init_list_ctor)
{
    tr2::optional<int> o1 = {};
    BOOST_CHECK (!o1);
}

BOOST_AUTO_TEST_CASE(disengaged_ctor)
{
    tr2::optional<int> o1;
    BOOST_CHECK (!o1);

    tr2::optional<int> o2 = tr2::nullopt;
    BOOST_CHECK (!o2);

    tr2::optional<int> o3 = o2;
    BOOST_CHECK (!o3);

    BOOST_CHECK (o1 == tr2::nullopt);
    BOOST_CHECK (o1 == tr2::optional<int>{});
    BOOST_CHECK (!o1);
    BOOST_CHECK (bool(o1) == false);

    BOOST_CHECK (o2 == tr2::nullopt);
    BOOST_CHECK (o2 == tr2::optional<int>{});
    BOOST_CHECK (!o2);
    BOOST_CHECK (bool(o2) == false);

    BOOST_CHECK (o3 == tr2::nullopt);
    BOOST_CHECK (o3 == tr2::optional<int>{});
    BOOST_CHECK (!o3);
    BOOST_CHECK (bool(o3) == false);

    BOOST_CHECK (o1 == o2);
    BOOST_CHECK (o2 == o1);
    BOOST_CHECK (o1 == o3);
    BOOST_CHECK (o3 == o1);
    BOOST_CHECK (o2 == o3);
    BOOST_CHECK (o3 == o2);
}

BOOST_AUTO_TEST_CASE(value_ctor)
{
  OracleVal v;
  tr2::optional<Oracle> oo1(v);
  BOOST_CHECK (oo1 != tr2::nullopt);
  BOOST_CHECK (oo1 != tr2::optional<Oracle>{});
  BOOST_CHECK (oo1 == tr2::optional<Oracle>{v});
  BOOST_CHECK (!!oo1);
  BOOST_CHECK (bool(oo1));
  // NA: BOOST_CHECK (oo1->s == sValueCopyConstructed);
  BOOST_CHECK (oo1->s == sMoveConstructed);
  BOOST_CHECK (v.s == sValueConstructed);

  tr2::optional<Oracle> oo2(std::move(v));
  BOOST_CHECK (oo2 != tr2::nullopt);
  BOOST_CHECK (oo2 != tr2::optional<Oracle>{});
  BOOST_CHECK (oo2 == oo1);
  BOOST_CHECK (!!oo2);
  BOOST_CHECK (bool(oo2));
  // NA: BOOST_CHECK (oo2->s == sValueMoveConstructed);
  BOOST_CHECK (oo2->s == sMoveConstructed);
  BOOST_CHECK (v.s == sMovedFrom);

  {
      OracleVal v;
      tr2::optional<Oracle> oo1{tr2::inplace, v};
      BOOST_CHECK (oo1 != tr2::nullopt);
      BOOST_CHECK (oo1 != tr2::optional<Oracle>{});
      BOOST_CHECK (oo1 == tr2::optional<Oracle>{v});
      BOOST_CHECK (!!oo1);
      BOOST_CHECK (bool(oo1));
      BOOST_CHECK (oo1->s == sValueCopyConstructed);
      BOOST_CHECK (v.s == sValueConstructed);

      tr2::optional<Oracle> oo2{tr2::inplace, std::move(v)};
      BOOST_CHECK (oo2 != tr2::nullopt);
      BOOST_CHECK (oo2 != tr2::optional<Oracle>{});
      BOOST_CHECK (oo2 == oo1);
      BOOST_CHECK (!!oo2);
      BOOST_CHECK (bool(oo2));
      BOOST_CHECK (oo2->s == sValueMoveConstructed);
      BOOST_CHECK (v.s == sMovedFrom);
  }
}

// @todo Реализовать
//BOOST_AUTO_TEST_CASE(optional_nullopt_assigment)
//{
//    auto oi = tr2::optional<int>{1};
//    BOOST_CHECK (!!oi);
//
//    oi = {};
//    BOOST_CHECK (!oi);
//}
//
//BOOST_AUTO_TEST_CASE(optional_nullopt_ref_assigment)
//{
//    int var = 1;
//    auto oi = tr2::optional<int&>{var};
//    BOOST_CHECK (!!oi);
//
//    oi = {};
//    BOOST_CHECK (!oi);
//}

BOOST_AUTO_TEST_CASE(optional_assignment_test)
{
    static_assert(ural::is_assignable<int&, int>::value, "is_assignable is buggy");

    tr2::optional<int> oi;
    oi = tr2::optional<int>{1};
    BOOST_CHECK (*oi == 1);

    oi = tr2::nullopt;
    BOOST_CHECK (!oi);

    oi = 2;
    BOOST_CHECK (*oi == 2);

    oi = ural::nullopt;
    BOOST_CHECK (!oi);
}

BOOST_AUTO_TEST_CASE(optional_ref_assignment_test)
{
    int var = 1;
    int var_2 = 2;

    tr2::optional<int&> oi;
    oi = tr2::optional<int&>{var};
    BOOST_CHECK (*oi == 1);

    oi = tr2::nullopt;
    BOOST_CHECK (!oi);

    oi = var_2;
    BOOST_CHECK (*oi == 2);

    oi = tr2::nullopt;
    BOOST_CHECK (!oi);
}

/// @cond false
template <class T>
struct MoveAware
{
  T val;
  bool moved;
  MoveAware(T val) : val(val), moved(false) {}
  MoveAware(MoveAware const&) = delete;
  MoveAware(MoveAware&& rhs) : val(rhs.val), moved(rhs.moved) {
    rhs.moved = true;
  }
  MoveAware& operator=(MoveAware const&) = delete;
  MoveAware& operator=(MoveAware&& rhs) {
    val = (rhs.val);
    moved = (rhs.moved);
    rhs.moved = true;
    return *this;
  }
};
/// @endcond

BOOST_AUTO_TEST_CASE(moved_from_state)
{
  // first, test mock:
  MoveAware<int> i{1}, j{2};
  BOOST_CHECK (i.val == 1);
  BOOST_CHECK (!i.moved);
  BOOST_CHECK (j.val == 2);
  BOOST_CHECK (!j.moved);

  MoveAware<int> k = std::move(i);
  BOOST_CHECK (k.val == 1);
  BOOST_CHECK (!k.moved);
  BOOST_CHECK (i.val == 1);
  BOOST_CHECK (i.moved);

  k = std::move(j);
  BOOST_CHECK (k.val == 2);
  BOOST_CHECK (!k.moved);
  BOOST_CHECK (j.val == 2);
  BOOST_CHECK (j.moved);

  // now, test optional
  tr2::optional<MoveAware<int>> oi{1}, oj{2};
  BOOST_CHECK (oi);
  BOOST_CHECK (!oi->moved);
  BOOST_CHECK (oj);
  BOOST_CHECK (!oj->moved);

  tr2::optional<MoveAware<int>> ok = std::move(oi);
  BOOST_CHECK (ok);
  BOOST_CHECK (!ok->moved);
  BOOST_CHECK (oi);
  BOOST_CHECK (oi->moved);

  ok = std::move(oj);
  BOOST_CHECK (ok);
  BOOST_CHECK (!ok->moved);
  BOOST_CHECK (oj);
  BOOST_CHECK (oj->moved);
}

BOOST_AUTO_TEST_CASE(copy_move_ctor_optional_int)
{
  tr2::optional<int> oi;
  tr2::optional<int> oj = oi;

  BOOST_CHECK (!oj);
  BOOST_CHECK (oj == oi);
  BOOST_CHECK (oj == tr2::nullopt);
  BOOST_CHECK (!bool(oj));

  oi = 1;
  tr2::optional<int> ok = oi;
  BOOST_CHECK (!!ok);
  BOOST_CHECK (bool(ok));
  BOOST_CHECK (ok == oi);
  BOOST_CHECK (ok != oj);
  BOOST_CHECK (*ok == 1);

  tr2::optional<int> ol = std::move(oi);
  BOOST_CHECK (!!ol);
  BOOST_CHECK (bool(ol));
  BOOST_CHECK (ol == oi);
  BOOST_CHECK (ol != oj);
  BOOST_CHECK (*ol == 1);
}

BOOST_AUTO_TEST_CASE(optional_optional)
{
  tr2::optional<tr2::optional<int>> oi1 = tr2::nullopt;
  BOOST_CHECK (oi1 == tr2::nullopt);
  BOOST_CHECK (!oi1);

  {
  tr2::optional<tr2::optional<int>> oi2 {tr2::inplace};
  BOOST_CHECK (oi2 != tr2::nullopt);
  BOOST_CHECK (bool(oi2));
  BOOST_CHECK (*oi2 == tr2::nullopt);
  //BOOST_CHECK (!(*oi2));
  //std::cout << typeid(**oi2).name() << std::endl;
  }

  {
  tr2::optional<tr2::optional<int>> oi2 {tr2::inplace, tr2::nullopt};
  BOOST_CHECK (oi2 != tr2::nullopt);
  BOOST_CHECK (bool(oi2));
  BOOST_CHECK (*oi2 == tr2::nullopt);
  BOOST_CHECK (!*oi2);
  }

  {
  tr2::optional<tr2::optional<int>> oi2 {tr2::optional<int>{}};
  BOOST_CHECK (oi2 != tr2::nullopt);
  BOOST_CHECK (bool(oi2));
  BOOST_CHECK (*oi2 == tr2::nullopt);
  BOOST_CHECK (!*oi2);
  }

  tr2::optional<int> oi;
  auto ooi = tr2::make_optional(oi);
  static_assert( std::is_same<tr2::optional<tr2::optional<int>>, decltype(ooi)>::value, "");

}

BOOST_AUTO_TEST_CASE(example_guard)
{
  using namespace tr2;
  //FAILS: optional<Guard> ogx(Guard("res1"));
  //FAILS: optional<Guard> ogx = "res1";
  //FAILS: optional<Guard> ogx("res1");
  optional<Guard> oga;                     // Guard is non-copyable (and non-moveable)
  optional<Guard> ogb(inplace, "res1");   // initialzes the contained value with "res1"
  BOOST_CHECK (bool(ogb));
  BOOST_CHECK (ogb->val == "res1");

  optional<Guard> ogc(inplace);           // default-constructs the contained value
  BOOST_CHECK (bool(ogc));
  BOOST_CHECK (ogc->val == "");

  oga.emplace("res1");                     // initialzes the contained value with "res1"
  BOOST_CHECK (bool(oga));
  BOOST_CHECK (oga->val == "res1");

  oga.emplace();                           // destroys the contained value and
                                           // default-constructs the new one
  BOOST_CHECK (bool(oga));
  BOOST_CHECK (oga->val == "");

  oga = nullopt;                        // OK: make disengaged the optional Guard
  BOOST_CHECK (!(oga));
  //FAILS: ogb = {};                          // ERROR: Guard is not Moveable
}

/// @cond false
struct Process
{
    static void process(){}
    static void process(int){}
    static void processNil(){}
};
/// @endcond

BOOST_AUTO_TEST_CASE(example1)
{
  using namespace tr2;
  optional<int> oi;                 // create disengaged object
  optional<int> oj = nullopt;          // alternative syntax
  oi = oj;                          // assign disengaged object
  optional<int> ok = oj;            // ok is disengaged

  if (oi)  BOOST_CHECK(false);           // 'if oi is engaged...'
  if (!oi) BOOST_CHECK(true);            // 'if oi is disengaged...'

  if (oi != nullopt) BOOST_CHECK(false);    // 'if oi is engaged...'
  if (oi == nullopt) BOOST_CHECK(true);     // 'if oi is disengaged...'

  BOOST_CHECK(oi == ok);                 // two disengaged optionals compare equal

  ///////////////////////////////////////////////////////////////////////////
  optional<int> ol{1};              // ol is engaged; its contained value is 1
  ok = 2;                           // ok becomes engaged; its contained value is 2
  oj = ol;                          // oj becomes engaged; its contained value is 1

  BOOST_CHECK(oi != ol);                 // disengaged != engaged
  BOOST_CHECK(ok != ol);                 // different contained values
  BOOST_CHECK(oj == ol);                 // same contained value
  BOOST_CHECK(oi < ol);                  // disengaged < engaged
  BOOST_CHECK(ol < ok);                  // less by contained value

  /////////////////////////////////////////////////////////////////////////////
  optional<int> om{1};              // om is engaged; its contained value is 1
  optional<int> on = om;            // on is engaged; its contained value is 1
  om = 2;                           // om is engaged; its contained value is 2
  BOOST_CHECK (on != om);                // on still contains 3. They are not pointers

  /////////////////////////////////////////////////////////////////////////////
  int i = *ol;                      // i obtains the value contained in ol
  BOOST_CHECK (i == 1);
  *ol = 9;                          // the object contained in ol becomes 9
  BOOST_CHECK(*ol == 9);
  BOOST_CHECK(ol == make_optional(9));

  ///////////////////////////////////
  int p = 1;
  optional<int> op = p;
  BOOST_CHECK(*op == 1);
  p = 2;
  BOOST_CHECK(*op == 1);                 // value contained in op is separated from p

  ////////////////////////////////
  if (ol)
    Process::process(*ol);                   // use contained value if present
  else
    Process::process();                      // proceed without contained value

  if (!om)
    Process::processNil();
  else
    Process::process(*om);

  /////////////////////////////////////////
  // use 0 if ol is disengaged
  Process::process(ol.value_or(0));

  ////////////////////////////////////////////
  ok = nullopt;                         // if ok was engaged calls T's dtor
  oj = nullopt;                           // assigns a temporary disengaged optional
}

BOOST_AUTO_TEST_CASE(example_const_optional)
{
  using tr2::optional;
  const optional<int> c = 4;
  int i = *c;                        // i becomes 4
  BOOST_CHECK (i == 4);
  // FAILS: *c = i;                            // ERROR: cannot assign to const int&
}

BOOST_AUTO_TEST_CASE(example_ref)
{
  using namespace tr2;
  int i = 1;
  int j = 2;
  optional<int&> ora;                 // disengaged optional reference to int
  optional<int&> orb = i;             // contained reference refers to object i

  *orb = 3;                          // i becomes 3
  // FAILS: ora = j;                           // ERROR: optional refs do not have assignment from T
  // FAILS: ora = {j};                         // ERROR: optional refs do not have copy/move assignment
  // FAILS: ora = orb;                         // ERROR: no copy/move assignment
  ora.emplace(j);                    // OK: contained reference refers to object j
  ora.emplace(i);                    // OK: contained reference now refers to object i

  ora = nullopt;                        // OK: ora becomes disengaged
}

template <typename T>
T getValue( tr2::optional<T> newVal = tr2::nullopt, tr2::optional<T&> storeHere = tr2::nullopt )
{
  T cached{};

  if (newVal) {
    cached = *newVal;

    if (storeHere) {
      *storeHere = *newVal; // LEGAL: assigning T to T
    }
  }
  return cached;
}

BOOST_AUTO_TEST_CASE(example_optional_arg)
{
  int iii = 0;
  iii = getValue<int>(iii, iii);
  iii = getValue<int>(iii);
  iii = getValue<int>();

  {
    using namespace ural;
    optional<Guard> grd1{inplace, "res1", 1};   // guard 1 initialized
    optional<Guard> grd2;

    grd2.emplace("res2", 2);                     // guard 2 initialized
    grd1 = nullopt;                                 // guard 1 released

  }                                              // guard 2 released (in dtor)
}

/// @cond false
std::tuple<Date, Date, Date> getStartMidEnd();
std::tuple<Date, Date, Date> getStartMidEnd()
{ return std::tuple<Date, Date, Date>{Date{1}, Date{2}, Date{3}}; }

ural::optional<char> readNextChar();
ural::optional<char> readNextChar(){ return{}; }

struct Runner
{
    static void run(ural::optional<std::string>) {}
    static void run(std::complex<double>) {}
    static void run(Date const&, Date const&, Date const&) {}
};
/// @endcond

BOOST_AUTO_TEST_CASE(example_date)
{
  using namespace ural;
  // Date doesn't have default ctor (no good default date)
  optional<Date> start, mid, end;

  std::tie(start, mid, end) = getStartMidEnd();
  Runner::run(*start, *mid, *end);
}

template <class T>
void assign_norebind(tr2::optional<T&>& optref, T& obj)
{
  if (optref) *optref = obj;
  else        optref.emplace(obj);
}

BOOST_AUTO_TEST_CASE(example_conceptual_model)
{
  using namespace ural;

  optional<int> oi = 0;
  optional<int> oj = 1;
  optional<int> ok = nullopt;

  oi = 1;
  oj = nullopt;
  ok = 0;

  BOOST_CHECK_EQUAL(false, oi == nullopt);
  BOOST_CHECK_EQUAL(false, oj == 0);
  BOOST_CHECK_EQUAL(false, ok == 1);
}

BOOST_AUTO_TEST_CASE(example_rationale)
{
  using namespace ural;
  if (optional<char> ch = readNextChar()) {
    // ...
  }

  //////////////////////////////////
  optional<int> opt1 = nullopt;
  optional<int> opt2 = {};

  opt1 = nullopt;
  opt2 = nullopt;

  if (opt1 == nullopt) {}
  if (!opt2) {}
  if (opt2 == optional<int>{}) {}



  ////////////////////////////////
    // pick the second overload
  Runner::run(nullopt);
  // FAILS: run({});              // ambiguous

  if (opt1 == nullopt) {} // fine
  // FAILS: if (opt2 == {}) {}   // ilegal

  ////////////////////////////////
  BOOST_CHECK (optional<unsigned>{}  < optional<unsigned>{0});
  BOOST_CHECK (optional<unsigned>{0} < optional<unsigned>{1});
  BOOST_CHECK (!(optional<unsigned>{}  < optional<unsigned>{}) );
  BOOST_CHECK (!(optional<unsigned>{1} < optional<unsigned>{1}));

  BOOST_CHECK (optional<unsigned>{}  != optional<unsigned>{0});
  BOOST_CHECK (optional<unsigned>{0} != optional<unsigned>{1});
  BOOST_CHECK (optional<unsigned>{}  == optional<unsigned>{} );
  BOOST_CHECK (optional<unsigned>{0} == optional<unsigned>{0});

  /////////////////////////////////
  optional<int> o;
  o = make_optional(1);         // copy/move assignment
  o = 1;           // assignment from T
  o.emplace(1);    // emplacement

  ////////////////////////////////////
  int isas = 0, i = 9;
  optional<int&> asas = i;
  assign_norebind(asas, isas);

  /////////////////////////////////////
  ////tr2::optional<std::vector<int>> ov2 = {2, 3};
  ////BOOST_CHECK (bool(ov2));
  ////BOOST_CHECK ((*ov2)[1] == 3);
  ////
  ////////////////////////////////
  ////std::vector<int> v = {1, 2, 4, 8};
  ////optional<std::vector<int>> ov = {1, 2, 4, 8};

  ////BOOST_CHECK (v == *ov);
  ////
  ////ov = {1, 2, 4, 8};

  ////std::allocator<int> a;
  ////optional<std::vector<int>> ou { emplace, {1, 2, 4, 8}, a };

  ////BOOST_CHECK (ou == ov);

  //////////////////////////////
  // inconvenient syntax:
  {

      tr2::optional<std::vector<int>> ov2{tr2::inplace, {2, 3}};

      BOOST_CHECK (bool(ov2));
      BOOST_CHECK ((*ov2)[1] == 3);

      ////////////////////////////

      std::vector<int> v = {1, 2, 4, 8};
      optional<std::vector<int>> ov{tr2::inplace, {1, 2, 4, 8}};

      BOOST_CHECK (v == *ov);

      ov.emplace({1, 2, 4, 8});
/*
      std::allocator<int> a;
      optional<std::vector<int>> ou { emplace, {1, 2, 4, 8}, a };

      BOOST_CHECK (ou == ov);
*/
  }

  /////////////////////////////////
  {
  typedef int T;
  optional<optional<T>> ot {inplace};
  optional<optional<T>> ou {inplace, nullopt};
  optional<optional<T>> ov {optional<T>{}};

  optional<int> oi;
  auto ooi = make_optional(oi);
  static_assert( std::is_same<optional<optional<int>>, decltype(ooi)>::value, "");
  }
}

bool fun(std::string , ural::optional<int> oi = ural::nullopt);
bool fun(std::string , ural::optional<int> oi)
{
  return bool(oi);
}

BOOST_AUTO_TEST_CASE(example_converting_ctor)
{
  using namespace ural;

  BOOST_CHECK (true == fun("dog", 2));
  BOOST_CHECK (false == fun("dog"));
  // just to be explicit
  BOOST_CHECK (false == fun("dog", nullopt));
}

BOOST_AUTO_TEST_CASE(bad_comparison)
{
  tr2::optional<int> oi, oj;
  int i;
  bool b = (oi == oj);
  b = (oi >= i);
  b = (oi == i);
  b = b;
}

BOOST_AUTO_TEST_CASE(value_or)
{
  tr2::optional<int> oi = 1;
  int i = oi.value_or(0);
  BOOST_CHECK (i == 1);

  oi = tr2::nullopt;
  BOOST_CHECK (oi.value_or(3) == 3);

  tr2::optional<std::string> os{"AAA"};
  BOOST_CHECK (os.value_or("BBB") == "AAA");
  os = tr2::nullopt;
  BOOST_CHECK (os.value_or("BBB") == "BBB");
}

BOOST_AUTO_TEST_CASE(mixed_order)
{
  using namespace ural;

  optional<int> oN {nullopt};
  optional<int> o0 {0};
  optional<int> o1 {1};

  BOOST_CHECK( (oN <   0));
  BOOST_CHECK ( (oN <   1));
  BOOST_CHECK (!(o0 <   0));
  BOOST_CHECK ( (o0 <   1));
  BOOST_CHECK (!(o1 <   0));
  BOOST_CHECK (!(o1 <   1));

  BOOST_CHECK (!(oN >=  0));
  BOOST_CHECK (!(oN >=  1));
  BOOST_CHECK ( (o0 >=  0));
  BOOST_CHECK (!(o0 >=  1));
  BOOST_CHECK ( (o1 >=  0));
  BOOST_CHECK ( (o1 >=  1));

  BOOST_CHECK (!(oN >   0));
  BOOST_CHECK (!(oN >   1));
  BOOST_CHECK (!(o0 >   0));
  BOOST_CHECK (!(o0 >   1));
  BOOST_CHECK ( (o1 >   0));
  BOOST_CHECK (!(o1 >   1));

  BOOST_CHECK ( (oN <=  0));
  BOOST_CHECK ( (oN <=  1));
  BOOST_CHECK ( (o0 <=  0));
  BOOST_CHECK ( (o0 <=  1));
  BOOST_CHECK (!(o1 <=  0));
  BOOST_CHECK ( (o1 <=  1));

  BOOST_CHECK ( (0 >  oN));
  BOOST_CHECK ( (1 >  oN));
  BOOST_CHECK (!(0 >  o0));
  BOOST_CHECK ( (1 >  o0));
  BOOST_CHECK (!(0 >  o1));
  BOOST_CHECK (!(1 >  o1));

  BOOST_CHECK (!(0 <= oN));
  BOOST_CHECK (!(1 <= oN));
  BOOST_CHECK ( (0 <= o0));
  BOOST_CHECK (!(1 <= o0));
  BOOST_CHECK ( (0 <= o1));
  BOOST_CHECK ( (1 <= o1));

  BOOST_CHECK (!(0 <  oN));
  BOOST_CHECK (!(1 <  oN));
  BOOST_CHECK (!(0 <  o0));
  BOOST_CHECK (!(1 <  o0));
  BOOST_CHECK ( (0 <  o1));
  BOOST_CHECK (!(1 <  o1));

  BOOST_CHECK ( (0 >= oN));
  BOOST_CHECK ( (1 >= oN));
  BOOST_CHECK ( (0 >= o0));
  BOOST_CHECK ( (1 >= o0));
  BOOST_CHECK (!(0 >= o1));
  BOOST_CHECK ( (1 >= o1));
}

/// @cond false
struct BadRelops
{
  int i;
};

constexpr bool operator<(BadRelops a, BadRelops b) { return a.i < b.i; }
// intentional error!
constexpr bool operator>(BadRelops a, BadRelops b) { return a.i < b.i; }
///  @endcond

BOOST_AUTO_TEST_CASE(bad_relops)
{
  using namespace ural;
  BadRelops a{1}, b{2};
  BOOST_CHECK (a < b);
  BOOST_CHECK (a > b);

  optional<BadRelops> oa = a, ob = b;
  BOOST_CHECK (oa < ob);
  BOOST_CHECK (!(oa > ob));

  BOOST_CHECK (oa < b);
  BOOST_CHECK (oa > b);

  BOOST_CHECK(oa <= ob);
  BOOST_CHECK(oa <= b);
  BOOST_CHECK(a <= ob);

  BOOST_CHECK(ob >= oa);
  BOOST_CHECK(ob >= a);
  BOOST_CHECK(b >= oa);

  optional<BadRelops&> ra = a, rb = b;
  BOOST_CHECK (ra < rb);
  BOOST_CHECK (!(ra > rb));

  BOOST_CHECK (ra < b);
  BOOST_CHECK (ra > b);
}

BOOST_AUTO_TEST_CASE(mixed_equality)
{
  using namespace ural;

  BOOST_CHECK (make_optional(0) == 0);
  BOOST_CHECK (make_optional(1) == 1);
  BOOST_CHECK (make_optional(0) != 1);
  BOOST_CHECK (make_optional(1) != 0);

  optional<int> oN {nullopt};
  optional<int> o0 {0};
  optional<int> o1 {1};

  BOOST_CHECK (o0 ==  0);
  BOOST_CHECK ( 0 == o0);
  BOOST_CHECK (o1 ==  1);
  BOOST_CHECK ( 1 == o1);
  BOOST_CHECK (o1 !=  0);
  BOOST_CHECK ( 0 != o1);
  BOOST_CHECK (o0 !=  1);
  BOOST_CHECK ( 1 != o0);

  BOOST_CHECK ( 1 != oN);
  BOOST_CHECK ( 0 != oN);
  BOOST_CHECK (oN !=  1);
  BOOST_CHECK (oN !=  0);
  BOOST_CHECK (!( 1 == oN));
  BOOST_CHECK (!( 0 == oN));
  BOOST_CHECK (!(oN ==  1));
  BOOST_CHECK (!(oN ==  0));

  std::string cat{"cat"}, dog{"dog"};
  optional<std::string> oNil{}, oDog{"dog"}, oCat{"cat"};

  BOOST_CHECK (oCat ==  cat);
  BOOST_CHECK ( cat == oCat);
  BOOST_CHECK (oDog ==  dog);
  BOOST_CHECK ( dog == oDog);
  BOOST_CHECK (oDog !=  cat);
  BOOST_CHECK ( cat != oDog);
  BOOST_CHECK (oCat !=  dog);
  BOOST_CHECK ( dog != oCat);

  BOOST_CHECK ( dog != oNil);
  BOOST_CHECK ( cat != oNil);
  BOOST_CHECK (oNil !=  dog);
  BOOST_CHECK (oNil !=  cat);
  BOOST_CHECK (!( dog == oNil));
  BOOST_CHECK (!( cat == oNil));
  BOOST_CHECK (!(oNil ==  dog));
  BOOST_CHECK (!(oNil ==  cat));
}

BOOST_AUTO_TEST_CASE(const_propagation)
{
  using namespace ural;

  optional<int> mmi{0};
  static_assert(std::is_same<decltype(*mmi), int&>::value, "WTF");

  const optional<int> cmi{0};
  static_assert(std::is_same<decltype(*cmi), const int&>::value, "WTF");

  optional<const int> mci{0};
  static_assert(std::is_same<decltype(*mci), const int&>::value, "WTF");

  const optional<const int> cci{0};
  static_assert(std::is_same<decltype(*cci), const int&>::value, "WTF");
}

static_assert(std::is_base_of<std::logic_error, ural::bad_optional_access>::value, "");

BOOST_AUTO_TEST_CASE(safe_value)
{
  using namespace ural;

  try {
    optional<int> ovN{}, ov1{1};

    int& r1 = ov1.value();
    BOOST_CHECK(r1 == 1);

    try {
      ovN.value();
      BOOST_CHECK (false);
    }
    catch (bad_optional_access const&) {
    }

    { // ref variant
      int i1 = 1;
      optional<int&> orN{}, or1{i1};

      int& r2 = or1.value();
      BOOST_CHECK (r2 == 1);

      try {
        orN.value();
        BOOST_CHECK (false);
      }
      catch (bad_optional_access const&) {
      }
    }
  }
  catch(...) {
    BOOST_CHECK (false);
  }
}

BOOST_AUTO_TEST_CASE(optional_ref)
{
  using namespace tr2;
  // FAILS: optional<int&&> orr;
  // FAILS: optional<nullopt_t&> on;
  int i = 8;
  optional<int&> ori;
  BOOST_CHECK (!ori);
  ori.emplace(i);
  BOOST_CHECK (bool(ori));
  BOOST_CHECK (*ori == 8);
  BOOST_CHECK (&*ori == &i);
  *ori = 9;
  BOOST_CHECK (i == 9);

  // FAILS: int& ir = ori.value_or(i);
  int ii = ori.value_or(i);
  BOOST_CHECK (ii == 9);
  ii = 7;
  BOOST_CHECK (*ori == 9);

  int j = 22;
  auto&& oj = make_optional(std::ref(j));
  *oj = 23;
  BOOST_CHECK (&*oj == &j);
  BOOST_CHECK (j == 23);
}

BOOST_AUTO_TEST_CASE(optional_ref_const_propagation)
{
  using namespace ural;

  int i = 9;
  const optional<int&> mi = i;
  int& r = *mi;
  optional<const int&> ci = i;
  static_assert(std::is_same<decltype(*mi), int&>::value, "WTF");
  static_assert(std::is_same<decltype(*ci), const int&>::value, "WTF");

  r = r;
}

BOOST_AUTO_TEST_CASE(optional_ref_assign)
{
  using namespace ural;

  int i = 9;
  optional<int&> ori = i;

  int j = 1;
  ori = optional<int&>{j};
  ori = {j};
  // FAILS: ori = j;

  optional<int&> orx = ori;
  ori = orx;

  optional<int&> orj = j;

  BOOST_CHECK (ori);
  BOOST_CHECK (*ori == 1);
  BOOST_CHECK (ori == orj);
  BOOST_CHECK (i == 9);

  *ori = 2;
  BOOST_CHECK (*ori == 2);
  BOOST_CHECK (ori == 2);
  BOOST_CHECK (2 == ori);
  BOOST_CHECK (ori != 3);

  BOOST_CHECK (ori == orj);
  BOOST_CHECK (j == 2);
  BOOST_CHECK (i == 9);

  ori = ural::nullopt;
  BOOST_CHECK (!ori);
  BOOST_CHECK (ori != orj);
  BOOST_CHECK (j == 2);
  BOOST_CHECK (i == 9);
}

BOOST_AUTO_TEST_CASE(optional_vector_swap_member)
{
    typedef std::vector<int> Type;
    Type const z1 = {1, 3, 5};
    Type const z2 = {2, 4};

    auto o1 = ural::make_optional(z1);
    auto o2 = ural::make_optional(z2);

    o1.swap(o2);

    BOOST_CHECK(o1 == z2);
    BOOST_CHECK(o2 == z1);
}

BOOST_AUTO_TEST_CASE(optional_vector_swap_free)
{
    typedef std::vector<int> Type;
    Type const z1 = {1, 3, 5};
    Type const z2 = {2, 4};

    auto o1 = ural::make_optional(z1);
    auto o2 = ural::make_optional(z2);

    using std::swap;
    swap(o1, o2);

    BOOST_CHECK(o1 == z2);
    BOOST_CHECK(o2 == z1);
}

BOOST_AUTO_TEST_CASE(optional_vector_swap_free_strict)
{
    typedef std::vector<int> Type;
    Type const z1 = {1, 3, 5};
    Type const z2 = {2, 4};

    auto o1 = ural::make_optional(z1);
    auto o2 = ural::make_optional(z2);

    ural::swap(o1, o2);

    BOOST_CHECK(o1 == z2);
    BOOST_CHECK(o2 == z1);
}

BOOST_AUTO_TEST_CASE(optional_ref_swap)
{
  using namespace ural;
  int i = 0;
  int j = 1;
  optional<int&> oi = i;
  optional<int&> oj = j;

  BOOST_CHECK (&*oi == &i);
  BOOST_CHECK (&*oj == &j);

  std::swap(oi, oj);
  BOOST_CHECK (&*oi == &j);
  BOOST_CHECK (&*oj == &i);
}

BOOST_AUTO_TEST_CASE(optional_ref_swap_member)
{
  using namespace ural;
  int i = 0;
  int j = 1;
  optional<int&> oi = i;
  optional<int&> oj = j;

  BOOST_CHECK (&*oi == &i);
  BOOST_CHECK (&*oj == &j);

  oi.swap(oj);
  BOOST_CHECK (&*oi == &j);
  BOOST_CHECK (&*oj == &i);
}

BOOST_AUTO_TEST_CASE(optional_value_swap_member)
{
  using namespace ural;
  int const i = 0;
  int const j = 1;
  optional<int> oi = i;
  optional<int> oj = j;

  BOOST_CHECK_EQUAL(i, *oi);
  BOOST_CHECK_EQUAL(j, *oj);

  oi.swap(oj);
  BOOST_CHECK_EQUAL(j, *oi);
  BOOST_CHECK_EQUAL(i, *oj);
}

BOOST_AUTO_TEST_CASE(optional_initialization)
{
    using namespace tr2;
    using std::string;
    string s = "STR";

    optional<string> os{s};
    optional<string> ot = s;
    optional<string> ou{"STR"};
    optional<string> ov = string{"STR"};

    BOOST_CHECK(os == s);
    BOOST_CHECK(ot == s);
    BOOST_CHECK(ou == s);
    BOOST_CHECK(ov == s);
}

#include <unordered_set>

BOOST_AUTO_TEST_CASE(optional_hashing)
{
    using namespace tr2;
    using std::string;

    std::hash<int> hi;
    std::hash<optional<int>> hoi;
    std::hash<string> hs;
    std::hash<optional<string>> hos;

    BOOST_CHECK (hi(0) == hoi(optional<int>{0}));
    BOOST_CHECK (hi(1) == hoi(optional<int>{1}));
    BOOST_CHECK (hi(3198) == hoi(optional<int>{3198}));

    BOOST_CHECK (hs("") == hos(optional<string>{""}));
    BOOST_CHECK (hs("0") == hos(optional<string>{"0"}));
    BOOST_CHECK (hs("Qa1#") == hos(optional<string>{"Qa1#"}));

    std::unordered_set<optional<string>> set;
    BOOST_CHECK(set.find({"Qa1#"}) == set.end());

    set.insert({"0"});
    BOOST_CHECK(set.find({"Qa1#"}) == set.end());

    set.insert({"Qa1#"});
    BOOST_CHECK(set.find({"Qa1#"}) != set.end());
}

/// @cond false
// optional_ref_emulation
template <class T>
struct generic
{
  typedef T type;
};

template <class U>
struct generic<U&>
{
  typedef std::reference_wrapper<U> type;
};

template <class X>
bool generic_fun()
{
  ural::optional<typename generic<X>::type> op;
  return bool(op);
}
/// @endcond

BOOST_AUTO_TEST_CASE(optional_ref_emulation)
{
  using namespace ural;
  optional<generic<int>::type> oi = 1;
  BOOST_CHECK (*oi == 1);

  int i = 8;
  int j = 4;
  optional<generic<int&>::type> ori {i};
  BOOST_CHECK (*ori == 8);

  // !DIFFERENT THAN optional<T&>
  BOOST_CHECK ((void*)&*ori != (void*)&i);

  *ori = j;
  BOOST_CHECK (*ori == 4);
}

BOOST_AUTO_TEST_CASE(moved_on_value_or)
{
  using namespace tr2;
  optional<Oracle> oo{inplace};

  BOOST_CHECK (oo);
  BOOST_CHECK (oo->s == sDefaultConstructed);

  Oracle o = std::move(oo).value_or( Oracle{OracleVal{}} );
  BOOST_CHECK (oo);
  BOOST_CHECK (oo->s == sMovedFrom);
  BOOST_CHECK (o.s == sMoveConstructed);

  optional<MoveAware<int>> om {inplace, 1};
  BOOST_CHECK (om);
  BOOST_CHECK (om->moved == false);

  MoveAware<int> m = std::move(om).value_or( MoveAware<int>{1} );

  BOOST_CHECK (om);
  BOOST_CHECK (om->moved == true);

  BOOST_CHECK(m.moved == false);
};

BOOST_AUTO_TEST_CASE(optional_ref_hashing)
{
    using namespace tr2;
    using std::string;

    std::hash<int> hi;
    std::hash<optional<int&>> hoi;
    std::hash<string> hs;
    std::hash<optional<string&>> hos;

    int i0 = 0;
    int i1 = 1;
    BOOST_CHECK (hi(0) == hoi(optional<int&>{i0}));
    BOOST_CHECK (hi(1) == hoi(optional<int&>{i1}));

    string s{""};
    string s0{"0"};
    string sCAT{"CAT"};
    BOOST_CHECK (hs("") == hos(optional<string&>{s}));
    BOOST_CHECK (hs("0") == hos(optional<string&>{s0}));
    BOOST_CHECK (hs("CAT") == hos(optional<string&>{sCAT}));

    std::unordered_set<optional<string&>> set;
    BOOST_CHECK(set.find({sCAT}) == set.end());

    set.insert({s0});
    BOOST_CHECK(set.find({sCAT}) == set.end());

    set.insert({sCAT});
    BOOST_CHECK(set.find({sCAT}) != set.end());
}

/// @cond false
struct Combined
{
  int m;
  int n;

  constexpr Combined() : m{5}, n{6} {}
  constexpr Combined(int m, int n) : m{m}, n{n} {}
};

struct Nasty
{
  int m;
  int n;

  constexpr Nasty() : m{5}, n{6} {}
  constexpr Nasty(int m, int n) : m{m}, n{n} {}

  int operator&() { return n; }
  int operator&() const { return n; }
};
/// @endcond

BOOST_AUTO_TEST_CASE(arrow_operator)
{
  using namespace ural;

  optional<Combined> oc1{inplace, 1, 2};
  BOOST_CHECK (oc1);
  BOOST_CHECK (oc1->m == 1);
  BOOST_CHECK (oc1->n == 2);

  optional<Nasty> on{inplace, 1, 2};
  BOOST_CHECK (on);
  BOOST_CHECK (on->m == 1);
  BOOST_CHECK (on->n == 2);
}

BOOST_AUTO_TEST_CASE(arrow_wit_optional_ref)
{
  using namespace ural;

  Combined c{1, 2};
  optional<Combined&> oc = c;
  BOOST_CHECK (oc);
  BOOST_CHECK (oc->m == 1);
  BOOST_CHECK (oc->n == 2);

  Nasty n{1, 2};
  Nasty m{3, 4};
  Nasty p{5, 6};

  optional<Nasty&> on{n};
  BOOST_CHECK (on);
  BOOST_CHECK (on->m == 1);
  BOOST_CHECK (on->n == 2);

  on = {m};
  BOOST_CHECK (on);
  BOOST_CHECK (on->m == 3);
  BOOST_CHECK (on->n == 4);

  on.emplace(p);
  BOOST_CHECK (on);
  BOOST_CHECK (on->m == 5);
  BOOST_CHECK (on->n == 6);

  optional<Nasty&> om{inplace, n};
  BOOST_CHECK (om);
  BOOST_CHECK (om->m == 1);
  BOOST_CHECK (om->n == 2);
}

//// constexpr tests

/// @cond false
// these 4 classes have different noexcept signatures in move operations
struct NothrowBoth {
  NothrowBoth(NothrowBoth&&) noexcept(true) {};
  void operator=(NothrowBoth&&) noexcept(true) {};
};
struct NothrowCtor {
  NothrowCtor(NothrowCtor&&) noexcept(true) {};
  void operator=(NothrowCtor&&) noexcept(false) {};
};
struct NothrowAssign {
  NothrowAssign(NothrowAssign&&) noexcept(false) {};
  void operator=(NothrowAssign&&) noexcept(true) {};
};
struct NothrowNone {
  NothrowNone(NothrowNone&&) noexcept(false) {};
  void operator=(NothrowNone&&) noexcept(false) {};
};

struct Previous_declarator
{
    static void test_noexcept()
    {
      {
          static_assert(std::is_nothrow_move_assignable<NothrowBoth>::value, "WTF!");
          static_assert(std::is_nothrow_move_constructible<NothrowBoth>::value, "WTF!");

        tr2::optional<NothrowBoth> b1, b2;
        static_assert(noexcept(tr2::optional<NothrowBoth>{std::move(b1)}), "bad noexcept!");
        static_assert(noexcept(b1 = std::move(b2)), "bad noexcept!");
      }
      {
        tr2::optional<NothrowCtor> c1, c2;
        static_assert(noexcept(tr2::optional<NothrowCtor>{std::move(c1)}), "bad noexcept!");
        static_assert(!noexcept(c1 = std::move(c2)), "bad noexcept!");
      }
      {
        tr2::optional<NothrowAssign> a1, a2;
        static_assert(!noexcept(tr2::optional<NothrowAssign>{std::move(a1)}), "bad noexcept!");
        static_assert(!noexcept(a1 = std::move(a2)), "bad noexcept!");
      }
      {
        tr2::optional<NothrowNone> n1, n2;
        static_assert(!noexcept(tr2::optional<NothrowNone>{std::move(n1)}), "bad noexcept!");
        static_assert(!noexcept(n1 = std::move(n2)), "bad noexcept!");
      }
    }


    static void constexpr_test_disengaged()
    {
        static_assert(std::is_trivially_destructible<int>::value,
                      "int must have trivial destructor");
        constexpr ural::details::optional_base_constexpr<int> ob0{};

        constexpr tr2::optional<int> g0{};
        constexpr tr2::optional<int> g1{tr2::nullopt};
        static_assert( !g0, "initialized!" );
        static_assert( !g1, "initialized!" );

        static_assert( bool(g1) == bool(g0), "ne!" );

        static_assert( g1 == g0, "ne!" );
        static_assert( !(g1 != g0), "ne!" );
        static_assert( g1 >= g0, "ne!" );
        static_assert( !(g1 > g0), "ne!" );
        static_assert( g1 <= g0, "ne!" );
        static_assert( !(g1 < g0), "ne!" );

        static_assert( g1 == tr2::nullopt, "!" );
        static_assert( !(g1 != tr2::nullopt), "!" );
        static_assert( g1 <= tr2::nullopt, "!" );
        static_assert( !(g1 < tr2::nullopt), "!" );
        static_assert( g1 >= tr2::nullopt, "!" );
        static_assert( !(g1 > tr2::nullopt), "!" );

        static_assert(  (tr2::nullopt == g0), "!" );
        static_assert( !(tr2::nullopt != g0), "!" );
        static_assert(  (tr2::nullopt >= g0), "!" );
        static_assert( !(tr2::nullopt >  g0), "!" );
        static_assert(  (tr2::nullopt <= g0), "!" );
        static_assert( !(tr2::nullopt <  g0), "!" );

        static_assert(  (g1 != tr2::optional<int>(1)), "!" );
        static_assert( !(g1 == tr2::optional<int>(1)), "!" );
        static_assert(  (g1 <  tr2::optional<int>(1)), "!" );
        static_assert(  (g1 <= tr2::optional<int>(1)), "!" );
        static_assert( !(g1 >  tr2::optional<int>(1)), "!" );
        static_assert( !(g1 >  tr2::optional<int>(1)), "!" );
    }
};

constexpr tr2::optional<int> g0{};
constexpr tr2::optional<int> g2{2};
static_assert( g2, "not initialized!" );
static_assert( *g2 == 2, "not 2!" );
static_assert( g2 == tr2::optional<int>(2), "not 2!" );
static_assert( g2 != g0, "eq!" );

constexpr tr2::optional<Combined> gc0{tr2::inplace};
static_assert(gc0->n == 6, "WTF!");

// optional refs
int gi = 0;
constexpr tr2::optional<int&> gori = gi;
constexpr tr2::optional<int&> gorn{};
constexpr int& gri = *gori;
static_assert(gori, "WTF");
static_assert(!gorn, "WTF");
static_assert(gori != tr2::nullopt, "WTF");
static_assert(gorn == tr2::nullopt, "WTF");
static_assert(&gri == &*gori, "WTF");

constexpr int gci = 1;
constexpr tr2::optional<int const&> gorci = gci;
constexpr tr2::optional<int const&> gorcn{};

static_assert(gorcn <  gorci, "WTF");
static_assert(gorcn <= gorci, "WTF");
static_assert(gorci == gorci, "WTF");
static_assert(*gorci == 1, "WTF");
static_assert(gorci == gci, "WTF");

namespace constexpr_optional_ref_and_arrow
{
  using namespace ural;
  constexpr Combined c{1, 2};
  constexpr optional<Combined const&> oc = c;
  static_assert(oc, "WTF!");
  static_assert(oc->m == 1, "WTF!");
  static_assert(oc->n == 2, "WTF!");
}

#include <ural/utility/tracers.hpp>

BOOST_AUTO_TEST_CASE(optional_test)
{
    typedef int Basic_type;
    typedef ural::regular_tracer<Basic_type, ural::single_thread_policy> Type;

    //Конструктор без параметров: конструктор и деструктор объекта не вызывались
    auto const destroyed_old = Type::destroyed_objects();
    {
        ural::optional<Type> x0;

        BOOST_CHECK(!x0);

        BOOST_CHECK_EQUAL(0U, Type::active_objects());
    }
    BOOST_CHECK_EQUAL(destroyed_old, Type::destroyed_objects());

    // Конструктор с аргументом: вызываются конструктор и деструктор
    {
        ural::optional<Type> x0{ural::inplace, 42};

        BOOST_CHECK(!!x0);

        BOOST_CHECK_EQUAL(1U, Type::active_objects());
    }
    BOOST_CHECK_EQUAL(destroyed_old + 1, Type::destroyed_objects());
}

BOOST_AUTO_TEST_CASE(optional_throw_test)
{
    typedef std::vector<std::string> Type;
    ural::optional<Type> x0{ural::nullopt};
    BOOST_CHECK_THROW(x0.value(), std::logic_error);

    ural::optional<Type&> x_def;
    BOOST_CHECK_THROW(x_def.value(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(optional_bad_access_test)
{
    ural::optional<int> x;
    ural::optional<int&> y;

    BOOST_CHECK_THROW(x.value(), ural::bad_optional_access);
    BOOST_CHECK_THROW(y.value(), ural::bad_optional_access);
}

BOOST_AUTO_TEST_CASE(optional_int_test)
{
    typedef int Type;

    URAL_CONCEPT_ASSERT(ural::optional<Type>, ural::concepts::Regular);

    // Конструктор копирования
    {
        ural::optional<Type> x0{ural::nullopt};
        ural::optional<Type> x1{13};
        ural::optional<Type> const x2{42};

        BOOST_CHECK(!x0);
        BOOST_CHECK(!!x1);
        BOOST_CHECK(!!x2);

        BOOST_CHECK(nullptr == x0.get_pointer());
        BOOST_CHECK_EQUAL(13, x1.value());
        BOOST_CHECK_EQUAL(42, x2.value());

        BOOST_CHECK_EQUAL(x0, x0);
        BOOST_CHECK_EQUAL(x1, x1);
        BOOST_CHECK_EQUAL(x2, x2);
        BOOST_CHECK(x0 != x1);
        BOOST_CHECK(x2 != x1);

        auto x0_c = x0;
        auto x1_c = x1;

        BOOST_CHECK(!x0_c);
        BOOST_CHECK(!!x1_c);
    }
}

BOOST_AUTO_TEST_CASE(optional_none_assign)
{
    typedef std::string Type;
    ural::optional<Type> x0;
    ural::optional<Type> x1{"42"};

    x0 = ural::nullopt;
    x1 = ural::nullopt;

    BOOST_CHECK(!x0);
    BOOST_CHECK(!x1);
}

BOOST_AUTO_TEST_CASE(optional_move_ctor_and_assignment_test)
{
    typedef std::string Type;

    std::string const s("hello, world");

    ural::optional<Type> x0;
    ural::optional<Type> x1(s);
    ural::optional<Type> x2(std::move(x1));
    ural::optional<Type> x3(std::move(x0));

    BOOST_CHECK(!x0);
    BOOST_CHECK(!!x1);
    BOOST_CHECK(!x3);
    BOOST_CHECK_EQUAL(s, x2.value());

    x3 = std::string("abc");
    BOOST_CHECK_EQUAL("abc", x3.value());

    x0 = std::move(x2);
    x1 = std::move(x3);
    BOOST_CHECK_EQUAL("hello, world", x0.value());
    BOOST_CHECK_EQUAL("abc", x1.value());
    BOOST_CHECK(!!x2);
    BOOST_CHECK(!!x3);

    x2 = x1;
    x3 = x0;
    BOOST_CHECK_EQUAL(x2.value(), x1.value());
    BOOST_CHECK_EQUAL(x3.value(), x0.value());
}

BOOST_AUTO_TEST_CASE(optional_assign_value_test)
{
    typedef std::string Type;

    std::string hw = "Hello, world!";
    ural::optional<Type> x0;
    x0 = hw;
    BOOST_CHECK_EQUAL(hw, x0);
    BOOST_CHECK_EQUAL(x0, hw);

    hw = "BSHS";
    x0 = hw;
    BOOST_CHECK_EQUAL(hw, x0.value());
}

BOOST_AUTO_TEST_CASE(optional_ostreaming)
{
    std::string hw = "Hello, world!";
    ural::optional<std::string> const x {hw};

    std::ostringstream os;
    os << x;

    std::ostringstream z;
    z << "{" << hw << "}";

    BOOST_CHECK_EQUAL(z.str(), os.str());
}

BOOST_AUTO_TEST_CASE(optional_ostreaming_empty)
{
    ural::optional<std::string> const x = ural::nullopt;

    std::ostringstream os;
    os << x;

    BOOST_CHECK_EQUAL("{}", os.str());
}

BOOST_AUTO_TEST_CASE(optional_less_operator_test)
{
    ural::optional<std::string> x0;
    ural::optional<std::string> x1("abc");
    ural::optional<std::string> x2("hellow");

    BOOST_CHECK(!(x0 < x0));
    BOOST_CHECK(!(x1 < x0));
    BOOST_CHECK(x0 < x1);
    BOOST_CHECK(x0 < x2);
    BOOST_CHECK(x1 < x2);
    BOOST_CHECK(x0 < x1.value());
    BOOST_CHECK(x0 < x2.value());
    BOOST_CHECK(x1.value() < x2.value());
    BOOST_CHECK(x1 < x2.value());
    BOOST_CHECK(x1.value() < x2);
    BOOST_CHECK(!(x2 < x1));
}

BOOST_AUTO_TEST_CASE(optional_less_or_equal_operator_test)
{
    ural::optional<std::string> x0;
    ural::optional<std::string> x1("abc");
    ural::optional<std::string> x2("hellow");

    BOOST_CHECK(x0 <= x0);
    BOOST_CHECK(!(x1 <= x0));
    BOOST_CHECK(x0 <= x1);
    BOOST_CHECK(x0 <= x2);
    BOOST_CHECK(x1 <= x2);
    BOOST_CHECK(x0 <= x1.value());
    BOOST_CHECK(x0 <= x2.value());
    BOOST_CHECK(x1.value() <= x2.value());
    BOOST_CHECK(x1 <= x2.value());
    BOOST_CHECK(x1.value() <= x2);
    BOOST_CHECK(!(x2 <= x1));
}

BOOST_AUTO_TEST_CASE(optional_greater_operator_test)
{
    ural::optional<std::string> x0;
    ural::optional<std::string> x1("abc");
    ural::optional<std::string> x2("hellow");

    BOOST_CHECK(!(x0 > x0));
    BOOST_CHECK(!(x0 > x1));
    BOOST_CHECK(x1 > x0);
    BOOST_CHECK(x2 > x0);
    BOOST_CHECK(x2 > x1);
    BOOST_CHECK(x1.value() > x0);
    BOOST_CHECK(x2.value() > x0);
    BOOST_CHECK(x2.value() > x1.value());
    BOOST_CHECK(x2 > x1.value());
    BOOST_CHECK(x2.value() > x1);
    BOOST_CHECK(x2 > x1);
}

BOOST_AUTO_TEST_CASE(optional_greater_or_equal_operator_test)
{
    ural::optional<std::string> x0;
    ural::optional<std::string> x1("abc");
    ural::optional<std::string> x2("hellow");

    BOOST_CHECK(x0 >= x0);
    BOOST_CHECK(!(x0 >= x1));
    BOOST_CHECK(x1 >= x0);
    BOOST_CHECK(x2 >= x0);
    BOOST_CHECK(x2 >= x1);
    BOOST_CHECK(x1.value() >= x0);
    BOOST_CHECK(x2.value() >= x0);
    BOOST_CHECK(x2.value() >= x1.value());
    BOOST_CHECK(x2 >= x1.value());
    BOOST_CHECK(x2.value() >= x1);
    BOOST_CHECK(x2 >= x1);
}

BOOST_AUTO_TEST_CASE(optional_value_or_test)
{
    ural::optional<int> x0;
    ural::optional<int> x1(42);

    BOOST_CHECK_EQUAL(-1, x0.value_or(-1));
    BOOST_CHECK_EQUAL(42, x1.value_or(-1));
}

BOOST_AUTO_TEST_CASE(optional_ref_default_init_test)
{
    ural::optional<int&> x0;

    BOOST_CHECK(ural::empty(x0));
    BOOST_CHECK(!x0);
    BOOST_CHECK(!x0.get_pointer());

    ural::optional<int&> x1(ural::nullopt);

    BOOST_CHECK(ural::empty(x1));
    BOOST_CHECK(!x1);
    BOOST_CHECK(!x1.get_pointer());
}

BOOST_AUTO_TEST_CASE(optional_ref_value_init_test)
{
    typedef int Type;
    typedef Type & Ref;

    Type value = 42;
    Ref  r_value = value;

    ural::optional<Ref> x1(value);
    ural::optional<Ref> x2(r_value);

    BOOST_CHECK(!ural::empty(x1));
    BOOST_CHECK(!!x1);
    BOOST_CHECK(!!x1.get_pointer());
    BOOST_CHECK_EQUAL(&value, x2.get_pointer());
    BOOST_CHECK_EQUAL(value, x2.value());

    BOOST_CHECK(!ural::empty(x2));
    BOOST_CHECK(!!x2);
    BOOST_CHECK(!!x2.get_pointer());
    BOOST_CHECK_EQUAL(&r_value, x1.get_pointer());
    BOOST_CHECK_EQUAL(r_value, x1.value());
}

BOOST_AUTO_TEST_CASE(optional_ref_assign_value_test)
{
    typedef ural::optional<int&> Optional;
    int var = 42;

    Optional x0;
    x0 = var;

    BOOST_CHECK(!!x0);
    BOOST_CHECK_EQUAL(&var, x0.get_pointer());
    BOOST_CHECK_EQUAL(var, x0.value());

    x0 = ural::nullopt;

    BOOST_CHECK(!x0);
    BOOST_CHECK(nullptr == x0.get_pointer());
}

BOOST_AUTO_TEST_CASE(optional_ref_assign_test)
{
    typedef ural::optional<int&> Optional;
    int var = 42;

    Optional x0;
    Optional x0_1;
    Optional x0_2;
    Optional x1(var);
    Optional x1_1(var);
    Optional x1_2(var);

    x0_1 = x0;
    x0_2 = x1;
    x1_1 = x0;
    x1_2 = x1;

    BOOST_CHECK(!x0_1);
    BOOST_CHECK(!x1_1);

    BOOST_CHECK(!!x0_2);
    BOOST_CHECK_EQUAL(&var, x0_2.get_pointer());

    BOOST_CHECK(!!x1_2);
    BOOST_CHECK_EQUAL(&var, x1_2.get_pointer());
}

BOOST_AUTO_TEST_CASE(optional_inplace_ctor)
{
    typedef std::vector<int> Type;

    typedef ural::optional<Type> Optional;

    size_t const n = 10;
    auto const filler = 42;

    Optional const y{ural::inplace, n, filler};

    BOOST_CHECK(!!y);
    BOOST_CHECK_EQUAL(n, y.value().size());

    BOOST_CHECK(ural::all_of(*y, [=](int x){ return x == filler; }));
}

BOOST_AUTO_TEST_CASE(optional_emplace_test)
{
    typedef std::vector<int> Type;

    typedef ural::optional<Type> Optional;

    Optional x;
    x.emplace(5, 2);

    Type const z(5, 2);

    BOOST_CHECK(!!x);
    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  x.value().begin(), x.value().end());
}

/* Основано на github.com/akrzemi1/Optional/blob/master/test_type_traits.cpp
*/

BOOST_AUTO_TEST_CASE(optional_type_traits_test)
{
    struct Val
    {
      Val(){}
      Val( Val const & ){}
      Val( Val && ) noexcept {}

      Val & operator=( Val const & ) = delete;
      Val & operator=( Val && ) noexcept = delete;
    };

    struct Safe
    {
        Safe(){}
        Safe( Safe const & ){}
        Safe( Safe && ) noexcept {}

        Safe & operator=( Safe const & ){ return *this; }
        Safe & operator=( Safe && ) noexcept { return *this; }
    };

    struct Unsafe
    {
        Unsafe(){}
        Unsafe( Unsafe const & ){}
        Unsafe( Unsafe && ){}

        Unsafe & operator=( Unsafe const & ){ return *this; }
        Unsafe & operator=( Unsafe && ) { return *this; }
    };


    static_assert(std::is_nothrow_move_constructible<Safe>::value, "WTF!");
    static_assert(!std::is_nothrow_move_constructible<Unsafe>::value, "WTF!");

    static_assert(std::is_assignable<Safe&, Safe&&>::value, "WTF!");
    static_assert(!std::is_assignable<Val&, Val&&>::value, "WTF!");

    static_assert(std::is_nothrow_move_assignable<Safe>::value, "WTF!");
    static_assert(!std::is_nothrow_move_assignable<Unsafe>::value, "WTF!");
}
// end constexpr tests
/// @endcond
