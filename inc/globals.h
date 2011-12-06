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

#ifndef SLURP_GLOBALS_H
#define SLURP_GLOBALS_H

namespace slurp {

    const static int VERSION_ID[3] = { 0, 0, 9 };
    const static int FLAGS_VALID = 1;
    const static int FLAGS_VERBOSE = 2;
    const static int FLAGS_HEADLESS = 4;
    const static int FLAGS_CONTD = 8;
    const static int FLAGS_INVALID = -1;

/* these are defined in slurper.cpp */
    extern const char *USAGE_MESSAGE;
    extern const char *HELP_MENU;
    extern const char *LICENSE_INFO;

} 

#endif				/* SLURP_GLOBALS_H */
