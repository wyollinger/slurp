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
PROTO_OPT ((("http")|("https")|("ftp"))"://")*

%x X_COMMENT X_TAG
%x X_DONTCARE X_DCA X_DCP
%x X_REF1 X_REFA X_REFP
%x X_LINK_D2 X_LINK_D3
 
%%

<INITIAL,X_COMMENT>"<!--"       {
  yy_push_state(X_COMMENT, yyscanner);
}

<X_COMMENT>"-->"                {
  yy_pop_state(yyscanner);
}

<X_COMMENT>.|\n                 {

}
 
"<"{ID}                         {
  std::cout << "tag start [" << yytext << "]\n";
  yy_push_state(X_TAG, yyscanner);
}

<X_TAG>">"                      {
  yy_pop_state(yyscanner);
  std::cout << "tag end [" << yytext << "]\n";
}

<X_TAG>{HREF}                   {
  yy_push_state(X_REF1, yyscanner);
}

<X_TAG>{ATTR}                   {
  yy_push_state(X_DONTCARE, yyscanner);
}

<X_TAG>.|\n                     {

}
 
<X_REF1>\"                      {
  yy_push_state(X_REFA, yyscanner);
}

<X_REF1>\'                      {
  yy_push_state(X_REFP, yyscanner);
}

<X_REF1>{SPACE}|\n              {
  /* entry depth: 1 */
  yyless(yyleng-1);
  std::cout << "pushing link state with [" << yytext << "]\n";
  yy_pop_state(yyscanner);
}

<X_REF1>">"                     {
  /* entry depth: 1 */
  yyless(yyleng-1);
  std::cout << "pushing link state of depth 2 with [" << yytext << "]\n";
  yy_push_state(X_LINK_D2, yyscanner);
}
 
<X_REFA>\"                      {
  /* entry depth: 2*/
  yyless(yyleng-1);
  std::cout << "pushing link state of depth 3 with [" << yytext << "]\n";
  yy_push_state(X_LINK_D3, yyscanner);
}

<X_REFP>\'                      {
  /* entry depth: 2 */
  yyless(yyleng-1);
  std::cout << "pushing link state of depth 3 with [" << yytext << "]\n";
  yy_push_state(X_LINK_D3, yyscanner);
}

<X_REF1,X_REFA,X_REFP>.         {
  yymore();
}

<X_REFA,X_REFP>\n               {
  yyerror("ERRO");
}
 
<X_DONTCARE>" "|\n              {
  yy_pop_state(yyscanner);
}

<X_DONTCARE>\"                  {
  yy_push_state(X_DCA, yyscanner);
}

<X_DONTCARE>\'                  {
  yy_push_state(X_DCP, yyscanner);
}

<X_DONTCARE>">"                 {
  yyless(yyleng-1); 
  yy_pop_state(yyscanner);
}

<X_DONTCARE>.                   {

}
 
<X_DCA>\"                       {
  yy_pop_state(yyscanner); 
  yy_pop_state(yyscanner);
}

<X_DCP>\'                       {
  yy_pop_state(yyscanner); 
  yy_pop_state(yyscanner);
}

<X_DCA,X_DCP>.                  {

}

<X_DCA,X_DCP>\n                 {
  yyerror("There was a scan error");
}
 
.|\n                            {

}
 
<X_LINK_D2>{PROTO_OPT}          {
  std::cout << "in link state d2 at beginning of URI " << std::endl;
  std::cout << "optional protocol info: [" << yytext << "]\n";
  yy_pop_state(yyscanner);                     
  yy_pop_state(yyscanner);                     
}

<X_LINK_D3>{PROTO_OPT}          {
  std::cout << "in link state d3 at beginning of URI " << std::endl;
  std::cout << "optional protocol info: [" << yytext << "]\n";
  yy_pop_state(yyscanner);                     
  yy_pop_state(yyscanner);                     
  yy_pop_state(yyscanner);                     
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
