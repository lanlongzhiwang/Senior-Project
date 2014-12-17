#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

//Read value from ADC0 for accelerator
double read_accelerator(){
    
    double voltage;
	double result;
	FILE *fd = NULL;

	//open the ADC0 device file
	fd = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw","r");
	if(fd == NULL){
		printf("open the hum1 file is error!\n");
     	exit(0);
    }
    
    fscanf(fd,"%[^\n]%*c", string); // Scanf the first line
    voltage = atoi(string);         // Convert from ASCII to int
    voltage = voltage/4095*1.8;     // 12-bit ADC and max voltage is 1.8 voltage
    result = voltage/1.8*30;        // Max speed is 30 mph
    
    fclose(fd);

	return result;  
    
}
//Read value from ADC1 for brake
double read_break(){
    
	double result;
	FILE *fd = NULL;

	//open the humidity sensor device file
	fd = fopen("/sys/bus/iio/devices/iio:device0/in_voltage1_raw","r");
	if(fd == NULL){
		printf("open the hum2 file is error!\n");
     	exit(0);
    }
    
    fscanf(fd,"%[^\n]%*c", string); //scanf the first line
    voltage = atoi(string);         // Convert from ASCII to int
    voltage = voltage/4095*1.8;     // 12-bit ADC and max voltage is 1.8 voltage
    result = voltage/1.8*30;        // Max speed is 30 mph
    
    fclose(fd);

	return result;  
    
}
//Write data to slave through I2C
void write_i2c(int slave_address, int sub_slave_address, int data){
    
    int fd, result;
    unsigned char buffer[2];
    
    //Open i2c device
	fd = open(i2c_device, O_RDWR);
	if(fd < 0)  fprintf(stderr, "\Error! Can not open i2c!\n");
    
    //Set slave address
    result = ioctl(fd, I2C_SLAVE, slave_address);
	if(result < 0)  fprintf(stderr, "\Error! Can not set slave!\n");
	
	//Write data to sub slave address
	buffer[0] = sub_slave_address;
    buffer[1] = data;
    if ( (write(fd, buffer, 2)) != 2) fprintf(stderr,"Error to writing!\n");
    
    close(fd);
    
}
//Read data from slave through I2C
int write_i2c(int slave_address, int sub_slave_address){
    
    int fd, result;
    unsigned char buffer[1];
    unsigned char rbuffer[1];
    
    //Open i2c device
	fd = open(i2c_device, O_RDWR);
	if(fd < 0)  fprintf(stderr, "\Error! Can not open i2c!\n");
    
    //Set slave address
    result = ioctl(fd, I2C_SLAVE, slave_address);
	if(result < 0)  fprintf(stderr, "\Error! Can not set slave!\n");
	
	//Read data from sub slave address
	buffer[0] = sub_slave_address;
    if ( (write(fd, rbuffer, 1)) != 1) fprintf(stderr,"Error to writing!\n");
    if ( (read(fd, rbuffer, 1)) != 1) fprintf(stderr,"Error to reading!\n");
    
    close(fd);
    
    return rbuffer[0];
    
}

int main(int argc, char **argv){

	return 0;
}
