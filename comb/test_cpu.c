#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <sys/time.h>

int main(int argc, char **argv) {
    
    int fd, result;
    char i2c_device[]="/dev/i2c-1";
    unsigned char buffer[2];
    int amp, freq;
    
    //Open i2c device
    fd = open(i2c_device, O_RDWR);
    if(fd < 0)  fprintf(stderr, "\Error! Can not open i2c!\n");
    
    //Set slave address
    result = ioctl(fd, I2C_SLAVE, 0x77);
    if(result < 0)  fprintf(stderr, "\Error! Can not set slave!\n");
    
    //Write data to sub slave address
    /*freq = 60;
    buffer[0] = 0x00; //Reg 0 frequency
    buffer[1] = frequency;   //frequency 60
    if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to set frequency!\n");
    
    amp = 1491;       // 100%
    buffer[0] = 0x01; //Reg 1 Amp buff 1
    buffer[1] = (amp >> 8) & 0x00ff;   //frequency 60
    if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to set Amp buff 1!\n");
    
    buffer[0] = 0x02; //Reg 1 Amp buff 2
    buffer[1] = amp & 0x00ff;   //frequency 60
    if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to set Amp buff 2!\n");*/
    
    while(1){
        printf("Input frequency = ");
        scanf("%d", &freq);
        printf("Input amplitude = ");
        scanf("%d", &amp);
        buffer[0] = 0x00; //Reg 0 frequency
        buffer[1] = frequency;   //frequency 60
        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to set frequency!\n");
        
        buffer[0] = 0x01; //Reg 1 Amp buff 1
        buffer[1] = (amp >> 8) & 0x00ff;   //frequency 60
        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to set Amp buff 1!\n");
        
        buffer[0] = 0x02; //Reg 1 Amp buff 2
        buffer[1] = amp & 0x00ff;   //frequency 60
        if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to set Amp buff 2!\n");
    }
    
    close(fd);
}