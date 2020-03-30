Description
------------------
Non - preemptive message passing Interface is a minuscule implementation of complex software applications in which thread creation, execution, schedulling, signal and wait on shared resources is all controlled by user thread library and not the kernel. As there are no kernel intervention user-level threads context switches faster than kernel-threads (eg: Posix) making it faster for an application that has a large number of threads that need frequent concurrent execution. However, the limitation of this code is the schedular designed can only manage a single core and not multiple cores which kernel can perform making this approach as Non-preemptive. 

Non-preemtive scheduling
------------------
Non-Preemptive scheduling is defined as when a process once allocated to the CPU holds the CPU until it gets terminated or it reaches a waiting state. In this code clients and servers are these processes and they continue to execute until they reach the waiting state due to a blocking receive call.

Message Passing System
------------------
This code is an example of a message-passing system implemented with the use of user-threads.
This message-passing system has the following implementation.

    - use of user-defined ports which is kernel global
    - Direct communication between clients and server: clients use server local port_id (0) and server identifies clients from its port_id from the packets send
    - asynchronous message passing of type Non-blocking send and Blocking receive\n
        - the sender sends and forgets
        - receive blocks if no information is received
    
File sharing service
------------------
A stateless server and client codes have been written that use a message passing interface to transfer text files. The code flow is explained below
    1) client requests for sending a file 
    2) client sends the filename 
    3) server verifies the filename size if exceeds 15 characters server responds with an error
    4) if filename is under 15 characters server responds with a go signal to send file data and renames at its side a 
       filename1.txt.server
    5) clients send file data
    6) server verifies if the file size has not exceeded 1MB in that case, an error is returned
    7) Server checks at a time only 3 clients are being served, a request for another client is denied by the server
       with a message printed as "DO NOT DO THAT" 
    8) If the file is uploaded, any client which was blocked by the server before gets the chance to upload its data

Note : The server is partially stateless as it maintains a record of client files in a lookup table

Dependencies
-------------------
gcc 

Build
-------------------
gcc file_test.c -o executableName

Run
-------------------
./executableName filename1.txt filename2.txt filename3.txt ....

eg: ./exe file_test.c message.h queue.h sem.h tcb.h thread.h

output 5 files with name file_test.c.server message.h.server queue.h.server sem.h.server tcb.h.server thread.h.server
will be created

Stop
------------------
CTRL + C

Sample text files can be downloaded from here : http://textfiles.com/directory.html





   

  


