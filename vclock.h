#ifndef VCLOCK_H
#define VCLOCK_H

#include <vector>
#include <algorithm>

using std::vector;
typedef unsigned short int usint;

class VClock : public vector<usint> {
	public:
		VClock(size_t n);
		VClock(usint *buffer, size_t n);

		const usint *data();

		/**
		 * Metoda przepisująca bufor (adres) do wektora
		 */
		void assign(usint *buffer, size_t n);

		/**
		 * Metoda łącząca dwa zegary
		 * Dla każdej pozycji jest wybierane maksimum
		 */
		void concat(VClock &other);

		bool operator>=(VClock &other);
		bool operator<=(VClock &other);
		bool operator>(VClock &other);
		bool operator<(VClock &other);
};

#endif // VCLOCK_H