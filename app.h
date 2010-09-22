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

#ifndef APP_H
#define APP_H

#include <cstdio>
#include <list>
#include "joblist.h"

#define PROG_NAME "slave"

typedef unsigned int uint;

#ifdef VERBOSE
// Local
inline void PRINT_VERBOSE(int id, uint lamport, const char *oper, int arg) {
	printf("%s[%d], %u: %s[local] %d\n", PROG_NAME, id, lamport, oper, arg);
	fflush(stdout);
}

inline void PRINT_VERBOSE(int id, uint lamport, const char *oper) {
	printf("%s[%d], %u: %s[local]\n", PROG_NAME, id, lamport, oper);
	fflush(stdout);
}

// Remote
inline void PRINT_VERBOSE(int id, uint lamport, const char *oper, uint who, int arg) {
	printf("%s[%d], %u: %s[%u] %d\n", PROG_NAME, id, lamport, oper, who, arg);
	fflush(stdout);
}

inline void PRINT_VERBOSE(int id, uint lamport, const char *oper, uint who) {
	printf("%s[%d], %u: %s[%u]\n", PROG_NAME, id, lamport, oper, who);
	fflush(stdout);
}

// inline void PRINT_VERBOSE(int id, const char *oper, int arg) {
// 	printf("%s[%d]: %s %d\n", PROG_NAME, id, oper, arg);
// 	fflush(stdout);
// }
//
// inline  void PRINT_VERBOSE(int id, const char *oper) {
// 	printf("%s[%d]: %s\n", PROG_NAME, id, oper);
// 	fflush(stdout);
// }
#else
#define PRINT_VERBOSE(id, lamport, oper, arg)
#define PRINT_VERBOSE(id, oper, arg)
#define PRINT_VERBOSE(id, oper)
#endif

using std::printf;
using std::list;

class App {
	public:
 		App(uint procNo, const JobList &jobList);
		virtual ~App();

		virtual void run() = 0;
		virtual void resume(const HaltState &resumeState, list<Msg> &msgSaved) = 0;

	protected:
		virtual void send(Instr instr, int arg, int objNum) = 0;
		/**
		 * Metoda uruchamia aplikacje
		 * @param amount ile iteracji ma wykonać aplikacja
		 */
		uint runLocal(const uint amount);

		bool done;
		int reg;
		int obj;
		uint procNo;

		uint lamport;

		char halt;
		const JobList &jobList;
		unsigned int instrNo;
};

#endif // APP_H
