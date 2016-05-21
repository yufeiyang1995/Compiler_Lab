#include <stdio.h>
extern FILE* yyin;
extern int yydebug;
extern void yyrestart(FILE* f);
extern void yyparse();
extern void semantic();
extern void interMediateCode();
extern void print_text(char* s);
void print();
 
int main(int argc, char** argv) {
	if (argc <= 2) return 1;
	FILE* f = fopen(argv[1], "r");
	if (!f){
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	//print();
	//semantic();
	interMediateCode();
	print_text(argv[2]);
	return 0;
}
