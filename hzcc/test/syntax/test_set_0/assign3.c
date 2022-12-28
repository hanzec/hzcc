
int bar(int a)
{
  int b;
  int c;

  c = b = a;

  a += ( c += (b += 2) + 1 );

  if ( (a-=3) < (b-=4) ) {

    a *= (b *= 4) / 2;

  } else {

    a /= (b /= 2) + 1;

  }

  c += ++a + ++b;
  c -= a++ + b++;

  c *= a-- + b--;
  c /= --a + --b + 1;

  return ++a < ++b + ++c;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.