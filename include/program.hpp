#ifndef __PROGRAM_HPP__
#define __PROGRAM_HPP__

#include <boost/spirit/include/qi.hpp>

#include "statement.hpp"

namespace client {
   namespace parser {
      namespace qi = boost::spirit::qi;
      namespace ascii = boost::spirit::ascii;

      template <typename Iterator>
      struct program : qi::grammar<Iterator, ast::program(), skipper<Iterator> > {
         program(error_handler<Iterator>& error_handler);

         statement<Iterator> body;
         qi::rule<Iterator, std::string(), skipper<Iterator> > name;
         qi::rule<Iterator, ast::identifier(), skipper<Iterator> > identifier;
         qi::rule<Iterator, ast::program(), skipper<Iterator> > start;
      };
   }
}

#endif // __PROGRAM_HPP__
