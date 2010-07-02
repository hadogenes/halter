#include <pvm3.h>
#include <list>
#include <sstream>
using std::list;

#include "joblist.h"
#include "tids.h"
#include "joblist.h"

#define TIXML_USE_TICPP
#include <ticpp.h>
#define DEFAULT_NAME "halt.xml"


inline void saveMsg(const Msg &currMsg, ticpp::Element *parent) {
		ticpp::Element* xmlMsgWho = new ticpp::Element("who");
		ticpp::Element* xmlMsgInstr = new ticpp::Element("instr");
		ticpp::Element* xmlMsgArg = new ticpp::Element("arg");

		xmlMsgWho->SetValue(currMsg.who);
		xmlMsgInstr->SetValue(currMsg.oper.instr);
		xmlMsgArg->SetValue(currMsg.oper.arg);

		parent->LinkEndChild(xmlMsgWho);
		parent->LinkEndChild(xmlMsgInstr);
		parent->LinkEndChild(xmlMsgArg);
}

int main(int argc, char *argv[]) {

	pvmtaskinfo *taskp;
	int ntask;
	Tids tids;
	Msg haltMsg = { -1, { MARKER, pvm_mytid() } };

	pvm_tasks(0, &ntask, &taskp);

	tids.resize(ntask);

	for (int i = 0; i < ntask; ++i)
		if (taskp[i].ti_tid != pvm_mytid())
			tids[i] = taskp[i].ti_tid;

	pvm_initsend(PvmDataDefault);
	pvm_pkbyte((char *) &haltMsg, sizeof(haltMsg), 1);
	pvm_mcast(tids.data(), tids.size(), NORMAL);


	int who, procState, msgNum;
	WaitFor haltState;
	Msg currMsg;

	ticpp::Document xmlHalter;
	ticpp::Declaration decl("1.0", "utf-8", "");
	xmlHalter.InsertEndChild(decl);


	ticpp::Element *xmlRoot, *xmlSlave, *xmlProcState, *xmlHaltState, *xmlChanState;

	for (; ntask >= 0; --ntask) {
		pvm_recv(-1, STATE);
		pvm_upkint(&who, 1, 1);
		pvm_upkint(&procState, 1, 1);
		pvm_upkbyte((char *) haltState, sizeof(WaitFor), 1);

		// Allokowanie pamięci do nowego węzła
		xmlRoot = new ticpp::Element("halter");
		xmlSlave = new ticpp::Element("slave");
		xmlProcState = new ticpp::Element("procState");
		xmlHaltState = new ticpp::Element("haltState");
		xmlChanState = new ticpp::Element("chanState");

		xmlSlave->SetAttribute("slaveNo", who);

		xmlProcState->SetValue(procState);
		xmlHaltState->SetValue(haltState);
		xmlSlave->LinkEndChild(xmlProcState);
		xmlSlave->LinkEndChild(xmlHaltState);

		pvm_upkint(&msgNum, 1, 1);

		for (int i = 0; i < msgNum; ++i) {
			pvm_upkbyte((char *) &currMsg, sizeof(Msg), 1);
			saveMsg(currMsg, xmlChanState);
		}

		xmlSlave->LinkEndChild(xmlChanState);
		xmlRoot->LinkEndChild(xmlSlave);
	}

	pvm_exit();

	xmlHalter.LinkEndChild(xmlRoot);
	xmlHalter.SaveFile(argc == 2 ? argv[1] : DEFAULT_NAME);
	return 0;
}
