/*
 * Copyright (C) 2009  Cyril Soldani
 * 
 * This file is part of QuickCheck++.
 * 
 * QuickCheck++ is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * QuickCheck++ is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * QuickCheck++. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include "quickcheck/quickcheck.hh"

using namespace quickcheck;

// some definitions missing from the examples in the documentation

typedef int Elem;
typedef std::vector<int> Vector;
using std::string;

template<class A>
std::string toString(A a)
{
   std::stringstream ss;
   ss << a;
   return ss.str();
}

static void reverse(Vector& xs)
{
   std::reverse(xs.begin(), xs.end());
}

static bool isSorted(const Vector& xs)
{
   if (xs.empty())
      return true;
   int prev = xs[0];
   for (size_t i = 0; i < xs.size(); ++i)
      if (prev > xs[i])
         return false;
      else
         prev = xs[i];
   return true;
}

static void insert(Elem x, Vector& xs)
{
   Vector::iterator pos = std::lower_bound(xs.begin(), xs.end(), x);
   xs.insert(pos, x);
}

// and now the exact code from the documentation

class PReverseCancelsReverse : public Property<Vector> {
   bool holdsFor(const Vector& xs) {
      Vector ys = xs;
      reverse(ys);
      reverse(ys);
      return xs == ys;
   }
};

class PReverseCancelsReverse2 : public Property<Vector> {
   bool holdsFor(const Vector& xs) {
      Vector ys = xs;
      reverse(ys);
      reverse(ys);
      return xs == ys;
   }
   bool isTrivialFor(const Vector& xs) {
      return xs.size() < 2;
   }
};

class PReverseIsIdentity : public Property<Vector> {
   bool holdsFor(const Vector& xs) {
      Vector ys = xs;
      reverse(ys);  // reverse done only once!
      return xs == ys;
   }
};

class PInsertKeepsSorted : public Property<Elem, Vector> {
   bool holdsFor(const Elem& x, const Vector& xs) {
      Vector ys = xs;
      insert(x, ys);
      return isSorted(ys);
   }
};

class PInsertKeepsSorted2 : public Property<Elem, Vector> {
   bool holdsFor(const Elem& x, const Vector& xs) {
      Vector ys = xs;
      insert(x, ys);
      return isSorted(ys);
   }
   bool accepts(const Elem&, const Vector& xs) {
      return isSorted(xs);
   }
};

class PInsertKeepsSorted3 : public Property<Elem, Vector> {
   bool holdsFor(const Elem& x, const Vector& xs) {
      Vector ys = xs;
      insert(x, ys);
      return isSorted(ys);
   }
   bool accepts(const Elem&, const Vector& xs) {
      return isSorted(xs);
   }
   const string classify(const Elem& x, const Vector& xs) {
      if (xs.empty())
         return "in empty";
      if (x < xs[0])
         return "at head";
      if (x > xs.back())
         return "at end";
      return "inside";
   }
};

class PInsertKeepsSorted4 : public Property<Elem, Vector> {
   bool holdsFor(const Elem& x, const Vector& xs) {
      Vector ys = xs;
      insert(x, ys);
      return isSorted(ys);
   }
   bool accepts(const Elem&, const Vector& xs) {
      return isSorted(xs);
   }
   const string classify(const Elem& x, const Vector& xs) {
      string s = toString(xs.size());
      if (xs.empty() || x <= xs[0])
         s += ", at head";
      if (xs.empty() || x >= xs.back())
         s += ", at end";
      if (!xs.empty() && x >= xs[0] && x <= xs.back())
         s += ", inside";
      return s;
   }
};

class PInsertKeepsSorted5 : public Property<Elem, Vector> {
   bool holdsFor(const Elem& x, const Vector& xs) {
      Vector ys = xs;
      insert(x, ys);
      return isSorted(ys);
   }
   // no need for the predicate anymore as we generate only valid input
   // bool accepts(const Elem& x, const Vector& xs) {
   //    return isSorted(xs);
   // }
   const string classify(const Elem& x, const Vector& xs) {
      string s = toString(xs.size());
      if (xs.empty() || x <= xs[0])
         s += ", at head";
      if (xs.empty() || x >= xs.back())
         s += ", at end";
      if (!xs.empty() && x >= xs[0] && x <= xs.back())
         s += ", inside";
      return s;
   }
   void generateInput(size_t n, Elem& x, Vector& xs) {
      generate(n, x);
      generate(n, xs);
      sort(xs.begin(), xs.end());
   }
};

struct Point {
   int x;
   int y;
};

void generate(size_t n, Point& out);
void generate(size_t n, Point& out)
{  
   generate(n, out.x); // no need to decrease n here, as int is not composite
   generate(n, out.y);
}

static int getTheAnswer(int i)
{  
   while (i < 0) {
      // eat CPU cycles until interrupted
   }
   return 42;
}

class PBottom : public Property<int> {
   bool holdsFor(const int& i) {
      return getTheAnswer(i) == 42;
   }
};

static unsigned iterativeSquare(unsigned n)
{
   unsigned res = 0;
   for (unsigned i = 0; i < n; ++i)
      res += n;
   return res;
}

class PSquareDivisibleByRoot : public Property<unsigned> {
   bool holdsFor(const unsigned& i) {
      return unsigned(sqrt(iterativeSquare(i))) == i;
   }
};

int main()
{
   PReverseCancelsReverse revRev;
   revRev.check();
   revRev.check(300);

   PReverseIsIdentity revId;
   revId.check();

   PInsertKeepsSorted insertKeepsSorted;
   insertKeepsSorted.check();

   PInsertKeepsSorted2 insertKeepsSorted2;
   insertKeepsSorted2.check();
   insertKeepsSorted2.check(100, 2000);

   PReverseCancelsReverse2 revRev2;
   revRev2.check();

   PInsertKeepsSorted3 insertKeepsSorted3;
   insertKeepsSorted3.check(100, 2000);

   PInsertKeepsSorted4 insertKeepsSorted4;
   insertKeepsSorted4.check(100, 2000);

   PInsertKeepsSorted5 insertKeepsSorted5;
   insertKeepsSorted5.check();

   PSquareDivisibleByRoot squareDivisibleByRoot;
   squareDivisibleByRoot.check();
   squareDivisibleByRoot.addFixed(std::numeric_limits<unsigned>::max());
   squareDivisibleByRoot.check();

   check<PReverseCancelsReverse>("that reverse cancels reverse");
   check<PReverseCancelsReverse>("that reverse cancels reverse", 200);
   check<PReverseCancelsReverse>("that reverse cancels reverse", 200, 600);
   check<PReverseIsIdentity>("that reverse is identity");

   std::cout
      << "!!! Following check will loop until interrupted (press Ctrl-C)..."
      << std::endl << std::endl;
   check<PBottom>("this property is stupid", 100, 0, true);

   return 0;
}
