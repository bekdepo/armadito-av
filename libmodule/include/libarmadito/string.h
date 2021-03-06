/***

Copyright (C) 2015, 2016 Teclib'

This file is part of Armadito core.

Armadito core is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Armadito core is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Armadito core.  If not, see <http://www.gnu.org/licenses/>.

***/

/**
 * \file string.h
 *
 * \brief definition of usefull string functions, to replace unsafe strcpy and sprintf
 *
 */

#ifndef __LIBARMADITO_STRING_H_
#define __LIBARMADITO_STRING_H_

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/**
 * \fn char *a6o_strcat_(const char *src, ...);
 * \brief varargs string concatenation with allocation
 *
 * Concatenates a NULL terminated argument list of strings.
 * The result is allocated using malloc() and must therefore be free()d after use.
 *
 * NOTE: this function should not be called directly. The macro a6o_strcat must
 * be used instead, so that the NULL terminating argument is always there.
 *
 * \param[in] src               the first argument
 *
 * \return the concatenation, or NULL if src argument is NULL
 */
char *a6o_strcat_(const char *src, ...);

#define a6o_strcat(SRC, ...) a6o_strcat_((SRC), __VA_ARGS__, NULL)

#define a6o_strdup(S) a6o_strcat_((S), NULL)

#endif
