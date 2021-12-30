# spsc_ring

 a single producer single customer lock-free (atomic) queue, initially write for a async logging module

 lock-free single producer single consumer ring buffer (bounded)

 It's simple to implement a lock free SPSC ring buffer for there won't be any contention.
 But there are some details need to be handled to implement correctly and with high efficiency.
 
 NOTICE: This implementation is modeled after **Intel DPDK::rte_ring** and **Linux kernel::kfifo** 

 design documentation of DPDK::rte_ring:
   - https://doc.dpdk.org/guides-17.11/prog_guide/ring_lib.html#use-cases
 related source code :
   - http://code.dpdk.org/dpdk/v19.11/source/lib/librte_ring/rte_ring.c
   - https://elixir.bootlin.com/linux/latest/source/include/linux/kfifo.h
 use alignas to prevent performance strike caused by cache line false sharing
   -
   https://www.intel.com/content/www/us/en/develop/documentation/vtune-cookbook/top/tuning-recipes/false-sharing.html
   - false sharing solution learned from github.com/rigtorp/SPSCQueue and github.com/MengRao/SPSC_Queue

 feature:
  - use malloc and in-place constructor, free + in-place destructor to support non-POD types,
  - emplace
  - support batch dequeue
 
 further improvement: use atomic instead of volatile + memory fence
