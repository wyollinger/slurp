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

#ifndef SLURP_SCANNER_H
#define SLURP_SCANNER_H

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_core.hpp>

#include <QList>
#include <QString>
#include <QRunnable>
#include <string>

#include "eventer.h"

namespace lex = boost::spirit::lex;
namespace phoenix = boost::phoenix;

namespace slurp {

    struct distance_func {
	template < typename Iterator1, typename Iterator2 > 
	struct result:boost::iterator_difference < Iterator1 > {};

	template < typename Iterator1, typename Iterator2 > 
	    typename result < Iterator1, Iterator2 >::type
	    operator () (Iterator1 & begin, Iterator2 & end) const {
	        return std::distance(begin, end);
            }
    };

    phoenix::function < distance_func > const distance = distance_func();

    template < typename Lexer > 
        struct word_count_tokens: lex::lexer < Lexer > {
	    word_count_tokens() : c(0),
	    w(0), l(0), word("[^ \t\n]+"), eol("\n"), any(".") {

	    this -> self = 
	        word[++phoenix::ref(w), 
	            phoenix::ref(c) += 
		        distance(lex::_start, lex::_end)] | 
			eol[++phoenix::ref(c), 
                        ++phoenix::ref(l)] | 
			any[++phoenix::ref(c)];

	    } 

	std::size_t c, w, l;
	lex::token_def <> word, eol, any;
    };

    class Scanner : public QRunnable {
	Eventer *owner;
	QString data;

        public:

	Scanner(Eventer * owner, const QString & data) {
	    this->owner = owner;
	    this->data = data;
	}

	void run();
    };
}				/* namespace slurp */

#endif				/* SLURP_SCANNER_H */
