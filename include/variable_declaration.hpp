#ifndef __VARIABLE_DECLARATION_HPP__
#define __VARIABLE_DECLARATION_HPP__

#include "ast.hpp"
#include "skipper.hpp"
#include "error_handler.hpp"
#include "annotation.hpp"
#include "type_setting.hpp"
#include "expression.hpp"

namespace algc {
   namespace parser {
      namespace qi = boost::spirit::qi;
      namespace ascii = boost::spirit::ascii;

      template<typename Iterator>
      struct variable_declaration
         : qi::grammar<Iterator, algc::ast::variable_declaration_list(), skipper<Iterator> >
      {
         variable_declaration(error_handler<Iterator>& error_handler)
            : variable_declaration::base_type(variable_declaration_list), expr(error_handler)
         {
            qi::_1_type _1;
            qi::_2_type _2;
            qi::_3_type _3;
            qi::_4_type _4;

            qi::_val_type _val;

            qi::string_type string_;
            qi::lit_type lit;
            qi::raw_type raw;
            qi::lexeme_type lexeme;
            qi::alpha_type alpha;
            qi::alnum_type alnum;
            qi::uint_type uint_;
            qi::blank_type blank;

            using qi::on_error;
            using qi::on_success;
            using qi::fail;
            using qi::eol;
            using qi::no_skip;

            using boost::phoenix::function;

            typedef function<algc::error_handler<Iterator> > error_handler_function;
            typedef function<algc::annotation<Iterator> > annotation_function;
            typedef function<algc::type_setting<Iterator> > type_setting_function;

            name =
                 !lexeme[expr.keywords >> !(alnum | '_')]
               >> raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
               ;

            identifier = name;

            primitive_type =
                 string_("integer")
               | string_("real")
               | string_("boolean")
               | string_("character")
               | string_("string")
               ;

            array_type =
                 string_("array_of_integer")
               | string_("array_of_real")
               | string_("array_of_boolean")
               | string_("array_of_character")
               | string_("array_of_string")
               ;

            variable_declaration_list =
                 lexeme["variables" >> !(alnum | '_')]
               > +variable_declaration_
               ;

            variable_declaration_ =
               (
                  (
                       primitive_type
                     > (identifier % ',')
                  )
                  |
                  (
                       array_type
                     > ((identifier > '[' >> uint_ >> ']') % ',')
                  )
               )
               > no_skip[*blank >> eol]
               ;

            BOOST_SPIRIT_DEBUG_NODES(
               (variable_declaration_)
               (primitive_type)
               (array_type)
               (identifier)
            );


            ///////////////////////////////////////////////////////////////////////
            // Error handling: on error in expr, call error_handler.
            on_error<fail>(variable_declaration_list,
            error_handler_function(error_handler)(
                "Error! Expecting ", _4, _3));

            // Annotation: on success in start, call annotation.
            on_success(identifier,
               annotation_function(error_handler.iters)(_val, _1));

            // Type_setting: on success in variable_declaration, call type_setting function
            on_success(variable_declaration_,
               type_setting_function(error_handler.iters)(_val, _1));
         }

         qi::rule<Iterator, ast::variable_declaration(), skipper<Iterator> > variable_declaration_;
         qi::rule<Iterator, ast::variable_declaration_list(), skipper<Iterator> > variable_declaration_list;
         qi::rule<Iterator, ast::type_name(), skipper<Iterator> > primitive_type;
         qi::rule<Iterator, ast::type_name(), skipper<Iterator> > array_type;
         qi::rule<Iterator, std::string(), skipper<Iterator> > name;
         qi::rule<Iterator, ast::identifier(), skipper<Iterator> > identifier;

         expression<Iterator> expr; /// used only to retrieve keywords :(
      };
   }
}


#endif // __VARIABLE_DECLARATION_HPP__
