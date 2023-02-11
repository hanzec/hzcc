
/*
 * Extra credit features
 */

char a = '\n';

int b = 0x03a2ce;

float c = 2.99792458e8;

const char* d = "To put \" in a string, escape it with \\";

// Increase length as needed for your lexer
int this_is_a_very_long_identifier_name_designed_to_exceed_the_lexeme_limit;

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ Line     6 Token 311 Text char
//CHECK-NEXT: File __FILE__ Line     6 Token 306 Text a
//CHECK-NEXT: File __FILE__ Line     6 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line     6 Token 302 Text '\n'
//CHECK-NEXT: File __FILE__ Line     6 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     8 Token 310 Text int
//CHECK-NEXT: File __FILE__ Line     8 Token 306 Text b
//CHECK-NEXT: File __FILE__ Line     8 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line     8 Token 303 Text 0x03a2ce
//CHECK-NEXT: File __FILE__ Line     8 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    10 Token 312 Text float
//CHECK-NEXT: File __FILE__ Line    10 Token 306 Text c
//CHECK-NEXT: File __FILE__ Line    10 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line    10 Token 304 Text 2.99792458e8
//CHECK-NEXT: File __FILE__ Line    10 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    12 Token 400 Text const
//CHECK-NEXT: File __FILE__ Line    12 Token 311 Text char
//CHECK-NEXT: File __FILE__ Line    12 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line    12 Token 306 Text d
//CHECK-NEXT: File __FILE__ Line    12 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line    12 Token 305 Text "To put \" in a string, escape it with \\"
//CHECK-NEXT: File __FILE__ Line    12 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    15 Token 310 Text int
//CHECK-NEXT: File __FILE__ Line    15 Token 306 Text this_is_a_very_long_identifier_name_designed_to_exceed_the_lexeme_limit
//CHECK-NEXT: File __FILE__ Line    15 Token  59 Text ;