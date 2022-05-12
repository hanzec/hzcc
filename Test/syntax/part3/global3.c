
int x, y[20], z;
float velocity[14];

int x(char a);

float just, more, vars;

int y(float z);

char buffer[1024], char filename[128];

char g, h, i;
int b;

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 11 near text char
//CHECK-NEXT:         Expected identifier