/*++
Copyright (c) 2021 Microsoft Corporation

Module Name:

    dd_fdd

Abstract:

    Finite domain abstraction for using BDDs as sets of integers, inspired by BuDDy's fdd module.

Author:

    Nikolaj Bjorner (nbjorner) 2021-04-20
    Jakob Rath 2021-04-20

--*/
#pragma once

#include "math/dd/dd_bdd.h"
#include "util/vector.h"
#include "util/rational.h"

namespace dd {

    enum class find_t { empty, singleton, multiple };
    std::ostream& operator<<(std::ostream& out, find_t x);

    /**
     * Finite domain abstraction over BDDs.
     */
    class fdd {
        unsigned_vector m_pos2var;  // pos -> BDD var
        unsigned_vector m_var2pos;  // var -> pos (pos = place number in the bit representation, 0 is LSB's place)
        bdd_manager*    m;
        bddv            m_var;

        static unsigned_vector seq(unsigned count, unsigned start = 0, unsigned step = 1) {
            unsigned_vector result;
            unsigned k = start;
            for (unsigned i = 0; i < count; ++i, k += step)
                result.push_back(k);
            return result;
        }

        unsigned var2pos(unsigned var) const;

    public:
        /** Initialize FDD using BDD variables from 0 to num_bits-1. */
        fdd(bdd_manager& manager, unsigned num_bits, unsigned start = 0, unsigned step = 1) : fdd(manager, seq(num_bits, start, step)) { }
        fdd(bdd_manager& manager, unsigned_vector const& vars) : fdd(manager, unsigned_vector(vars)) { }
        fdd(bdd_manager& manager, unsigned_vector&& vars);

        unsigned num_bits() const { return m_pos2var.size(); }
        unsigned_vector const& bdd_vars() const { return m_pos2var; }

        bddv const& var() const { return m_var; }

        /** Equivalent to var() != 0 */
        bdd non_zero() const;

        /** Checks whether the integer val is contained in the BDD when viewed as set of integers.
         * Precondition: the bdd only contains variables managed by this fdd.
         */
        bool contains(bdd b, rational const& val) const;

        /** Returns an integer contained in the BDD, if any, and whether the BDD is a singleton.
         * Precondition: the bdd only contains variables managed by this fdd.
         */
        find_t find(bdd b, rational& out_val) const;

        /** Like find, but returns hint if it is contained in the BDD. */
        find_t find_hint(bdd b, rational const& hint, rational& out_val) const;
    };

}
