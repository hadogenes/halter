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

#define PROG_NAME "slave"

Monitor::Monitor(int procNo, const JobList &jobList, const Tids &tids) : App(procNo, jobList),
		tids(tids), recvMark(tids.size(), false) {
	this->involved = false;
}

void Monitor::send(Instr instr, int arg, int objNo) {
	Msg outMsg = { this->procNo, { instr, arg } };
// 	++this->vClock[this->procNo];

	pvm_initsend(PvmDataDefault);
// 	pvm_pkushort(this->vClock.data(), this->vClock.size(), 1);
// 	pvm_pkint(&this->procNo, 1, 1);

	pvm_pkbyte((char *) &outMsg, sizeof(outMsg), 1);

	pvm_send(this->tids[objNo], NORMAL);

	if (instr == GET)
		this->halt = WAIT_GET;
}

void Monitor::receive() {
// 	VClock inClk(this->vClock.size());
	int who;
	Msg inMsg;

	do {
		pvm_recv(-1, NORMAL);

		pvm_upkbyte((char *) &inMsg, sizeof(inMsg), 1);

		this->msgBuf.push_back(inMsg);
	}
	while (pvm_probe(-1, NORMAL) > 0);
}

void Monitor::recordState(int init) {
#ifdef DEBUG
				printf("Recording state (%x)\n", this->procNo);
				fflush(stdout);
#endif
	this->involved = true;

	for (int i = 0; i < this->recvMark.size(); ++i)
		this->recvMark[i] = false;

	// Oznaczamy, że od "siebie" odebraliśmy już wiadomość
	this->recvMark[this->procNo] = true;

	this->procInstrState = this->instrNo;
	this->procHaltState = this->halt;
	this->initializer = init;

	for (int i = 0; i < this->tids.size(); ++i)
		if (i != this->procNo)
			this->send(MARKER, init, i);
}

void Monitor::sendState() {
#ifdef DEBUG
				printf("Sending state (%x)\n", this->procNo);
				fflush(stdout);
#endif
	list<Msg>::iterator it;
	int msgNum = this->chanState.size();

	pvm_initsend(PvmDataDefault);
	pvm_pkint(&this->procNo, 1, 1);

	// Zapamiętanie stanu procesu
	pvm_pkint(&this->procInstrState, 1, 1);
	pvm_pkbyte((char *) &this->procHaltState, sizeof(this->procHaltState), 1);

	// Zapamiętanie stanu wiadomości
	pvm_pkint(&msgNum, 1, 1);

	for (it = this->chanState.begin(); it != this->chanState.end(); ++it)
		pvm_pkbyte((char *) &(*it), sizeof(Msg), 1);

	pvm_send(this->initializer, STATE);

	// Po wysłaniu wracamy do normalnej pracy
	this->involved = false;
	this->chanState.clear();
}

void Monitor::resume(const int savedInstrState, const WaitFor savedHaltState, list<Msg> &msgSaved) {
	// Oddtwarzanie stau systemu
	this->instrNo = savedInstrState;
	this->halt = savedHaltState;

	// Odtwarzanie stanu wiadomości
	this->msgBuf.swap(msgSaved);

#ifdef DEBUG
		printf("%s[%d]: resumed slave\n", PROG_NAME, this->procNo);
		fflush(stdout);
#endif
}


void Monitor::run() {
	while (1) {
		// runRemote wykonuje wszystko co jest w buforze lokalnym - nie odbiera wiadomości
		// Potrzebne do wznawiania - standardowo bufor jest pusty
		if (this->msgBuf.size() > 0)
			this->runRemote();

		if ((!this->done) && (this->halt == WAIT_NONE)) {
			this->runLocal(MAX_ITER);

			if (pvm_probe(-1, NORMAL) == 0)
				continue;
		}

		this->receive();
	}
}

bool Monitor::isAllChanDone() {
	for (int i = 0; i < this->recvMark.size(); ++i)
		if (!this->recvMark[i])
			return false;

	return true;
}


void Monitor::runRemote() {
	list<Msg>::iterator it = this->msgBuf.begin();

	for (; it != this->msgBuf.end(); it = this->msgBuf.erase(it)) {
		switch ((*it).oper.instr) {
			case SET:
				this->obj = (*it).oper.arg;
				break;

			case GET:
				this->send(GET_RESP, this->obj, (*it).who);
				break;

			case INC:
				++this->obj;
				break;

			case ADD:
				this->obj += (*it).oper.arg;
				break;

			case GET_RESP:
				this->reg = (*it).oper.arg;
				this->halt = WAIT_NONE;
				break;

			case MARKER:
#ifdef DEBUG
				printf("Recive marker from %x\n", (*it).who);
				fflush(stdout);
#endif
				if (!this->involved)
					this->recordState((*it).oper.arg);

				if ((*it).who != -1)
					this->recvMark[(*it).who] = true;

				if (this->isAllChanDone())
					this->sendState();

				break;

			default:
				break;
		}

		if ((this->halt == WAIT_VAL) && (this->obj == this->reg))
			this->halt = WAIT_NONE;

		if ((this->involved) && ((*it).who != -1))
			if (!this->recvMark[(*it).who])
				this->chanState.push_back(*it);
	}
}



