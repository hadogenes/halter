#ifndef VCLOCK_H
#define VCLOCK_H

#include <vector>

typedef unsigned short int usint;

class VClock : std::vector<usint> {
	public:
		VClock(usint *buffer, size_t n);

		void assign(usint *buffer, size_t n);

		bool operator>=(VClock &other);
		bool operator<=(VClock &other);
		bool operator>(VClock &other);
		bool operator<(VClock &other);
};

#endif // VCLOCK_H