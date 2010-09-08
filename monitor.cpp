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

Monitor::Monitor(uint procNo, const JobList& jobList, const Tids& tids) : App(procNo, jobList),
		tids(tids), recvMark(tids.size(), false) {
	this->involved = false;
	this->lamport = 0;
}

void Monitor::send(Instr instr, int arg, int objNo) {
	Msg outMsg = { this->procNo, this->lamport, { instr, arg } };

	pvm_initsend(PvmDataDefault);

	pvm_pkbyte((char *) &outMsg, sizeof(outMsg), 1);

	pvm_send(this->tids[objNo], NORMAL);

	if (instr == GET) {
		PRINT_VERBOSE(this->procNo, this->lamport, "halt", (int) WAIT_GET);
		this->halt |= WAIT_GET;
	}
}

void Monitor::receive() {
	Msg inMsg;

	do {
		pvm_recv(-1, NORMAL);

		pvm_upkbyte((char *) &inMsg, sizeof(inMsg), 1);

		this->msgBuf.push_back(inMsg);
	}
	while (pvm_probe(-1, NORMAL) > 0);
}

void Monitor::recordState(int init) {
	PRINT_VERBOSE(this->procNo, this->lamport, "Recording state");

	this->involved = true;

	for (uint i = 0; i < this->recvMark.size(); ++i)
		this->recvMark[i] = false;

	// Oznaczamy, że od "siebie" odebraliśmy już wiadomość
	this->recvMark[this->procNo] = true;

	this->procInstrState = this->instrNo;
	this->procHaltState = this->halt;
	this->procLamportState = this->lamport;
	this->initializer = init;

	for (uint i = 0; i < this->tids.size(); ++i)
		if (i != this->procNo)
			this->send(MARKER, init, i);
}

void Monitor::sendState() {
	PRINT_VERBOSE(this->procNo, this->lamport, "Sending state");

	list<Msg>::iterator it;
	int msgNum = this->chanState.size();

	pvm_initsend(PvmDataDefault);
	pvm_pkuint(&this->procNo, 1, 1);

	// Zapamiętanie stanu procesu
	pvm_pkint(&this->procInstrState, 1, 1);
	pvm_pkbyte((char *) &this->procHaltState, sizeof(this->procHaltState), 1);
	pvm_pkuint(&this->procLamportState, 1, 1);

	// Zapamiętanie stanu wiadomości
	pvm_pkint(&msgNum, 1, 1);

	for (it = this->chanState.begin(); it != this->chanState.end(); ++it)
		pvm_pkbyte((char *) &(*it), sizeof(Msg), 1);

	pvm_send(this->initializer, STATE);

	// Po wysłaniu wracamy do normalnej pracy
	this->involved = false;
	this->chanState.clear();
}

void Monitor::resume(const int savedInstrState, const WaitFor savedHaltState, const uint savedLamport, list<Msg> &msgSaved) {
	// Oddtwarzanie stau systemu
	this->instrNo = savedInstrState;
	this->halt = savedHaltState;
	this->lamport = savedLamport;

	// Odtwarzanie stanu wiadomości
	this->msgBuf.swap(msgSaved);

	PRINT_VERBOSE(this->procNo, this->lamport, "resumed slave");
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
	for (uint i = 0; i < this->recvMark.size(); ++i)
		if (!this->recvMark[i])
			return false;

	return true;
}


void Monitor::runRemote() {
	list<Msg>::iterator it = this->msgBuf.begin();

	for (; it != this->msgBuf.end(); it = this->msgBuf.erase(it)) {
		this->lamport = std::max(this->lamport, (*it).laport);

		switch ((*it).oper.instr) {
			case SET:
				PRINT_VERBOSE(this->procNo, this->lamport, "SET", (*it).who, (*it).oper.arg);
				this->obj = (*it).oper.arg;
				break;

			case GET:
				PRINT_VERBOSE(this->procNo, this->lamport, "GET", (*it).who, (*it).oper.arg);
				++this->lamport;
				this->send(GET_RESP, this->obj, (*it).who);
				break;

			case INC:
				PRINT_VERBOSE(this->procNo, this->lamport, "INC", (*it).who);
				++this->obj;
				break;

			case ADD:
				PRINT_VERBOSE(this->procNo, this->lamport, "ADD", (*it).who, (*it).oper.arg);
				this->obj += (*it).oper.arg;
				break;

			case GET_RESP:
				PRINT_VERBOSE(this->procNo, this->lamport, "GET_RESP", (*it).who, (*it).oper.arg);
				PRINT_VERBOSE(this->procNo, this->lamport, "unhalt", (int) WAIT_GET);
				this->reg = (*it).oper.arg;
				this->halt &= ~WAIT_GET;
				break;

			case MARKER:
				PRINT_VERBOSE(this->procNo, this->lamport, "Recive marker", (*it).who);
				if (!this->involved)
					this->recordState((*it).oper.arg);

				if ((*it).who != (uint) -1)
					this->recvMark[(*it).who] = true;

				if (this->isAllChanDone())
					this->sendState();

				break;

			default:
				break;
		}

		if ((this->halt & WAIT_VAL) && (this->obj == this->reg)) {
			PRINT_VERBOSE(this->procNo, this->lamport, "unhalt", (int) WAIT_VAL);
			this->halt &= ~WAIT_VAL;
		}

		if ((this->involved) && ((*it).who != (uint) -1))
			if (!this->recvMark[(*it).who])
				this->chanState.push_back(*it);
	}
}



