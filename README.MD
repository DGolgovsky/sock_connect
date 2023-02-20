## Socket Connection wrapper shared library

Shared library that realize sockets connections and could transfer data-packages.

#### Navigation

- [Navigation](#navigation)
- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Variations](#variations)
- [License](#license)

#### Installation:

---
Compile from source

* `git clone https://github.com/DGolgovsky/sock_connect.git` clone git repository
* `cd sock_connect` change directory to cloned repo-dir
* `./ci-build.sh -r` compile **latest** release version
* also you can use `./ci-build.sh -h` for print help information for another variations of building
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
1. Connection with `tcp` transmission
    * `auto tcp = std::make_shared<connector<tcp>>(IP, PORT);`
2. Connection with `udp` transmission
    * `auto udp = std::make_shared<connector<udp>>(INADDR_ANY, PORT);`
3. Connection with `sun-domain` sockets transmission
    * `auto unix = std::make_shared<connector<sun>>(sun_path);`
4. Connection with `usb` transmission
    * `auto usb = std::make_shared<connector<usb>>(dev_path, speed);`
5. **`DEPRECATED`** Transform `const char*` ip-addr `192.168.1.42` to `uint32_t`
    * `uint32_t IP = ip_to_int(argv[1]);`

#### Examples:

---
* Receiving `tcp` as client:

```cpp
uint16_t msg = 0;
auto recv_sz = sizeof(uint16_t);

/* Create new socket connection */
auto tcp = std::make_shared<connector<tcp>>(IP, PORT);
while (tcp->status()) {
    // While tcp establish connection to server
    if (tcp->connect()) {
        // While data receiving do something with it
        // if tcp connection lost tcp->receive returns false
        while (tcp->receive(&msg, recv_sz)) {
            // do_something with received data
            // ...
        }
        // Trying to reconnect if connection lost
        tcp.reset(new connector<tcp>(IP, PORT));
    }
}
```

* Receiving `tcp` as multi-server:

```cpp
template<class T>
void handler(T tcp, int client_id) {
    tcp->assign_thread(client_id);
    uint16_t msg = 0;
    auto recv_sz = sizeof(uint16_t);
    while ((tcp->receive(&msg, recv_sz))) {
        // do_something with received message
        // ...
    }
    tcp->shutdown(client_id);
}

/* Used if [this] - recv server */
void receiver()
{
    /* Creating new connection */
    auto tcp = std::make_shared<connector<tcp>>(IP, PORT);

    tcp->bind(false); /* You can use it as tcp->bind(); for bind && listen */
    tcp->listen(); /* You may don't need this part if you bind with (true) option */

    int client_id = 0;
    while ((client_id = tcp->accept()) > 0) {
        /* Accepting new connections and start receiving with the new thread */
        std::thread thread(handler<decltype(tcp)>, tcp, client_id);
        thread.detach();
    }
}
```

* Sending `tcp` as client to server, broadcasting `udp`:

```cpp
auto tcp = std::make_shared<connector<tcp>>(INADDR_LOOPBACK, PORT); // 127.0.0.1
auto udp = std::make_shared<connector<udp>>(INADDR_ANY, PORT); // 0.0.0.0

uint16_t msg = 0;
auto msg_sz = sizeof(uint16_t);

// Infinite loop. This allows to execute tcp_sender as daemon
while (true) {
    if (tcp->connect()) {
        // If connection successful,
        // send data while status() returns true
        while (tcp->status()) {
            msg = rand() % 50; // Generate random data
            if (msg) {
                // Trying to send message
                // Continue if sending is failure,
                // Also tcp->status() will return false
                if (!tcp->send(&msg, msg_sz)) continue;
                // Send udp package
                udp->send(&msg, msg_sz);                
            }
        }
        // Reconnecting if connection lost
        tcp.reset(new connector<tcp>(INADDR_LOOPBACK, PORT));
    }
}
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
