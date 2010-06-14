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

App::App(int procNo, Oper* jobList, size_t jobNo) : procNo(procNo), jobList(jobList, jobNo) {
	this->done = false;
	this->reg = 0;
	this->obj = 0;
	this->instrNo = 0;
	this->halt = WAIT_NONE;
}

void App::runLocal(const int amount) {
	for (int i = 0; (i < amount) && (this->instrNo < this->jobList.size()) && (this->halt == WAIT_NONE); ++i, ++this->instrNo) {
		const Oper &currOper = this->jobList[this->instrNo];

		if ((currOper.instr == REGSET) || (currOper.instr == WAIT) || (currOper.arg == this->procNo)) {
			switch (currOper.instr) {
				case SET:
					this->obj = this->reg;
					break;
				case GET:
					this->reg = this->obj;
					break;
				case REGSET:
					this->reg = currOper.arg;
					break;
				case INC:
					++this->obj;
					break;
				case ADD:
					this->obj += this->reg;
					break;
				case WAIT:
					this->halt = WAIT_VAL;
					break;
			}
		}
		else
			this->send(currOper.instr, this->reg, currOper.arg);
	}


	// Jeśli przetwarzanie już się zakończyło i na nic nie czekamy, to zaznacz to
	if ((this->instrNo >= this->jobList.size()) && (this->halt == WAIT_NONE))
		this->done = true;
}

