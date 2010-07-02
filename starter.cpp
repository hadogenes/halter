#include <cstdio>
#include <cstdlib>
#include <pvm3.h>

#define SLAVENAME "slave"

#include <pvm3.h>
#include "joblist.h"
#include "init.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <numb of slave>", argv[0]);
		exit(1);
	}

	int slaveNum = atoi(argv[1]);

	Tids tids(slaveNum);
	pvm_spawn(SLAVENAME, NULL, PvmTaskDefault, "", tids.size(), tids.data());

	init(tids);

	char resume = 0;

	pvm_initsend(PvmDataDefault);
	pvm_pkbyte(&resume, 1, 1);
	pvm_mcast(tids.data(), tids.size(), RESUME);


	pvm_exit();
	return 0;
}
