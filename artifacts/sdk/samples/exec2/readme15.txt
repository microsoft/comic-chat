------------------------------------------------------------------
    Microsoft Chat SDK 2.0 Exec2 Readme File
                         January 1998
------------------------------------------------------------------

             (c) Copyright Microsoft Corporation, 1998

CONTENTS
========

OVERVIEW OF THE EXEC2 SAMPLE
BUILDING THE EXEC2 SAMPLE


OVERVIEW OF THE EXEC2 SAMPLE
==============================
This is a Visual Basic application, which uses the Microsoft Chat Protocol 2.0. 
The sample allows you to setup a celebrity chat, where you specify hosts, moderators
and special guests in a room. This application is most useful for auditorium mode chats, 
where the messages that the users are sending are only sent to hosts. When moderators
and special guests use the exec2 tool, they will not see the messages sent by the users. 
Only the hosts will see the messages from users. 

To setup a celebrity chat with auditorium mode, you will need to follow these steps:

* hosts logons to server using exec2.
* create a room with auditorium mode using the create room command in exec2. 
* the moderator and special guests logon to the server using exec2.
* the host can then choose to assign special guest and moderator status by right clicking
  on the users name and then selecting special guest and moderator status. 

Exec2 also has many more functionalities: 

* Assigning limits to how many people can chat in the room
* Setting the topic of the room
* Kicking, banning and reinstating an user based on time limit
* automatically kicking, banning users based on banned word
* Reading in a text file that has questions and answers, so hosts can use prepared text in the chats
* Keeping a log of the chat. 
* launch a netmeeting call between hosts and users in the chat room. 

BUILDING THE EXEC2 SAMPLE
===========================

The Exec2 sample can be built with Visual Basic 5.0. 

1. Select the Open project command from the File menu.

2. Open the exec2.vbp file and build the application.

3. To use the netmeeting launch feature in exec2 you will need to register the nmstart.ocx. You 
   can do this by copying the nmstart.ocx to the windows\system directory and then 
   typing "regsvr32 nmstart.ocx" at the dos command. 

