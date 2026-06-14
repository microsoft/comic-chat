#include "stdafx.h"

#include "script.h"

CScript::CScript() {
	m_state = 0;
	m_timeEnteredState = CTime::GetCurrentTime();
	m_panelsInState = 0;
	m_isMaster = 0;
	m_isLeaving = 0;
	m_isEntering = 0;

}

void CScript::GetNextBackDrop() {
	if (m_isMaster) {	// contemplate transitioning to new state
	}
}

void CScript::SyncScript(unsigned short state) {
	if (m_state != state) {
		m_state = state;
		m_timeEnteredState = CTime::GetCurrentTime();
		m_panelsInState = 0;
	}
}

