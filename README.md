Microservice UDP

TO-DO:
- Add timeouts on every send, recv and connect
- Should gracefully handle timeouts (exit that particular microservice)
- Decimal places on currency exchange result
X Convert all money vars to long double

- Voting looping
X Voting: Show candidates
- Voting: Secure voting
- Voting: Voting summary
- Client-side voting
- Service-side voting
- Server-side voting

- Figure out how to get/print microservice IP
    - Need to change Indirection server IP to school IP

BUGS:
- Client fails when sending string command
- Infinite loop after exit from client on server.cpp
- Need send/recv loop for TCP to ensure all bytes are being sent (from ass1)


TESTING:
- Browsing between services
- Running all local
- Running all Linux
- Running microservices on Linux, else local
- Running microservices and Server on Linux, else local
- Confirm MSG_CONFIRM Mac issue

