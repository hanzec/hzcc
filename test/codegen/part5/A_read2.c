
void test(float f);

void test1()
{
  char A[10];
  char c;

  c = A[3+4];
  return;
}

int a;

void test2(int b)
{
  int A[11];
  a = A[ A[1] ];
  return;
}

void test3(int i)
{
  float A[12];
  test(A[i*i+1]);
  return;
}
