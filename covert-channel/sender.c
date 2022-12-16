#include "../lib/util.h"
#include <time.h>

unsigned int INTER_BIT_DELAY = 10;
float INTER_ACCESS_DELAY = 0.001;
unsigned int LOOPCOUNT = 1000;

void delay(float delay_ms){
  clock_t current_time = clock();
  clock_t  end_time= current_time + delay_ms*1000;
  while(clock()<end_time){}
}

void startOfFrame(unsigned char *addr1, unsigned char *addr0){
  int a[8] = {1,0,1,0,1,0,1,1};
  char c;
  for(int i = 0; i<8; i++){
    if(a[i]){
      for(int i = 0; i<LOOPCOUNT; i++){
        c = *addr1;
        delay(INTER_ACCESS_DELAY);
      }
    }else{
      for(int i = 0; i<LOOPCOUNT; i++){
        c = *addr0;
        delay(INTER_ACCESS_DELAY);
      }
    }
    delay(INTER_BIT_DELAY);
  }
}

void syncSendRecv(unsigned char *addr1, unsigned char *addr0){
  char c;
  unsigned int n = 200;
  while(n>0){
      if(n%2){
        for(int i = 0; i<LOOPCOUNT; i++){
          c = *addr1;
          delay(INTER_ACCESS_DELAY);
        }
      }else{
        for(int i = 0; i<LOOPCOUNT; i++){
          c = *addr0;
          delay(INTER_ACCESS_DELAY);
        }
      }
      n--;
      delay(INTER_BIT_DELAY);
  }
}

void transmitLength(unsigned int msglength, unsigned char *addr1, unsigned char *addr0){
  char c;
  for(int i = 15; i>=0; i--){
    if(msglength>>i & 1){
      for(int i = 0; i<LOOPCOUNT; i++){
        c = *addr1;
        delay(INTER_ACCESS_DELAY);
      }
    }else{
      for(int i = 0; i<LOOPCOUNT; i++){
        c = *addr0;
        delay(INTER_ACCESS_DELAY);
      }
    }
    delay(INTER_BIT_DELAY);
  }
}

int main(int argc, char **argv) {

	char msg[51];
	unsigned int msg_len = 0;
	clock_t t_send;
	double trans_time;
	double trans_rate;

  for(int i = 0; i<52; i++){
    msg[i] = '\0';
  }

	//Establish your cache covert channel
  char *filename = "share_mem.txt";

  map_handle_t *handle;     // declaring a handle for file mapping
  unsigned char *addr;
  addr = (char *) map_file(filename, &handle);

  unsigned char *addr1 = addr + 0x100;
  unsigned char *addr0 = addr + 0x1000;

  syncSendRecv(addr1, addr0);

	printf("Enter message to be send: ");
	fgets (msg, 51, stdin);

	char *binary = string_to_binary(msg);
  size_t len = strlen(binary);
  msg_len = strlen(msg);

  printf("[Sender] Sending message of length %lu bits\n", len);

	t_send = clock();

  // Transmit message over the cache covert channel

  startOfFrame(addr1, addr0);
  transmitLength(len, addr1, addr0);

  char c;
	for(int i = 0; i<len; i++){

    if(i%40 == 0){
      startOfFrame(addr1, addr0);
    }

		if(binary[i] == '1'){
      for(int i = 0; i<LOOPCOUNT; i++){
        c = *addr1;
        delay(INTER_ACCESS_DELAY);
      }
		}else{
      for(int i = 0; i<LOOPCOUNT; i++){
        c = *addr0;
        delay(INTER_ACCESS_DELAY);
      }
		}
		delay(INTER_BIT_DELAY);
	}

	t_send = clock() - t_send;
	trans_time = ((double) t_send) / CLOCKS_PER_SEC;
	trans_rate = (double) (msg_len * 8) / trans_time;

  printf("[Sender] Data Sent : %s \n", msg);
	printf("[Sender] Total data Send : %u bytes\n", msg_len);
	printf("[Sender] Time to send data : %lf second\n", trans_time);
	printf("[Sender] Data transmission rate : %lu bps\n", (unsigned long) trans_rate);

  unmap_file(handle);
	return 0;
}
