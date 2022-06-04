dood
oddo
do_
_do
do

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 306 Text dood
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text oddo
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text do_
//CHECK-NEXT: File __FILE__ Line     4 Token 306 Text _do
//CHECK-NEXT: File __FILE__ Line     5 Token 405 Text do