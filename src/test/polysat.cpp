#include "math/polysat/solver.h"
#include "ast/ast.h"

namespace polysat {
    // test resolve, factoring routines
    // auxiliary 

    struct solver_scope {
        reslimit lim;
    };

    struct scoped_solver : public solver_scope, public solver {
        scoped_solver(): solver(lim) {}

        void check() {
            std::cout << check_sat() << "\n";
            statistics st;
            collect_statistics(st);
            std::cout << st << "\n";
            std::cout << *this << "\n";
        }
    };

    /**
     * most basic linear equation solving.
     * they should be solvable.
     * they also illustrate some limitations of basic solver even if it solves them.
     * Example
     *   the value to a + 1 = 0 is fixed at 3, there should be no search.
     */

    static void test_l1() {
        scoped_solver s;
        auto a = s.var(s.add_var(2));
        s.add_eq(a + 1);
        s.check();
        // Expected result: SAT with a = 3
    }
    
    static void test_l2() {
        scoped_solver s;
        auto a = s.var(s.add_var(2));
        auto b = s.var(s.add_var(2));
        s.add_eq(2*a + b + 1);
        s.add_eq(2*b + a);
        s.check();
        // Expected result: SAT with a = 2, b = 3
    }

    static void test_l3() {
        scoped_solver s;
        auto a = s.var(s.add_var(2));
        auto b = s.var(s.add_var(2));
        s.add_eq(3*b + a + 2);
        s.check();
        // Expected result: SAT
    }

    static void test_l4() {
        scoped_solver s;
        auto a = s.var(s.add_var(3));
        // auto b = s.var(s.add_var(3));
        s.add_eq(4*a + 2);
        s.check();
        // Expected result: UNSAT
    }

    // Goal: test propagate_eq in case of 2*a*x + 2*b == 0
    static void test_l5() {
        scoped_solver s;
        auto a = s.var(s.add_var(3));
        auto b = s.var(s.add_var(3));
        s.add_eq(a + 2*b + 4);
        s.add_eq(a + 4*b + 4);
        s.check();
        // Expected result: UNSAT
    }

    
    /**
     * This one is unsat because a*a*(a*a - 1)
     * is 0 for all values of a.
     */
    static void test_p1() {
        scoped_solver s;
        auto a = s.var(s.add_var(2));
        auto p = a*a*(a*a - 1) + 1;
        s.add_eq(p);
        s.check();
    }

    /**
     * has solution a = 3
     */
    static void test_p2() {
        scoped_solver s;
        auto a = s.var(s.add_var(2));
        auto p = a*(a-1) + 2;
        s.add_eq(p);
        s.check();
    }


    /**
     * Check unsat of:
     * u = v*q + r
     * r < u
     * v*q > u
     */
    static void test_ineq1() {
        scoped_solver s;
        auto u = s.var(s.add_var(5));
        auto v = s.var(s.add_var(5));
        auto q = s.var(s.add_var(5));
        auto r = s.var(s.add_var(5));
        s.add_eq(u - (v*q) - r, 0);
        s.add_ult(r, u, 0);
        s.add_ult(u, v*q, 0);
        s.check();
    }

    /**
     * Check unsat of:
     * n*q1 = a - b
     * n*q2 + r2 = c*a - c*b
     * n > r2 > 0
     */
    static void test_ineq2() {
        scoped_solver s;
        auto n = s.var(s.add_var(5));
        auto q1 = s.var(s.add_var(5));
        auto a = s.var(s.add_var(5));
        auto b = s.var(s.add_var(5));
        auto c = s.var(s.add_var(5));
        auto q2 = s.var(s.add_var(5));
        auto r2 = s.var(s.add_var(5));
        s.add_eq(n*q1 - a + b);
        s.add_eq(n*q2 + r2 - c*a + c*b);
        s.add_ult(r2, n);
        s.add_diseq(n);
        s.check();
    }


    /**
     * Monotonicity example from certora (1)
     *  expected: unsat
     */
    static void test_monot1() {
        scoped_solver s;
	    auto bw = 5;

        auto tb1 = s.var(s.add_var(bw));
        auto tb2 = s.var(s.add_var(bw));
        auto a = s.var(s.add_var(bw));
        auto v = s.var(s.add_var(bw));
        auto base1 = s.var(s.add_var(bw));
        auto base2 = s.var(s.add_var(bw));
        auto elastic1 = s.var(s.add_var(bw));
        auto elastic2 = s.var(s.add_var(bw));
        auto err = s.var(s.add_var(bw));

        auto rem1 = s.var(s.add_var(bw));
        auto quot2 = s.var(s.add_var(bw));
        auto rem2 = s.var(s.add_var(bw));
        auto rem3 = s.var(s.add_var(bw));
        auto quot4 = s.var(s.add_var(bw));
        auto rem4 = s.var(s.add_var(bw));

        s.add_diseq(elastic1);

        // tb1 = (v * base1) / elastic1;
        s.add_eq((tb1 * elastic1) + rem1 - (v * base1));

        // quot2 = (a * base1) / elastic1
        s.add_eq((quot2 * elastic1) + rem2 - (a * base1));
	
	    s.add_eq(base1 + quot2 - base2);

	    s.add_eq(elastic1 + a - elastic2);

	    // tb2 = ((v * base2) / elastic2);
           s.add_eq((tb2 * elastic2) + rem3 - (v * base2));

	    // quot4 = v / (elastic1 + a);
	    s.add_eq((quot4 * (elastic1 + a)) + rem4 - v);

	    s.add_eq(quot4 + 1 - err);

        s.add_ult(tb1, tb2);
        s.check();
    }

    /**
     * Monotonicity example from certora (2)
     *  expected: unsat
     *
     *  only difference to (1) is the inequality right before the check
     */
    static void test_monot2() {
        scoped_solver s;
        auto bw = 5;
        
        auto tb1 = s.var(s.add_var(bw));
        auto tb2 = s.var(s.add_var(bw));
        auto a = s.var(s.add_var(bw));
        auto v = s.var(s.add_var(bw));
        auto base1 = s.var(s.add_var(bw));
        auto base2 = s.var(s.add_var(bw));
        auto elastic1 = s.var(s.add_var(bw));
        auto elastic2 = s.var(s.add_var(bw));
        auto err = s.var(s.add_var(bw));
        
        auto rem1 = s.var(s.add_var(bw));
        auto quot2 = s.var(s.add_var(bw));
        auto rem2 = s.var(s.add_var(bw));
        auto rem3 = s.var(s.add_var(bw));
        auto quot4 = s.var(s.add_var(bw));
        auto rem4 = s.var(s.add_var(bw));
        
        s.add_diseq(elastic1);
        
        // tb1 = (v * base1) / elastic1;
        s.add_eq((tb1 * elastic1) + rem1 - (v * base1));
        
        // quot2 = (a * base1) / elastic1
        s.add_eq((quot2 * elastic1) + rem2 - (a * base1));
        
        s.add_eq(base1 + quot2 - base2);
        
        s.add_eq(elastic1 + a - elastic2);
        
        // tb2 = ((v * base2) / elastic2);
        s.add_eq((tb2 * elastic2) + rem3 - (v * base2));
        
        // quot4 = v / (elastic1 + a);
        s.add_eq((quot4 * (elastic1 + a)) + rem4 - v);
        
        s.add_eq(quot4 + 1 - err);
        
        s.add_ult(tb1 + err, tb2);
        s.check();
    }


    // Goal: we probably mix up polysat variables and PDD variables at several points; try to uncover such cases
    // NOTE: actually, add_var seems to keep them in sync, so this is not an issue at the moment (but we should still test it later)
    // static void test_mixed_vars() {
    //     scoped_solver s;
    //     auto a = s.var(s.add_var(2));
    //     auto b = s.var(s.add_var(4));
    //     auto c = s.var(s.add_var(2));
    //     s.add_eq(a + 2*c + 4);
    //     s.add_eq(3*b + 4);
    //     s.check();
    //     // Expected result:
    // }

    // convert assertions into internal solver state
    // support small grammar of formulas.
    void internalize(solver& s, expr_ref_vector& fmls) {

    }
}


void tst_polysat() {
    polysat::test_l1();
    polysat::test_l2();
    polysat::test_l3();
    polysat::test_l4();
    polysat::test_l5();
#if 0
    // worry about this later
    polysat::test_p1();
    polysat::test_p2();
    polysat::test_ineq1();
    polysat::test_ineq2();
#endif
}

// TBD also add test that loads from a file and runs the polysat engine.
// sketch follows below:

void tst_polysat_argv(char** argv, int argc, int& i) {
    // set up SMT2 parser to extract assertions
    // assume they are simple bit-vector equations (and inequations)
    // convert to solver state.
    // std::ifstream is(argv[0]);
    // cmd_context ctx(false, &m);
    // ctx.set_ignore_check(true);
    // VERIFY(parse_smt2_commands(ctx, is));
    // auto fmls = ctx.assertions();
    // trail_stack stack;
    // solver s(stack);
    // polysat::internalize(s, fmls);
    // std::cout << s.check() << "\n";
}
