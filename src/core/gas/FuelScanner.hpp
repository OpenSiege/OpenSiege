
/*
 * This file is part of gas
 *
 * Copyright (C) 2017 aaron andersen <aaron@fosslib.net>
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

#pragma once

#include <stack>
#include <string>
#include <cstring>

namespace ehb
{
    class FuelScanner
    {
    public:

        FuelScanner(const char * content);

        int scan(std::string * yylval);

    private:

        enum
        {
            embedded_statement,
            expression_statement,
            string_literal
        };

    private:

        std::stack<int> state;

        const char * content;
        const char * cursor;
        const char * limit;
        const char * marker;
    };

    inline FuelScanner::FuelScanner(const char * content) : content(content), cursor(content)
    {
        limit = content + strlen(content);
    }
}
