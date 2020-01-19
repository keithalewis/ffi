# Foreign Function Interface    

Call C functions at runtime.  

Open a dynamic link library.

Load symbols from the library.

Call the function.

## Open a dynamically linked library and give it a name.  

$RTLD_LAZY libc.so.6 dlopen : libc -- pointer // document it is a pointer
$RTLD_LAZY libc.so.6 dlopen : libc == pointer //makes sure libc is a pointer
`RTLD_LAZY libc.so.6 dlopen : libc == pointer //use ` for immediate compilation

$VAR is looked up (in global dictionary?) when ":" compiles the string stack
`VAR is looked up immediately and put into thunk

## Get the symbol.   

puts $libc dlsym : puts -- pointer

Create a thunk that knows the stack arguments

`FFI_DEFAULT_ABI `SINT [ `POINTER ] cif : puts_cif


Implicitly specify the signature.  

"Hello World" puts  


Use `...` for immediate execution



compile: stream -> thunk // stream of characters to unit of execution  
execute: (thunk x dictionary) -> (stack -> stack')  // a thunk and dictionary takes a stack and returns a stack

Not really a function stack -> stack' since it modifies the thunk stack

stream: sequence of charaters

thunk: function with arguments supplied from a stack of thunks that operates on the stack of thunks

Execute can look up thunks by name in a dictionary if they are deferred by $
and replace $name by the thunk.

stream ; -- thunk // put thunk on stack
thunk : name // add thunk to dictionary with name


Call the function: f(x, ...)

Call the function given a stack f(s)
	get void* pointers to each element of stack: ffi_call

call(function, stack)
// use ABI to control stack effect?

Curry a function.
	Specify a subset of arguments
	Stack is the complement of missing arguments.

compile: turn a stream of characters into a _thunk_  
execute: call the thunk  
environment: a thunk stack and a dictionary of thunks

-- is used to describe the stack 

while read token
	if token is not keyword // don't look up every token
		push token on string stack
	else
		compile string stack to thunk and push on thunk stack
		// the tos must be in the dictionary and have a cif
		// thunks can have missing arguments ("_") that they expect to find on the thunk stack
		// the cif is used to ensure type safety
		lookup token and execute its thunk with the current thunk stack and dictionary

thunk compile(string_stack)
	symbol = pop // the name of a C function that has been loaded
	// use the symbol cif to safely pop stack arguments
	for i = nargs - 1; ... 
		thunk_stack.push(parse type[i] string_stack.pop)

thunk parse type string
	if immediate string
		return 
	else
		return parse type string

keywords  

; - compile string stack and push on thunk stack
: name - compile string stack and put thunk in dict with name
{ body } - compile body and push on thunk stack
  
reverse order of function arguments  

global dictionary of last resort
local dictionary stack
  
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

using dict = std::map<string, closure>;  
using dicts = std::list<dict>; // active dict first

closure& c = lookup(dicts);
lookup(dicts ds, string s)
{
	for (d : ds) {
		auto i = d.find(s);

		if (i != d.end()) {
			return i.second;
		}
	}

	return not_found;
}

while (read tok) {
	if (tok == '\n') {
		make_closue // name = tos, use that to parse args
	}
	if (tok == ':') {
		make_closure // name = tos, use that to parse args
		name = next_token
		add(name, tos);
	}
	if (tok == ';') {
		make_closure // name = tos, use that to parse args
		call tos
	}
}
  
# Remarks  
  
Use -- for stack comments and actually check the stack  
