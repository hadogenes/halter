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

#ifndef MONITOR_H
#define MONITOR_H

#define MAX_ITER 10

#include "vclock.h"
#include "app.h"
#include "tids.h"
#include <vector>
#include <list>
#include <pvm3.h>

using std::vector;
using std::list;

class Monitor : public App {
	public:
		Monitor(int procNo, const JobList &jobList, const Tids &tids);

		void run();
		void resume(const int savedState, list<Msg> &msgSaved);

	private:
		void runRemote();

		void send(Instr instr, int arg, int objNo);
		void sendState();
		void receive();

		bool isAllChanDone();
		void recordState(int init);

		const Tids &tids;

		list<Msg> msgBuf;

		vector<bool> recvMark;
		list<Msg> chanState;
		bool involved;
		int procState;
		int initializer;

};

#endif // MONITOR_H
