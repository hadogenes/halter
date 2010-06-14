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

Monitor::Monitor(usint slaveNum, int procNo) : procNo(procNo), vClock(slaveNum + 1), vRecordClock(slaveNum + 1) {
}

void Monitor::set(int obj) {
	if (obj == this->procNo)
		this->obj = this->reg;
}

void Monitor::get(int obj){
	if (obj != this->procNo)
		this->halt = WAIT_GET;
}

void Monitor::regset(int val) {
	this->reg = val;
}

void Monitor::add(int obj) {
	if (obj == this->procNo)
		this->obj += this->reg;
}
void Monitor::inc(int obj) {
	if (obj == this->procNo)
		++this->obj;
}

void Monitor::wait() {
	if (this->obj != this->reg)
		this->halt = WAIT_VAL;
}

void Monitor::send(Instr instr, int arg, int obj) {
	++this->vClock[this->procNo];
}

void Monitor::run() {
	while (1) {
		//if ((this->done) || (this->halt != WAIT_NONE))

	}
}


