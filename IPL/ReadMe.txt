IPL stand for "Interpreted Programming Language".

IPL is Dynamic language that I have been working on, on my spare time.
it compile the into byte codes that the Virtual Machine(VM) will use to run the application.
to run a program on the main file I a passing a char string containg the source of the progrma to run.
as for now it support variables that can be integers, floats, bolean, string, and vectors. Because it
is dynamic language just by saying n=1; it is concider an interget.

so far the syntax supported are as follows

//loops
for i=1 to 5
	print(i);
endfor;

/////////////

if(i=0) then
	print("Cero");
else
	print("one");
endif;

///---------------------------------------
you can pass a pointer to a C function that the interpreter can call at run time.
