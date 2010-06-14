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

		usint *data();
		const usint *data() const;

		/**
		 * Metoda przepisująca bufor (adres) do wektora
		 */
		void fill(usint *buffer, size_t n);

		/**
		 * Metoda łącząca dwa zegary
		 * Dla każdej pozycji jest wybierane maksimum
		 */
		void concat(const VClock& other);

		bool operator>=(const VClock &other) const;
		bool operator<=(const VClock &other) const;
		bool operator>(const VClock &other) const;
		bool operator<(const VClock &other) const;
		void operator=(const VClock& other);
};

#endif // VCLOCK_H