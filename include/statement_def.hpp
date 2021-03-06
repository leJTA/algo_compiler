#include "statement.hpp"
#include "error_handler.hpp"
#include "annotation.hpp"

namespace algc {
   namespace parser {
       template <typename Iterator>
       statement<Iterator>::statement(error_handler<Iterator>& error_handler)
         : statement::base_type(statement_list), expr(error_handler)
       {
         qi::_1_type _1;
         qi::_2_type _2;
         qi::_3_type _3;
         qi::_4_type _4;

         qi::_val_type _val;

         qi::raw_type raw;
         qi::lexeme_type lexeme;
         qi::alpha_type alpha;
         qi::alnum_type alnum;
         qi::lit_type lit;
         qi::blank_type blank;

         using qi::on_error;
         using qi::on_success;
         using qi::fail;
         using qi::no_skip;
         using qi::eol;
         using boost::phoenix::function;

         typedef function<algc::error_handler<Iterator> > error_handler_function;
         typedef function<algc::annotation<Iterator> > annotation_function;

         statement_list =
            +statement_
            ;

         statement_ =
              assignment
            | read_statement
            | write_statement
            | if_statement
            | while_statement
            | repeat_until_statement
            | for_statement
            | return_statement
            | procedure_call_statement
            ;

         identifier =
                !lexeme[expr.keywords >> !(alnum | '_')]
            >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
            ;

         assignment =
                identifier > -('[' > expr > ']')
            >   ":="
            >   expr
            >   no_skip[*blank >> eol]
            ;

         read_statement =
              lit("read")
            > '('
            > identifier
            > ')'
            > no_skip[*blank >> eol]
            ;

         write_statement =
              lit("write")
            > '('
            > (expr % ',')
            > ')'
            > no_skip[*blank >> eol]
            ;

         if_statement =
               lexeme["if" >> !(alnum | '_')]
            >  expr
            >  lexeme["then" >> !(alnum | '_')]
            >  statement_list
            >
               -(
                 lexeme["else" >> !(alnum | '_')] // make sure we have whole words
               > statement_list
               )
            >  lexeme["endif" >> !(alnum | '_')]
            ;

         while_statement =
              lexeme["while" >> !(alnum | '_')] // make sure we have whole words
            > expr
            > lexeme["do" >> !(alnum | '_')]
            > statement_list
            > lexeme["endwhile" >> !(alnum | '_')]
            ;

         repeat_until_statement =
              lexeme["repeat" >> !(alnum | '_')]
            > statement_list
            > lexeme["until" >> !(alnum | '_')]
            > expr
            > no_skip[*blank >> eol]
            ;

         for_statement =
              lexeme["for" >> !(alnum | '_')]
            > identifier
            > ":="
            > expr
            > lexeme["to" >> !(alnum | '_')]
            > expr
            > -(
                  lexeme["by" >> !(alnum | '_')] > expr
               )
            > lexeme["do" >> !(alnum | '_')]
            > statement_list
            > lexeme["endfor" >> !(alnum | '_')]
            ;

         return_statement =
              lexeme["return" >> !(alnum | '_')]
            > expr
            ;

         procedure_call_statement =
              lexeme["call" >> !(alnum | '_')]
            > (identifier >> '(')
            > -(expr % ',')
            > ')'
            ;

         // Debugging and error handling and reporting support.
         BOOST_SPIRIT_DEBUG_NODES(
            (statement_list)
            (identifier)
            (assignment)
         );

         // Error handling: on error in statement_list, call error_handler.
         on_error<fail>(statement_list,
            error_handler_function(error_handler)(
                "Error! Expecting ", _4, _3));

         // Annotation: on success in read_statement, assignment,
         // return_statement, for_statement and procedure_call_statement, call annotation.
         on_success(assignment,
            annotation_function(error_handler.iters)(_val, _1));
         on_success(read_statement,
            annotation_function(error_handler.iters)(_val, _1));
         on_success(return_statement,
            annotation_function(error_handler.iters)(_val, _1));
         on_success(procedure_call_statement,
            annotation_function(error_handler.iters)(_val, _1));
         on_success(for_statement,
            annotation_function(error_handler.iters)(_val, _1));
      }
   }
}
