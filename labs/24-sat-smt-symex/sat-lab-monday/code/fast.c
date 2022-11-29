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

// Each clause is a list of literals, along with two of them that are currently
// being watched. Notice there is no counter here!
struct clause {
    int *literals, n_literals;
    int watching[2];
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

// In this implementation instead of tracking literal -> clauses containing
// that literal, we only track literal -> clauses *watching* that literal. We
// need to modify this during the solve, so we'll use a linked list instead of
// an array.
struct clause_list {
    struct clause *clause;
    struct clause_list *next;
};
struct clause_list **LIT_TO_CLAUSES = NULL;

// Here we track the list of literal implications discovered when visiting
// clauses. We represent this in two ways, a direct list (BCP_LIST, N_BCP_LIST)
// of the clauses and a bool vector IS_BCP_LISTED to determine if a given
// literal is already in the list.
int *BCP_LIST = NULL;
unsigned N_BCP_LIST = 0;
char *IS_BCP_LISTED = NULL;

/****** HELPER METHODS ******/

// LIT_TO_CLAUSES maps literals -> ids. This converts a literal to an index
// into that map. Basically, the ordering goes -1, 1, -2, 2, ...
int literal_to_id(int literal) {
    return (2 * abs(literal)) + (literal > 0);
}

// Uses literal_to_id to index into LIT_TO_CLAUSES
struct clause_list **clauses_watching(int literal) {
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

// Specify the @which'th watched literal for clause CLAUSES[clause_i]. Called
// by main().
void init_watcher(int clause_i, int which, int literal) {
    // Set watching[which] = literal on clause clause_i, then prepend the
    // corresponding watching node to the correct LIT_TO_CLAUSES list.
    assert(!"Implement me!");
}

// Add a literal to the BCP list.
void queue_bcp(int literal) {
    // Append the literal to BCP_LIST, if it is not already in the list. Don't
    // forget you can use IS_BCP_LISTED as a bool vector version of BCP_LIST.
    assert(!"Implement me!");
}

// Pop a literal from the BCP list.
int dequeue_bcp() {
    // Pop a literal off of BCP_LIST and return it. Don't forget to update
    // IS_BCP_LISTED.
    assert(!"Implement me!");
}

/****** KEY OPERATIONS ******/

// Attempt to assign the given literal. Any clauses that are watching -lit get
// visited and processed.
int set_literal(int literal, enum decision_type type) {
    int var = abs(literal);
    // Update the main assignment vector
    ASSIGNMENT[var] = literal > 0;
    // And add a new node on the decision stack.
    DECISION_STACK[N_DECISION_STACK].var = var;
    DECISION_STACK[N_DECISION_STACK++].type = type;

    // Visit any clauses that contain a literal now made false.
    for (struct clause_list **w = clauses_watching(-literal); *w;) {
        struct clause *clause = (*w)->clause;
        int watch_id = (clause->watching[0] == -literal) ? 0 : 1;
        int other_watch = clause->watching[!watch_id];
        int new_watch_lit = 0;

        // Look for an unwatched literal not set to zero in this clause,
        // placing it in new_watch_lit (or set new_watch_lit = 0 if all
        // literals are either watched or zero).
        assert(!"Implement me");

        if (new_watch_lit) {
            // Switch the watcher to be watching this new literal instead.
            // Note: we need to change both the clause struct itself as well as
            // move its watcher element onto the correct linked list in
            // LIT_TO_CLAUSES.
            assert(!"Implement me");
        } else {
            // No other non-zero literal was found in the clause. This is a
            // conflict if the other watched literal is zero, else it is an
            // implication we need to BCP.
            if (is_literal_true(-other_watch)) {
                // CONFLICT
                return 0; // trigger a resolution.
            } else if (is_literal_true(other_watch)) {
                // This clause is already sat, don't have to do anything.
            } else {
                // BCP IMPLICATION, tell bcp() to set other_watch to true.
                queue_bcp(other_watch);
            }
            w = &((*w)->next);
        }
    }
    return 1;
}

// Undo the latest assignment on the decision stack. Then update all the
// n_zeros counters. Note that undoing an assignment can never cause a new
// conflict, so we don't need to report anything.
void unset_latest_assignment() {
    // Pop a node off the decision stack
    unsigned var = DECISION_STACK[--N_DECISION_STACK].var;
    // Update the partial assignment
    ASSIGNMENT[var] = UNASSIGNED;
    // No longer need to update any counters or watched literals!
}

/****** DP METHODS ******/

// From the paper: "The operation of decide() is to select a variable that is
// not currently assigned, and give it a value. This variable assignment is
// referred to as a decision. As each new decision is made, a record of that
// decision is pushed onto the decision stack.  This function will return false
// if no unassigned variables remain and true otherwise."
int decide() {
    // Iterate over variables until we find an unassigned one, placing it in v
    // (or return 0 if none is found). This can probably be exactly the same as
    // in basic.c.
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
    while (N_BCP_LIST) {
        if (!set_literal(dequeue_bcp(), IMPLIED))
            return 0;
    }
    return 1;
}

// From the paper: "... to deal with a conflict, we can just undo all those
// implications, flip the value of the decision assignment, and allow BCP to
// then proceed as normal. If both values have already been tried for this
// decision, then we backtrack through the decision stack until we encounter a
// decision that has not been tried both ways, and proceed from there in the
// manner described above. ... If no decision can be found which has not been
// tried both ways, that indicates that f is not satisfiable."
int resolveConflict() {
    // Clear the pending BCP list (don't forget IS_BCP_LISTED!), then use
    // unset_latest_assignment() to unwind the decision stack until we find a
    // decision that is only TRIED_ONE_WAY. If no such decision is found,
    // return 0.
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

    BCP_LIST = calloc(2 * N_VARS, sizeof(BCP_LIST[0]));
    IS_BCP_LISTED = calloc(2 * N_VARS, sizeof(IS_BCP_LISTED[0]));

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
        }
    }

    for (size_t i = 0; i < N_CLAUSES; i++) {
        if (!CLAUSES[i].n_literals) return unsatisfiable();
        if (CLAUSES[i].n_literals == 1) {
            // If a clause has only one literal, it must be set.
            queue_bcp(CLAUSES[i].literals[0]);
        } else {
            // Watch the first 2 literals.
            init_watcher(i, 0, CLAUSES[i].literals[0]);
            init_watcher(i, 1, CLAUSES[i].literals[1]);
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
