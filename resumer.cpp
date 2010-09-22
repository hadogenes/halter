#include <cstdio>
#include <cstdlib>
#include <pvm3.h>

#include <string>
#include <sstream>
using std::string;
using std::ostringstream;

#define SLAVENAME "slave"

#include <pvm3.h>
#include "joblist.h"
#include "init.h"

#define TIXML_USE_TICPP
#include <ticpp.h>
const string DEFAULT_NAME("/home/C/Studia/PR/halter/halt");

#define PROG_NAME "resumer"


inline void resumeMsg(const ticpp::Element *parent, Msg &currMsg) {
	parent->FirstChildElement("who")->GetText(&currMsg.who);
	parent->FirstChildElement("lamport")->GetText(&currMsg.laport);
	parent->FirstChildElement("instr")->GetText((int *) &currMsg.oper.instr);
	parent->FirstChildElement("arg")->GetText(&currMsg.oper.arg);
}

inline void resumeState(const Tids &tids) {
	int msgNum;
	uint slaveId;
	HaltState resumeState;
	char resume = 1;
	Msg currMsg;
	ostringstream filename;
	ticpp::Document xmlDoc;
	ticpp::Element *xmlChanState, *xmlRoot;
	ticpp::Iterator<ticpp::Element> it("message");

#ifdef DEBUG
		printf("%s: resuming state...\n", PROG_NAME);
		fflush(stdout);
#endif

	for (uint i = 0; i < tids.size(); ++i) {
		pvm_initsend(PvmDataDefault);
		pvm_pkbyte(&resume, 1, 1);

		filename.str("");
		filename << DEFAULT_NAME << i << ".xml";

		xmlDoc.LoadFile(filename.str());
		xmlRoot = xmlDoc.FirstChildElement("slave");

		xmlRoot->GetAttribute("id", &slaveId);
		xmlRoot->FirstChildElement("procState")->GetText(&resumeState.instrNo);
		xmlRoot->FirstChildElement("haltState")->GetText(&resumeState.halt);
		xmlRoot->FirstChildElement("lamport")->GetText(&resumeState.lamport);
		xmlRoot->FirstChildElement("obj")->GetText(&resumeState.obj);
		xmlRoot->FirstChildElement("reg")->GetText(&resumeState.reg);

#ifdef DEBUG
		printf("%s: resuming slave id: %u, tid %x\n", PROG_NAME, slaveId, tids[slaveId]);
		fflush(stdout);
#endif

		pvm_pkbyte((char *) &resumeState, sizeof(resumeState), 1);

#ifdef DEBUG
		printf("%s: resuming state: %d, halt: %x, lamport: %d\n", PROG_NAME, resumeState.instrNo, resumeState.halt, resumeState.lamport);
		fflush(stdout);
#endif

		xmlChanState = xmlRoot->FirstChildElement("chanState");

		xmlChanState->GetAttribute("num", &msgNum);
		pvm_pkint(&msgNum, 1, 1);

#ifdef DEBUG
		printf("%s: resuming slave id: %u, unpacking %d msg\n", PROG_NAME, slaveId, msgNum);
		fflush(stdout);
#endif

		// Zapisywanie stanu kanału
		for (it = it.begin(xmlChanState); it != it.end(); ++it) {
			resumeMsg(&(*it), currMsg);
			pvm_pkbyte((char *) &currMsg, sizeof(Msg), 1);
		}

		pvm_send(tids[slaveId], RESUME);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <numb of slave>", argv[0]);
		exit(1);
	}

	int slaveNum = atoi(argv[1]);

	Tids tids;

	tids.resize(slaveNum);
	pvm_spawn(SLAVENAME, NULL, PvmTaskDefault, "", tids.size(), tids.data());

	init(tids);

	resumeState(tids);

	pvm_exit();
	return 0;
}
