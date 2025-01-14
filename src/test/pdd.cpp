#include "math/dd/dd_pdd.h"

namespace dd {

class test {
public :

    static void hello_world() {
        pdd_manager m(3);
        pdd v0 = m.mk_var(0);
        pdd v1 = m.mk_var(1);
        pdd v2 = m.mk_var(2);
        std::cout << v0 << "\n";
        std::cout << v1 << "\n";
        std::cout << v2 << "\n";
        pdd c1 = v0 * v1 * v2;
        pdd c2 = v2 * v0 * v1;
        std::cout << c1 << "\n";
        VERIFY(c1 == c2);
        
        c1 = v0 + v1 + v2;
        c2 = v2 + v1 + v0;
        std::cout << c1 << "\n";
        VERIFY(c1 == c2);

        c1 = (v0+v1) * v2;
        c2 = (v0*v2) + (v1*v2);
        std::cout << c1 << "\n";
        VERIFY(c1 == c2);
        c1 = (c1 + 3) + 1;
        c2 = (c2 + 1) + 3;
        std::cout << c1 << "\n";
        VERIFY(c1 == c2);
        c1 = v0 - v1;
        c2 = v1 - v0;
        std::cout << c1 << " " << c2 << "\n";

        c1 = v1*v2;
        c2 = (v0*v2) + (v2*v2);
        pdd c3 = m.zero();
        VERIFY(m.try_spoly(c1, c2, c3));
        std::cout << c1 << " " << c2 << " spoly: " << c3 << "\n";

        c1 = v1*v2;
        c2 = (v0*v2) + (v1*v1);
        VERIFY(m.try_spoly(c1, c2, c3));
        std::cout << c1 << " " << c2 << " spoly: " << c3 << "\n";

        c1 = (v0*v1) - (v0*v0);
        c2 = (v0*v1*(v2 + v0)) + v2;
        c3 = c2.reduce(c1);
        std::cout << c1 << " " << c2 << " reduce: " << c3 << "\n";
        VERIFY(c3 == c3.reduce(c1));
    }

    static void reduce() {
        std::cout << "\nreduce\n";
        // a(b^2)cd + abc + bcd + bc + cd + 3 reduce by  bc
        pdd_manager m(4);
        pdd a = m.mk_var(0);
        pdd b = m.mk_var(1);
        pdd c = m.mk_var(2);
        pdd d = m.mk_var(3);
        pdd e = (a * b * b * c * d) + (2*a*b*c) + (b*c*d) + (b*c) + (c*d) + 3;
        std::cout << e << "\n";
        pdd f = b * c;
        pdd r_ef = m.reduce(e, f);
        m.display(std::cout);
        std::cout << "result of reduce " << e << " by " << f << " is " << r_ef <<  "\n";
        VERIFY(r_ef == (c*d) + 3);
        pdd r_fe = m.reduce(f, e);
        std::cout << "result of reduce " << f << " by " << e << " is " << r_fe << "\n" ;
        VERIFY(r_fe == f);

        // test that b*c*d is treated as the leading monomial
        f = b*c*d - d*d;
        r_ef = m.reduce(e, f);
        std::cout << "result of reduce " << e << " by " << f << " is " << r_ef <<  "\n";
        VERIFY(r_ef == (a*b*d*d) + (2*a*b*c) + (d*d) + (b*c) + (c*d) + 3);

        pdd k = d*d + 3*b;
        VERIFY(m.reduce(f, k) == b*c*d + 3*b);
        
    }



    static void large_product() {
        std::cout << "\nlarge_product\n";
        pdd_manager m(4);
        pdd a = m.mk_var(0);
        pdd b = m.mk_var(1);
        pdd c = m.mk_var(2);
        pdd d = m.mk_var(3);
        
        pdd e = a + c;
        for (unsigned i = 0; i < 5; i++) {
            e = e * e;
        }
        e = e * b;
        std::cout << e << "\n";
    }

    static void reset() {
        std::cout << "\ntest reset\n";
        pdd_manager m(4);
        pdd a = m.mk_var(0);
        pdd b = m.mk_var(1);
        pdd c = m.mk_var(2);
        pdd d = m.mk_var(3);
        std::cout << (a + b)*(c + d) << "\n";

        unsigned_vector l2v;
        for (unsigned i = 0; i < 4; ++i) 
            l2v.push_back(3 - i);
        m.reset(l2v);
        a = m.mk_var(0);
        b = m.mk_var(1);
        c = m.mk_var(2);
        d = m.mk_var(3);
        std::cout << (a + b)*(c + d) << "\n";
    }

    static void canonize() {
        std::cout << "\ncanonize\n";
        pdd_manager m(2);
        pdd a = m.mk_var(0);
        pdd b = m.mk_var(1);

        pdd e = (a - b) * ( a + b);
        pdd f = a * a  - b * b;
        VERIFY(e == f);

        e = (a - b)* (a - b);
        f = a * a - 2 * a * b + b * b;
        VERIFY(e == f);
        e = a - 3;
        e = e * e;
        f = a * a - 6 * a + 9;
        VERIFY(e == f);        
        e = 2 * a - 3;
        e = e * e;
        f = 4 * a * a - 12 * a + 9;
        VERIFY(e == f);        
    }


    static void iterator() {
        std::cout << "test iterator\n";
        pdd_manager m(4);
        pdd a = m.mk_var(0);
        pdd b = m.mk_var(1);
        pdd c = m.mk_var(2);
        pdd d = m.mk_var(3);
        pdd p = (a + b)*(c + 3*d) + 2;
        std::cout << p << "\n";
        for (auto const& m : p) {
            std::cout << m << "\n";
        }
    }
    static void order() {
        std::cout << "order\n";
        pdd_manager m(4);
        unsigned va = 0;
        unsigned vb = 1;
        unsigned vc = 2;
        unsigned vd = 3;
        pdd a = m.mk_var(va);
        pdd b = m.mk_var(vb);
        pdd c = m.mk_var(vc);
        pdd d = m.mk_var(vd);
        pdd p = a + b;
        std::cout << p << "\n";
        unsigned i = 0;
        for (auto const& m : p) {
            std::cout << m << "\n";        
            VERIFY(i != 0 ||( m.vars.size() == 1 && m.vars[0] == vb));
            VERIFY(i != 1 ||( m.vars.size() == 1 && m.vars[0] == va));
            i++;
        }
        pdd ccbbaa = c*c*b*b*a*a;
        pdd ccbbba = c*c*b*b*b*a;
        p = ccbbaa + ccbbba;

        i = 0;
        std::cout << "p = " <<  p << "\n";
        for (auto const& m : p) {
            std::cout << m << "\n";
            VERIFY(i != 0 ||( m.vars.size() == 6 && m.vars[4] == vb)); // the first one has to be ccbbba
            VERIFY(i != 1 ||( m.vars.size() == 6 && m.vars[4] == va)); // the second one has to be ccbbaa
            i++;
        }
        pdd dcbba = d*c*b*b*a;
        pdd dd = d * d;
        p = dcbba + ccbbba + dd;
        vector<unsigned> v;
        std::cout << "p = " <<  p << "\n";
        unsigned k = p.index();
        std::cout << "pdd(k) = " << pdd(k, m) << "\n";
        k = m.first_leading(k);
        do {
            if (m.is_val(k)) {
                VERIFY(m.val(k).is_one());
                break;
            }
            v.push_back(m.var(k));        
            std::cout << "pdd(k) = " << pdd(k, m) << "\n";
            std::cout << "push v" << m.var(k) << "\n";
            k = m.next_leading(k);
        } while(true);
        auto it = v.begin();
        std::cout << "v" << *it;
        it++;
        for( ; it != v.end(); it ++) {
            std::cout << "*v" << *it;
        }
        VERIFY(v.size() == 6);
        VERIFY(v[0] == vc);
        std::cout << "\n";
        v.reset();
        p = d*c*c*d + b*c*c*b + d*d*d;
        k = p.index();
        std::cout << "pdd(k) = " << pdd(k, m) << "\n";
        k = m.first_leading(k);
        do {
            if (m.is_val(k)) {
                VERIFY(m.val(k).is_one());
                break;
            }
            v.push_back(m.var(k));        
            std::cout << "pdd(k) = " << pdd(k, m) << "\n";
            std::cout << "push v" << m.var(k) << "\n";
            k = m.next_leading(k);
        } while(true);
        it = v.begin();
        std::cout << "v" << *it;
        it++;
        for( ; it != v.end(); it ++) {
            std::cout << "*v" << *it;
        }
        VERIFY(v.size() == 4);
        VERIFY(v[0] == vd);
        std::cout << "\n";
    
    }
    static void order_lm() {
        std::cout << "order_lm\n";
        pdd_manager m(4);
        unsigned va = 0;
        unsigned vb = 1;
        unsigned vc = 2;
        unsigned vd = 3;
        pdd a = m.mk_var(va);
        pdd b = m.mk_var(vb);
        pdd c = m.mk_var(vc);
        pdd d = m.mk_var(vd);
        pdd p = a + b;
        std::cout << p << "\n";
        unsigned i = 0;
        for (auto const& m : p) {
            std::cout << m << "\n";        
            VERIFY(i != 0 ||( m.vars.size() == 1 && m.vars[0] == vb));
            VERIFY(i != 1 ||( m.vars.size() == 1 && m.vars[0] == va));
            i++;
        }
        pdd ccbbaa = c*c*b*b*a*a;
        pdd ccbbba = c*c*b*b*b*a;
        p = ccbbaa + ccbbba;

        i = 0;
        std::cout << "p = " <<  p << "\n";
        for (auto const& m : p) {
            std::cout << m << "\n";
            VERIFY(i != 0 ||( m.vars.size() == 6 && m.vars[4] == vb)); // the first one has to be ccbbba
            VERIFY(i != 1 ||( m.vars.size() == 6 && m.vars[4] == va)); // the second one has to be ccbbaa
            i++;
        }
        pdd dcbba = d*c*b*b*a;
        pdd dd = d * d;
        pdd p0 = p + dd;
        std::cout << p0 << " > " << p << "\n";
        VERIFY(m.lm_lt(p, p0));
        VERIFY(m.lm_lt(p0 + a * b, p0 + b * b));
        
    }

    /**
     * Test polynomials mod 2^2
     */
    static void mod4_operations() {
        std::cout << "operations mod4\n";
        pdd_manager m(4, pdd_manager::mod2N_e, 2);
        unsigned va = 0;
        unsigned vb = 1;
        unsigned vc = 2;
        unsigned vd = 3;
        pdd a = m.mk_var(va);
        pdd b = m.mk_var(vb);
        pdd c = m.mk_var(vc);
        pdd d = m.mk_var(vd);
        pdd p = a - b;
        std::cout << p << "\n";

        // should be reduced to 0:
        p = a*a*(a*a - 1);
        std::cout << p << "\n"; 
        vector<std::pair<unsigned, rational>> sub0, sub1, sub2, sub3;
        sub0.push_back(std::make_pair(va, rational(0)));
        sub1.push_back(std::make_pair(va, rational(1)));
        sub2.push_back(std::make_pair(va, rational(2)));
        sub3.push_back(std::make_pair(va, rational(3)));
        std::cout << "sub 0 " << p.subst_val(sub0) << "\n";
        std::cout << "sub 1 " << p.subst_val(sub1) << "\n";
        std::cout << "sub 2 " << p.subst_val(sub2) << "\n";
        std::cout << "sub 3 " << p.subst_val(sub3) << "\n";

        std::cout << "expect 1 " << (2*a + 1).is_never_zero() << "\n";
        std::cout << "expect 1 " << (2*a*b + 2*b + 1).is_never_zero() << "\n";
        std::cout << "expect 0 " << (2*a + 2).is_never_zero() << "\n";
        SASSERT((2*a + 1).is_never_zero());
        SASSERT((2*a + 2*b + 1).is_never_zero());
        SASSERT(!(2*a*b + 3*b + 2).is_never_zero());
    }

    static void degree_of_variables() {
        std::cout << "degree of variables\n";
        pdd_manager m(4, pdd_manager::mod2N_e, 3);
        unsigned va = 0;
        unsigned vb = 1;
        unsigned vc = 2;
        pdd a = m.mk_var(va);
        pdd b = m.mk_var(vb);
        pdd c = m.mk_var(vc);

        SASSERT(a.var() == va);
        SASSERT(b.var() == vb);

        SASSERT(a.degree(va) == 1);
        SASSERT(a.degree(vb) == 0);
        SASSERT(a.degree(vc) == 0);
        SASSERT(c.degree(vc) == 1);
        SASSERT(c.degree(vb) == 0);
        SASSERT(c.degree(va) == 0);

        {
            pdd p = a * a * a;
            SASSERT(p.degree(va) == 3);
            SASSERT(p.degree(vb) == 0);
        }

        {
            pdd p = b * a;
            SASSERT(p.degree(va) == 1);
            SASSERT(p.degree(vb) == 1);
            SASSERT(p.degree(vc) == 0);
        }

        {
            pdd p = (a*a*b + b*a*b + b + a*c)*a + b*b*c;
            SASSERT(p.degree(va) == 3);
            SASSERT(p.degree(vb) == 2);
            SASSERT(p.degree(vc) == 1);
        }

        {
            // check that skipping marked nodes works (1)
            pdd p = b*a + c*a*a*a;
            SASSERT(p.degree(va) == 3);
        }

        {
            // check that skipping marked nodes works (2)
            pdd p = (b+c)*(a*a*a);
            SASSERT(p.degree(va) == 3);
        }

        {
            // check that skipping marked nodes works (3)
            pdd p = a*a*a*b*b*b*c + a*a*a*b*b*b;
            SASSERT(p.degree(va) == 3);
        }
    }

    static void factor() {
        std::cout << "factor\n";
        pdd_manager m(4, pdd_manager::mod2N_e, 3);

        unsigned const va = 0;
        unsigned const vb = 1;
        unsigned const vc = 2;
        unsigned const vd = 3;
        pdd const a = m.mk_var(va);
        pdd const b = m.mk_var(vb);
        pdd const c = m.mk_var(vc);
        pdd const d = m.mk_var(vd);

        auto test_one = [&m](pdd const& p, unsigned v, unsigned d) {
            pdd lc = m.zero();
            pdd rest = m.zero();
            std::cout << "Factoring p = " << p << " by v" << v << "^" << d << "\n";
            p.factor(v, d, lc, rest);
            std::cout << "  lc = " << lc << "\n";
            std::cout << "  rest = " << rest << "\n";
            pdd x = m.mk_var(v);
            pdd x_pow_d = m.one();
            for (unsigned i = 0; i < d; ++i) {
                x_pow_d *= x;
            }
            SASSERT( p == lc * x_pow_d + rest );
            SASSERT( d == 0 || rest.degree(v) < d );
            SASSERT( d != 0 || lc.is_zero() );
        };

        auto test_multiple = [=](pdd const& p) {
            for (auto v : {va, vb, vc, vd}) {
                for (unsigned d = 0; d <= 5; ++d) {
                    test_one(p, v, d);
                }
            }
        };

        test_multiple( b );
        test_multiple( b*b*b );
        test_multiple( b + c );
        test_multiple( a*a*a*a*a + a*a*a*b + a*a*b*b + c );
        test_multiple( c*c*c*c*c + b*b*b*c + 3*b*c*c + a );
        test_multiple( (a + b) * (b + c) * (c + d) * (d + a) );
    }

    static void max_pow2_divisor() {
        std::cout << "max_pow2_divisor\n";
        pdd_manager m(4, pdd_manager::mod2N_e, 256);

        unsigned const va = 0;
        unsigned const vb = 1;
        pdd const a = m.mk_var(va);
        pdd const b = m.mk_var(vb);

        SASSERT(m.zero().max_pow2_divisor() == UINT_MAX);
        SASSERT(m.one().max_pow2_divisor() == 0);
        pdd p = (1 << 20)*a*b + 1024*b*b*b;
        std::cout << p << " divided by 2^" << p.max_pow2_divisor() << "\n";
        SASSERT(p.max_pow2_divisor() == 10);
        SASSERT(p.div(rational::power_of_two(10)) == 1024*a*b + b*b*b);
        SASSERT((p + p).max_pow2_divisor() == 11);
        SASSERT((p * p).max_pow2_divisor() == 20);
        SASSERT((p + 2*b).max_pow2_divisor() == 1);
        SASSERT((p + b*b*b).max_pow2_divisor() == 0);
    }

    static void binary_resolve() {
        std::cout << "binary resolve\n";
        pdd_manager m(4, pdd_manager::mod2N_e, 4);

        unsigned const va = 0;
        unsigned const vb = 1;
        unsigned const vc = 2;
        pdd const a = m.mk_var(va);
        pdd const b = m.mk_var(vb);
        pdd const c = m.mk_var(vc);

        pdd r = m.zero();

        pdd p = a*a*b - a*a;
        pdd q = a*b*b - b*b;
        SASSERT(m.resolve(va, p, q, r));
        SASSERT(r == a*b*b*b - a*b*b);
        SASSERT(!m.resolve(va, q, p, r));
        SASSERT(!m.resolve(vb, p, q, r));
        SASSERT(m.resolve(vb, q, p, r));
        SASSERT(r == a*a*a*b - a*a*b);
        SASSERT(!m.resolve(vc, p, q, r));

        p = 2*a*a*b + 13*a*a;
        q = 6*a*b*b*b + 14*b*b*b;
        SASSERT(m.resolve(va, p, q, r));
        SASSERT(r == (2*b+13)*2*b*b*b*a);
        SASSERT(!m.resolve(va, q, p, r));
        SASSERT(!m.resolve(vb, p, q, r));
        SASSERT(m.resolve(vb, q, p, r));
        SASSERT(r == 9*a*a*a*b*b + 5*a*a*b*b);

        p = a*a*b - a*a + 4*a*c + 2;
        q = 3*b*b - b*b*b + 8*b*c;
        SASSERT(!m.resolve(va, p, q, r));
        SASSERT(!m.resolve(va, q, p, r));
        SASSERT(!m.resolve(vb, p, q, r));
        SASSERT(m.resolve(vb, q, p, r));
        SASSERT(r == 2*a*a*b*b + 8*a*a*b*c + 4*a*b*b*c + 2*b*b);
        SASSERT(m.resolve(vc, p, q, r));
        SASSERT(r == 2*a*a*b*b - 2*a*a*b - 3*a*b*b + a*b*b*b + 4*b);
        SASSERT(m.resolve(vc, q, p, r));
        SASSERT(r == -(2*a*a*b*b - 2*a*a*b - 3*a*b*b + a*b*b*b + 4*b));
    }

    static void pow() {
        std::cout << "pow\n";
        pdd_manager m(4, pdd_manager::mod2N_e, 5);

        unsigned const va = 0;
        unsigned const vb = 1;
        pdd const a = m.mk_var(va);
        pdd const b = m.mk_var(vb);

        SASSERT(a.pow(0) == m.one());
        SASSERT(a.pow(1) == a);
        SASSERT(a.pow(2) == a*a);
        SASSERT(a.pow(7) == a*a*a*a*a*a*a);
        SASSERT((3*a*b).pow(3) == 27*a*a*a*b*b*b);
    }

    static void subst_val() {
        std::cout << "subst_val\n";
        pdd_manager m(4, pdd_manager::mod2N_e, 2);

        unsigned const va = 0;
        unsigned const vb = 1;
        unsigned const vc = 2;
        unsigned const vd = 3;
        pdd const a = m.mk_var(va);
        pdd const b = m.mk_var(vb);
        pdd const c = m.mk_var(vc);
        pdd const d = m.mk_var(vd);

        {
            pdd const p = 2*a + b + 1;
            SASSERT(p.subst_val(va, rational(0)) == b + 1);
        }

        {
            pdd const p = a + 2*b;
            SASSERT(p.subst_val(va, rational(0)) == 2*b);
            SASSERT(p.subst_val(va, rational(2)) == 2*b + 2);
            SASSERT(p.subst_val(vb, rational(0)) == a);
            SASSERT(p.subst_val(vb, rational(1)) == a + 2);
            SASSERT(p.subst_val(vb, rational(2)) == a);
            SASSERT(p.subst_val(vb, rational(3)) == a + 2);
            SASSERT(p.subst_val(va, rational(0)).subst_val(vb, rational(3)) == 2*m.one());
        }

        {
            pdd const p = a + b + c + d;
            vector<std::pair<unsigned, rational>> sub;
            sub.push_back({vb, rational(2)});
            sub.push_back({vc, rational(3)});
            SASSERT(p.subst_val(sub) == a + d + 1);
        }

        {
            pdd const p = (a + b) * (b + c) * (c + d);
            vector<std::pair<unsigned, rational>> sub;
            sub.push_back({vb, rational(2)});
            sub.push_back({vc, rational(3)});
            SASSERT(p.subst_val(sub) == (a + 2) * (d + 3));
            sub.push_back({va, rational(3)});
            sub.push_back({vd, rational(2)});
            SASSERT(p.subst_val(sub) == m.one());
        }
    }

};

}

void tst_pdd() {
    dd::test::hello_world();
    dd::test::reduce();
    dd::test::large_product();
    dd::test::canonize();
    dd::test::reset();
    dd::test::iterator();
    dd::test::order();
    dd::test::order_lm();
    dd::test::mod4_operations();
    dd::test::degree_of_variables();
    dd::test::factor();
    dd::test::max_pow2_divisor();
    dd::test::binary_resolve();
    dd::test::pow();
    dd::test::subst_val();
}
