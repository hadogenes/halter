#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <string>
#include <string.h>

#include <fstream>
#include <sstream>

#define FILE_NAME_BASE "proc"

#include "joblist.h"

using std::list;
using std::string;
using std::ifstream;

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
		else if (strcmp(command, "WAIT") == 0) {
			newOper.instr = WAIT;
			newOper.arg = -1;
		}


		commandList.push_back(newOper);
	}

	file.close();

	// Przepisywanie zawartości listy do wektora
	commands.assign(commandList.begin(), commandList.end());
}

int main(int argc, char *argv[]) {
	if (argc != 1) {
		printf("Usage: %s <numb of slave>", argv[0]);
		exit(1);
	}

	int slaveNo = atoi(argv[1]);
	std::ostringstream ss;
	const string baseFileName(FILE_NAME_BASE);

	JobList proc;

	for (int j = 0; j < slaveNo; ++j) {
		ss << j;
		parseFile(baseFileName + ss.str(), proc);

		for (int i = 0; i < proc.size(); ++i) {
			printf("Proc nr %d, Instr nr %d: %d", j, i + 1, proc[i].instr);

			if (proc[i].instr != WAIT)
				printf(" %d", proc[i].arg);

			printf("\n");
		}

		ss.clear();
	}

	return 0;
}
