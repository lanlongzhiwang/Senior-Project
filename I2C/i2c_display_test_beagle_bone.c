#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

int main(int argc, char **argv) {

	int fd, result, i;

	char i2c_device[]="/dev/i2c-1";

	unsigned char buffer[17];

	/* Open i2c device */
	fd = open(i2c_device, O_RDWR);
	if(fd < 0)  fprintf(stderr, "\Error! Can not open i2c!\n");

// YOUR CODE HERE

	/* Set slave address */
	result = ioctl(fd, I2C_SLAVE, 0x70);
	if(result < 0)  fprintf(stderr, "\Error! Can not set slave!\n");

// YOUR CODE HERE

	/* Turn on oscillator */
	buffer[0] = (0x2 << 4) | (0x1);
	if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error writting to System Setup Register!\n");

// YOUR CODE HERE

	/* Turn on Display, No Blink */
	buffer[0] = 0x81;
	if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error writting to System Setup Register!\n");

// YOUR CODE HERE

	/* Set Brightness */
	buffer[0] = 0xE7;
	if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error to set brightness!\n");

// YOUR CODE HERE

	/* Clean all */
	for(i=0;i<17;i++){
	    buffer[i]=0x00;
	}
	
	//Display ECE471 but have it scroll left.
	while(1){
    	buffer[1] = 0x79;       //E
    	buffer[3] = 0x39;       //C
    	buffer[7] = 0x79;       //E
    	buffer[9] = 0x66;       //4
        write(fd,buffer,17);
        if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error writting!\n");
        
        usleep(300000);
        
        buffer[1] = 0x39;       //C
    	buffer[3] = 0x79;       //E
    	buffer[7] = 0x66;       //4
    	buffer[9] = 0x07;       //7
        write(fd,buffer,17);
        if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error writting!\n");
        
        usleep(300000);
        
        buffer[1] = 0x79;       //E
    	buffer[3] = 0x66;       //4
    	buffer[7] = 0x07;       //7
    	buffer[9] = 0x06;       //1
        write(fd,buffer,17);
        if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error writting!\n");
        
        usleep(300000);
        
        buffer[1] = 0x66;       //4
    	buffer[3] = 0x07;       //7
    	buffer[7] = 0x06;       //1
    	buffer[9] = 0x79;       //E
        write(fd,buffer,17);
        if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error writting!\n");
        
        usleep(300000);
        
        buffer[1] = 0x07;       //7
    	buffer[3] = 0x06;       //1
    	buffer[7] = 0x79;       //E
    	buffer[9] = 0x39;       //C
        write(fd,buffer,17);
        if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error writting!\n");
        
        usleep(300000);
        
        buffer[1] = 0x06;       //1
    	buffer[3] = 0x79;       //E
    	buffer[7] = 0x39;       //C
    	buffer[9] = 0x79;       //E
        write(fd,buffer,17);
        if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error writting!\n");
        
        usleep(300000);
	}

// YOUR CODE HERE

	/* Close device */
	close(fd);

// YOUR CODE HERE

	return 0;
}
