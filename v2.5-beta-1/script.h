class CScript {
public:
	unsigned short m_state;
	CTime m_timeEnteredState;
	unsigned short m_panelsInState;
	BOOL m_isMaster;
	BOOL m_isLeaving;
	BOOL m_isEntering;


	CScript();
	void GetNextBackDrop();
	void SyncScript(unsigned short state);
};

