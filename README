DPDK is a set of libraries and drivers for fast packet processing.
It supports many processor architectures and both FreeBSD and Linux.

The DPDK uses the Open Source BSD-3-Clause license for the core libraries
and drivers. The kernel components are GPL-2.0 licensed.

Please check the doc directory for release notes,
API documentation, and sample application information.

For questions and usage discussions, subscribe to: users@dpdk.org
Report bugs and issues to the development mailing list: dev@dpdk.org


## DPDK testbed setup

### 1. Topology setup
- using VMWare workstation to setup a Ubuntu 22.04 VM
- host <--> VM communication is setup using VMWare's NAT network

### 2. DPDK setup in VM

#### Install dependencies
`sudo apt install build-essential meson ninja libnuma-dev glibc-source -y`

#### Download DPDK
`git clone https://github.com/DPDK/dpdk.git`

#### Build DPDK
```
cd dpdk
meson setup -Dexamples=all build
cd build
ninja
sudo ldconfig
```

#### Enable hugepages
```
mkdir -p /dev/hugepages
mountpoint -q /dev/hugepages || mount -t hugetlbfs nodev /dev/hugepages
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
```

#### Bind NIC to DPDK
```
sudo modprobe vfio-pci enable_sriov=1
echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode
sudo ip link set ens33 (link name) down
sudo dpdk-devbind.py -b vfio-pci 02:01.0 (PCI address from dpdk-devbind.py -s)
```

#### To bind NIC back to kernel (disable DPDK)
```
sudo dpdk-devbind.py -b e1000 02:01.0
```

### 3. Test traffic analysis task
#### Start traffic analysis tool

```
cd dpdk/examples/traffic_analysis
make clean
make
sudo ./build/basic_stats -l 0 -n 1 
```

##### Start traffic generator
on host machine using scapy
```
sendp(Ether()/IP(dst="1.2.3.4",ttl=(1,1))/TCP(sport=1,dport=87), iface="VMware Virtual Ethernet Adapter for VMnet8")
```

> stats will stored in hash_table.txt file

##### To enable logging
 - configure log level in `dpdk/examples/traffic_analysis/basic_stats.c`
 - default log level is 0 which doesn't log anything
 - log level 1 logs only the packet headers
 - log level 2 logs the packet tuples as they are being stored in the hash table
 - log level 3 logs the updates on the hash table as index are being expired with the timer

> more details about the program in local README file of basic stats

