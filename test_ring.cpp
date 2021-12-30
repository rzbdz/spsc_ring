
#include <cstdio>
#include <vector>
#include <thread>
#include "spsc_ring.hpp"

constexpr int pdsz = 256;
int singlethread(void) {
  struct hh_t {
    int log_id;
    char data;
  };
  coring::spsc_ring<hh_t> my_ring(pdsz * 2);
  std::vector<hh_t> produce_queue(pdsz);
  for (int i = 0; i < pdsz; i++) {
    produce_queue[i].log_id = i;
    produce_queue[i].data = ('a' + i) & 0x7f;
  }
  std::vector<hh_t> consume_queue;
  for (int i = 0; i < pdsz; ++i) {
    assert(my_ring.try_push(produce_queue[i]));
  }
  hh_t* dt = my_ring.data();
  int sz = my_ring.size();
  for (int i = 0; i < pdsz; i++) {
    auto p = my_ring.front();
    consume_queue.emplace_back(*p);
    my_ring.pop();
  }
  assert(consume_queue.size() == pdsz);
  for (int i = 0; i < pdsz; i++) {
    assert(consume_queue[i].log_id == i);
    assert(consume_queue[i].data == (('a' + i) & 0x7f));
  }
  assert(my_ring.size() == 0);
  return 0;
}
int multithread(void) {
  struct hh_t {
    int log_id;
    char data;
  };
  coring::spsc_ring<hh_t> my_ring(pdsz * 2);
  std::vector<hh_t> produce_queue(pdsz);
  for (int i = 0; i < pdsz; i++) {
    produce_queue[i].log_id = i;
    produce_queue[i].data = ('a' + i) & 0x7f;
  }
  std::vector<hh_t> consume_queue;
  std::jthread producer([&]() {
    for (int i = 0; i < pdsz; ++i) {
      while (__glibc_unlikely(!my_ring.try_push(produce_queue[i])))
        ;
      std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
  });
  std::jthread consumer([&]() {
    for (int i = 0; i < pdsz; i++) {
      auto p = my_ring.front();
      while (p == nullptr) {
        // LOG_DEBUG("fuck loop, consumer\n");
        p = my_ring.front();
      }
      consume_queue.emplace_back(*p);
      my_ring.pop();
    }
  });
  producer.join();
  consumer.join();
  assert(consume_queue.size() == pdsz);
  for (int i = 0; i < pdsz; i++) {
    assert(consume_queue[i].log_id == i);
    assert(consume_queue[i].data == (('a' + i) & 0x7f));
  }
  assert(my_ring.size() == 0);
  return 0;
}
int main(void) {
  singlethread();
  multithread();
  printf("All test passed\n");
  return 0;
}