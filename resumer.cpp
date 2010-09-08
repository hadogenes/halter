#include <cstdio>
#include <cstdlib>
#include <pvm3.h>

#define SLAVENAME "slave"

#include <pvm3.h>
#include "joblist.h"
#include "init.h"

#define TIXML_USE_TICPP
#include <ticpp.h>
#define DEFAULT_NAME "/home/C/Studia/PR/halter/halt.xml"

#define PROG_NAME "resumer"


inline void resumeMsg(const ticpp::Element *parent, Msg &currMsg) {
	parent->FirstChildElement("who")->GetText(&currMsg.who);
	parent->FirstChildElement("lamport")->GetText(&currMsg.laport);
	parent->FirstChildElement("instr")->GetText((int *) &currMsg.oper.instr);
	parent->FirstChildElement("arg")->GetText(&currMsg.oper.arg);
}

inline void resumeState(const Tids &tids, ticpp::Element *xmlRoot) {
	int procState, msgNum;
	uint slaveId, lamport;
	char resume = 1;
	char haltState;
	Msg currMsg;
	ticpp::Element *xmlChanState;
	ticpp::Iterator<ticpp::Element> it("slave");

#ifdef DEBUG
		printf("%s: resuming state...\n", PROG_NAME);
		fflush(stdout);
#endif

	for (it = it.begin(xmlRoot); it != it.end(); ++it) {
		pvm_initsend(PvmDataDefault);
		pvm_pkbyte(&resume, 1, 1);

		(*it).GetAttribute("id", &slaveId);
		(*it).FirstChildElement("procState")->GetText(&procState);
		(*it).FirstChildElement("haltState")->GetText((int *) &haltState);
		(*it).FirstChildElement("lamport")->GetText(&lamport);

#ifdef DEBUG
		printf("%s: resuming slave id: %u, tid %x\n", PROG_NAME, slaveId, tids[slaveId]);
		fflush(stdout);
#endif

		pvm_pkint(&procState, 1, 1);
		pvm_pkbyte(&haltState, 1, 1);
		pvm_pkuint(&lamport, 1, 1);

#ifdef DEBUG
		printf("%s: resuming state: %d, halt: %x, lamport: %d\n", PROG_NAME, procState, (char) haltState, lamport);
		fflush(stdout);
#endif

		xmlChanState = (*it).FirstChildElement("chanState");

		xmlChanState->GetAttribute("num", &msgNum);
		pvm_pkint(&msgNum, 1, 1);

#ifdef DEBUG
		printf("%s: resuming slave id: %u, unpacking %d msg\n", PROG_NAME, slaveId, msgNum);
		fflush(stdout);
#endif

		// Zapisywanie stanu kanału
		for (int i = 0; i < msgNum; ++i) {
			resumeMsg(xmlChanState, currMsg);
			pvm_pkbyte((char *) &currMsg, sizeof(Msg), 1);
		}

		pvm_send(tids[slaveId], RESUME);
	}
}

int main(int argc, char *argv[]) {
	ticpp::Document xmlHalter;
	ticpp::Element *xmlRoot;
	int slaveNum;
	Tids tids;

	xmlHalter.LoadFile(/*argc == 2 ? argv[1] :*/ DEFAULT_NAME);

	// Pobieranie liczby slave'ów
	xmlRoot = xmlHalter.FirstChildElement("halter");
	xmlRoot->GetAttribute("num", &slaveNum);

#ifdef DEBUG
		printf("%s: slave num: %d\n", PROG_NAME, slaveNum);
		fflush(stdout);
#endif

	tids.resize(slaveNum);
	pvm_spawn(SLAVENAME, NULL, PvmTaskDefault, "", tids.size(), tids.data());

	init(tids);

	resumeState(tids, xmlRoot);

	pvm_exit();
	return 0;
}
