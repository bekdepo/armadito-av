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

#ifndef LIBIPC_COMMON_H
#define LIBIPC_COMMON_H

#define JSON_KEY_NAME "__key__"

enum deserial_error {
	ERR_BUFFER_TOO_SMALL = 1,
	ERR_TYPE_MISMATCH,
	ERR_NO_SUCH_FIELD,
};

#endif
