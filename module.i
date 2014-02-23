/* File : example.i */
%module example
%{
extern "C" {
/* Put headers and other declarations here */
extern double My_variable;
extern int    fact(int);
extern int    my_mod(int n, int m);
}
%}

extern double My_variable;
extern int    fact(int);
extern int    my_mod(int n, int m);

