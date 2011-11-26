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

#ifndef GLOBALS_H
#define GLOBALS_H

const static char* USAGE_MESSAGE = "slurp [options] urls ...";
const static int VERSION_ID[3] = {0,0,4};
const static int FLAGS_VALID    = 1;
const static int FLAGS_VERBOSE  = 2;
const static int FLAGS_HEADLESS = 4;
const static int FLAGS_CONTD    = 8;
const static int FLAGS_INVALID  = 0;

const static char* HELP_MENU = 
" slurp - a web crawler\n"\
"   usage: slurp [options] urls ...\n"\
"   where options are some combination of the following:\n"\
"     v - verbose\n"\
"     h - display this menu and exit\n"\
"     V - display version information exit\n"\
"     n - number of URIs to crawl\n"\
"     t - maximum number of threads to use\n"\
"   note: you cannot combine options, so -vn3 is not legal\n"\
"   instead, use -v -n3 or -v -n 3\n";	

const static char* LICENSE_INFO =
"Copyright (C) 2011 Joseph Max DeLiso\n\n"\
"This program is free software: you can redistribute it and/or modify\n"\
"it under the terms of the GNU General Public License as published by\n"\
"the Free Software Foundation, version 3 of the License.\n\n"\
"This program is distributed in the hope that it will be useful,\n"\
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"\
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"\
"GNU General Public License for more details.\n\n"\
"You should have received a copy of the GNU General Public License\n"\
"along with this program.  If not, see <http://www.gnu.org/licenses/>.\n";

void die( const char* errmsg, int errcode );

#endif
