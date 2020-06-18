## Shared library Socket Connection

Shared library that realize connections and could transfer data-packages.

#### Navigation

- [Navigation](#navigation)
- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Variations](#variations)
- [License](#license)

#### Installation:

---
1. download **`deb`** packages: [**latest**](https://github.com/DGolgovsky/sock_connect/raw/master/packages/build/libsock_connect-0.5-1-x86_64.deb "Latest stable package")
or [**devel**](https://github.com/DGolgovsky/sock_connect/raw/master/packages/debug/libsock_connect-0.5-1-x86_64.deb "Debug package") (with debug information) versions
    * install it using `sudo dpkg -i ${package_name}`
    * or install it using `sudo apt install ${package_name}` 
2. download **`rpm`** packages: [**latest**](https://github.com/DGolgovsky/sock_connect/raw/master/packages/build/libsock_connect-0.5-1-x86_64.rpm "Latest stable package")
or [**devel**](https://github.com/DGolgovsky/sock_connect/raw/master/packages/debug/libsock_connect-0.5-1-x86_64.rpm "Debug package") (with debug information) versions
    * install it using `sudo dnf -i ${package_name}`
    * or install it using `sudo zypper install ${package_name}` 
3. download packages for **`other`** linux distroes: [**latest**](https://github.com/DGolgovsky/sock_connect/raw/master/packages/build/libsock_connect-0.5-1-x86_64.tar.gz "Latest stable package")
or [**devel**](https://github.com/DGolgovsky/sock_connect/raw/master/packages/debug/libsock_connect-0.5-1-x86_64.tar.gz "Debug package") (with debug information) versions
    * extract downloaded package `tar xzfv libsock_connect-${VERSION}-x86_64.tar.gz`
    * `cd libsock_connect-${VERSION}-x86_64`
    * `sudo cp -rv * /usr/local/`
4. compile from source
    * `git clone https://github.com/DGolgovsky/sock_connect.git` clone git repository 
    * `cd sock_connect` change directory to cloned repo-dir
    * `./ci-build.sh -b` compile **latest** version
    * **OR** use `./ci-build.sh -h` for print help information for another variations of building
    * `cd build` jump to directory with compiled objects
    * `sudo make install` install library as `root`

Package provides headers installs at `/usr/local/include` and `.so` library, puts to `/usr/local/lib`

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
    * `auto tcp = new Connector<TCP>(IP, PORT);`
2. Connection with `UDP` transmission
    * `auto udp = new Connector<UDP>(INADDR_ANY, PORT);`
3. Connection with `UNIX` sockets transmission
    * `auto unix = new Connector<UNIX>(dev_path);`
4. Connection with `USB` transmission
    * `auto usb = new Connector<USB>(dev_path, speed);`
5. **`DEPRECATED`** Transform `const char*` ip-addr `192.168.1.42` to `uint32_t`
    * `uint32_t IP = ip_to_int(argv[1]);`

#### Examples:

---
* Receiving `TCP` as client:
```cpp
uint16_t msg = 0;
auto recv_sz = sizeof(uint16_t);

/* Create new socket connection */
auto tcp = new Connector<TCP>(IP, PORT);
while (true) {
    // While tcp establish connection to server
    if (tcp->Connect()) {
        // While data receiving do smtg with it
        // if tcp connection lost tcp->Receive returns false
        while (tcp->Receive(&msg, recv_sz)) {
            // do_something with received data
            // ...
        }
        // Trying reconnect if connection lost
        delete tcp;
        auto new_tcp = new Connector<TCP>(IP, PORT);
        tcp = new_tcp;
    }
}
delete tcp;
```
* Receiving `TCP` as multi-server:
```cpp
template<class T>
void handler(T tcp, int client_id) {
    uint16_t msg = 0;
    auto recv_sz = sizeof(uint16_t);
    while ((tcp->Receive(&msg, recv_sz))) {
        // do_something with received message
        // ...
    }
    tcp->Shutdown(client_id);
}

/* Used if [this] - recv server */
void receiver()
{
    /* Creating new connection */
    auto tcp = new Connector<TCP>(IP, PORT);

    tcp->Bind(); /* You can use it as TCP->Bind(true); for bind && listen */
    tcp->Listen(); /* You may don't need this part if you bind with (true) option */

    int client_id = 0;
    while ((client_id = tcp->Accept()) > 0) {
        /* Accepting new connections and start receiving with the new thread */
        std::thread thread(handler<decltype(tcp)>, tcp, client_id);
        thread.detach();
    }
    delete tcp;
}
```
* Sending `TCP` as client to server, broadcasting `UDP`:
```cpp
auto tcp = new Connector<TCP>(INADDR_LOOPBACK, PORT); // 127.0.0.1
auto udp = new Connector<UDP>(INADDR_ANY, PORT); // 0.0.0.0

uint16_t msg = 0;
auto msg_sz = sizeof(uint16_t);

// Infinite loop. This allow execute tcp_sender as daemon
while (true) {
    if (tcp->Connect()) {
        // If connection successfull,
        // send data while status() returns true
        while (tcp->status()) {
            msg = rand() % 50; // Generate random data
            if (msg) {
                // Trying to send message
                // Continue if sending is failure,
                // Also tcp->status() will return false
                if (!tcp->Send(&msg, msg_sz)) continue;
                // Send udp package
                udp->Send(&msg, msg_sz);                
            }
        }
        // Reconnecting if connection lost
        delete tcp;
        auto new_tcp = new Connector<TCP>(INADDR_LOOPBACK, PORT);
        tcp = new_tcp;
    }
}
delete udp;
delete tcp;
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