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

#include "uri.h"

inline void yyerror(const char *msg) { std::cerr << "ERROR! [" << msg << "]\n"; }

%}

%option stack 8bit noyywrap yylineno reentrant extra-type="std::vector<slurp::URI>*"
 
SPACE     [ \t]
ID        [[:alpha:]]([[:alnum:]]|:|-|_)*
HREF      [Hh][Rr][Ee][Ff]{SPACE}*={SPACE}*
ATTR      {ID}{SPACE}*={SPACE}*
IPV4ADDR  (([[:digit:]]{1,3}"."){3}([[:digit:]]{1,3}))
HEX4      ([[:xdigit:]]{1,4})
HEXSEQ    ({hex4}(:{hex4}*)) 
HEXPART   ({hexseq}|({hexseq}::({hexseq}?))|::{hexseq})
IPV6ADDR  ({hexpart}(":"{IPV4ADDR})?)

%x X_COMMENT X_TAG
%x X_DONTCARE X_DCA X_DCP
%x X_REF1 X_REFA X_REFP
%x X_LINK_D2 X_LINK_D3
 
%%

<INITIAL,X_COMMENT>"<!--"       {
  yy_push_state(X_COMMENT, yyscanner);
  std::cout << "pushing COMMENT state\n";
}

<X_COMMENT>"-->"                {
  yy_pop_state(yyscanner);
  std::cout << "popping COMMENT state\n";
}

<X_COMMENT>.|\n                 {

}
 
"<"{ID}                         {
  yy_push_state(X_TAG, yyscanner);
  std::cout << "tag start [" << yytext << "]\npushing TAG state\n";
}

<X_TAG>">"                      {
  yy_pop_state(yyscanner);
  std::cout << "tag end [" << yytext << "]\npopping TAG state\n";
}

<X_TAG>{HREF}                   {
  yy_push_state(X_REF1, yyscanner);
  std::cout << "pushing REF1 state\n";
}

<X_TAG>{ATTR}                   {
  yy_push_state(X_DONTCARE, yyscanner);
  std::cout << "pushing DONTCARE state\n";
}

<X_TAG>.|\n                     {

}
 
<X_REF1>\"                      {
  yy_push_state(X_REFA, yyscanner);
  std::cout << "pushing REFA state\n";
}

<X_REF1>\'                      {
  yy_push_state(X_REFP, yyscanner);
  std::cout << "pushing REFP state\n";
}

<X_REF1>{SPACE}|\n              {
  /* entry depth: 1 */
  yyless(yyleng-1);
  yy_push_state(X_LINK_D2, yyscanner);
  std::cout << "pushing LINK_D2 state with [" << yytext << "]\n";
}

<X_REF1>">"                     {
  /* entry depth: 1 */
  yyless(yyleng-1);
  yy_push_state(X_LINK_D2, yyscanner);
  std::cout << "pushing LINK_D2 state with [" << yytext << "]\n";
}
 
<X_REFA>\"                      {
  /* entry depth: 2*/
  yyless(yyleng-1);
  yy_push_state(X_LINK_D3, yyscanner);
  std::cout << "pushing LINK_D3 state from X_REFA with [" << yytext << "]\n";
  yyget_extra(yyscanner) -> push_back( std::string(yytext) );
  yy_pop_state(yyscanner);
  yy_pop_state(yyscanner);
  yy_pop_state(yyscanner);
}

<X_REFP>\'                      {
  /* entry depth: 2 */
  yyless(yyleng-1);
  yy_push_state(X_LINK_D3, yyscanner);
  std::cout << "pushing LINK_D3 stat from X_REFP with [" << yytext << "]\n";
  yyget_extra(yyscanner) -> push_back( std::string(yytext) );
  yy_pop_state(yyscanner);
  yy_pop_state(yyscanner);
  yy_pop_state(yyscanner);
}

<X_REF1,X_REFA,X_REFP>.         {
  yymore();
}

<X_REFA,X_REFP>\n               {
  yyerror("ERRO");
}
 
<X_DONTCARE>" "|\n              {
  yy_pop_state(yyscanner);
  std::cout << "popping DONTCARE state\n";
}

<X_DONTCARE>\"                  {
  yy_push_state(X_DCA, yyscanner);
  std::cout << "pushing DCA state\n";
}

<X_DONTCARE>\'                  {
  yy_push_state(X_DCP, yyscanner);
  std::cout << "pushing DCP state\n";
}

<X_DONTCARE>">"                 {
  yyless(yyleng-1); 
  yy_pop_state(yyscanner);
  std::cout << "popping DONTCARE state\n";
}

<X_DONTCARE>.                   {

}
 
<X_DCA>\"                       {
  yy_pop_state(yyscanner); 
  yy_pop_state(yyscanner);
  std::cout << "popping state\npopping state\n";
}

<X_DCP>\'                       {
  yy_pop_state(yyscanner); 
  yy_pop_state(yyscanner);
  std::cout << "popping state\npopping state\n";
}

<X_DCA,X_DCP>.                  {

}

<X_DCA,X_DCP>\n                 {
  yyerror("There was a scan error");
}
 
.|\n                            {

}
%%

namespace slurp {
  std::vector<URI>* scanHTML( const char* html )
  {
    yyscan_t scanner;
    std::vector<URI>* URIs = new std::vector<URI>();

    yylex_init(&scanner); 
    yylex_init_extra( URIs, &scanner );
    yy_scan_string( html, scanner );

    yylex( scanner );
    yylex_destroy( scanner );
     
    return URIs;
  }
}
