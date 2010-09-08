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

#include "app.h"
#include <unistd.h>

App::App(uint procNo, const JobList& jobList) : procNo(procNo), jobList(jobList) {
	this->done = false;
	this->reg = 0;
	this->obj = 0;
	this->instrNo = 0;
	this->halt = WAIT_NONE;

	PRINT_VERBOSE(this->procNo, this->lamport, "end init");
}

App::~App() {
}


uint App::runLocal(const uint amount) {
	uint i = 0;
	for (; (i < amount) && (this->instrNo < this->jobList.size()) && (this->halt == WAIT_NONE); ++i, ++this->instrNo, ++this->lamport) {
		const Oper &currOper = this->jobList[this->instrNo];

		if ((currOper.instr & (REGSET | PRINT | WAIT)) || (currOper.arg == (int) this->procNo)) {
			switch (currOper.instr) {
				case SET:
					PRINT_VERBOSE(this->procNo, this->lamport, "SET");
					this->obj = this->reg;
					break;
				case GET:
					PRINT_VERBOSE(this->procNo, this->lamport, "GET");
					this->reg = this->obj;
					break;
				case REGSET:
					PRINT_VERBOSE(this->procNo, this->lamport, "REGSET", currOper.arg);
					this->reg = currOper.arg;
					break;
				case INC:
					PRINT_VERBOSE(this->procNo, this->lamport, "INC");
					++this->obj;
					break;
				case ADD:
					PRINT_VERBOSE(this->procNo, this->lamport, "ADD", this->reg);
					this->obj += this->reg;
					break;
				case PRINT:
					printf("%s[%d]: obj: %d, reg: %d\n", PROG_NAME, this->procNo, this->obj, this->reg);
					fflush(stdout);
					break;
				case WAIT:
					PRINT_VERBOSE(this->procNo, this->lamport, "WAIT", this->reg);

					if (this->obj != this->reg) {
						PRINT_VERBOSE(this->procNo, this->lamport, "halt", (int) WAIT_VAL);
						this->halt |= WAIT_VAL;
					}

					break;
				default:
#ifdef DEBUG
					printf("App::runLocal(%d): Wrong instr: %d %d.\n", this->procNo, currOper.instr, currOper.arg);
					fflush(stdout);
#endif // DEBUG
					break;
			}
		}
		else
			this->send(currOper.instr, this->reg, currOper.arg);

		usleep(1000000);
	}


	// Jeśli przetwarzanie już się zakończyło i na nic nie czekamy, to zaznacz to
	if ((this->instrNo >= this->jobList.size()) && (this->halt == WAIT_NONE)) {
		this->done = true;
		PRINT_VERBOSE(this->procNo, this->lamport, "done");
	}

	return i;
}

