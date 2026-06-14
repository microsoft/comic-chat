class CIrcSocket : public CAsyncSocket {
public:
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode) { TRACE("Out of Band socket on error %d.\n", nErrorCode); }
	virtual void ProcessMessage(char *);
};