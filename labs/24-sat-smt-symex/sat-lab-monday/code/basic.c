#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define append_field(OBJ, FIELD) (*({ \
    (OBJ).FIELD = realloc((OBJ).FIELD, (++((OBJ).n_##FIELD)) * sizeof((OBJ).FIELD[0])); \
    (OBJ).FIELD + ((OBJ).n_##FIELD - 1); \
}))

/****** GLOBAL DATA STRUCTURES ******/

// Any sequence of events you want to cross-check across the basic & optimized
// solvers can be logged by calling xprintf. It will only log if the LOG_XCHECK
// flag is set by main(), i.e., if the user passes an argument.
int LOG_XCHECK = 0;
#define xprintf(...) { if (LOG_XCHECK) { fprintf(stderr, __VA_ARGS__); } }

// The number of variables and clauses. This is specified by the "p cnf ..."
// line of the input.
unsigned N_VARS = 0, N_CLAUSES = 0;

// Each clause is a list of literals, along with a count of the number of
// literals set to zero in the current partial assignment. The clause is
// implied once n_zeros == (n_lits - 1) and falsified if n_zeros == n_lits.
struct clause {
    int *literals, n_literals;
    int n_zeros;
};
struct clause *CLAUSES = NULL;

// The current partial assignment is a simple map from var_id -> assignment.
enum assignment {
    UNASSIGNED  = -1,
    FALSE       = 0,
    TRUE        = 1,
};
enum assignment *ASSIGNMENT = NULL;

// We maintain a decision stack that logs every time we assign a variable &
// why. In the Chaff paper a distinction is made between "decisions" and
// "assignments", the latter being assignments as a result of BCP. For us, the
// only difference will be the decision_type.
enum decision_type {
    IMPLIED         = 0,
    TRIED_ONE_WAY   = 1,
    TRIED_BOTH_WAYS = 2,
};
struct decision {
    unsigned var;
    enum decision_type type;
};
struct decision *DECISION_STACK = NULL;
unsigned N_DECISION_STACK = 0;

// We maintain a list of literal -> clauses having that literal. For the basic
// implementation, these lists don't need to be changed after initialization so
// we'll just use a simple heap array.
struct clause_list {
    struct clause **clauses;
    int n_clauses;
};
struct clause_list *LIT_TO_CLAUSES = NULL;

/****** HELPER METHODS ******/

// LIT_TO_CLAUSES maps literals -> ids. This converts a literal to an index
// into that map. Basically, the ordering goes -1, 1, -2, 2, ...
int literal_to_id(int literal) {
    return (2 * abs(literal)) + (literal > 0);
}

// Uses literal_to_id to index into LIT_TO_CLAUSES
struct clause_list *clauses_touching(int literal) {
    return LIT_TO_CLAUSES + literal_to_id(literal);
}

// Absolute value, used for turning a literal into a variable id.
int abs(int x) { return (x < 0) ? -x : x; }

int is_literal_true(int lit) {
    int var = abs(lit);
    return ASSIGNMENT[var] != UNASSIGNED && ASSIGNMENT[var] == (lit > 0);
}

// Called when a solution is found
int satisfiable() {
    printf("SAT\n");
    for (int i = 1; i < N_VARS; i++)
        printf("%d ", is_literal_true(i) ? i : -i);
    printf("\n");
    return 0;
}

// Called when it is proved that no solution exists
int unsatisfiable() { printf("UNSAT\n"); return 0; }

/****** KEY OPERATIONS ******/

// Attempt to assign the given literal. Then update all the n_zeros counters.
// If this assignment causes a conflict (i.e., for some clause n_zeros ==
// n_literals), this method will return 0. Otherwise it will return 1.
int set_literal(int literal, enum decision_type type) {
    int var = abs(literal);
    assert(ASSIGNMENT[var] == UNASSIGNED);
    // Update the main assignment vector
    ASSIGNMENT[var] = literal > 0;
    // And add a new node on the decision stack.
    DECISION_STACK[N_DECISION_STACK].var = var;
    DECISION_STACK[N_DECISION_STACK++].type = type;

    // Update clause counters, check if any is completely false
    assert(!"Implement me!");
}

// Undo the latest assignment on the decision stack. Then update all the
// n_zeros counters. Note that undoing an assignment can never cause a new
// conflict, so we don't need to report anything.
void unset_latest_assignment() {
    // Pop a node off the decision stack
    unsigned var = DECISION_STACK[--N_DECISION_STACK].var;
    int literal = ASSIGNMENT[var] ? var : -var;

    // Update the partial assignment
    ASSIGNMENT[var] = UNASSIGNED;

    // Iterate over the clauses containing -literal and decrement their
    // n_zeros since this literal is no longer set.
    assert(!"Implement me!");
}

/****** DP METHODS ******/

// From the paper: "The operation of decide() is to select a variable that is
// not currently assigned, and give it a value. This variable assignment is
// referred to as a decision. As each new decision is made, a record of that
// decision is pushed onto the decision stack.  This function will return false
// if no unassigned variables remain and true otherwise."
int decide() {
    // Iterate over variables until we find an unassigned one, placing it in v
    // (or return 0 if none is found).
    int v;
    assert(!"Implement me!");

    // Otherwise, try setting it false. Note this should never cause a
    // conflict, otherwise it should have been BCP'd.
    assert(set_literal(-v, TRIED_ONE_WAY));

    // Log this decision for xcheck.
    xprintf("Decide: %d\n", -v);
    return 1;
}

// From the paper: "The operation of bcp() ... is to identify any variable
// assignments required by the current variable state to satisfy f. ... if a
// clause consists of only literals with value 0 and one unassigned literal,
// then that unassigned literal must take on a value of 1 to make f sat.  ...
// In the pseudo-code from above, bcp() carries out BCP transitively until
// either there are no more implications (in which case it returns true) or a
// conflict is produced (in which case it returns false)."
int bcp() {
    int any_change = 0;
    for (size_t i = 0; i < N_CLAUSES; i++) {
        struct clause *clause = CLAUSES + i;
        if ((clause->n_zeros + 1) != clause->n_literals) continue;

        // At this point, the clause is either satisfied or implied.  For each
        // literal in the clause:
        // - If any literal is set to true, this clause is already sat so we
        //   don't need to do anything.
        // - Otherwise, if we find a literal that is unset, try to set it. If
        //   setting it causes a conflict, return 0. Otherwise, record that
        //   there was a change and go on to the next clause.
        assert(!"Implement me!");
    }

    return any_change ? bcp() : 1;
}

// From the paper: "... to deal with a conflict, we can just undo all those
// implications, flip the value of the decision assignment, and allow BCP to
// then proceed as normal. If both values have already been tried for this
// decision, then we backtrack through the decision stack until we encounter a
// decision that has not been tried both ways, and proceed from there in the
// manner described above. ... If no decision can be found which has not been
// tried both ways, that indicates that f is not satisfiable."
int resolveConflict() {
    // Unwind the decision stack by calling unset_latest_assignment() until we
    // find a decision that is only TRIED_ONE_WAY.
    // If you have unwinded the entire decision stack, return 0 (the formula is
    // unsat!)
    assert(!"Implement me!");

    // Otherwise, take that decision and flip it:
    unsigned var = DECISION_STACK[N_DECISION_STACK - 1].var;

    int new_value = !ASSIGNMENT[var];
    unset_latest_assignment();
    set_literal(new_value ? var : -var, TRIED_BOTH_WAYS);

    return 1;
}

int main(int argc, char **argv) {
    LOG_XCHECK = argc > 1;
    // Read comment lines at the start.
    for (char c; (c = getc(stdin)) == 'c';)
        while (getc(stdin) != '\n');

    assert(scanf(" cnf %u %u\n", &N_VARS, &N_CLAUSES) == 2);
    N_VARS++;

    ASSIGNMENT = malloc(N_VARS * sizeof(ASSIGNMENT[0]));
    memset(ASSIGNMENT, -1, N_VARS * sizeof(ASSIGNMENT[0]));

    DECISION_STACK = calloc(N_VARS, sizeof(DECISION_STACK[0]));

    CLAUSES = calloc(N_CLAUSES, sizeof(struct clause));

    LIT_TO_CLAUSES = calloc(N_VARS * 2, sizeof(LIT_TO_CLAUSES[0]));

    for (size_t i = 0; i < N_CLAUSES; i++) {
        int literal = 0;
        for (assert(scanf("%d ", &literal)); literal; assert(scanf("%d ", &literal))) {
            // Dedup any repeated literals in the clause.
            int repeat = 0;
            for (size_t j = 0; j < CLAUSES[i].n_literals && !repeat; j++)
                repeat = (CLAUSES[i].literals[j] == literal);
            if (repeat) continue;

            // Append to the clause's literal list.
            append_field(CLAUSES[i], literals) = literal;

            // Append to the list of clauses touching this literal. Hint: use
            // clauses_touching and append!
            assert(!"Implement me");
        }
    }

    // Basic DP from the Chaff paper:
    if (!bcp()) return unsatisfiable(); // needed to handle unit clauses
    while (1) {
        if (!decide())
            return satisfiable();

        while (!bcp())
            if (!resolveConflict())
                return unsatisfiable();
    }
}
