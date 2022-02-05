
/*
   Just a list of tokens,
   to test lexers.  Won't parse.
*/

,
.
;
(
)
[
]
{
}
>
<
=
+
-
*
/
%
:
?
~
|
&
!

void
int
float
char

void_ident
int_ident
float_ident
char_ident

'a'
'4'
';'
' '

0
5
42
157
12345
123456789
12345678901234567890

1.0
12.3
12.34567890
1234567890.1234567890

"string"
"string with spaces"
"string with /* a comment */ inside"
"hello, world!\n"

// Symbols

==
!=
>=
<=
++
--
||
&&
+=
-=
*=
/=

// Keywords

const
struct
for
while
do
if
else
break
continue
return
switch
case
default

todo
inf
pif
iffy

//RUN: %mycc -1 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules
//RUN: %mycc -1 "%s" -o "%t" && cat "%t" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules
//RUN: %mycc -1 "%s" -o "%t" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules --check_prefix="CHECK_ERR"
//RUN: %mycc --fno_color --fnice_error -1 "%s" -o "%t" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules --check_prefix="CHECK_ERR_NICE"

/**
 * Parser test
 */
//CHECK:      File __FILE__ Line     7 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     8 Token  46 Text .
//CHECK-NEXT: File __FILE__ Line     9 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    10 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line    11 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line    12 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line    13 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line    14 Token 123 Text {
//CHECK-NEXT: File __FILE__ Line    15 Token 125 Text }
//CHECK-NEXT: File __FILE__ Line    16 Token  62 Text >
//CHECK-NEXT: File __FILE__ Line    17 Token  60 Text <
//CHECK-NEXT: File __FILE__ Line    18 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line    19 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line    20 Token  45 Text -
//CHECK-NEXT: File __FILE__ Line    21 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line    22 Token  47 Text /
//CHECK-NEXT: File __FILE__ Line    23 Token  37 Text %
//CHECK-NEXT: File __FILE__ Line    24 Token  58 Text :
//CHECK-NEXT: File __FILE__ Line    25 Token  63 Text ?
//CHECK-NEXT: File __FILE__ Line    26 Token 306 Text ~
//CHECK-NEXT: File __FILE__ Line    27 Token 124 Text |
//CHECK-NEXT: File __FILE__ Line    28 Token  38 Text &
//CHECK-NEXT: File __FILE__ Line    29 Token  33 Text !
//CHECK-NEXT: File __FILE__ Line    31 Token 301 Text void
//CHECK-NEXT: File __FILE__ Line    32 Token 301 Text int
//CHECK-NEXT: File __FILE__ Line    33 Token 301 Text float
//CHECK-NEXT: File __FILE__ Line    34 Token 301 Text char
//CHECK-NEXT: File __FILE__ Line    36 Token 306 Text void_ident
//CHECK-NEXT: File __FILE__ Line    37 Token 306 Text int_ident
//CHECK-NEXT: File __FILE__ Line    38 Token 306 Text float_ident
//CHECK-NEXT: File __FILE__ Line    39 Token 306 Text char_ident
//CHECK-NEXT: File __FILE__ Line    41 Token 302 Text 'a'
//CHECK-NEXT: File __FILE__ Line    42 Token 302 Text '4'
//CHECK-NEXT: File __FILE__ Line    43 Token 302 Text ';'
//CHECK-NEXT: File __FILE__ Line    44 Token 302 Text ' '
//CHECK-NEXT: File __FILE__ Line    46 Token 303 Text 0
//CHECK-NEXT: File __FILE__ Line    47 Token 303 Text 5
//CHECK-NEXT: File __FILE__ Line    48 Token 303 Text 42
//CHECK-NEXT: File __FILE__ Line    49 Token 303 Text 157
//CHECK-NEXT: File __FILE__ Line    50 Token 303 Text 12345
//CHECK-NEXT: File __FILE__ Line    51 Token 303 Text 123456789
//CHECK-NEXT: File __FILE__ Line    52 Token 303 Text 12345678901234567890
//CHECK-NEXT: File __FILE__ Line    54 Token 304 Text 1.0
//CHECK-NEXT: File __FILE__ Line    55 Token 304 Text 12.3
//CHECK-NEXT: File __FILE__ Line    56 Token 304 Text 12.34567890
//CHECK-NEXT: File __FILE__ Line    57 Token 304 Text 1234567890.1234567890
//CHECK-NEXT: File __FILE__ Line    59 Token 305 Text "string"
//CHECK-NEXT: File __FILE__ Line    60 Token 305 Text "string with spaces"
//CHECK-NEXT: File __FILE__ Line    61 Token 305 Text "string with /* a comment */ inside"
//CHECK-NEXT: File __FILE__ Line    62 Token 305 Text "hello, world!\n"
//CHECK-NEXT: File __FILE__ Line    66 Token 351 Text ==
//CHECK-NEXT: File __FILE__ Line    67 Token 352 Text !=
//CHECK-NEXT: File __FILE__ Line    68 Token 353 Text >=
//CHECK-NEXT: File __FILE__ Line    69 Token 354 Text <=
//CHECK-NEXT: File __FILE__ Line    70 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line    71 Token 356 Text --
//CHECK-NEXT: File __FILE__ Line    72 Token 357 Text ||
//CHECK-NEXT: File __FILE__ Line    73 Token 358 Text &&
//CHECK-NEXT: File __FILE__ Line    74 Token 361 Text +=
//CHECK-NEXT: File __FILE__ Line    75 Token 362 Text -=
//CHECK-NEXT: File __FILE__ Line    76 Token 363 Text *=
//CHECK-NEXT: File __FILE__ Line    77 Token 364 Text /=
//CHECK-NEXT: File __FILE__ Line    81 Token 401 Text const
//CHECK-NEXT: File __FILE__ Line    82 Token 402 Text struct
//CHECK-NEXT: File __FILE__ Line    83 Token 403 Text for
//CHECK-NEXT: File __FILE__ Line    84 Token 404 Text while
//CHECK-NEXT: File __FILE__ Line    85 Token 405 Text do
//CHECK-NEXT: File __FILE__ Line    86 Token 406 Text if
//CHECK-NEXT: File __FILE__ Line    87 Token 407 Text else
//CHECK-NEXT: File __FILE__ Line    88 Token 408 Text break
//CHECK-NEXT: File __FILE__ Line    89 Token 409 Text continue
//CHECK-NEXT: File __FILE__ Line    90 Token 410 Text return
//CHECK-NEXT: File __FILE__ Line    91 Token 411 Text switch
//CHECK-NEXT: File __FILE__ Line    92 Token 412 Text case
//CHECK-NEXT: File __FILE__ Line    93 Token 413 Text default
//CHECK-NEXT: File __FILE__ Line    95 Token 306 Text todo
//CHECK-NEXT: File __FILE__ Line    96 Token 306 Text inf
//CHECK-NEXT: File __FILE__ Line    97 Token 306 Text pif
//CHECK-NEXT: File __FILE__ Line    98 Token 306 Text iffy

/**
* Base error message
*/
//CHECK_ERR: Lexer error in file __FILE__ line 51 near text: 1
//CHECK_ERR-NEXT:         integer number out of range

/**
* Error checking for nice colored error messages
*/
//CHECK_ERR_NICE:      __FILE__:51:0: error: integer number out of range
//CHECK_ERR_NICE-NEXT:            51|12345678901234567890
//CHECK_ERR_NICE-NEXT:               ^