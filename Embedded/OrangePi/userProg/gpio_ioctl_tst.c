#include <linux/gpio.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <time.h>

#define DEV_NAME "/dev/gpiochip0"
//#define GET_INFO_GPIO
//#define PRINT_CHIPINFO
//#define PRINT_LINEINFO
#define SET_OUTPUT
//#define GET_INPUT

void delay_ms(int ms);

int main(void){
	int fd, ret;
	fd=open(DEV_NAME, O_RDONLY);
	if (fd < 0){
		printf("Unable to open %s: %s\n", DEV_NAME, strerror(errno));
		close(fd);
		return 1;
	}
/*Get some info about DEV_NAME*/
	#ifdef GET_INFO_GPIO
		struct gpiochip_info info; //from gpio.h
		struct gpioline_info line_info; //from gpio.h
		ret=ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info);
		if (ret == -1){
			printf("Unable to get chip info from ioctl: %s\n", strerror(errno));
			close(fd);
			return 1;
		}
		#ifdef PRINT_CHIPINFO
			printf("Chip name: %s\n", info.name);
			printf("Chip label: %s\n", info.label);
			printf("Number of lines: %d\n", info.lines);
		#endif /*PRINT_CHIPINFO*/
		for(unsigned int i = 0; i < info.lines; i++){
			line_info.line_offset = i;
			ret=ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &line_info);
			if (ret == -1){
				printf("Unable to get line info from offset %d, %s\n", i, strerror(errno));
				close(fd);
				return 1;
			}
			#ifdef PRINT_LINEINFO
				//if (i > 0 && i%50 == 0){
					//printf("press some to continue..");
					//getchar();
				//}
				printf("offset: %d, name: %s, consumer: %s. Flags:\t[%s]\t[%s]\t[%s]\t[%s]\t[%s]\n",
				i,
				line_info.name,
				line_info.consumer,
				(line_info.flags & GPIOLINE_FLAG_IS_OUT) ? "OUTPUT" : "INPUT",
				(line_info.flags & GPIOLINE_FLAG_ACTIVE_LOW) ? "ACTIVE_LOW" : "ACTIVE_HIGHT",
				(line_info.flags & GPIOLINE_FLAG_OPEN_DRAIN) ? "OPEN_DRAIN" : "...",
				(line_info.flags & GPIOLINE_FLAG_OPEN_SOURCE) ? "OPENSOURCE" : "...",
				(line_info.flags & GPIOLINE_FLAG_KERNEL) ? "KERNEL" : "...");
			#endif /*PRINT_LINEINFO*/
		}
	#endif/*GET_INFO_GPIO */

	#ifdef GET_INPUT
		struct gpiohandle_request rq;
		rq.lineoffsets[0]=4; //what line
		//rq.flags = GPIOHANDLE_REQUEST_INPUT;
		rq.lines = 1; //how much lines
		ret=ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);
		if (ret == -1){
			printf("Unable to line handle from ioctl: %s\n", strerror(errno));
			return 1;
		}
		close(fd);
		struct gpiohandle_data data;
		ret=ioctl(rq.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
		if (ret == -1){
			printf("Unable to set line values using ioctl: %s\n", strerror(errno));
			close(rq.fd);
			return 1;
		}
		else{
			printf("Value of GPIO at offset 4: %d\n", data.values[0]);
		}
		close(rq.fd);
	#endif /*GET_INPUT*/

/*OrPI 3 LTS pin offset:4 is green_led seted to output, let him blink... */
	#ifdef SET_OUTPUT
		struct gpiohandle_request rq;
		rq.lineoffsets[0]=4; //what line
		rq.lines = 1; //how much lines
		rq.flags = GPIOHANDLE_REQUEST_OUTPUT;
		ret=ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);
		if (ret == -1){
			printf("Unable to line handle from ioctl: %s\n", strerror(errno));
			return 1;
		}
		close(fd);
		struct gpiohandle_data data;
		//data.values[0] = 0; //reset
		data.values[0] = 1; //set
		ret=ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
		if (ret == -1){
			printf("Unable to set line values using ioctl: %s\n", strerror(errno));
			close(rq.fd);
			return 1;
		}
		close(rq.fd);
	#endif /*SET_OUTPUT*/


	close(fd);
	printf("Done!\n");
	return 0;
}

void delay_ms(int ms){
	clock_t start_time = clock();
	while(clock() < start_time + ms);
}
