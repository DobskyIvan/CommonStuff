
#include <linux/input.h>

#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//#define SOME_DEV "/dev/input/event3"
#define SOME_DEV "/dev/input/by-path/platform-7081400.i2c-platform-axp221-pek-event"
#define GET_DEV_VER
#define GET_DEV_INFO
#define KEY_READ
#define GET_INPUT_EVENT

//#define MACROS_TST

#ifndef test_bit
	#define TEST_BIT(bit, array) (array[bit / 8] & (1 << (bit % 8)))
#endif /*test_bit*/

int main(void){
	int fd, tmp;
	#ifdef MACROS_TST
	uint8_t arr[]={0xDE, 0xAD};
	for(int i = 0; i < 16; i++){
		printf("Bit%d: %d\n", i, TEST_BIT(i, arr) ? 1 : 0);
	}
	#endif /*MACROS_TST*/
	fd=open(SOME_DEV, O_RDONLY);
	printf("Here some info about '%s':\n", SOME_DEV);
	if(fd<0){
		printf("Unable to open %s: %s\n", SOME_DEV, strerror(errno));
		close(fd);
		return 1;
	}
	#ifdef GET_DEV_VER
	int32_t version;
	tmp=ioctl(fd, EVIOCGVERSION, &version);
	if(tmp==-1){
		printf("Unable to get driver version %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	printf("evdev %s driver version is %d.%d.%d\n", SOME_DEV,
		version >> 16, (version >> 8) & 0xff, version & 0xff);
	#endif /*GET_DEV_VER*/
	#ifdef GET_DEV_INFO
	struct input_id device_info; //from linux/input.h
	tmp=ioctl(fd, EVIOCGID, &device_info);
	if(tmp==-1){
		printf("Unable to get device info %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	printf("vendor: %04hx, product %04hx, version %04hx, bustype %04hx\n",
		device_info.vendor, device_info.product, device_info.version,
		device_info.bustype);
	#endif /*GET_DEV_VER*/
	#ifdef KEY_READ
	uint8_t key_b[KEY_MAX/8+1]; //KEY_MAX from linux/input.h
	uint8_t keyWasPressedFlag = 0;
	memset(key_b, 0, sizeof(key_b));
	tmp=ioctl(fd, EVIOCGKEY(sizeof(key_b)), key_b);
	if(tmp==-1){
		printf("Unable to read keys %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	printf("Pressed keys(state check, nonblocking):\n");
	for(int i = 0; i < KEY_MAX; i++){
		if(TEST_BIT(i, key_b)){
			printf("Key 0x%02x, ", i);
			keyWasPressedFlag = 1;
			switch(i){
				case 116:
					printf("KEY_POWER\n");
					break;
				default:
					printf("Unknown\n");
					break;
			}
		}
	}
	if(!keyWasPressedFlag)printf("No key pressed\n");
	#endif /*KEY_READ*/
	#ifdef GET_INPUT_EVENT
	struct input_event ie; //from linux/input.h
	fd_set set; //from sys/select.h
	struct timeval timeout; //from sys/select.h
	FD_ZERO(&set); //clear the set, macros from sys/select.h
	FD_SET(fd, &set); //add filedescriptor to the set, --//--
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	printf("Waiting for event(blocking), press key to close session or expect timeout ~10s\n");
	tmp=select(fd+1, &set, NULL, NULL, &timeout);
	if(tmp==-1){
		printf("Unable to select input event fd set %s\n", strerror(errno));
		close(fd);
		return 1;
	}
	else if(tmp==0){
		printf("Timeout from select input event fd set\n");
	}
	else{
		tmp=read(fd, &ie, sizeof(struct input_event));
		if(tmp==-1){
			printf("Unable to read device input event fd %s\n", strerror(errno));
			close(fd);
			return 1;
		}
		printf("Event caught: time %ld.%06ld, type %d, code %d, value %d\n",
			ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);
	}
	#endif /*GET_INPUT_EVENT*/
	printf("From programm: DONE!\n");
	//printf("From programm: LOOP!\n");
	close(fd);
	//while(1);
	return 0;
}
