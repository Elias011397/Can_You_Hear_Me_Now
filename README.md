## Can You Hear Me Now

---

### Compile and run
```
cd "<REPO_PATH>/can_you_hear_me_now"
base='do_you_see_what_i_see' && g++ -std=c++14 "${base}.cpp" -o "${base}.out" && "./${base}.out"
```

---

### Note 1
When adapting code to send and receive datagrams in udp/ip, we
must not use `listen()` and `accept()` functions as we no longer
are relying on an established connection. Alongside that,
we now use `sendto()` and `recvfrom()` instead of `send()` and `recv()`.
Since udp is not reliable for delivery or error checking. We can
remedy most of this by using methods like Retransmission, flow control,
congestion control, and buffering.

---

### Note 2
To adapt the code to handle multiple TCP/IP clients without threads in C++ using only the standard library, we can use the `select()` function to manage multiple connections in a single thread.

In this implementation, we use the `select()` function to monitor multiple file descriptors (sockets) for input or output readiness. We can use `FD_SET` to add the server socket to the file descriptor set, and we add new client sockets to the set when they connect. When there is client events/activity, `select()` waits for this and processes it.
