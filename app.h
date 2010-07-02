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

using std::printf;
using std::list;

class App {
	public:
 		App(int procNo, const JobList &jobList);
		virtual ~App();

		virtual void run() = 0;
		virtual void resume(const int savedState, const WaitFor savedHaltState, list<Msg> &msgSaved) = 0;

	protected:
		virtual void send(Instr instr, int arg, int objNum) = 0;
		/**
		 * Metoda uruchamia aplikacje
		 * @param amount ile iteracji ma wykonać aplikacja
		 */
		void runLocal(const int amount);

		bool done;
		int reg;
		int obj;
		int procNo;

		WaitFor halt;
		const JobList &jobList;
		int instrNo;
};

#endif // APP_H
