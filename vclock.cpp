#include "vclock.h"
#include <string.h>

VClock::VClock(size_t n) : vector<usint>(n, 0) {
}

VClock::VClock(usint *buffer, size_t n) {
	this->fill(buffer, n);
}

usint *VClock::data() {
	return this->_M_impl._M_start;
}

const usint *VClock::data() const {
	return this->_M_impl._M_start;
}

void VClock::fill(usint *buffer, size_t n) {
	this->clear();

	this->_M_impl._M_start = buffer;
	this->_M_impl._M_finish = buffer + n;
	this->_M_impl._M_end_of_storage = this->_M_impl._M_finish;
}

void VClock::concat(const VClock &other) {
	for (int i = 0; i < this->size(); ++i)
			this->_M_impl._M_start[i] = std::max(this->_M_impl._M_start[i], other[i]);
}


bool VClock::operator<=(const VClock &other) const {
	for (int i = 0; i < this->size(); ++i)
		if (this->_M_impl._M_start[i] > other[i])
			return false;

	return true;
}

bool VClock::operator>=(const VClock &other) const {
	for (int i = 0; i < this->size(); ++i)
		if (this->_M_impl._M_start[i] < other[i])
			return false;

	return true;
}

bool VClock::operator<(const VClock& other) const {
	for (int i = 0; i < this->size(); ++i)
		if (this->_M_impl._M_start[i] >= other[i])
			return false;

	return true;
}

bool VClock::operator>(const VClock &other) const {
	for (int i = 0; i < this->size(); ++i)
		if (this->_M_impl._M_start[i] <= other[i])
			return false;

	return true;
}

void VClock::operator=(const VClock &other) {
	memcpy(this->_M_impl._M_start, other.data(), this->size() * sizeof(VClock::value_type));
}
