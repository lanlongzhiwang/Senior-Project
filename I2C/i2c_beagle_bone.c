#include <stdio.h>  
#include <stdlib.h>   //exit()  
#include <fcntl.h>    //define O_RDWR  
#include <linux/i2c-dev.h>  
#include <sys/ioctl.h>  
  
void main()  
{  
        int file,i;  
        int addr = ?????????; //i2c device address of dispic
        char *filename = "/dev/i2c-1";  
        char wbuf[] = {0x0200, ???}; //first byte is address to write. others are bytes to be written  
        char read_start_buf[1];   
        char rbuf[1000] = {0};  
  
        if((file = open(filename, O_RDWR)) < 0)  
        {  
                perror("Failed to open i2c device.\n");  
                exit(1);  
        }  
  
        if(ioctl(file, I2C_SLAVE, addr) < 0)  
        {  
                printf("Failed to access bus.\n");  
                exit(1);  
        }  
  
        write(file, wbuf, 2); //write somenumber to register 0x20.   
  
        for(i=0;i<6;i++)  
        {  
                read_start_buf[0] = 0x0202;   
                write(file, read_start_buf, 1); //reposition file pointer to register 0x0202  
                read(file, rbuf, 1); //read register 0x0202 and save to rbuf  
                printf("%x", rbuf[0]);  
        }  
  
        close(file);  
}  