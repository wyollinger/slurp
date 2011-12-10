
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
#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_core.hpp>

#include <QDebug>

#include "scanner.h"
#include "eventer.h"

namespace lex = boost::spirit::lex;
namespace phoenix = boost::phoenix;

namespace slurp {

	void Scanner::run() {
		typedef lex::lexertl::token < char const *, lex::omit, boost::mpl::false_ > token_type;
		typedef lex::lexertl::actor_lexer < token_type > lexer_type;
		char const *first = reinterpret_cast< const char*> ( data.data() );
		char const *last = &first[data.size()];
		word_count_tokens < lexer_type > word_count_lexer;

		lexer_type::iterator_type iter = word_count_lexer.begin(first, last);
		lexer_type::iterator_type end = word_count_lexer.end();

		while (iter != end && token_is_valid(*iter))
			++iter;
		
		if (iter == end) {
		    qDebug() << "debug: parse completed successfully: c="
			     << word_count_lexer.c << " w="
			     << word_count_lexer.w << " l="
			     << word_count_lexer.l;
		} else {
                    qDebug() << "debug: there was a parse error";
		}
	}

}				/* namespace slurp */
