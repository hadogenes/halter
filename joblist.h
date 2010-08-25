/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef JOBLIST_H
#define JOBLIST_H

#define SLAVENAME "slave"

#include <vector>

enum Instr { SET = 0x1, GET = 0x2, REGSET = 0x4, INC = 0x8, ADD = 0x10, PRINT = 0x20, WAIT = 0x40, GET_RESP = 0x80, MARKER = 0x100 };
enum MsgId { INIT = 0x1, RESUME = 0x2, NORMAL = 0x4, STATE = 0x8 };
enum WaitFor { WAIT_NONE, WAIT_GET, WAIT_VAL };

struct Oper {
	Instr instr;
	int arg;
};

struct Msg {
	int who;
	Oper oper;
};

class JobList : public std::vector<Oper> {
	public:
		JobList();

		/**
		 * Metoda przepisująca bufor (adres) do wektora
		 */
		void fill(Oper *buffer, size_t n);

		const Oper *data();
};

#endif // JOBLIST_H
