class CLink {
public:
	CLink(int start, int length, const char *url) { m_start = start; m_length = length; m_url = strdup(url); }
	CLink(const CLink &f) { m_start = f.m_start, m_length = f.m_length; m_url = strdup(f.m_url); }
	~CLink() { free((void *) m_url); }
	const char *m_url;
	int m_start;
	int m_length;
};

BOOL FLaunchBrowser(const TCHAR *szURL);
