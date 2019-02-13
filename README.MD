## sock_connect

Shared library that realize connections.

### Navigation

<!-- TOC -->

- [Navigation](#sock_connect)    
    - [Installation](#installation)
    - [Usage](#usage)
    - [Examples](#examples)
    - [Variations](#variations)
    - [License](#license)

<!-- /TOC -->

#### Installation:

---
1. download **deb** packages: [**latest**](https://github.com/DGolgovsky/sock_connect/raw/master/packages/build/sock_connect-0.1-Linux.deb "Latest stable package")
or [**devel**](https://github.com/DGolgovsky/sock_connect/raw/master/packages/devel/sock_connect-0.1-Linux.deb "Debug package") (with debug information) versions

    * install it using `sudo dpkg -i ${package_name}`
    * or install it using `sudo apt install ${package_name}` 

2. compile from source
    * `git clone https://github.com/DGolgovsky/sock_connect.git` clone git repository 
    * `cd sock_connect` change directory to cloned repo-dir
    * `./ci-build.sh` compile **latest** version with or `./ci-build.sh ON` for **debug** version
    * `cd build` jump to directory with compiled objects
    * `sudo make install` install library as `root`

Package provides headers installs at `/usr/include` and `.so` library, puts to `/usr/lib`

#### Usage:

---
* Include header in your source files where do you need it.
```cpp
#include <sock_connect.h>
```

* Compile with `-lsock_connect`

#### Variations:

---

1. Connection with `TCP` transmission
    * `auto TCP = new Connector<TCP>(IP, PORT);`
2. Connection with `UDP` transmission
    * `auto UDP = new Connector<UDP>(INADDR_ANY, PORT);`
3. Connection with `UNIX` sockets transmission
    * `auto UNIX = new Connector<UNIX>(dev_path);`
4. Connection with `USB` transmission
    * `auto USB = new Connector<USB>(dev_path, speed);`
5. Transform `const char*` ip-addr `192.168.1.42` to `uint32_t`
    * `uint32_t IP = ip_to_int(argv[1]);`

#### Examples:

---
* For examples used `Queue` from [**this**](https://github.com/DGolgovsky/data "Wrapper for message structure to send/recv at control system") repository whis realize `priority_queue` wrapper
```cpp
static auto g_queue = new Queue();
```
* Receiving `TCP` as client:
```cpp
uint16_t msg = 0;
auto recv_sz = sizeof(uint16_t);

auto TCP = new Connector<TCP>(IP, PORT);
while (true) {
    if (TCP->Connect()) {
        while (TCP->Receive(&msg, recv_sz)) {
            g_queue->push(&msg);
        }
        // Trying reconnect if connection lost
        delete TCP;
        auto new_TCP = new Connector<TCP>(IP, PORT);
        TCP = new_TCP;
    }
}
delete TCP;
```
* Receiving `TCP` as server:
```cpp
template<class T>
void handler(T TCP, int client_id) {
    uint16_t msg = 0;
    auto recv_sz = sizeof(uint16_t);
    while ((TCP->Receive(&msg, recv_sz))) {
        g_queue->push(&msg);
    }
    TCP->Shutdown(client_id);
}

/* Used if [this] - recv server */
void receiver()
{
    /* Creating new connection */
    auto *TCP = new Connector<TCP>(IP, PORT);

    TCP->Bind(); /* You can use it as TCP->Bind(true); for bind && listen */
    TCP->Listen(); /* You don't need this part if you bind with (true) option */

    int client_id = 0;
    while ((client_id = TCP->Accept()) > 0) {
        /* Accepting new connections and start receiving with the new thread */
        std::thread thread(handler<decltype(TCP)>, TCP, client_id);
        thread.detach();
    }
    delete TCP;
}
```
* Sending `TCP`, broadcasting `UDP`:
```cpp
auto TCP = new Connector<TCP>(IP, PORT);
auto UDP = new Connector<UDP>(INADDR_ANY, PORT); // 0.0.0.0

uint16_t msg = 0;
auto msg_sz = sizeof(uint16_t);

while (true) {
    if (TCP->Connect()) {
        while (TCP->status()) {
            msg = g_queue->top<uint16_t>(); // Reading messages queue
            if (msg) {
                if (!TCP->Send(&msg, msg_sz)) continue;
                UDP->Send(&msg, msg_sz);
                g_queue->pop();
            }
        }
        // Reconnecting
        delete TCP;
        auto new_TCP = new Connector<TCP>(IP, PORT);
        TCP = new_TCP;
    }
}
delete UDP;
delete TCP;
```
#### License:

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
