%option stack 8bit noyywrap yylineno reentrant
 
%{
 
#include <iostream>
#include <vector>
#include <string>
 
static std::vector<std::string> hrefs;
inline void yyerror(const char *msg) { std::cerr << msg << std::endl; }
 
%}
 
SPACE   [ \t]
ID      [[:alpha:]]([[:alnum:]]|:|-|_)*
HREF    [Hh][Rr][Ee][Ff]{SPACE}*={SPACE}*
ATTR    {ID}{SPACE}*={SPACE}*
 
%x X_COMMENT X_TAG
%x X_DONTCARE X_DCA X_DCP
%x X_REF1 X_REFA X_REFP
 
%%
 
<INITIAL,X_COMMENT>"<!--"       yy_push_state(X_COMMENT, yyscanner);
<X_COMMENT>"-->"                yy_pop_state(yyscanner);
<X_COMMENT>.|\n                 ;
 
"<"{ID}                         yy_push_state(X_TAG, yyscanner);
<X_TAG>">"                      yy_pop_state(yyscanner);
<X_TAG>{HREF}                   yy_push_state(X_REF1, yyscanner);
<X_TAG>{ATTR}                   yy_push_state(X_DONTCARE, yyscanner);
<X_TAG>.|\n                     ;
 
<X_REF1>\"                      yy_push_state(X_REFA, yyscanner);
<X_REF1>\'                      yy_push_state(X_REFP, yyscanner);
<X_REF1>{SPACE}|\n              {
  yyless(yyleng-1);
  hrefs.push_back(std::string(yytext));
  std::cout<<yytext<<" ";
  yy_pop_state(yyscanner);
}
<X_REF1>">"                     {
  yyless(yyleng-1);
  hrefs.push_back(std::string(yytext));
  std::cout<<yytext<<" ";
  yy_pop_state(yyscanner);
}
 
<X_REFA>\"                      {
  yyless(yyleng-1);
  hrefs.push_back(std::string(yytext));
  std::cout<<yytext<<" ";
  yy_pop_state(yyscanner);
  yy_pop_state(yyscanner);
}
<X_REFP>\'                      {
  yyless(yyleng-1);
  hrefs.push_back(std::string(yytext));
  std::cout<<yytext<<" ";
  yy_pop_state(yyscanner);
  yy_pop_state(yyscanner);
}
<X_REF1,X_REFA,X_REFP>.         yymore();
<X_REFA,X_REFP>\n               yyerror("ERRO");
 
<X_DONTCARE>" "|\n              yy_pop_state(yyscanner);
<X_DONTCARE>\"                  yy_push_state(X_DCA, yyscanner);
<X_DONTCARE>\'                  yy_push_state(X_DCP, yyscanner);
<X_DONTCARE>">"                 yyless(yyleng-1); yy_pop_state(yyscanner);
<X_DONTCARE>.                   ;
 
<X_DCA>\"                       yy_pop_state(yyscanner); yy_pop_state(yyscanner);
<X_DCP>\'                       yy_pop_state(yyscanner); yy_pop_state(yyscanner);
<X_DCA,X_DCP>.                  ;
<X_DCA,X_DCP>\n                 yyerror("There was a scan error");
 
.|\n                            ;
 
%%
 
