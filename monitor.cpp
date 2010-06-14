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

#include "monitor.h"

Monitor::Monitor(int procNo, Oper *jobList, size_t jobNo, int *tids, usint slaveNum) : App(procNo, jobList, jobNo), tids(tids, slaveNum + 1), vClock(slaveNum + 1), vRecordClock(slaveNum + 1) {
	this->recorded = false;
}

void Monitor::send(Instr instr, int arg, int objNo) {
	Oper outMsg = { instr, arg };
	++this->vClock[this->procNo];

	pvm_initsend(PvmDataDefault);
	pvm_pkushort(this->vClock.data(), this->vClock.size(), 1);
	pvm_pkint(&this->procNo, 1, 1);

	pvm_pkbyte((char *) &outMsg, sizeof(outMsg), 1);

	pvm_send(this->tids[objNo], 1);

	if (outMsg.instr == GET)
		this->halt = WAIT_GET;
}

void Monitor::receive() {
	VClock inClk(this->vClock.size());
	int who;
	Oper inMsg;

	do {
		pvm_recv(-1, -1);
		pvm_upkushort(inClk.data(), inClk.size(), 1);
		this->vClock.concat(inClk);

		pvm_upkint(&who, 1, 1);

		pvm_upkbyte((char *) &inMsg, sizeof(inMsg), 1);

		switch (inMsg.instr) {
			case SET:
				this->obj = inMsg.arg;

				if ((this->halt == WAIT_VAL) && (this->obj == this->reg))
					this->halt = WAIT_NONE;

				break;

			case GET:
				this->send(GET_RESP, this->obj, who);
				break;

			case INC:
				++this->obj;

				if ((this->halt == WAIT_VAL) && (this->obj == this->reg))
					this->halt = WAIT_NONE;

				break;

			case ADD:
				this->obj += inMsg.arg;

				if ((this->halt == WAIT_VAL) && (this->obj == this->reg))
					this->halt = WAIT_NONE;

				break;

			case GET_RESP:
				this->reg = inMsg.arg;
				this->halt = WAIT_NONE;
				break;

			default:
				break;
		}

	} while (pvm_probe(-1, -1) > 0);
}


void Monitor::run() {
	while (1) {
		if ((!this->done) && (this->halt == WAIT_NONE)) {
			this->runLocal(MAX_ITER);

			if (pvm_probe(-1, -1) <= 0)
				continue;
		}

		this->receive();
	}
}


