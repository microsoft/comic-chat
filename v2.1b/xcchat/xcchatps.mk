
xcchatps.dll: dlldata.obj xcchat_p.obj xcchat_i.obj
	link /dll /out:xcchatps.dll /def:xcchatps.def /entry:DllMain dlldata.obj xcchat_p.obj xcchat_i.obj kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib 

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL $<

clean:
	@del xcchatps.dll
	@del xcchatps.lib
	@del xcchatps.exp
	@del dlldata.obj
	@del xcchat_p.obj
	@del xcchat_i.obj
