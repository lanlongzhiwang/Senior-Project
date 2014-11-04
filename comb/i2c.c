#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

int main(int argc, char **argv) {

	int fd, result, i;

	char i2c_device[]="/dev/i2c-1";

	unsigned char buffer[2];

	/* Open i2c device */
	fd = open(i2c_device, O_RDWR);
	if(fd < 0)  fprintf(stderr, "\Error! Can not open i2c!\n");

// YOUR CODE HERE

	/* Set slave address */
	result = ioctl(fd, I2C_SLAVE, 0x77);
	if(result < 0)  fprintf(stderr, "\Error! Can not set slave!\n");

	while(1){

        printf("1. Vlotage\n2. Frequency\nWhich one you want change: ");
        scanf("%d",&i);
        if(i == 1){
            buffer[0] = 0x09;
    	    printf("Please input the Voltage(24 - 60): ");
    	    scanf("%d",&buffer[1]);
    	    if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writting!\n");
    	    printf("\n");
        }
        else if(i == 2){
            buffer[0] = 0x07;
    	    printf("Please input the frequency(10 - 60): ");
    	    scanf("%d",&buffer[1]);
    	    if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writting!\n");
    	    printf("\n");
        }

	}

// YOUR CODE HERE

	/* Close device */
	close(fd);

// YOUR CODE HERE

	return 0;
}
