#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

int main(int argc, char **argv) {

	int fd, result, i, voltage, inputv;

	char i2c_device[]="/dev/i2c-1";

	unsigned char buffer[2];
	unsigned char rbuffer[1];

    voltage = 50;
	/* Open i2c device */
	fd = open(i2c_device, O_RDWR);
	if(fd < 0)  fprintf(stderr, "\Error! Can not open i2c!\n");

// YOUR CODE HERE

	/* Set slave address */
	result = ioctl(fd, I2C_SLAVE, 0x77);
	if(result < 0)  fprintf(stderr, "\Error! Can not set slave!\n");

	while(1){

        printf("1. Voltage\n2. Frequency\nWhich one do you want to change: ");
        scanf("%d",&i);
        if(i == 1){
            
            //printf("The current Voltage is: %d Volt\n", voltage);
    	    printf("Please input the Voltage(24 - 60): ");
    	    scanf("%d",&inputv);
    	    if(inputv == 24){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 104;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 25){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 100;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 26){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 95;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 27){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 95;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 28){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 90;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 29){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 85;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    
    	    else if(inputv == 30){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 85;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 31){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 52;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 32){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 50;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 33){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 48;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 34){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 43;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 35){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 42;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 36){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 41;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 37){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 39;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 38){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 36;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 39){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 32;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 40){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 30;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 41){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 29;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 42){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 27;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 43){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 25;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 44){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 23;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 45){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 23;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 46){
    	        buffer[0] = 10;
    	        buffer[1] = 22;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 10;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 47){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 20;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 48){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 19;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 49){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 18;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 50){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 15;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 51){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 13;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 52){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 13;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 53){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 10;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 54){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 7;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 55){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 7;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 56){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 0;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 57){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 0;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 58){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 59){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
    	    else if(inputv == 60){
    	        buffer[0] = 10;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x09;
    	        buffer[1] = 2;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        buffer[0] = 0x05;
    	        buffer[1] = 3;
    	        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	        printf("\n");
    	    }
        }
        else if(i == 2){
            buffer[0] = 0x07;
            if ( (write(fd, buffer, 1)) != 1) fprintf(stderr,"Error to writing!\n");
            if ( (read(fd, rbuffer, 1)) != 1) fprintf(stderr,"Error to reading!\n");
            printf("The current Frequency is: %d Hz\n", rbuffer[0]);
    	    printf("Please input the frequency(10 - 60): ");
    	    scanf("%d",&buffer[1]);
    	    if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    	    printf("\n");
        }

	}

// YOUR CODE HERE

	/* Close device */
	close(fd);

// YOUR CODE HERE

	return 0;
}
