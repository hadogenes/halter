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

#ifndef APP_H
#define APP_H

class App {
	public:
		App();

		//void run() = 0;

	protected:
		/**
		 * Metoda uruchamia aplikacje
		 * @param amount ile iteracji ma wykonać aplikacja
		 */
		void run(const int amount);

		//void send() = 0;
		//void get() = 0;

		bool done;
		int reg;
		int obj;
};

#endif // APP_H
