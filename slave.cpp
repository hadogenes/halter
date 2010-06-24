#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <pvm3.h>

#include <list>

#include "app.h"
#include "tids.h"
#include "joblist.h"
#include "monitor.h"

using std::list;

int main(int argc, char *argv[]) {
	char resume;
	int procNo, slaveNum, jobNum, savedState, msgNum;
	Tids tids;
	JobList jobList;
	list<Msg> msgSaved;

	App *app = NULL;

	pvm_upkint(&procNo, 1, 1);
	pvm_upkint(&slaveNum, 1, 1);

	tids.resize(slaveNum);
	pvm_upkint(tids.data(), tids.size(), 1);

	pvm_upkint(&jobNum, 1, 1);

	jobList.resize(jobNum);
	pvm_upkbyte((char *) jobList.data(), sizeof(Oper) * jobList.size(), 1);


	pvm_recv(-1, RESUME);
	pvm_upkbyte(&resume, 1, 1);

	if (resume) {
		Msg newMsg;
		pvm_upkint(&savedState, 1, 1);
		pvm_upkint(&msgNum, 1, 1);

		for (int i = 0; i < msgNum; ++i) {
			pvm_upkbyte((char *) &newMsg, sizeof(Msg), 1);
			msgSaved.push_back(newMsg);
		}
	}

	app = new Monitor(procNo, jobList, tids);

	if (resume)
		app->resume(savedState, msgSaved);

	app->run();

	pvm_exit();

	delete app;
	return 0;
}