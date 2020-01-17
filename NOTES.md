# Foreign Function Interface

fun arg0 arg1 ... argn

push argn, ..., arg1, arg 0 onto call stack and call fun

argn ... arg1 arg0 fun instead?

l: int 0
w: int 0
c: int 0
buf: char[1024]
rw file.txt fopen -- FILE*
1024 buf fgets -- char*
dup 0 =? "fgets failed" perror; errno exit
ws: " \t\r"
ws strtok -- char*

rw file.txt fopen -- FILE*
do # push ftell on control stack
dup getc -- FILE* int
dup EOF =? 0 exit; # print l,w,c?
= EOF 0 exit
!= EOF
++c
dup isspace -- int
dup 0 !=? ++w # set in space?
"\n" =? ++l
TRUE while # pop and fseek top of control stack
