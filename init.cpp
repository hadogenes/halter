#include <cstdio>
#include <cstdlib>
#include <list>
#include <string>
#include <string.h>

#include <fstream>
#include <sstream>
using std::list;
using std::string;
using std::ifstream;

#include <pvm3.h>
#include "joblist.h"
#include "init.h"

void parseFile(const string &filename, JobList &commands) {
	ifstream file(filename.c_str());
	list<Oper> commandList;
	Oper newOper;

	string line;
	int space;
	char command[10];
	int arg;

	if (!file.is_open()) {
		printf("Error: nie udalo sie otworzyc pliku '%s'.\n", filename.c_str());
		exit(1);
	}

	while (! file.eof()) {
		getline(file, line);
		space = line.find(' ');

		// Parsowanie lini
		if (space != string::npos)
			sscanf(line.c_str(), "%s %d", command, &arg);
		else
			strcpy(command, line.c_str());


		// Parsowanie instrukcji
		if (strcmp(command, "SET") == 0) {
			newOper.instr = SET;
			newOper.arg = arg;
		}
		else if (strcmp(command, "GET") == 0) {
			newOper.instr = GET;
			newOper.arg = arg;
		}
		else if (strcmp(command, "REGSET") == 0) {
			newOper.instr = REGSET;
			newOper.arg = arg;
		}
		else if (strcmp(command, "INC") == 0) {
			newOper.instr = INC;
			newOper.arg = arg;
		}
		else if (strcmp(command, "ADD") == 0) {
			newOper.instr = ADD;
			newOper.arg = arg;
		}
		else if (strcmp(command, "PRINT") == 0) {
			newOper.instr = PRINT;
			newOper.arg = -1;
		}
		else if (strcmp(command, "WAIT") == 0) {
			newOper.instr = WAIT;
			newOper.arg = -1;
		}
		else
			continue;


		commandList.push_back(newOper);
	}

	file.close();

	// Przepisywanie zawartości listy do wektora
	commands.assign(commandList.begin(), commandList.end());
}

void init(Tids &tids) {
	std::ostringstream ss;

	JobList proc;
	int jobNum;
	int slaveNum = tids.size();

	for (int i = 0; i < slaveNum; ++i) {
		ss << i;
		parseFile(FILE_NAME_BASE + ss.str(), proc);

		pvm_initsend(PvmDataDefault);
		pvm_pkint(&i, 1, 1);

		pvm_pkint(&slaveNum, 1, 1);
		pvm_pkint(tids.data(), tids.size(), 1);

		jobNum = proc.size();
		pvm_pkint(&jobNum, 1, 1);
		pvm_pkbyte((char *) proc.data(), sizeof(Oper) * proc.size(), 1);

		pvm_send(tids[i], INIT);

		ss.str("");
	}
}