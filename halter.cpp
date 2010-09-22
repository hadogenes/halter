#include <pvm3.h>
#include <string>
#include <sstream>
using std::string;
using std::ostringstream;

#include "joblist.h"
#include "tids.h"
#include "joblist.h"

#define TIXML_USE_TICPP
#include <ticpp.h>
const string DEFAULT_NAME("/home/C/Studia/PR/halter/halt");

#define PROG_NAME "halter"


inline void saveMsg(const Msg &currMsg, ticpp::Element *parent) {
		ticpp::Element* xmlMsgWho = new ticpp::Element("who");
		ticpp::Element* xmlMsgLamport = new ticpp::Element("lamport");
		ticpp::Element* xmlMsgInstr = new ticpp::Element("instr");
		ticpp::Element* xmlMsgArg = new ticpp::Element("arg");

		xmlMsgWho->SetText(currMsg.who);
		xmlMsgLamport->SetText(currMsg.laport);
		xmlMsgInstr->SetText((int) currMsg.oper.instr);
		xmlMsgArg->SetText(currMsg.oper.arg);

		parent->LinkEndChild(xmlMsgWho);
		parent->LinkEndChild(xmlMsgLamport);
		parent->LinkEndChild(xmlMsgInstr);
		parent->LinkEndChild(xmlMsgArg);
}

inline void saveState(int ntask) {
	uint who;
	int msgNum;
	HaltState resumeState;
	Msg currMsg;
	ticpp::Document *xmlHalter;
	const ticpp::Declaration decl("1.0", "utf-8", "");
	ticpp::Element *xmlSlave, xmlElement, *xmlChanState, *xmlMsg;
	ostringstream filename;

#ifdef DEBUG
	printf("%s: in func saveState\n", PROG_NAME);
	fflush(stdout);
#endif

	for (; ntask > 0; --ntask) {
		pvm_recv(-1, STATE);
		pvm_upkuint(&who, 1, 1);
		pvm_upkbyte((char *) &resumeState, sizeof(resumeState), 1);

#ifdef DEBUG
	printf("%s: recived from, id: %d\n", PROG_NAME, who);
	fflush(stdout);
#endif

		filename.str("");
		filename << DEFAULT_NAME << who << ".xml";
		xmlHalter = new ticpp::Document(filename.str().c_str());
		xmlHalter->InsertEndChild(decl);

		// Allokowanie pamięci do nowego węzła
		xmlSlave = new ticpp::Element("slave");
		xmlSlave->SetAttribute("id", who);

		xmlElement.SetValue("procState");
		xmlElement.SetText(resumeState.instrNo);
		xmlSlave->InsertEndChild(xmlElement);

		xmlElement.SetValue("lamport");
		xmlElement.SetText(resumeState.lamport);
		xmlSlave->InsertEndChild(xmlElement);

		xmlElement.SetValue("haltState");
		xmlElement.SetText(resumeState.halt);
		xmlSlave->InsertEndChild(xmlElement);

		xmlElement.SetValue("obj");
		xmlElement.SetText(resumeState.obj);
		xmlSlave->InsertEndChild(xmlElement);

		xmlElement.SetValue("reg");
		xmlElement.SetText(resumeState.reg);
		xmlSlave->InsertEndChild(xmlElement);

		xmlChanState = new ticpp::Element("chanState");

		pvm_upkint(&msgNum, 1, 1);
		xmlChanState->SetAttribute("num", msgNum);

		// Zapisywanie stanu kanału
		for (int i = 0; i < msgNum; ++i) {
			pvm_upkbyte((char *) &currMsg, sizeof(Msg), 1);

			xmlMsg = new ticpp::Element("message");
			saveMsg(currMsg, xmlMsg);
			xmlChanState->LinkEndChild(xmlMsg);
		}

		xmlSlave->LinkEndChild(xmlChanState);
		xmlHalter->LinkEndChild(xmlSlave);

		xmlHalter->SaveFile();
		delete xmlHalter;
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

	Msg haltMsg = { -1, 1, { MARKER, pvm_mytid() } };
	pvm_initsend(PvmDataDefault);
	pvm_pkbyte((char *) &haltMsg, sizeof(haltMsg), 1);
	pvm_mcast(tids.data(), tids.size(), NORMAL);

#ifdef DEBUG
	printf("%s: send done, tid: %x\n", PROG_NAME, haltMsg.oper.arg);
	fflush(stdout);
#endif

	saveState(ntask);

#ifdef DEBUG
	printf("%s: saved\n", PROG_NAME);
	fflush(stdout);
#endif
	pvm_exit();
	return 0;
}
