#include "graph.hpp"
#include "poisson.hpp"
#include "polynomial.hpp"
#include "test.hpp"
#include "utils.hpp"

// This tests the tpoly class.  We operate here on the polynomials
// with integer coefficients, so that it's easier to test the class.

int
main()
{
  // We do all the testing for a given S == 7.
  tpoly<int>::set_S(7);

  // Test that assigning coefficients work.
  {
    tpoly<int> p1(1);
    p1[5] = 2;

    tpoly<int> p2;
    p2[0] = 1;
    p2(2, 5);

    EXPECT(p1, p2);
  }

  // Test the term function.
  {
    tpoly<int> p1(10);
    tpoly<int> p2;

    p2(10, 0);
    // This term should be rejected.
    p2(10, 7);

    EXPECT(p1, p2);
  }

  // Test the copy constructor of tpoly<int>.
  {
    tpoly<int> p;
    p(3, 1)(1, 0);

    tpoly<int> p2(p);
    EXPECT(p.size(), p2.size());
    EXPECT(p[0], p2[0]);
    EXPECT(p[1], p2[1]);
  }

  // Test the copy constructor of fp_poly.
  {
    fp_poly p;
    p(3.0, 1)(1.0, 0);

    fp_poly p2(p);
    EXPECT(p.size(), p2.size());
    EXPECT(p[0], p2[0]);
    EXPECT(p[1], p2[1]);
  }

  // Test the copy constructor of dist_poly.
  {
    dist_poly p;
    p(distro(poisson(3.0)), 1)(distro(poisson(1.0)), 0);
    EXPECT(poisson::how_many(), 2);
    dist_poly p2(p);
    EXPECT(poisson::how_many(), 4);

    EXPECT(p.size(), p2.size());
    EXPECT(p[0].mean(), p2[0].mean());
    EXPECT(p[1].mean(), p2[1].mean());
  }

  // Test the multiplication: *
  {
    tpoly<int> p1;
    tpoly<int> p2;

    p1(3, 1)(1, 0);
    p2(1, 6)(2, 0);

    tpoly<int> p3 = p1 * p2;
    tpoly<int> result;

    // 1*x^6 + 6*x^1 + 2
    result(1, 6)(6, 1)(2, 0);

    EXPECT(p3, result);
  }

  // Test the sum function
  {
    fp_poly p;

    p(3.0, 1)(1.0, 0);

    EXPECT(sum(p), 4.0);
  }

  // Test the sum function
  {
    dist_poly p;

    p(distro(poisson(3.0)), 1)(distro(poisson(1.0)), 0);
    EXPECT(poisson::how_many(), 2);

    distro r = sum(p);
    EXPECT(poisson::how_many(), 3);
    EXPECT(r.mean(), 4.0);
  }

  // Make sure that dist_poly destroys its distributions.
  {
    dist_poly dp;
    dp(distro(poisson(3.0)), 1)(distro(poisson(1.0)), 0);
    EXPECT(dist_poly::how_many(), 1);
    EXPECT(poisson::how_many(), 2);
  }
  EXPECT(poisson::how_many(), 0);

  // Make sure that the += operator for dist_poly works fine.
  {
    dist_poly a, b;

    // We can add empty polynomials.
    a += b;
    EXPECT(poisson::how_many(), 0);

    // This is the first polynomial.
    a(distro(poisson(3.0)), 2)(distro(poisson(1.0)), 0);
    EXPECT(poisson::how_many(), 2);

    // And still we can add an empty polynomial.
    a += b;
    EXPECT(poisson::how_many(), 2);
    EXPECT(a.size(), 2);
    EXPECT(a[0].mean(), 1.0);
    EXPECT(a[2].mean(), 3.0);

    // A non-empty polynomial.
    b(distro(poisson(3.0)), 2)(distro(poisson(1.0)), 1);
    EXPECT(poisson::how_many(), 4);

    // We add a non-empty polynomial.
    a += b;
    EXPECT(poisson::how_many(), 5);
    EXPECT(a.size(), 3);
    EXPECT(a[0].mean(), 1.0);
    EXPECT(a[1].mean(), 1.0);
    EXPECT(a[2].mean(), 6.0);
  }
  EXPECT(poisson::how_many(), 0);

  // Test the function:
  // dist_poly sum(const map<Vertex, dist_poly> &c);
  {
    EXPECT(poisson::how_many(), 0);
    EXPECT(dist_poly::how_many(), 0);

    map<Vertex, dist_poly> vdm;
    EXPECT(poisson::how_many(), 0);
    EXPECT(dist_poly::how_many(), 0);

    vdm[0](distro(poisson(3.0)), 2)(distro(poisson(1.0)), 0);
    EXPECT(poisson::how_many(), 2);
    EXPECT(dist_poly::how_many(), 1);

    vdm[10](distro(poisson(3.0)), 2)(distro(poisson(1.0)), 1);
    EXPECT(poisson::how_many(), 4);
    EXPECT(dist_poly::how_many(), 2);

    dist_poly res = sum(vdm);
    EXPECT(poisson::how_many(), 7);
    EXPECT(dist_poly::how_many(), 3);

    EXPECT(res[0].mean(), 1.0);
    EXPECT(res[1].mean(), 1.0);
    EXPECT(res[2].mean(), 6.0);
  }
  EXPECT(poisson::how_many(), 0);

  return 0;
}
