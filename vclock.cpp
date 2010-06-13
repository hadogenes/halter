#include "vclock.h"

VClock::VClock(size_t n) /*: vector(n, 0)*/ {
}

VClock::VClock(usint *buffer, size_t n) {
	this->assign(buffer, n);
}

const usint *VClock::data() {
	return this->_M_impl._M_start;
}

void VClock::assign(usint *buffer, size_t n) {
	this->clear();

	this->_M_impl._M_start = buffer;
	this->_M_impl._M_finish = buffer + n;
	this->_M_impl._M_end_of_storage = this->_M_impl._M_finish;
}

void VClock::concat(VClock &other) {
	for (int i = 0; i < this->size(); ++i)
			this->_M_impl._M_start[i] = std::max(this->_M_impl._M_start[i], other[i]);
}


bool VClock::operator<=(VClock &other) {
	for (int i = 0; i < this->size(); ++i)
		if (this->_M_impl._M_start[i] > other[i])
			return false;

	return true;
}

bool VClock::operator>=(VClock &other) {
	for (int i = 0; i < this->size(); ++i)
		if (this->_M_impl._M_start[i] < other[i])
			return false;

	return true;
}

bool VClock::operator<(VClock &other) {
	for (int i = 0; i < this->size(); ++i)
		if (this->_M_impl._M_start[i] >= other[i])
			return false;

	return true;
}

bool VClock::operator>(VClock &other) {
	for (int i = 0; i < this->size(); ++i)
		if (this->_M_impl._M_start[i] <= other[i])
			return false;

	return true;
}