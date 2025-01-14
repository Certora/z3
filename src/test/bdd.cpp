#include "math/dd/dd_bdd.h"
#include "math/dd/dd_fdd.h"

namespace dd {

class test_bdd {
public:

    static void test1() {
        bdd_manager m(20);
        bdd v0 = m.mk_var(0);
        bdd v1 = m.mk_var(1);
        bdd v2 = m.mk_var(2);
        bdd c1 = v0 && v1 && v2;
        bdd c2 = v2 && v0 && v1;
        std::cout << c1 << "\n";
        SASSERT(c1 == c2);
        std::cout << "cnf size: " << c1.cnf_size() << "\n";

        c1 = v0 || v1 || v2;
        c2 = v2 || v1 || v0;
        std::cout << c1 << "\n";
        SASSERT(c1 == c2);
        std::cout << "cnf size: " << c1.cnf_size() << "\n";
    }

    static void test2() {
        bdd_manager m(20);
        bdd v0 = m.mk_var(0);
        bdd v1 = m.mk_var(1);
        bdd v2 = m.mk_var(2);
        SASSERT(m.mk_ite(v0,v0,v1) == (v0 || v1));
        SASSERT(m.mk_ite(v0,v1,v1) == v1);
        SASSERT(m.mk_ite(v1,v0,v1) == (v0 && v1));
        SASSERT(m.mk_ite(v1,v0,v0) == v0);
        SASSERT(m.mk_ite(v0,!v0,v1) == (!v0 && v1));
        SASSERT(m.mk_ite(v0,v1,!v0) == (!v0 || v1));
        SASSERT((!(v0 && v1)) == (!v0 || !v1));
        SASSERT((!(v0 || v1)) == (!v0 && !v1));
    }

    static void test3() {
        bdd_manager m(20);
        bdd v0 = m.mk_var(0);
        bdd v1 = m.mk_var(1);
        bdd v2 = m.mk_var(2);
        bdd c1 = (v0 && v1) || v2;
        bdd c2 = m.mk_exists(0, c1);
        std::cout << c1 << "\n";
        std::cout << c2 << "\n";
        SASSERT(c2 == (v1 || v2));
        c2 = m.mk_exists(1, c1);
        SASSERT(c2 == (v0 || v2));
        c2 = m.mk_exists(2, c1);
        SASSERT(c2.is_true());
        SASSERT(m.mk_exists(3, c1) == c1);
        c1 = (v0 && v1) || (v1 && v2) || (!v0 && !v2);
        c2 = m.mk_exists(0, c1);
        SASSERT(c2 == (v1 || (v1 && v2) || !v2));
        c2 = m.mk_exists(1, c1);
        SASSERT(c2 == (v0 || v2 || (!v0 && !v2)));
        c2 = m.mk_exists(2, c1);
        SASSERT(c2 == ((v0 && v1) || v1 || !v0));
    }

    static void test4() {
        bdd_manager m(3);
        bdd v0 = m.mk_var(0);
        bdd v1 = m.mk_var(1);
        bdd v2 = m.mk_var(2);
        bdd c1 = (v0 && v2) || v1;
        std::cout << "before reorder:\n";
        std::cout << c1 << "\n";
        std::cout << c1.bdd_size() << "\n";
        m.gc();
        m.try_reorder();
        std::cout << "after reorder:\n";
        std::cout << c1 << "\n";
        std::cout << c1.bdd_size() << "\n";
    }

    static void test_xor() {
        bdd_manager m(4);
        bdd v0 = m.mk_var(0);
        bdd v1 = m.mk_var(0);
        SASSERT((m.mk_false() ^ v0) == v0);
        SASSERT((v0 ^ m.mk_false()) == v0);
        SASSERT((m.mk_true() ^ v0) == !v0);
        SASSERT((v0 ^ m.mk_true()) == !v0);
        SASSERT((v0 ^ v1) == ((v0 && !v1) || (!v0 && v1)));
    }

    static void test_bddv_ops_on_constants() {
        std::cout << "test_bddv_ops_on_constants\n";
        unsigned const num_bits = 3;
        rational const modulus = rational::power_of_two(num_bits);
        bdd_manager m(num_bits);

        SASSERT_EQ(m.to_val(m.mk_zero(num_bits)), rational(0));
        SASSERT_EQ(m.to_val(m.mk_ones(num_bits)), modulus - 1);

        for (unsigned n = 0; n < 8; ++n) {
            rational const nr(n);
            SASSERT_EQ(m.to_val(m.mk_num(nr, num_bits)), nr);
        }

        for (unsigned n = 0; n < 8; ++n) {
            for (unsigned k = 0; k < 8; ++k) {
                rational const nr(n);
                rational const kr(k);
                bddv const nv = m.mk_num(nr, num_bits);
                bddv const kv = m.mk_num(kr, num_bits);
                SASSERT_EQ((nv + kv).to_val(), (nr + kr) % modulus);
                SASSERT_EQ((nv - kv).to_val(), (nr - kr + modulus) % modulus);
                SASSERT_EQ((nv * kr).to_val(), (nr * kr) % modulus);
                SASSERT_EQ((nv * kv).to_val(), (nr * kr) % modulus);
                bdd eq = m.mk_eq(nv, kv);
                SASSERT(eq.is_const() && (eq.is_true() == (n == k)));
                eq = m.mk_eq(nv, kr);
                SASSERT(eq.is_const() && (eq.is_true() == (n == k)));

                bdd cmp = nv <= kv;
                SASSERT(cmp.is_const() && (cmp.is_true() == (nr <= kr)));
                cmp = nv >= kv;
                SASSERT(cmp.is_const() && (cmp.is_true() == (nr >= kr)));
                cmp = nv < kv;
                SASSERT(cmp.is_const() && (cmp.is_true() == (nr < kr)));
                cmp = nv > kv;
                SASSERT(cmp.is_const() && (cmp.is_true() == (nr > kr)));

                // signed versions
                rational const nrs = (nr < modulus / 2) ? nr : nr - modulus;
                rational const krs = (kr < modulus / 2) ? kr : kr - modulus;
                cmp = nv.sle(kv);
                SASSERT(cmp.is_const() && (cmp.is_true() == (nrs <= krs)));
                cmp = nv.sge(kv);
                SASSERT(cmp.is_const() && (cmp.is_true() == (nrs >= krs)));
                cmp = nv.slt(kv);
                SASSERT(cmp.is_const() && (cmp.is_true() == (nrs < krs)));
                cmp = nv.sgt(kv);
                SASSERT(cmp.is_const() && (cmp.is_true() == (nrs > krs)));

                bddv quotv = m.mk_zero(num_bits);
                bddv remv = m.mk_zero(num_bits);
                nv.quot_rem(kv, quotv, remv);
                if (k != 0) {
                    SASSERT_EQ(quotv.to_val(), rational(n / k));
                    SASSERT_EQ(remv.to_val(), rational(n % k));
                } else {
                    // std::cout << "n=" << n << "  k=" << k << "\n";
                    // std::cout << "  quot: " << quotv.to_val() << "\n";
                    // std::cout << "   rem: " << remv.to_val() << "\n";
                }
            }
        }
    }

    static void test_bddv_eqfind_small() {
        std::cout << "test_bddv_eqfind_small\n";
        bdd_manager m(4);
        fdd const x_dom(m, 1);
        bddv const x = x_dom.var();
        bdd x_is_one = (x == rational(1));
        std::cout << "x_is_one:\n" << x_is_one << "\n";
        rational r;
        auto res = x_dom.find(x_is_one, r);
        SASSERT_EQ(res, find_t::singleton);
        SASSERT_EQ(r, rational(1));
    }

    static void test_bddv_eqfind() {
        std::cout << "test_bddv_eqfind\n";
        bdd_manager m(4);

        unsigned_vector bits;
        bits.push_back(0);
        bits.push_back(1);
        bits.push_back(4);
        bits.push_back(27);

        fdd const x_dom(m, bits);
        bddv const x = x_dom.var();
        bddv const zero = m.mk_zero(x_dom.num_bits());
        bddv const one = m.mk_num(rational(1), x_dom.num_bits());
        bddv const five = m.mk_num(rational(5), x_dom.num_bits());

        SASSERT((one == rational(1)).is_true());
        SASSERT((five == rational(5)).is_true());
        SASSERT((five == rational(4)).is_false());
        SASSERT((five == five).is_true());
        SASSERT((five == one).is_false());

        // Test the three different mk_eq overloads
        {
            bdd const x_is_five = m.mk_eq(x, rational(5));
            rational r;
            auto res = x_dom.find(x_is_five, r);
            SASSERT_EQ(res, find_t::singleton);
            SASSERT_EQ(r, 5);
        }

        {
            bdd const x_is_five = m.mk_eq(bits, rational(5));
            rational r;
            auto res = x_dom.find(x_is_five, r);
            SASSERT_EQ(res, find_t::singleton);
            SASSERT_EQ(r, 5);
        }

        {
            bdd const x_is_five = m.mk_eq(x, five);
            rational r;
            auto res = x_dom.find(x_is_five, r);
            SASSERT_EQ(res, find_t::singleton);
            SASSERT_EQ(r, 5);
        }
    }

    static void test_bddv_addsub() {
        std::cout << "test_bddv_addsub\n";
        unsigned_vector bits;
        bits.push_back(0);
        bits.push_back(1);
        bits.push_back(2);
        bdd_manager m(bits.size());
        bddv const x = m.mk_var(bits);
        SASSERT_EQ(x - rational(3) == rational(2), x == rational(5));
        SASSERT_EQ(x + rational(3) == rational(5), x == rational(2));
        SASSERT_EQ(x - rational(3) == rational(6), x == rational(1));
        SASSERT_EQ(x + rational(3) == rational(2), x == rational(7));
    }

    static void test_bddv_mul() {
        std::cout << "test_bddv_mul\n";
        unsigned const num_bits = 4;
        bdd_manager m(num_bits);

        unsigned_vector bits;
        bits.push_back(0);
        bits.push_back(1);
        bits.push_back(4);
        bits.push_back(27);

        bddv const x = m.mk_var(bits);
        bddv const zero = m.mk_zero(num_bits);
        bddv const one = m.mk_num(rational(1), num_bits);
        bddv const five = m.mk_num(rational(5), num_bits);
        bddv const six = m.mk_num(rational(6), num_bits);

        // 5*x == 1 (mod 16)  =>  x == 13 (mod 16)
        bdd const five_inv = x * five == one;
        SASSERT_EQ(five_inv, x == rational(13));

        // 6*x == 1 (mod 16)  =>  no solution for x
        bdd const six_inv = x * six == one;
        SASSERT(six_inv.is_false());

        // 6*x == 0 (mod 16)  =>  x is either 0 or 8 (mod 16)
        bdd const b = six * x == zero;
        SASSERT_EQ(b, x == rational(0) || x == rational(8));
        SASSERT((b && (x != rational(0)) && (x != rational(8))).is_false());
        SASSERT_EQ(b && (x != rational(0)), x == rational(8));

        SASSERT_EQ((x * zero).bits(), (x * rational(0)).bits());
        SASSERT_EQ((x *  one).bits(), (x * rational(1)).bits());
        SASSERT_EQ((x * five).bits(), (x * rational(5)).bits());
        SASSERT_EQ((x *  six).bits(), (x * rational(6)).bits());
    }

    static void test_bddv_ule() {
        std::cout << "test_bddv_ule\n";
        unsigned const num_bits = 4;
        bdd_manager m(num_bits);

        unsigned_vector bits;
        bits.push_back(0);
        bits.push_back(1);
        bits.push_back(2);
        bits.push_back(3);

        bddv const x = m.mk_var(bits);
        bddv const three = m.mk_num(rational(3), num_bits);
        bddv const four = m.mk_num(rational(4), num_bits);
        bddv const five = m.mk_num(rational(5), num_bits);

        SASSERT_EQ(x >= four && x < five, x == four);
        SASSERT_EQ(four <= x && x < five, x == four);
        SASSERT_EQ(three < x && x < five, x == four);
        SASSERT_EQ(three < x && x <= four, x == four);
        SASSERT_EQ(three <= x && x < five, x == four || x == three);
    }

    static void test_fdd3() {
        std::cout << "test_fdd3\n";
        unsigned const w = 3;  // bit width
        bdd_manager m(w);

        fdd const x_dom(m, w);
        bddv const& x = x_dom.var();

        // Encodes the values x satisfying a*x + b == 0 (mod 2^w) as BDD.
        auto mk_affine = [] (unsigned a, bddv const& x, unsigned b) {
            return (rational(a)*x + rational(b) == rational(0));
        };

        vector<bdd> num;
        for (unsigned n = 0; n < (1<<w); ++n)
            num.push_back(x == rational(n));

        for (unsigned k = 0; k < (1 << w); ++k) {
            for (unsigned n = 0; n < (1 << w); ++n) {
                SASSERT(x_dom.contains(num[k], rational(n)) == (n == k));
                rational r;
                SASSERT_EQ(x_dom.find(num[n] || num[k], r), (n == k) ? find_t::singleton : find_t::multiple);
                SASSERT(r == n || r == k);
            }
        }

        bdd s0127 = num[0] || num[1] || num[2] || num[7];
        SASSERT( x_dom.contains(s0127, rational(0)));
        SASSERT( x_dom.contains(s0127, rational(1)));
        SASSERT( x_dom.contains(s0127, rational(2)));
        SASSERT(!x_dom.contains(s0127, rational(3)));
        SASSERT(!x_dom.contains(s0127, rational(4)));
        SASSERT(!x_dom.contains(s0127, rational(5)));
        SASSERT(!x_dom.contains(s0127, rational(6)));
        SASSERT( x_dom.contains(s0127, rational(7)));

        bdd s123 = num[1] || num[2] || num[3];
        SASSERT((s0127 && s123) == (num[1] || num[2]));

        SASSERT(mk_affine(0, x, 0).is_true());
        SASSERT(mk_affine(0, x, 1).is_false());
        // 2*x == 0 (mod 2^3) has the solutions 0, 4
        SASSERT(mk_affine(2, x, 0) == (num[0] || num[4]));

        // 4*x + 2 == 0 (mod 2^3) has no solutions
        SASSERT(mk_affine(4, x, 2).is_false());
        // 3*x + 2 == 0 (mod 2^3) has the unique solution 2
        SASSERT(mk_affine(3, x, 2) == num[2]);
        // 2*x + 2 == 0 (mod 2^3) has the solutions 3, 7
        SASSERT(mk_affine(2, x, 2) == (num[3] || num[7]));
    }

    static void test_fdd4() {
        std::cout << "test_fdd4\n";
        bdd_manager m(4);
        fdd const y_dom(m, 4);
        bddv const& y = y_dom.var();
        // 12*y + 8 == 0 (mod 2^4) has the solutions 2, 6, 10, 14
        bdd equation = rational(12) * y + rational(8) == rational(0);
        bdd expected = (y == rational(2)) || (y == rational(6)) || (y == rational(10)) || (y == rational(14));
        SASSERT(equation == expected);
    }

    static void test_fdd_reorder() {
        std::cout << "test_fdd_reorder\n";
        bdd_manager m(4);
        fdd const x_dom(m, 4);
        bddv const& x = x_dom.var();

        vector<bdd> num;
        for (unsigned n = 0; n < (1ul << x_dom.num_bits()); ++n) {
            num.push_back(x == rational(n));
            SASSERT(x_dom.contains(num[n], rational(n)));
            rational r;
            SASSERT_EQ(x_dom.find(num[n], r), find_t::singleton);
            SASSERT_EQ(r, n);
        }

        // need something extra to skew costs and trigger a reorder
        bdd atleast3 = (x >= rational(3));
        SASSERT(x_dom.contains(atleast3, rational(3)));

        auto const old_levels = m.m_var2level;
        std::cout << "old levels: " << old_levels << "\n";
        m.gc();
        m.try_reorder();
        std::cout << "new levels: " << m.m_var2level << "\n";
        SASSERT(old_levels != m.m_var2level);  // ensure that reorder actually did something.

        // Should still give the correct answer after reordering
        for (unsigned n = 0; n < (1ul << x_dom.num_bits()); ++n) {
            SASSERT(x_dom.contains(num[n], rational(n)));
            rational r;
            SASSERT_EQ(x_dom.find(num[n], r), find_t::singleton);
            SASSERT_EQ(r, n);
        }
    }

    static void test_fdd_twovars() {
        std::cout << "test_fdd_twovars\n";
        bdd_manager m(6);
        fdd const x_dom(m, 3, 0, 2);
        fdd const y_dom(m, 3, 1, 2);
        bddv const& x = x_dom.var();
        bddv const& y = y_dom.var();
        SASSERT_EQ(x - y <= rational(0), x == y);
    }

    static void test_fdd_find_hint() {
        std::cout << "test_fdd_find_hint\n";
        bdd_manager m(4);
        fdd const x_dom(m, 4);
        bddv const& x = x_dom.var();

        bdd s358 = x == rational(3) || x == rational(5) || x == rational(8);
        rational r;
        SASSERT_EQ(x_dom.find_hint(s358, rational(8), r), find_t::multiple);
        SASSERT_EQ(r, 8);
        SASSERT_EQ(x_dom.find_hint(s358, rational(5), r), find_t::multiple);
        SASSERT_EQ(r, 5);
        SASSERT_EQ(x_dom.find_hint(s358, rational(3), r), find_t::multiple);
        SASSERT_EQ(r, 3);
        SASSERT_EQ(x_dom.find_hint(s358, rational(7), r), find_t::multiple);
        SASSERT(r == 3 || r == 5 || r == 8);

        SASSERT_EQ(x_dom.find_hint(x == rational(5), rational(3), r), find_t::singleton);
        SASSERT_EQ(r, 5);
        SASSERT_EQ(x_dom.find_hint(x == rational(5), rational(5), r), find_t::singleton);
        SASSERT_EQ(r, 5);

        SASSERT_EQ(x_dom.find_hint(s358 && (x == rational(4)), rational(5), r), find_t::empty);
    }

};

}

void tst_bdd() {
    dd::test_bdd::test1();
    dd::test_bdd::test2();
    dd::test_bdd::test3();
    dd::test_bdd::test4();
    dd::test_bdd::test_xor();
    dd::test_bdd::test_bddv_ops_on_constants();
    dd::test_bdd::test_bddv_eqfind_small();
    dd::test_bdd::test_bddv_eqfind();
    dd::test_bdd::test_bddv_addsub();
    dd::test_bdd::test_bddv_mul();
    dd::test_bdd::test_bddv_ule();
    dd::test_bdd::test_fdd3();
    dd::test_bdd::test_fdd4();
    dd::test_bdd::test_fdd_reorder();
    dd::test_bdd::test_fdd_twovars();
    dd::test_bdd::test_fdd_find_hint();
}
