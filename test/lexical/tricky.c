
/* Tricky cases */

struct thing {
	int x, y;
};

int main(int argc, const char* const* argv)
{
	printf("This is a C++ comment: // evil\n");
	3;
	'4';
	// Also kind of evil: /*
	not in the comment.
	/* More strange comments /* /* /* */
		*/   // <- outside the comment, so * and /
	if breakfast then eat; else starve;

	/*/ Still in a comment /*/
	out;
	/**/
	/* */
	/***/
	/***********/

	// Fun with symbols
	//
	x + y ++ z +++ what ++++ the +++++ heck ?

	charity
	re/**/char/**/ge
	avoid
	a// another split
void

	BYE

	// Bad symbols, lexer should complain

	$ `
}

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ Line     4 Token 513 Text struct
//CHECK-NEXT: File __FILE__ Line     4 Token 306 Text thing
//CHECK-NEXT: File __FILE__ Line     4 Token 123 Text {
//CHECK-NEXT: File __FILE__ Line     5 Token 310 Text int
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text x
//CHECK-NEXT: File __FILE__ Line     5 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text y
//CHECK-NEXT: File __FILE__ Line     5 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     6 Token 125 Text }
//CHECK-NEXT: File __FILE__ Line     6 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     8 Token 310 Text int
//CHECK-NEXT: File __FILE__ Line     8 Token 306 Text main
//CHECK-NEXT: File __FILE__ Line     8 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     8 Token 310 Text int
//CHECK-NEXT: File __FILE__ Line     8 Token 306 Text argc
//CHECK-NEXT: File __FILE__ Line     8 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     8 Token 400 Text const
//CHECK-NEXT: File __FILE__ Line     8 Token 311 Text char
//CHECK-NEXT: File __FILE__ Line     8 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line     8 Token 400 Text const
//CHECK-NEXT: File __FILE__ Line     8 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line     8 Token 306 Text argv
//CHECK-NEXT: File __FILE__ Line     8 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     9 Token 123 Text {
//CHECK-NEXT: File __FILE__ Line    10 Token 306 Text printf
//CHECK-NEXT: File __FILE__ Line    10 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line    10 Token 305 Text "This is a C++ comment: // evil\n"
//CHECK-NEXT: File __FILE__ Line    10 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line    10 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    11 Token 303 Text 3
//CHECK-NEXT: File __FILE__ Line    11 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    12 Token 302 Text '4'
//CHECK-NEXT: File __FILE__ Line    12 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    14 Token 306 Text not
//CHECK-NEXT: File __FILE__ Line    14 Token 306 Text in
//CHECK-NEXT: File __FILE__ Line    14 Token 306 Text the
//CHECK-NEXT: File __FILE__ Line    14 Token 306 Text comment
//CHECK-NEXT: File __FILE__ Line    14 Token  46 Text .
//CHECK-NEXT: File __FILE__ Line    16 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line    16 Token  47 Text /
//CHECK-NEXT: File __FILE__ Line    17 Token 510 Text if
//CHECK-NEXT: File __FILE__ Line    17 Token 306 Text breakfast
//CHECK-NEXT: File __FILE__ Line    17 Token 306 Text then
//CHECK-NEXT: File __FILE__ Line    17 Token 306 Text eat
//CHECK-NEXT: File __FILE__ Line    17 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    17 Token 505 Text else
//CHECK-NEXT: File __FILE__ Line    17 Token 306 Text starve
//CHECK-NEXT: File __FILE__ Line    17 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    20 Token 306 Text out
//CHECK-NEXT: File __FILE__ Line    20 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    28 Token 306 Text x
//CHECK-NEXT: File __FILE__ Line    28 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line    28 Token 306 Text y
//CHECK-NEXT: File __FILE__ Line    28 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line    28 Token 306 Text z
//CHECK-NEXT: File __FILE__ Line    28 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line    28 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line    28 Token 306 Text what
//CHECK-NEXT: File __FILE__ Line    28 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line    28 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line    28 Token 306 Text the
//CHECK-NEXT: File __FILE__ Line    28 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line    28 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line    28 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line    28 Token 306 Text heck
//CHECK-NEXT: File __FILE__ Line    28 Token  63 Text ?
//CHECK-NEXT: File __FILE__ Line    30 Token 306 Text charity
//CHECK-NEXT: File __FILE__ Line    31 Token 306 Text re
//CHECK-NEXT: File __FILE__ Line    31 Token 311 Text char
//CHECK-NEXT: File __FILE__ Line    31 Token 306 Text ge
//CHECK-NEXT: File __FILE__ Line    32 Token 306 Text avoid
//CHECK-NEXT: File __FILE__ Line    33 Token 306 Text a
//CHECK-NEXT: File __FILE__ Line    34 Token 314 Text void
//CHECK-NEXT: File __FILE__ Line    36 Token 306 Text BYE