#include "distro.hpp"
#include "tabdistro.hpp"
#include "test.hpp"

#include <sstream>
#include <string>

using namespace std;


int
main()
{
  EXPECT(tabdistro::how_many(), 0);
  tabdistro d;
  EXPECT(tabdistro::how_many(), 1);

  // The most probable value is zero, and this is the only value now.
  EXPECT(d.size(), 1);

  // The probability of 0.
  EXPECT(d.get_prob(0), 1.0);

  // The probability of 1.
  EXPECT(d.get_prob(1), 0.0);

  // We can't insert the probability for 0.
  EXPECT(d.set_prob(prob_pair(0.0, 0)), false);

  // Insert (0.75, 8)
  EXPECT(d.set_prob(prob_pair(0.75, 8)), true);

  // The probability of 8.
  EXPECT(d.get_prob(8), 0.75);

  // The probability of 0.
  EXPECT(d.get_prob(0), 0.25);

  // There are now two probability entries.
  EXPECT(d.size(), 2);

  // Now we can't insert another probability for 8.
  EXPECT(d.set_prob(prob_pair(0.25, 8)), false);

  // Can't insert again (0.75, 8)
  EXPECT(d.set_prob(prob_pair(0.75, 8)), false);

  // There are still two values.
  EXPECT(d.size(), 2);

  // We know what the first value should be.
  EXPECT(d.get_ith_max(0).first, 0.75);
  EXPECT(d.get_ith_max(0).second, 8);

  // We know what the second value should be.
  EXPECT(d.get_ith_max(1).first, 0.25);
  EXPECT(d.get_ith_max(1).second, 0);

  // Get the mean
  double mint = 0;
  mint += double(0.75) * int (8);
  EXPECT(d.mean(), mint);

  // Insert (0.25, 4)
  EXPECT(d.set_prob(make_pair(0.25, 4)), true);

  // The probability of 8.
  EXPECT(d.get_prob(8), 0.75);

  // The probability of 4.
  EXPECT(d.get_prob(4), 0.25);

  // The probability of 0.
  EXPECT(d.get_prob(0), 0.0);

  // The probability of 10.
  EXPECT(d.get_prob(10), 0.0);

  // There are two prob pairs now, because there's no pair for 0.
  EXPECT(d.size(), 2);

  // We know what the first value should be now.
  EXPECT(d.get_ith_max(0).first, 0.75);
  EXPECT(d.get_ith_max(0).second, 8);

  // We know what the second value should be now.
  EXPECT(d.get_ith_max(1).first, 0.25);
  EXPECT(d.get_ith_max(1).second, 4);

  // Get the mean
  mint += double(0.25) * int (4);
  EXPECT(d.mean(), mint);

  // Now test the "<<" operator on the d distro.
  ostringstream out;
  out << d;
  EXPECT(out.str(), "tabdistro(7)");

  // The distribution in string
  EXPECT(d.to_tabular(),
         "0 0\n1 0\n2 0\n3 0\n4 0.25\n"
         "5 0\n6 0\n7 0\n8 0.75\n");

  {
    // Check the convolution of two distributions d.
    tabdistro conv = d + d;
    EXPECT(conv.get_ith_max(0).first, 0.5625);
    EXPECT(conv.get_ith_max(0).second, 16);

    EXPECT(conv.get_ith_max(1).first, 0.3750);
    EXPECT(conv.get_ith_max(1).second, 12);

    EXPECT(conv.get_ith_max(2).first, 0.0625);
    EXPECT(conv.get_ith_max(2).second, 8);

    EXPECT(conv.size(), 3);
  }

  {
    // Empty distribution
    tabdistro c;
    EXPECT(tabdistro::how_many(), 2);

    // Convolution of the empty tabdistro and a non-empty tabdistro.
    tabdistro conv = c + d;

    EXPECT(conv, d);
  }
  EXPECT(tabdistro::how_many(), 1);

  // Test how the tabdistro is enveloped.
  {
    distro d1 = d;
    EXPECT(distro::how_many(), 1);
    EXPECT(tabdistro::how_many(), 2);
    distro d2 = d1;
    EXPECT(distro::how_many(), 2);
    EXPECT(tabdistro::how_many(), 3);
  }
  EXPECT(distro::how_many(), 0);
  EXPECT(tabdistro::how_many(), 1);

  // Test the += operator for tabdistro.
  {
    distro d1 = tabdistro();
    distro d2 = tabdistro();
    d1 += d2;
    EXPECT(d1.mean(), 0.0);
  }

  // Test the += operator for tabdistro.
  {
    tabdistro t;
    t.set_prob(make_pair(0.5, 2));
    EXPECT(t.mean(), 1.0);

    t += t;
    EXPECT(t.mean(), 2.0);
    EXPECT(t.size(), 3);
  }

  // Test the += operator for distros of tabdistros.
  {
    tabdistro t;
    t.set_prob(make_pair(0.5, 2));
    EXPECT(t.mean(), 1.0);

    distro d(t);
    d += d;
    EXPECT(d.mean(), 2.0);
  }

  // Test the + operator for tabdistros.
  {
    tabdistro t1;
    t1.set_prob(make_pair(0.75, 2));
    EXPECT(t1.mean(), 1.5);

    tabdistro t2;
    t2.set_prob(make_pair(0.75, 1));
    EXPECT(t2.mean(), 0.75);

    tabdistro s1 = t1 + t2;
    EXPECT(s1.mean(), 2.25);

    tabdistro s2 = t2 + t1;
    EXPECT(s2.mean(), 2.25);
  }

  // Test the += operator for distros of tabdistros.
  {
    tabdistro t1;
    t1.set_prob(make_pair(0.75, 2));
    EXPECT(t1.mean(), 1.5);

    tabdistro t2;
    t2.set_prob(make_pair(0.75, 1));
    EXPECT(t2.mean(), 0.75);

    {
      distro d1(t1);
      distro d2(t2);
      d1 += d2;
      EXPECT(d1.mean(), 2.25);
    }

    {
      distro d1(t1);
      distro d2(t2);
      d2 += d1;
      EXPECT(d2.mean(), 2.25);
    }
  }

  // Test the += operator for distros of tabdistros and nodistro.
  {
    tabdistro t;
    t.set_prob(make_pair(0.5, 2));
    EXPECT(t.mean(), 1.0);

    {
      distro d1 = nodistro();
      distro d2(t);

      d1 += d2;
      EXPECT(d1.mean(), 1.0);
    }

    {
      distro d1 = nodistro();
      distro d2(t);

      d2 += d1;
      EXPECT(d2.mean(), 1.0);
    }
  }


  return 0;
}
