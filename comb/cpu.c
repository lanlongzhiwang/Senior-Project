#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define frequency 0            // Define frequency register is 0
#define PWM1_voltage_h 1       // Define PWM1 voltage higher 8-bits register is 1
#define PWM1_voltage_l 2       // Define PWM1 voltage lowr 8-bits register is 2
#define PWM1_current_h 3       // Define PWM1 current higher 8-bits register is 3
#define PWM1_current_l 4       // Define PWM1 current lowr 8-bits register is 4
#define PWM2_voltage_h 5       // Define PWM2 voltage higher 8-bits register is 5
#define PWM2_voltage_l 6       // Define PWM2 voltage lowr 8-bits register is 6
#define PWM2_current_h 7       // Define PWM2 current higher 8-bits register is 7
#define PWM2_current_l 8       // Define PWM2 current lowr 8-bits register is 8
#define PWM3_voltage_h 9       // Define PWM3 voltage higher 8-bits register is 9
#define PWM3_voltage_l 10      // Define PWM3 voltage lowr 8-bits register is 10
#define PWM3_current_h 11      // Define PWM3 current higher 8-bits register is 11
#define PWM3_current_l 12      // Define PWM3 current lowr 8-bits register is 12
#define DC_voltage_h 13        // Define PWM2 voltage higher 8-bits register is 13
#define DC_voltage_l 14        // Define PWM2 voltage lowr 8-bits register is 14

//Read value from ADC0 for accelerator
double read_accelerator(){
    
    double voltage;
	double result;
	char string[50];
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
    
    double voltage;
	double result;
	char string[50];
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
    char i2c_device[]="/dev/i2c-1";
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
int read_i2c(int slave_address, int sub_slave_address){
    
    int fd, result;
    char i2c_device[]="/dev/i2c-1";
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
//Write data to a txt file for display read
void write_txt(double p1_v, double p1_c, double p2_v, double p2_c,
               double p3_v, double p3_c, double dc){
                   
    FILE* fp = fopen("data.txt","w");
    
    fprintf(fp, "The PWM1 RMS Voltage is %.2lf Volt\n\
                 The PWM1 RMS Current is %f Amp\n\
                 The PWM2 RMS Voltage is %.2lf Volt\n\
                 The PWM2 RMS Current is %f Amp\n\
                 The PWM3 RMS Voltage is %.2lf Volt\n\
                 The PWM3 RMS Current is %f Amp\n\
                 The DC Voltage is %.2lf Volt\n",\
                 p1_v, p1_c, p2_v, p2_c, p3_v, p3_c, dc);
    
    fclose(fp);
        
}

int main(int argc, char **argv){
    
    int i; 
    double PWM1_V_RMS, PWM1_C_RMS, 
           PWM2_V_RMS, PWM2_C_RMS,
           PWM3_V_RMS, PWM3_C_RMS,
           DC_V_RMS;
    double rate;
    
    //dspic30f5015 have 10-bit ADC, so max value is 1023
    rate = 5/1024;
    
    while(1){
        
        //Combine higher 8-bit and lower 8-bit then divide 4 and times 100, equal to times 25
        PWM1_V_RMS = read_i2c(dspic30f5015_1, PWM1_voltage_h) << 8;
        PWM1_V_RMS += read_i2c(dspic30f5015_1, PWM1_voltage_l);
        PWM1_V_RMS = rate * PWM1_V_RMS * 25;
        PWM1_C_RMS = read_i2c(dspic30f5015_1, PWM1_current_h) << 8;
        PWM1_C_RMS += read_i2c(dspic30f5015_1, PWM1_current_l);
        PWM1_C_RMS = rate * PWM1_C_RMS * 25;
        
        PWM2_V_RMS = read_i2c(dspic30f5015_1, PWM2_voltage_h) << 8;
        PWM2_V_RMS += read_i2c(dspic30f5015_1, PWM2_voltage_l);
        PWM2_V_RMS = rate * PWM2_V_RMS * 25;
        PWM2_C_RMS = read_i2c(dspic30f5015_1, PWM2_current_h) << 8;
        PWM2_C_RMS += read_i2c(dspic30f5015_1, PWM2_current_l);
        PWM2_C_RMS = rate * PWM2_C_RMS * 25;
        
        PWM3_V_RMS = read_i2c(dspic30f5015_1, PWM3_voltage_h) << 8;
        PWM3_V_RMS += read_i2c(dspic30f5015_1, PWM3_voltage_l);
        PWM3_V_RMS = rate * PWM3_V_RMS * 25;
        PWM3_C_RMS = read_i2c(dspic30f5015_1, PWM3_current_h) << 8;
        PWM3_C_RMS += read_i2c(dspic30f5015_1, PWM3_current_l);
        PWM3_C_RMS = rate * PWM3_C_RMS * 25;
        
        DC_V_RMS = read_i2c(dspic30f5015_1, DC_voltage_h) << 8;
        DC_V_RMS += read_i2c(dspic30f5015_1, DC_voltage_l);
        DC_V_RMS = rate * DC_V_RMS * 25;
        
        //write all data into a txt file
        write_txt(PWM1_V_RMS, PWM1_C_RMS, 
                  PWM2_V_RMS, PWM2_C_RMS,
                  PWM3_V_RMS, PWM3_C_RMS,
                  DC_V_RMS);
    }

	return 0;
}
