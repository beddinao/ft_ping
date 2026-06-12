# *`ft_ping`*

`42 cursus compliant`

minimal command-line tool to test network connectivity by sending ICMP echo requests ("pings") to a specified host and displaying response times, similar to the standard `ping` utility.

## usage

```
Usage
  ft_ping [options] <destination>

Options:
  <destination>             dns name or ip address
  -c --count <count>        stop after <count> replies
                              0xffffffff > <count> >= 0
                              default: infinite
  -i --interval <interval>  time between sending each packet in seconds
                              0xffffffff > <interval> >= 0
                              default: 1
  -W --timeout <timeout>    time to wait for response in milliseconds
                              0xffffffff > <timeout> >= 0
                              default: 10000 -> 10s
  -Q --tos <tos>            set Quality Of Serivce bits
                              0xff <= <tos> >= 0
                              default: 0
  -t --ttl <ttl>            set Time To Live <ttl> bits
                              0xff <= <ttl> >= 0
                              default: 64
  -f --flood                flood ping
  -n --numeric              no dns name resolution
  -v --verbose              verbose output
  -V --version              prints version and exits
  -? --help                 shows this help and exits

```

## example output

```
$ ft_ping 1337.ma
FT_PING 1337.ma (104.18.32.248)
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=0 ttl=116 time=20.142 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=1 ttl=116 time=20.044 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=2 ttl=116 time=75.906 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=3 ttl=116 time=32.949 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=4 ttl=116 time=20.244 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=5 ttl=116 time=20.095 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=6 ttl=116 time=20.431 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=7 ttl=116 time=20.278 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=8 ttl=116 time=20.611 ms
64 bytes from 104.18.32.248 (104.18.32.248): icmp_seq=9 ttl=116 time=20.158 ms
^C
quiting...
=== 104.18.32.248 stats ===
10 packets transmitted, 10 received, 0 lost, 0.00% packet loss, time: 10985.542 ms
```

## requirements

- Linux
- Root privileges (required for sending raw ICMP packets)
- A C compiler and `make`
