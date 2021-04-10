
/*
 * This file is part of gas
 *
 * Copyright (C) 2017-2020 OpenSiege Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

%skeleton "lalr1.cc"
%require "3.2"
%defines
%define api.namespace { ehb }
%define api.value.type { std::string }
%define parse.assert
%define parser_class_name { FuelParser }

%code requires {

    #include <string>
    #include "gas/Fuel.hpp"
    #include "gas/FuelScanner.hpp"

}

%code {

    static int yylex (std::string * yylval, ehb::FuelScanner & scanner)
    {
        return scanner.scan(yylval);
    }

}

%lex-param { ehb::FuelScanner & scanner }
%parse-param { ehb::FuelScanner & scanner }
%parse-param { ehb::FuelBlock * node }

%token Expression "expression"
%token Identifier "identifier"

%start translation_unit

%%

translation_unit
    : element_body
    ;

element
    : '[' element_name ']' '{' element_body '}' { node = node->parent(); }
    | '[' element_name ']' error '{' element_body '}' {
        node = node->parent();

        // account for rogue characters found in gpg gas file
        yyerrok;
    }
    ;

element_name
    : simple_identifier { node = node->appendChild($1); }
    | simple_identifier ':' simple_identifier ',' simple_identifier ':' simple_identifier { node = node->appendChild($7, $3); }
    | simple_identifier ',' simple_identifier ':' simple_identifier ',' simple_identifier ':' simple_identifier {

        // TODO: do something with $1... like check for "dev"?
        node = node->appendChild($9, $5);

    }
    ;

element_body
    :
    | element_body_list
    ;

element_body_list
    : element_body_item
    | element_body_list element_body_item
    ;

element_body_item
    : attribute
    | element
    ;

attribute
    : identifier '=' expression_statement ';' { node->appendValue($1, $3); }
    | type_id identifier '=' expression_statement ';' { node->appendValue($2, $1, $4); }
    ;

type_id
    : simple_identifier
    ;

expression_list
    : "expression"
    | expression_list "expression" { $$ = $1 + $2; }
    ;

expression_statement
    :
    | expression_list {

        $$ = $1;

        $$.erase(0, $$.find_first_not_of(" \n\r\t"));
        $$.erase($$.find_last_not_of(" \n\r\t") + 1);

    }
    ;

simple_identifier
    : "identifier"
    ;

identifier
    : simple_identifier
    | identifier ':' simple_identifier { $$ = $1 + ':' + $3; }
    ;

%%

#include <iostream>

namespace ehb
{
    void FuelParser::error(const std::string & msg)
    {
        // TODO: don't print this to cerr, but somewhere else user defined
        std::cerr << msg << std::endl;
    }
}
