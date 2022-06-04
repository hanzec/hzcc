
  if

  else

  ifif
  ifelse
  elseif

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     2 Token 406 Text if
//CHECK-NEXT: File __FILE__ Line     4 Token 407 Text else
//CHECK-NEXT: File __FILE__ Line     6 Token 306 Text ifif
//CHECK-NEXT: File __FILE__ Line     7 Token 306 Text ifelse
//CHECK-NEXT: File __FILE__ Line     8 Token 306 Text elseif