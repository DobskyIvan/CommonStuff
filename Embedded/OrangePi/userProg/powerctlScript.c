
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

#define PWR_BUTTON_DEV "/dev/input/by-path/platform-7081400.i2c-platform-axp221-pek-event"
#define KEY_READ
#define GET_INPUT_EVENT

#ifndef test_bit
	#define TEST_BIT(bit, array) (array[bit / 8] & (1 << (bit % 8)))
#endif /*test_bit*/

int main(void){
	printf("From powerctlScript: Custom power manager start here\n");
	int pwrButtonEvent_fd, tmp;
	pwrButtonEvent_fd=open(PWR_BUTTON_DEV, O_RDONLY);
	if(pwrButtonEvent_fd<0){
		printf("From powerctlScript: Unable to open %s: %s\n", PWR_BUTTON_DEV, strerror(errno));
		close(pwrButtonEvent_fd);
		return 1;
	}
	#ifdef KEY_READ
	uint8_t key_b[KEY_MAX/8+1]; //KEY_MAX from linux/input.h
	memset(key_b, 0, sizeof(key_b));
	tmp=ioctl(pwrButtonEvent_fd, EVIOCGKEY(sizeof(key_b)), key_b);
	if(tmp==-1){
		printf("From powerctlScript: Unable to read keys map %s\n", strerror(errno));
		close(pwrButtonEvent_fd);
		return 1;
	}
	if(TEST_BIT(116, key_b)){
		printf("From powerctlScript: KEY_POWER- button pressed, system start shuting down\n");
		//DO SOME ACTION
		system("shutdown now");
	}
	else printf("From powerctlScript: KEY_POWER- button not pressed, system start waiting some event...\n");
	#endif /*KEY_READ*/

	#ifdef GET_INPUT_EVENT
	struct input_event ie; //from linux/input.
	#endif /*GET_INPUT_EVENT*/

//	while(1){ //main loop
		#ifdef GET_INPUT_EVENT
		tmp=read(pwrButtonEvent_fd, &ie, sizeof(struct input_event)); //blocking!!!
		if(tmp==-1){
			printf("From powerctlScript: Unable to read device input event fd %s\n", strerror(errno));
			close(pwrButtonEvent_fd);
			return 1;
		}
		if(ie.type == EV_KEY && ie.code == 116){
			printf("Button toggle event caught with value %d\n", ie.value);
			system("shutdown now");
		}
		#endif /*GET_INPUT_EVENT*/
//	}
	printf("From powerctlScript: DONE!\n");
	close(pwrButtonEvent_fd);
	return 0;
}
