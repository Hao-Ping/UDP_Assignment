# UDP (User Datagram Protocol)
* The server listens to UDP messages on a specific IP and port then reply the same message back to the client.

* The client sends an UDP message to the server and display the echo message. If the client doesn't get the echo, it will resend message using exponential backoff algorithm.

## Exponential Backoff

Network communication can fail anytime. Client applications deal with this by retrying. However, if clients retry without waiting, they may overwhelm the system.

Exponential backoff makes clients wait progressively longer between consecutive retries.
```
wait_interval = base_interval * multiplier^n
```
base_interval is the first retry interval;

n is the number of failures that have occured;

multiplier is an arbitrary multiplier.(multiplier = 2 in my case)

## Usage 
(after compiling both .c file to "server.exe" & "client.exe")

Server: 
```bash
./server <IP> <Port> #ex: ./server 127.0.0.1 5000
```
Client:
```bash
./client <IP> <Port> <Message> <Max-retry> #ex: ./client 127.0.0.1 5000 HELLO 5
```
## Demo
https://youtu.be/-x_hHYsj0-Q
