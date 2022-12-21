#include "../lib/util.h"
#include <stdbool.h>

unsigned int THRESHOLD, T_MONITOR;
unsigned char *addr0, *addr1;

int getNextBit() __attribute__((always_inline));

void delay(float delay_ms){
  clock_t current_time = clock();
  clock_t  end_time= current_time + delay_ms*1000;
  while(clock()<end_time){}
}

void findCacheHitMissThreshold(){

  unsigned long long sum_access_time_cache = 0, sum_access_time_no_cache = 0;
  unsigned int access_time, access_time_cache, access_time_no_cache, n = 0;

  access_time = measure_one_block_access_time(addr1);
  delay(10);
  clflush(addr1);

  while(n<4000){
    if(n%4==0){
      clflush(addr1);
      access_time = measure_one_block_access_time(addr1);
      sum_access_time_no_cache = sum_access_time_no_cache + access_time;
    }else{
      access_time = measure_one_block_access_time(addr1);
      sum_access_time_cache = sum_access_time_cache + access_time;
    }
    n++;
  }
  clflush(addr1);

  access_time_cache = sum_access_time_cache/3000;
  access_time_no_cache = sum_access_time_no_cache/1000;
  printf("[Receiver] Access time before clflush %u\n", access_time_cache);
  printf("[Receiver] Access time after clflush %u\n", access_time_no_cache);

  THRESHOLD =  access_time_cache + (access_time_no_cache - access_time_cache)/3;
  printf("[Receiver] THRESHOLD: %d\n", THRESHOLD);

}

void findAvgBetweenConsecutiveBits(){
  unsigned long long no_access_count = 0 ,sum = 0;
  unsigned int avg = 0, n = 0, access_time;
  bool flag1 = false, flag0 = false;

  printf("[Receiver] Start Sender Now\n");

  while(n<100){
    access_time = measure_one_block_access_time(addr1);
    if(access_time<THRESHOLD){
      if(flag1){
        sum = sum + no_access_count;
        n++;
        flag1 = false;
      }
      no_access_count = 0;
      flag0 = true;
    }
    clflush(addr1);

    access_time = measure_one_block_access_time(addr0);
    if(access_time<THRESHOLD){
      if(flag0){
        sum = sum + no_access_count;
        n++;
        flag0 = false;
      }
      no_access_count = 0;
      flag1 = true;
    }
    clflush(addr0);

    no_access_count++;
  }
  avg = sum/100;
  printf("[Receiver] Average Time Between Consecutive Bits: %u\n",avg);

  T_MONITOR = (2*avg)/3;
}

inline int getNextBit(){
  unsigned int access_time, count0 = 0, count1 = 0;
  unsigned long long no_access_count = 0 ;

  while(1){

    access_time = measure_one_block_access_time(addr1);
    if (access_time < THRESHOLD) {
      count1++;
      no_access_count = 0;
    }
    clflush(addr1);

    access_time = measure_one_block_access_time(addr0);
    if (access_time < THRESHOLD) {
      count0++;
      no_access_count = 0;
    }
    clflush(addr0);

    if(no_access_count > T_MONITOR){
      if(count1>count0){
        return 1;
      }else{
        return 0;
      }
    }

    no_access_count++;
  }
}

void waitForStartOfFrame(){
  bool dlim = false, start = false;
  int prev_bit = 0, bit = 0;

  while(1){
    prev_bit = bit;
    bit = getNextBit();

    if(prev_bit == 1 && bit == 0){
      dlim = true;
    }
    if(dlim && prev_bit == 1 && bit == 1){
      return;
    }
  }
}

int main(int argc, char **argv) {

  unsigned int msg_len = 0;
  clock_t t_recv;
  double recv_time;
  double recv_rate;

  char *filename = "share_mem.txt";

  map_handle_t *handle;     //declaring a handle for file mapping
  unsigned char *addr;

  addr = (char *) map_file(filename, &handle);

  addr1 = addr + 0x100;
  addr0 = addr + 0x1000;

  printf("[Receiver] Starting Receiver\n");

  printf("[Receiver] Calibration Started\n");
  findCacheHitMissThreshold();
  findAvgBetweenConsecutiveBits();
  printf("[Receiver] Calibration Done\n");

  printf("[Receiver] Waiting for data from sender\n");
  waitForStartOfFrame();

  t_recv = clock();

//Receive length of message
  unsigned int length = 0, bit = 0;
  for(int j = 15; j>=0; j--){
    bit = getNextBit();
    length = length | bit << j;
  }
  printf("[Receiver] Receiving message of length %u bits\n", length );

  char str[length+1];
  str[length] = '\0';

  for(int i = 0; i<length; i++){
/*
*Syncronizing sender and receiver every 40 bits or 5 characters.
*This will avoid burst error.
*/
    if(i%40 == 0){
      waitForStartOfFrame();
    }

    bit = getNextBit();
    str[i] = bit?  '1': '0';
  }

  char *msg = binary_to_string(str);

  t_recv = clock() - t_recv;
  msg_len = strlen(msg);
  recv_time = ((double) t_recv) / CLOCKS_PER_SEC;
  recv_rate = (double) (msg_len * 8) / recv_time;

  printf("\n[Receiver] Received data : %s\n", msg);
  printf("[Receiver] Total data received : %u bytes\n", msg_len);
  printf("[Receiver] Time taken to receive data : %lf second\n", recv_time);
  printf("[Receiver] Data reception rate : %lu bps\n", (unsigned long) recv_rate);

  unmap_file(handle);
  return 0;
}
