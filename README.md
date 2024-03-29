# UDP Microservices

This is a Microservices application that uses client/socket programming over TCP and UDP. All mandatory funtionalities from the assignment description are working.

HOW TO RUN:
1. First, compile all the files using:
    - g++ -O2 -Wall server.cpp -o server
    - g++ -O2 -Wall client.cpp -o client
    - g++ -O2 -Wall translator.cpp -o translator
    - g++ -O2 -Wall currency.cpp -o currency
    - g++ -O2 -Wall voting.cpp -o voting

2. Next, run server.cpp. From the command line type:
    - ./server <microservices IP address>
where <microservices IP address> is the IP address where your microservices will/ are running. If the argument field is left blank, default IP is "0.0.0.0". This can be used when running microservices locally. If you are running the microservices on another machine, make sure you use this field to specify that IP.

3. Next, run the 3 microservices in the desired location. You can either run the microservices on the same local machine as the rest of the application OR you can run them on a seperate machine. If you are running this on a seperate machine, it should be on the same IP specified in step 2 (e.i., <microservices IP address>). The testing section will cover in more detail the tested locations that my code works on. If you used the compilations from step 1, the commands to run the microservices are as follows:
    - ./translator
    - ./currency
    - ./voting

4. Finally, run client.cpp. This file can be run on any Linux machine, but it must take the IP of the machine that server.cpp is running on as an argument:
    - ./client <server IP address>
 where <server IP address> is the IP of the machine running server.cpp. If your server is running on the same machine as client, you can leave this field blank or specify "0.0.0.0". If the argument field is left blank, default IP is "0.0.0.0".

 HOW TO USE:
 All interactions are done through the client terminal. Please follow the prompts on screen and the app will work smoothly! Inputs must be in the correct format. If an input error is made, the client will be informed. However, if you do run into an error, restarting the server and the client should fix it.

 If you find that your requests keep on timing out, ensure that the microservices are running and the IP address is correct.

 FEATURES:
 - All mandatory features
- Able to switch between services without any crashes as long as correct format was used when selecting services (see KNOWN BUGS).
- If client is exited gracefully using the correct command, the server will be notified and drop the client without having to close the server. This means that you can rerun the client and reconnect to the server without having to restart the server.
- Able to get exchange rate between any of the listed currencies, not just CAD
- 3s TIMEOUT: If Indirection Server does not get a response from a microservice after 3s, the request will fail, the client will be notified, and the server will go back to waiting for a new request.
- A client can only vote once

TESTING SPECIFICATIONS:
- Tested on a 2018 Macbook Pro
- On Mac, the "MSG_CONFIRM" does not exist, so I had to define it myself in order to run local tests. When testing over LINUX (and in the submitted project files), I commented out this field.
- Ran on Linux over SSH
- Ran on local network and also on SSH [csx1.cpsc.ucalgary.ca] (compiled and ran fine on this!)

TESTS:

TEST 1 - All Local on MAC:
- Ran all files on local machines by specifying IP 0.0.0.0 OR by leaving IP field empty. Had to define the MSG_CONFIRM field.
- Everything worked!

TEST 2 - All LINUX over SSH:
- Ran all files over SSH [csx1.cpsc.ucalgary.ca] by specifying IP 0.0.0.0 OR by leaving IP field empty.
- Everything worked!

TEST 3 - Local client, server and microservices over SSH:
- Ran client on local machine using: ./client 136.159.5.25
- Ran server and microservices over SSH [csx1.cpsc.ucalgary.ca]. All of these worked for running the server: "$ ./server 136.159.5.25", "$ ./server 0.0.0.0", "./server" (IP didn't need to be specified here because server to service connection is over the same network)
- Everything worked!

TEST 4 - Local client and server, microservices over SSH:
- Ran client on local machine using: $ ./client.
- Ran server on local machine using: $ ./server 136.159.5.25
- Ran microservices over SSH [csx1.cpsc.ucalgary.ca].
- Everything worked!


 KNOWN BUGS:
- client.cpp gets stuck in an infinite loop when selecting the desired service if you accidentally type a string instead of a number. If this happens, please just restart client.cpp and server.cpp
