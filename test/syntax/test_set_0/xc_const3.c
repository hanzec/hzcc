
const int A[50];

char const message[105];

int sum(int const A[], int N);


//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.