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

#include <iomanip>
#include <iostream>
#include <set>

#include "quickcheck/quickcheck.hh"

using namespace quickcheck;

typedef std::vector<int> IntVec;

class OneOfProperty : public Property<IntVec> {
   bool accepts(const IntVec& xs) {
      return !xs.empty();
   }
};

class POneOfTakesFromGivenSet : public OneOfProperty {
   bool holdsFor(const IntVec& xs) {
      int x = oneOf<int>(xs.begin(), xs.end());
      return std::count(xs.begin(), xs.end(), x) > 0;
   }
};

class POneOfCanSelectAny : public OneOfProperty {
   /**
    * Specifies that oneOf can select any element of the given sequence. This
    * probabilistic test calls #oneOf many times while recording seen values.
    * It then checks that all values in sequence have been seen.
    *
    * \warning this test is probabilistic and may fail even if #oneOf is valid
    * \warning this test is slow (\c O(n^2) where \c n is the sequence size)
    *
    * \param xs the non-empty input sequence
    *
    * \return \c true if all values of \c xs were returned by calls to \c oneOf
    */
   bool holdsFor(const IntVec& xs) {
      std::set<int> seenValues;
      for (size_t i = 0; i < xs.size() * 100 && seenValues.size() < xs.size();
           ++i)
         seenValues.insert(oneOf<int>(xs.begin(), xs.end()));
      for (IntVec::const_iterator i = xs.begin(); i != xs.end(); ++i)
         if (seenValues.count(*i) == 0)
            return false;
      return true;
   };
   const std::string classify(const IntVec& xs) {
      for (IntVec::const_iterator i = xs.begin(); i != xs.end(); ++i)
         if (std::count(xs.begin(), xs.end(), *i) > 1)
            return "with duplicates";
      return "without duplicates";
   }
};

class PGenerateInRange : public Property<int, int> {
   bool holdsFor(const int& i1, const int& i2) {
      int low  = (i1 < i2) ? i1 : i2;
      int high = (i1 < i2) ? i2 : i1;
      int i = generateInRange(low, high);
      return low <= i && i <= high;
   }
};

static unsigned nTests;
static unsigned nSuccessfulTests;

template<class Prop>
static void countedCheck(const char *msg)
{
   if (check<Prop>(msg))
      ++nSuccessfulTests;
   ++nTests;
}

int main()
{
   countedCheck<POneOfTakesFromGivenSet>("oneOf takes from given set");
   countedCheck<POneOfCanSelectAny>("oneOf can select any");
   countedCheck<PGenerateInRange>(
      "generateInRange generates elements in the range");
   std::cout << std::setfill('-') << std::setw(80) << '-' << std::endl;
   std::cout << "* Summary: " << nSuccessfulTests << " of " << nTests
      << " tests passed." << std::endl;
   std::cout << std::setfill('-') << std::setw(80) << '-' << std::endl;
   if (nTests == nSuccessfulTests)
      return 0; // OK
   else
      return 1; // bad thing occured
}
