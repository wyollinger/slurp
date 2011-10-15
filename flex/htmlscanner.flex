/*
 * Slurp - a web crawler
 * Copyright (C) 2011 Joseph Max DeLiso

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

%{
 
#include <iostream>
#include <vector>
#include <string>
#include <cerrno>

inline void yyerror(const char *msg) { std::cerr << msg << std::endl; }

%}

%option stack 8bit noyywrap yylineno reentrant extra-type="std::vector<std::string>*"
 
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
  yyextra->push_back(yytext);
  yy_pop_state(yyscanner);
}
<X_REF1>">"                     {
  yyless(yyleng-1);
  yyextra->push_back(yytext);
  yy_pop_state(yyscanner);
}
 
<X_REFA>\"                      {
  yyless(yyleng-1);
  yyextra->push_back(yytext);
  yy_pop_state(yyscanner);
  yy_pop_state(yyscanner);
}
<X_REFP>\'                      {
  yyless(yyleng-1);
  yyextra->push_back(yytext);
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

namespace slurp {
  std::vector<std::string>* scanHTML( const char* html )
  {
    yyscan_t scanner;
    std::vector<std::string>* tagVec = new std::vector<std::string>();

    yylex_init(&scanner); 
    yylex_init_extra( tagVec, &scanner );
    yy_scan_string( html, scanner );

    yylex( scanner );
    yylex_destroy( scanner );
     
    return tagVec;
  }
}
