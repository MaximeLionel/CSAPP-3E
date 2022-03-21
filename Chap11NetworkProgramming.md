# 11.1 The Client-Server Programming Model
* Every network application is based on **Client-Server Programming Model**.
* In Client-Server Programming Model, an application consist of **a server process** and **one or more client processes**.
* A server manages some **resources** and it provides some **service** for clients by manipulating the resource.
* In Client-Server Programming Model, the fundamental operation is **transaction**.
* A Client-Server transaction consists of 4 steps:
  1. When a client need a service, it initiates a transaction by sending a request to a server.
  2. The server receives the request, interprets it, and manipulating its resources in the appropriate way (such as read a disk file, etc).
  3. The server sends a response to the client and wait for the next request.
  4. The client receives the response and manipulate it (such as display it on screen, etc).
* Clients and servers are **processes** but **not** machines.
* A single host can run many different clients and servers **concurrently**.

# 11.2 Networks
* To a host, a network is just another I/O device that serves as a source and sink for data.
![HardwareOrganizationOfNetworkHost](./Figure11_2.JPG)
* The most popular LAN (Local Area Network) is **Ethernet**.
* An Ethernet segment consists of some **wire** (twisted pairs of wires) and a **hub**.
![EthernetSegment](./Figure11_3.JPG)
* Each Ethernet adaptor has a global **unique 48bit address** that is stored in a **nonvolatile** memory on the adaptor.
  * A host can send a chunk of bits called a **frame** to other host on the memory.
  * Each frame includes **header** bits that identify **source** and **destination** of the frame and the **length** of the frame, then a payload of **data** bits.
  * Every host adaptor sees the frame, but **only** the destination host reads it.
* Multiple Ethernet segments can be connected to the larget LANs, called **bridged Ethernets**, using sets of wires and small boxes called **bridges**.
![BridgedEthernetSegments](./Figure11_4.JPG)
* At a higher level in the hierarchy, multiple incompatible LANs can be connected by specialized computers called **Routers** to form an **internet**
* Routers can also connect high-speed point-to-point phone connections which is WANs (Wide Area Networks).
* Routers can be used to build internets from arbitrary collections of LANs and WANs. 
![ASmallInternet](./Figure11_6.JPG)
* How to send data from one host to another destination of host across all incompatible networks? 
  * Protocol software that is running on each host and router.
  * Protocol must provide 2 basic capabilities:
    * Naming Scheme - assign each host an unique internet address.
    * Delivery Mechanism - bundle up data into **packets**.
      * A packet contains a **header** and a **payload**.
* Example:
![DataTravelOneHostToAnother](./Figure11_7.JPG)
  1. Client on host A invokes a **system call** to copy the data from client's virtual address space into a **kernel buffer**.
  2. **Protocol software** on host A creates a LAN1 frame by appending an **internet packet header (PH)** and **LAN1 frame header** to the data.
    * PH is addressed to internet host B.
    * FH is addressed to the router.
  3. LAN1 adaptor copies the frame to the network.
  4. The **router's LAN1 adaptor** receives the frame and passes it to the **protocol software**.
  5. Construct new frame:
     * The router get the destination internet address from PH.
     * The router use the destination address as an index to the router table to determine where to forward the packet.
     * The router strip off the old LAN1 header (FH) and append new LAN2 header addresses to host B.
     * The router pass the new frame to the adaptor.
  6. LAN2 adaptor receives the frame.
  7. The frame reaches host B. The adaptor pass it to **protocol software**.
  8. The protocol software strip off PH and FH. The protocol software will copy the data to the server's virtual address.

# 11.3 The Global IP Internet
* The basic hardware and software organization of an Internet client-server application:
  ![HWandSW](./Figure11_8.JPG)
* Each Internet host runs software that implements the **TCP/IP protocol (Transmission Control Protocol / Internet Protocol)**.
* Internet clients and servers communicate using a mix of **sockets interface functions** and **Unix I/O functions**.
* TCP/IP is actually a family of protocols.
* Datagrams: packets.
* UDP: unreliable datagram protocol.
* Internet Properties:
  * Hosts are mapped to **32bit IP addresses**.
    * IPv4 - 32bit address.
    * IPv6 - 128bit address.
  * IP Addresses are mapped to **Internet Domain Names**.
  * A process on one Internet host can communicate with a process on any other Internet host over a **connection**.
## 11.3.1 IP Addresses
* Network stores IP addresses in **IP address structure** below:
  ```
    struct in_addr{
        uint32_t    s_addr; /* Big-endian */
    }
  ```
* TCP/IP defines a uniform **network byte order** (big-endian byte order) for any integer data item.
* IP address format: **dotted decimal notation**.
  * 128.2.194.242 = 0x8002c2f2.
  ```
    linux> hostname -i  /* determine the dotted decimal address of your own host */
    128.2.194.242
  ```
  * Application program can convert back and forth between **IP addresses** and **dotted-decimal string**.




# Practical Problems
## Practical Problem 11.1
107.212.122.205:    0x68D47ACD
64.12.149.13:       0x400C950D
107.212.96.29:      0x6BD4601D
0.0.0.128:          0x00000080
255.255.255.0:      0xFFFFFF00
10.1.1.64:          0x0A010140
## Practical Problem 11.2
``` hex2dd.c
    
```