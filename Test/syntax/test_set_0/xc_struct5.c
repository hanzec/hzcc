
struct point {
  float x;
  float y;
  float z;
};

struct triangle {
  struct point P[3];
};

struct point camera_pos;
struct point camera_pointing;
struct point camera_up;

void drawSurface(struct triangle S[], int num_triangles);

struct triangle buffer[1024];

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.