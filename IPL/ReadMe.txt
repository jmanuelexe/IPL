IPL stand for "Interpreted Programming Language".

IPL is Dynamic language that I have been working on, on my spare time.
it compile the into byte codes that the Virtual Machine(VM) will use to run the application.
to run a program on the main file I a passing a char string containg the source of the progrma to run.
as for now it support variables that can be integers, floats, bolean, string, and vectors. Because it
is dynamic language just by saying n=1; it is concider an interget.

so far the syntax supported are as follows
v = 0;		// type is integer
v = 0.5;		// type is float
v = "this is a string";	//the length of the string is place on the first element so the max length is 255 chars

//functions declared with the funtion keyword.
function add(a, b)
{
	return a+b;
}
print(add(5, 7));

//loop only supported with for as follow
for i=1 to 5
	print(i);
endfor;

/////////////
if(i=0) then
	print("Cero");
else
	print("one");
endif;

// one dimencional array
v[0]="hello world";
v[1]=1;
v[2]=2.2;

print(v);

///---------------------------------------
you can pass a pointer to a C function that the interpreter can call at run time.

by calling addCFunction

TODO LIST
1 - After calling return from a function, the return value is left at the stack.
	Because of that the value is left on the stack which is bad after many calls it can lead to stack overfloat
2 - Array reference counting is not implemented. so the clean up happends at the end of the program.
3 - Objects are not implemente yet.
4 - while loop is not implemented(this is trivial).
5 - Closure is not implemented(this is trivial).