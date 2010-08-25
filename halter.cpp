#include <pvm3.h>

#include "joblist.h"
#include "tids.h"
#include "joblist.h"

#define TIXML_USE_TICPP
#include <ticpp.h>
#define DEFAULT_NAME "/home/C/Studia/PR/halter/halt.xml"

#define PROG_NAME "halter"


inline void saveMsg(const Msg &currMsg, ticpp::Element *parent) {
		ticpp::Element* xmlMsgWho = new ticpp::Element("who");
		ticpp::Element* xmlMsgInstr = new ticpp::Element("instr");
		ticpp::Element* xmlMsgArg = new ticpp::Element("arg");

		xmlMsgWho->SetText(currMsg.who);
		xmlMsgInstr->SetText((int) currMsg.oper.instr);
		xmlMsgArg->SetText(currMsg.oper.arg);

		parent->LinkEndChild(xmlMsgWho);
		parent->LinkEndChild(xmlMsgInstr);
		parent->LinkEndChild(xmlMsgArg);
}

inline void saveState(int ntask, ticpp::Element *xmlRoot) {
	int who, procState, msgNum;
	WaitFor haltState;
	Msg currMsg;
	ticpp::Element *xmlSlave, *xmlProcState, *xmlHaltState, *xmlChanState;

#ifdef DEBUG
	printf("%s: in func saveState\n", PROG_NAME);
	fflush(stdout);
#endif

	for (; ntask > 0; --ntask) {
		pvm_recv(-1, STATE);
		pvm_upkint(&who, 1, 1);
		pvm_upkint(&procState, 1, 1);
		pvm_upkbyte((char *) &haltState, sizeof(WaitFor), 1);

#ifdef DEBUG
	printf("%s: recived from, id: %d\n", PROG_NAME, who);
	fflush(stdout);
#endif

		// Allokowanie pamięci do nowego węzła
		xmlSlave = new ticpp::Element("slave");
		xmlProcState = new ticpp::Element("procState");
		xmlHaltState = new ticpp::Element("haltState");
		xmlChanState = new ticpp::Element("chanState");

		xmlSlave->SetAttribute("id", who);

		xmlProcState->SetText(procState);
		xmlHaltState->SetText(haltState);
		xmlSlave->LinkEndChild(xmlProcState);
		xmlSlave->LinkEndChild(xmlHaltState);

		pvm_upkint(&msgNum, 1, 1);
		xmlChanState->SetAttribute("num", msgNum);

		// Zapisywanie stanu kanału
		for (int i = 0; i < msgNum; ++i) {
			pvm_upkbyte((char *) &currMsg, sizeof(Msg), 1);
			saveMsg(currMsg, xmlChanState);
		}

		xmlSlave->LinkEndChild(xmlChanState);
		xmlRoot->LinkEndChild(xmlSlave);
	}
}

int main(int argc, char *argv[]) {
	pvmtaskinfo *taskp;
	int ntask;
	Tids tids;
	pvm_tasks(0, &ntask, &taskp);

	tids.resize(ntask - 2);

	int j = 0;
	for (int i = 0; i < ntask; ++i) {
#ifdef DEBUG
		printf("%s: tid[%d]: %x (%s)\n", PROG_NAME, i, taskp[i].ti_tid, taskp[i].ti_a_out);
		fflush(stdout);
#endif

		if ((taskp[i].ti_tid != pvm_mytid()) && (strcmp(taskp[i].ti_a_out, SLAVENAME) == 0)) {
			tids[j++] = taskp[i].ti_tid;
		}
	}

	// proces "halter" nie ma być brany pod uwagę
	ntask = tids.size();

	Msg haltMsg = { -1, { MARKER, pvm_mytid() } };
	pvm_initsend(PvmDataDefault);
	pvm_pkbyte((char *) &haltMsg, sizeof(haltMsg), 1);
	pvm_mcast(tids.data(), tids.size(), NORMAL);

#ifdef DEBUG
	printf("%s: send done, tid: %x\n", PROG_NAME, haltMsg.oper.arg);
	fflush(stdout);
#endif

	ticpp::Document xmlHalter;
	ticpp::Declaration decl("1.0", "utf-8", "");
	xmlHalter.InsertEndChild(decl);

	ticpp::Element *xmlRoot = new ticpp::Element("halter");
	xmlRoot->SetAttribute("num", ntask);

#ifdef DEBUG
	printf("%s: saving to file (slave: %d)\n", PROG_NAME, ntask);
	fflush(stdout);
#endif

	saveState(ntask, xmlRoot);

#ifdef DEBUG
	printf("%s: saving to file v2\n", PROG_NAME);
	fflush(stdout);
#endif

	xmlHalter.LinkEndChild(xmlRoot);
	xmlHalter.SaveFile(/*argc == 2 ? argv[1] :*/ DEFAULT_NAME);

#ifdef DEBUG
	printf("%s: saved\n", PROG_NAME);
	fflush(stdout);
#endif
	pvm_exit();
	return 0;
}
