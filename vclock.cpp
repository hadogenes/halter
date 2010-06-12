#include "vclock.h"

VClock::VClock(usint *buffer, size_t n) {
	this->_M_impl._M_start = buffer;
	this->_M_impl._M_finish = buffer + n;
	this->_M_impl._M_end_of_storage = this->_M_impl._M_finish;
}


bool VClock::operator<=(VClock &other) {
	for (int i = 0; i < this->size(); ++i)
		if (*(this->_M_impl._M_start + i) > other[i])
			return false;

	return true;
}

bool VClock::operator>=(VClock &other) {
	for (int i = 0; i < this->size(); ++i)
		if (*(this->_M_impl._M_start + i) < other[i])
			return false;

	return true;
}