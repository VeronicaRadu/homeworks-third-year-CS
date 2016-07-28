/*Rotaru Alexandru Andrei 335CC*/
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int ticnt = 0;
unsigned int tvcnt = 0;
unsigned int icnt = 0;
unsigned int vcnt = 0;

char** idents = NULL;
char** values = NULL;

%}
%option noyywrap
%x TCOLUMN RVALUES
s [ /:\.\'’$\-%]
w [A-Za-z0-9]*
ident {w}({s}{w})*
%%
<INITIAL>"Boarding Pass\n" {
	idents[ticnt++] = NULL;
	values[tvcnt++] = NULL;
	BEGIN(INITIAL);
}
<INITIAL>{ident} {
	idents[ticnt + icnt] = malloc(yyleng + 1);
	strcpy(idents[ticnt + icnt], yytext);
	if(':' != idents[ticnt + icnt][yyleng - 1])
		idents[ticnt + icnt][yyleng] = ':';	
	idents[ticnt + (icnt++)][yyleng + 1] = '\0';
	BEGIN(INITIAL);	
}
<INITIAL>[\n] {
	icnt = 0;
	BEGIN(INITIAL);
}
<INITIAL>[\t]+ {
	if(1 == icnt)
		BEGIN(TCOLUMN);
	else
		BEGIN(INITIAL); 
}
<TCOLUMN>{ident} {
	idents[ticnt + icnt] = malloc(yyleng + 1);
	strcpy(idents[ticnt + icnt], yytext);
	if(':' != idents[ticnt + icnt][yyleng - 1])
		idents[ticnt + icnt][yyleng] = ':';	
	idents[ticnt + (icnt++)][yyleng + 1] = '\0';
	BEGIN(TCOLUMN);
}
<TCOLUMN>[\t]+ {
	BEGIN(TCOLUMN);
}
<TCOLUMN>[\n] {
	BEGIN(RVALUES);
}

<RVALUES>{ident} {
	if(yytext[yyleng - 1] == ':') yytext[yyleng - 1] = '\0';
	values[tvcnt + (vcnt++)] = strdup(yytext);
	BEGIN(RVALUES);
}

<RVALUES>[\t]+ {
	BEGIN(RVALUES);
}

<RVALUES>[\n] {
	if(icnt == vcnt) {
		ticnt += icnt;
		tvcnt += vcnt;
	}
	icnt = 0;
	vcnt = 0;
	BEGIN(INITIAL);
}


%%

main() {
	idents = malloc(35 * sizeof(char*));
	values = malloc(35 * sizeof(char*));
	yylex();
	unsigned int i = 0;
	unsigned int maxl = 0;
	for(i = 1; i < ticnt; i++) {
		if(NULL == idents[i]) continue;
		unsigned int l = strlen(idents[i]);
		if(l > maxl) {
			maxl = l;
		}
	}
	for(i = 1; i < ticnt; i++) {
		if(NULL == idents[i]) {
			printf("\n\n");
		} else {
			printf("%-*s%s\n", maxl, idents[i], values[i]);
		}
		free(idents[i]);
		free(values[i]);
	}
	free(idents);
	free(values);
}
