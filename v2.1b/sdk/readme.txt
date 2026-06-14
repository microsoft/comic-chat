------------------------------------------------------------------
             Microsoft Chat SDK 2.0 Beta Readme File
                         October 1997            
------------------------------------------------------------------

             (c) Copyright Microsoft Corporation, 1997


HOW TO USE THIS DOCUMENT
========================

To view Readme.txt on screen in Windows Notepad, maximize the Notepad 
window.

To print Readme.txt, open the file in Notepad or another word processor,
and then on the File menu, click Print.


CONTENTS
========

ABOUT THIS RELEASE OF THE MICROSOFT CHAT SDK
INSTALLING THE CHAT SDK
OVERVIEW OF THE MICROSOFT CHAT SDK SAMPLES
KNOWN PROBLEMS
ABOUT SUPPORT


ABOUT THIS RELEASE OF THE MICROSOFT CHAT SDK
============================================

This SDK includes: 

· Two ActiveX Controls that allow the user to add chat functionality in 
  Web pages, Visual Basic applications, and other OLE-enabled applications.

· A set of samples that demonstrate ways to use the Controls. 

· Microsoft Chat 1.0j (mschat1j.exe) can be used with browsers that support Java. 

· Microsoft Chat 1.0j protocol beta (protocol.exe) can be used with browsers that support Java and JavaScript (or VBScript). 


INSTALLING THE CHAT SDK
=======================

To install the SDK, run chatsdk.exe. This executable program will:

1. Copy the binaries (chatsock.dll, mschat.ocx and mschatpr.ocx) to your 
   System directory. 

2. Register the controls as an OCX.

3. Create the following directories under the SDK directory that 
   you specify:

   Samples (Chat SDK samples)
   Help   (Chat SDK documentation)
   Chat1j  (Microsoft Chat 1.0 j)
   Include (includes the mschatpr.idl file needed)


OVERVIEW OF THE MICROSOFT CHAT SDK SAMPLES
==================================================

The Samples directory contains the following sample code, which 
demonstrates the use of the control in web pages, Microsoft Visual Basic 
Scripting and Visual Basic:

· Four Visual Basic applications that uses the Microsoft Chat Control 1.1

· Five HTML web pages that use Microsoft Chat Control 1.1 and use Visual Basic Scripting

· A sample application that demonstrate how to use the Chat Control 1.1 in a MDI MFC application.

· A sample application that uses Microsoft Chat Protocol 2.0 in a MFC application.

· Sample web pages that use Microsoft Chat Protocol 2.0 in web pages and uses Visual Basic Scripting.

For more information on the samples, see the Samples.txt file in the 
SDK directory you specified. In addition, each subdirectory under Samples 
contains a text file with more detailed information on that sample.

Some of the samples in the Chat SDK requires that you have Visual Basic 4.0 runtime, 
mschat.ocx, mschatpr.ocx and chatsock.dll installed on your system. If you have problems 
running any of the applications you can use the Application Setup Wizard in
Visual Basic 4.0 to create a setup program, which will create an installation 
program that installs all the required files for the samples. The Application setup 
wizard will not detect that the applications need to install chatsock.dll, you will
need to add this to the list of files to install, when you run the wizard. 

Mschatpr.ocx does not use chatsock.dll. 

The chat1j directory contains the samples used for Microsoft Chat 1.0j.

WARNING: If you are going to install and register mschat.ocx, chatsock.dll and mschatpr.ocx, please
install these files in the windows\system directory. If these files already exist 
in this directory, please verify that the versions that is on the machine is an 
earlier version than the files you are about to install. 

KNOWN PROBLEMS
==============
If you experience any problems connecting to the chat server with any of
the demos, please try to connect again, since the chat server might be down 
at this time.  If you continue to experience this problem, you may not be 
able to connect to the chat server if you are using a proxy server or firewall 
that doesn't pass port "6667". The system administrator for your Internet
Service Provider or your company can tell you if they allow pass through for
port "6667". 

ABOUT SUPPORT
============= 

Paid Support for Microsoft Chat SDK
===================================
Microsoft Chat SDK is supported by Microsoft Technical Support. You can 
ask questions through your Premier Level support contract. You can also ask 
questions through your Priority Level contract or purchase individual Priority 
Support incidents (essentially a one-time fee for one question). If you would 
like to understand more about Microsoft's paid support options, you can call 
Microsoft Support Sales at (800) 936-3500 from 6:00 a.m. to 6:00 p.m. Pacific 
time, Monday through Friday, excluding holidays. Please note that technical 
support is not available through this number. Microsoft Technical Support 
Information is also available on the World Wide Web at 
http://www.microsoft.com/support/. 

Free Support for Microsoft Chat SDK
===================================
Newsgroups are a great place for free peer support. As time and resources allow, 
Microsoft developers, program managers, support engineers, and test engineers 
visit the site to collect feedback and answer questions or correct 
misperceptions. There is no guarantee that you will receive a response from 
Microsoft to any newsgroup posting. 

To access newsgroups, use your preferred newsgroup reader and enter the news 
server address as news://msnews.microsoft.com. You can use the following URL to 
access the newsgroups directly from a Web browser: 
news:microsoft.public.newsgroup-name. The newsreader included with Internet 
Explorer version 3.0 supports multiple news servers; you can download the 
newsreader from http://www.microsoft.com/ie/ie3/imn.htm. 

The following newsgroups can be used to ask questions about Microsoft Chat 
SDK:
	microsoft.public.activex.controls.chatcontrol

Additional Support Information 
==============================
Microsoft Technical Support Information Services provides you with easy access 
to the latest technical and support information for Microsoft products. You can 
access a variety of low and no cost Information Services 24 hours a day, 365 
days a year. Many of the Microsoft Technical Support Information Services make 
reference to the following technical content: 

Microsoft Frequently Asked Questions (FAQ): Here you will find quick answers to 
the most common technical issues on using your favorite Microsoft product. 

Microsoft Software Library: The Microsoft Software Library contains hundreds of 
free software add-ons, bug fixes, peripheral drivers, software updates, and 
programming aids for easy downloading at your convenience. 

Microsoft Knowledge Base: The Microsoft Knowledge Base is the same database that 
Microsoft support engineers use to answer technical questions. It is a 
comprehensive collection of more than 70,000 detailed articles with technical 
information about Microsoft products, bug and fix lists, and answers to commonly 
asked technical questions. 

Internet services (World Wide Web and FTP sites): Customers access the Microsoft 
FAQ, Microsoft Software Library, and Microsoft Knowledge Base more than 850,000 
times each week on our Internet sites. Additional information, such as resource 
kits, white papers, and the latest information about Microsoft products is also 
readily available. It's easy to search through these technical sources to find 
what you need. If you're an Internet user, you can access this no-charge 
information (connect charges may apply) at the following locations: 

	The World Wide Web site is located at http://www.microsoft.com.

	The FTP site is located at ftp.microsoft.com. 

The Microsoft Network (MSN) and other online services: You can access the 
70,000-plus Microsoft Knowledge Base articles and the Microsoft Software Library 
files through MSN and other online services. Additional technical information 
and community services, such as the Microsoft Frequently Asked Questions, 
Member-to-Member Bulletin Board Services, and links to the World Wide Web, are 
available on MSN. To access Microsoft support services on MSN, choose "Go To 
Other Location" from the Edit menu and type "MSSUPPORT". 

Microsoft TechNet: Microsoft TechNet is the front-line resource for fast 
complete answers to technical questions on Microsoft desktop and systems 
products. From crucial data on client-server and workgroup computing, systems 
platforms, and database products, to the latest on support for Microsoft Windows 
and Macintosh(r)-based applications, it's all on Microsoft TechNet. Microsoft 
TechNet is $299 annually for a single user license, or $699 annually for a 
single-server, unlimited-users license. To subscribe to Microsoft TechNet, call 
(800) 344-2121. 

Microsoft Developer Network (MSDN): The Microsoft Developer Network is the 
comprehensive source of programming information and toolkits for those who write 
applications for the Microsoft Windows, Windows 95, and Windows NT operating 
systems, or use Microsoft products for development purposes. Members with an 
MSDN annual subscription are kept up-to-date through regular deliveries of 
information, a newsletter, and other information sources. To subscribe to the 
Microsoft Developer Network, call (800) 759-5474. 

Microsoft Download Service (MSDL): The Microsoft Download Service contains 
sample programs, device drivers, patches, software updates, and programming 
aids. Direct modem access to MSDL is available by dialing (206) 936-6735. The 
service is available 24 hours a day, 365 days a year. Connect information: 1200, 
2400, 9600, or 14400 baud, no parity, 8 data bits, and 1 stop bit. 

Microsoft FastTips: This automated service provides quick answers to your common 
technical questions via an automated toll-free telephone number, fax, or mail. 
To access FastTips or to receive a map and catalog, call the FastTips number 
listed for your product of interest: 

	Desktop applications(800) 936-4100 

	Personal Systems products(800) 936-4200 

	Development tools(800) 936-4300 

	Business Systems(800) 936-4400 

Per-Incident Electronic Service Requests: This service is available to Premier, 
Priority Comprehensive 35 and 75, and Priority Developer 35 customers. You can 
directly submit electronic service requests to Microsoft support engineers who 
receive the requests and work with you to resolve your technical problem. This 
capability also allows you to access Microsoft information databases so you can 
use the information to maintain and troubleshoot your Microsoft products 
independently. 


