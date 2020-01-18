# Foreign Function Interface    
  
Call C functions at runtime.  
  
reverse order of function arguments  
  
include stdio.h [-libc] -- add functions to dictionary  
  
-- create _cif_ for a function  
(FILE*) (const char*) fputs -> int   
pointer pointer fputs -> int  
  
: name # associate name with tos, pop tos
; # calls tos  
    
argn ... arg1 arg0 fun # creates a _closure_ on top of stack  
  
newline causes args to be safely parsed and put in closure  
  
argn ... arg1 arg0 fun ; #  ; calls closure thunk  
  
argn ... arg1 arg0 fun : foo # : assigns closure to foo  
  
argn ... arg1 arg0 fun ;  
# same as  
argn ... arg1 arg0 fun : foo   
foo  
  
argn ... _ arg0 fun : foo # curries arguments  
  
creates a closure with cif determined by missing args  
  
arg1 foo ; # calls with missing args  
  
How to call a function without consuming the stack?  
  
-- parser  
"argn ... _ arg0 fun" -> curried closure for fun  
  
Define variables  
  
1.23 double : d  
123 uint32 : ui  
  
Push variable onto stack  
  
d ;  

$d

{d}
  
every line is a closure  
  
keywords that do not consume the stack  
  
=0 <code executes if tos is 0>  
  
0 int : l  
0 int : w  
0 int : c  
char[1024] : buf  
// use {} for scoping to gc???  
// l ++ : l -- increment l  
  
false boolean : inspace ;  
  
r file.txt fopen ; -- FILE*    
if dup NULL == { "failed to open file" perror ; errno exit ; } 
if == NULL { "failed to open file" perror ; errno exit ; } 
while dup getc != EOF { // != EOF does not consume stack "!= x" same as "dup x !="  
	c ; 1 + : c ; // "++ c" prefix function causes side effect?  
	if dup isspace ++ w true : inspace else false : inspace ; // "? f ..." execute "..." if f is nonzero  
	if dup newline ++ l ;  
	drop -- FILE*  
}  
  
## Control flow  
  
while expr {  
	body  
}  
  
evaluate expr  
if tos true (!=0) evaluate body  
else continue  
  
  
# Remarks  
  
Use -- for stack comments and actually check the stack  
