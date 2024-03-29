
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
12345678901

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

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

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
//CHECK-NEXT: File __FILE__ Line    26 Token 126 Text ~
//CHECK-NEXT: File __FILE__ Line    27 Token 124 Text |
//CHECK-NEXT: File __FILE__ Line    28 Token  38 Text &
//CHECK-NEXT: File __FILE__ Line    29 Token  33 Text !
//CHECK-NEXT: File __FILE__ Line    31 Token 314 Text void
//CHECK-NEXT: File __FILE__ Line    32 Token 310 Text int
//CHECK-NEXT: File __FILE__ Line    33 Token 312 Text float
//CHECK-NEXT: File __FILE__ Line    34 Token 311 Text char
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
//CHECK-NEXT: File __FILE__ Line    52 Token 303 Text 12345678901
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
//CHECK-NEXT: File __FILE__ Line    81 Token 400 Text const
//CHECK-NEXT: File __FILE__ Line    82 Token 513 Text struct
//CHECK-NEXT: File __FILE__ Line    83 Token 508 Text for
//CHECK-NEXT: File __FILE__ Line    84 Token 517 Text while
//CHECK-NEXT: File __FILE__ Line    85 Token 504 Text do
//CHECK-NEXT: File __FILE__ Line    86 Token 510 Text if
//CHECK-NEXT: File __FILE__ Line    87 Token 505 Text else
//CHECK-NEXT: File __FILE__ Line    88 Token 500 Text break
//CHECK-NEXT: File __FILE__ Line    89 Token 502 Text continue
//CHECK-NEXT: File __FILE__ Line    90 Token 511 Text return
//CHECK-NEXT: File __FILE__ Line    91 Token 514 Text switch
//CHECK-NEXT: File __FILE__ Line    92 Token 501 Text case
//CHECK-NEXT: File __FILE__ Line    93 Token 503 Text default
//CHECK-NEXT: File __FILE__ Line    95 Token 306 Text todo
//CHECK-NEXT: File __FILE__ Line    96 Token 306 Text inf
//CHECK-NEXT: File __FILE__ Line    97 Token 306 Text pif
//CHECK-NEXT: File __FILE__ Line    98 Token 306 Text iffy
