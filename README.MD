## sock_connect

Shared library that realize connections.

Wrapper for linux TCP/UDP/unix/COM socket connections

### Navigation

<!-- TOC -->

- [Navigation](#sock_connect)    
    - [Installation](#installation)
    - [Usage](#usage)
    - [Examples](#Examples)
    - [Variations](#Variations)
    - [License](#license)

<!-- /TOC -->

#### Installation:

---
1. download **deb** packages: [**latest**](http://github.com/DGolgovsky/sock_connect/raw/master/packages/build/sock_connect-0.0.1-Linux.deb?inline=false "Latest stable package")
or [**devel**](http://github.com/DGolgovsky/sock_connect/raw/master/packages/build/sock_connect_devel-0.0.1-Linux.deb?inline=false "Debug package") (with debug information) versions

    * install it using `sudo dpkg -i ${package_name}`
    * or install it using `sudo apt install ${package_name}` 

2. compile from source
    * clone git repository `git clone git@github.com:DGolgovsky/sock_connect.git`
    * change directory into `cd sock_connect`
    * compile **latest** version with `./ci-build.sh` or `./ci-build.sh ON` for **debug** version
    * `cd build` and `sudo make install`

Package provides headers, installs at `/usr/include` and `.so` library, puts to `/usr/lib`

#### Usage:

---
* Include header in your source files where do you need it.
```cpp
#include <sock_connect.h>
```
```cpp
#include <devel/sock_connect.h>
```

* Compile with `-lsock_connect` or `-lsock_connect_devel` for devel version 

#### Examples:

---
* Receiving `TCP` as client:
```cpp
/* Creating new connection */
auto *TCP = new Connector<TCP>(INADDR_ANY, PORT);

while (true) {
    if (TCP->Connect()) { /* Connect to TCP sender */
        while (TCP->Receive(msg, 8*sizeof(char))) { // std::string &message

        /* Trying reconnect if connection lost
         * Receives -1 message size
         */
        if (!TCP->status()) {
            delete TCP; /* free memory */
            auto new_TCP = new Connector<TCP>(INADDR_ANY, PORT);
            TCP = new_TCP;
            break;
        }
    }
}
delete TCP;
```
* Receiving `TCP` as server:
```cpp
/* Creating new connection */
auto *TCP = new Connector<TCP>(INADDR_ANY, PORT);

TCP->Bind(); /* You can use it as TCP->Bind(true); for bind && listen */
TCP->Listen(); /* You don't need this part if you bind with (true) option */

ssize_t msg_sz;
int client_id;

while ((client_id = TCP.Accept()) > 0) {
    while ((msg_sz = TCP.Receive(client_id, msg))) {
        std::cout << msg << std::endl;
    }
    
    TCP.Shutdown(client_id);
}
delete TCP;
```
* Sending `TCP`:
```cpp
/* Creating new connection */
auto *TCP = new Connector<TCP>(INADDR_ANY, PORT);

std::string msg;

while (true) { // Reconnecting
    if (TCP->Connect()) {
        while (true) { // Reading messages queue
            msg = "Hello";
            if (!msg.empty()) {
                TCP->Send(msg); // std::string message
                
                /* Trying reconnect if connection lost */
                if (!TCP->state) {
                    delete _TCP;
                    auto new_TCP = new Connector<TCP>(INADDR_ANY, PORT);
                    TCP = new_TCP;
                    break;
                }                
            }
        }
    }
}
delete TCP;
```
* You can connect via `COM-port`
```cpp

const speed_t speed = B115200;
std::string dev_path = "/dev/ttyACM0";

auto handler = new Connector<USB>(dev_path, speed);
handler->Connect();

size_t size = 2 * sizeof(uint8_t);
uint8_t value[] = {1, 2};
uint8_t rec_value[12];

while(value[0] != 42) {
    std::cout << "Values to send: "; std::cin >> value[0] >> value[1]
    handler->Send(value, size);
    
    auto sz = handler->Receive(rec_value, 12);
    if (sz) {
        std::cout << "Received: ";
        for (auto zz = 0; zz < 12; ++zz)
            std::cout << std::hex << (int) rec_value[zz] << ";";
        std::cout << std::endl;
    }
    bzero(rec_value, 12);
}
handler->Shutdown();
delete handler;
```
#### Variations

---

1. Connection with TCP transmission
    * `Connector<TCP>(INADDR_ANY, PORT);`
2. Connection with UDP transmission
    * `Connector<UDP>(INADDR_ANY, PORT);`
3. Connection with UNIX sockets transmission
    * `Connector<UNIX>("/tmp/unix_socket");`
4. Connection with COM-port transmission
    * `Connector<USB>(dev_path, speed);`

#### License

---

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

[MIT License](../blob/master/LICENSE)
