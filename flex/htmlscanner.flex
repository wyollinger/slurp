/* 
 * Author: Harry Jackson: 
 *
 * License: GPL
 * For more information on poarsing HTML/SGML:
 *              http://www.w3.org/TR/WD-sgml-lex-960615/
 *
 * Description: flex++ lexical analyzer for parsing html
 *              from standard input. This is far from 
 *              complete and has been writtten for use as 
 *              the pasrser for a search engine. Any additions, 
 *              hints or constructive advice are most welcome. 
 *              This parser is far from finished and I just put 
 *               
 */

/* Options */
%option noyywrap

/* Includes/Globals */
%{
#include <stdlib.h>

int num_lines = 0;
int num_chars = 0;
int num_words = 0;
%}

/* Tag definitions */
RE	 \n
RS	 \r
SEPCHAR	 \011
SPACE	 \040
COM	"--"
CRO	"&#"
DSC	"]"
DSO	"["
ERO	"&"
ETAGO	"</"
LIT	\"
LITA	"'"
MDC  	">"
MDO 	"<!"
MSC 	"]]"
NET     "/"
PERO    "%"
PIC 	">"
PIO	"<?"
REFC	";"
STAGO	"<"
TAGC	">"
EQ      "="
UND     "_"
MINUS     "-"
COMMA   ","
SEMI_COLON ";"
COLON   ":"
PIPE    "|"
BANG    "!"
STOP    "."
ENCTAG  "/>"

Digit		[0-9]
LCLetter	[a-z]
UCLetter	[A-Z]
MixLetter       [A-Z|a-z]
Special		['()_,\-\./:=?]
LCNMCHAR	[\.-]
UCNMCHAR	[\.-]
UCHR1   "HR1"
LCHR1   "hr1"
LCHREF  ""

name_start_character	{LCLetter}|{UCLetter}
name_character		{name_start_character}|{Digit}|{LCNMCHAR}|{UCNMCHAR}

number          {Digit}
name		{name_start_character}{name_character}*
number_token	{Digit}{name_character}*
name_token	{name_character}+
s		{SPACE}|{RE}|{RS}|{SEPCHAR}
ps		({SPACE}|{RE}|{RS}|{SEPCHAR})+
    /* trailing white space */
ws		({SPACE}|{RE}|{RS}|{SEPCHAR})*
reference_end	{REFC}|{RE}

literal		({LIT}[^\"]*{LIT})|({LITA}[^\']*{LITA})

ignore  {COM}|{CRO}|{DSC}|{DSO}|{ERO}|{ETAGO}|{LIT}|{LITA}|{MDC}|{MDO}|{MSC}|{NET}|{PERO}|{PIC}|{PIO}|{REFC}|{STAGO}|{TAGC}|{EQ}|{UND}|{COLON}|{SEMI_COLON}|{COMMA}|{MINUS}|{PIPE}|{BANG}|{STOP}|{Digit}|{SPACE}{1,}|{SPACE}

word {MixLetter}+|{number}{4,}

s_hr1_tag       {STAGO}{UCHR1}{TAGC}|{STAGO}{LCHR1}{TAGC}
c_hr1_tag       {ETAGO}{UCHR1}{TAGC}|{ETAGO}{LCHR1}{TAGC}
anon_open_tag   {STAGO}{MixLetter}+{TAGC}
anon_close_tag ({ETAGO}{MixLetter}+{TAGC})|{ENCTAG}
href            {}{EQ}

%x TAG
%x CDATA
%x ATTR
%x ATTRVAL

%x HREF
%x HREFTEXT

%%

\n {  ++num_lines; 
  printf("# of lines = %d, # of chars = %d\n", num_lines, num_chars);
}

. {
    ++num_chars;
}

{s_hr1_tag}?	 {
    printf("o_hr1 %s\n", yytext);
    BEGIN(TAG);
}
{c_hr1_tag}?	 {
    printf("c_hr1  %s\n", yytext);
    BEGIN(INITIAL);
}
{word}?     {
   printf(" %s ", yytext);
}
{anon_close_tag}? {
    printf("\nT=%s\n", yytext);
    BEGIN(INITIAL);
}
{anon_open_tag}? {
    printf("T=%s\n", yytext);
    BEGIN(TAG);
}
{ignore}? {
    printf("");
}
<TAG>{word}? {
    printf("it=");
    BEGIN(ATTR);
}

<ATTR>{word}{EQ}? {
    printf("attr=%s\n", yytext);
}
      /* <a ^href = "xxx"> -- attribute name */
<ATTR>{name}{SPACE}*={ws} {
    printf("attr=%s\n", yytext);
    BEGIN(ATTRVAL);
}
       /* <img src="xxx" ^ismap> -- name */
<ATTR>{name}{ws}		{
    printf("attr=%s\n", yytext);
}
    
   /* <a name = ^xyz> -- name token */
<ATTRVAL>{name_token}{ws}	{
    printf("attr=%s\n", yytext);
    BEGIN(ATTR);
}
    
      /* <a href = ^"a b c"> -- literal */
<ATTRVAL>{literal}{ws}		{
    printf("attr=%s\n", yytext);
    BEGIN(ATTR);
}
    

%%

int *types = NULL;
char **strings = NULL;
size_t *lengths = NULL;
int qty = 0;

/*

int main( int , char**  )
    {
    FlexLexer* lexer = new yyFlexLexer;
    while(lexer->yylex() != 0)
     ;
    return 0;
}

*/
