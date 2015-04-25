#include <gtk/gtk.h>
#include <libintl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <sys/time.h>

#define FPWM 6500

gint status = 0;
char i2c_device[]="/dev/i2c-1";
unsigned char buffer[2];

typedef struct{
	GtkWidget *window;
	GtkWidget *frame;
	GtkWidget *label_f;
	GtkWidget *label_d;
	GtkWidget *label_f_b;
	GtkWidget *scale_f;
	GtkWidget *scale_d;
	GtkWidget *f_b;
	GtkWidget *close;
	int fd;
} stuff;

void close_p(GtkWidget *widget, gpointer data)
{
	g_signal_connect(widget, "destroy", G_CALLBACK (gtk_main_quit), NULL);
}

static void frequency_change(GtkRange *range, gpointer data)
{
	gint pos = gtk_range_get_value (range);
		
    buffer[0] = 0x12; 
    buffer[1] = pos;   
    //if ( (write(((stuff *)data)->fd, buffer, 2)) != 2) exit(1);
	write(((stuff *)data)->fd, buffer, 2);

	gchar *str = g_strdup_printf ("Frequency is %d Hz", pos);
	gtk_label_set_text (GTK_LABEL(   ((stuff *)data)->label_f      ), str);

	g_free(str);
}

static void duty_change(GtkRange *range, gpointer data)
{
	gint amp;
	int fd, result;

	gint pos = gtk_range_get_value (range);

	//amp = pos*1491/60;

	amp = pos*14900/100;

	buffer[0] = 0x13;
    buffer[1] = (amp >> 8) & 0x00ff;  
    //if ( (write(((stuff *)data)->fd, buffer, 2)) != 2) exit(1);
    write(((stuff *)data)->fd, buffer, 2);

    buffer[0] = 0x14; 
    buffer[1] = amp & 0x00ff;   
    //if ( (write(((stuff *)data)->fd, buffer, 2)) != 2) exit(1);
	write(((stuff *)data)->fd, buffer, 2);

	gchar *str = g_strdup_printf ("Duty cycle is %d %%", pos);
	gtk_label_set_text (GTK_LABEL (((stuff *)data)->label_d), str);

	g_free(str);
}

static void f_or_b(GtkWidget *widget, gpointer data)
{
	int fd, result;

	status ^= 1;

	buffer[0] = 0x15;
    buffer[1] = status;  
    //if ( (write(((stuff *)data)->fd, buffer, 2)) != 2) exit(1);
	write(((stuff *)data)->fd, buffer, 2);

	if(status == 0) {
		gchar *str = g_strdup_printf ("Forward");
		gtk_label_set_text (GTK_LABEL (((stuff *)data)->label_f_b), str);
		g_free(str);
	} else {
		gchar *str = g_strdup_printf ("Backward");
		gtk_label_set_text (GTK_LABEL (((stuff *)data)->label_f_b), str);
		g_free(str);
	}
}

int main(int argc, char** argv) {

	int result;

	stuff data;

	// Init gtk
	gtk_init(&argc, &argv);

	data.fd = open(i2c_device, O_RDWR);
	//if(data.fd < 0) exit(1);

	result = ioctl(data.fd, I2C_SLAVE, 0x77);
	//if(result < 0) exit(1);

	// Init window
	data.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(data.window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(data.window), 370, 150);
	gtk_window_set_title(GTK_WINDOW(data.window), "Motor Control");

	// Init frame
	data.frame = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(data.window), data.frame);

	// Init frequency scale
	data.scale_f = gtk_hscale_new_with_range(10, 60, 1);
	gtk_widget_set_size_request(data.scale_f, 200, 50);
	gtk_fixed_put(GTK_FIXED(data.frame), data.scale_f, 20, 0);

	// Init duty cycle scale
	data.scale_d = gtk_hscale_new_with_range(0, 100, 1);
	gtk_widget_set_size_request(data.scale_d, 200, 50);
	gtk_fixed_put(GTK_FIXED(data.frame), data.scale_d, 20, 50);

	// Init forward or backward button
	data.f_b = gtk_button_new_with_label("F/B");
  	gtk_widget_set_size_request(data.f_b, 80, 35);
  	gtk_fixed_put(GTK_FIXED(data.frame), data.f_b, 20, 100);

	// Init data.close button
	data.close = gtk_button_new_with_label("Close");
  	gtk_widget_set_size_request(data.close, 80, 35);
  	gtk_fixed_put(GTK_FIXED(data.frame), data.close, 280, 100);

	// Init frequency lable
	data.label_f = gtk_label_new("Frequency is 0 Hz");
	gtk_fixed_put(GTK_FIXED(data.frame), data.label_f, 240, 23); 

	// Init duct cycle lable
	data.label_d = gtk_label_new("Duty cycle is 0 %%");
	gtk_fixed_put(GTK_FIXED(data.frame), data.label_d, 240, 73); 

	// Init forward or backward lable
	data.label_f_b = gtk_label_new("Forward");
	gtk_fixed_put(GTK_FIXED(data.frame), data.label_f_b, 105, 108);

	gtk_widget_show_all(data.window);

	// data.close program
	g_signal_connect(data.window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect(data.close, "clicked", G_CALLBACK(data.window), NULL);

	// Update frequency change
	g_signal_connect(data.scale_f, "value-changed", G_CALLBACK (frequency_change), &data);

	// Update duty cycle change
	g_signal_connect(data.scale_d, "value-changed", G_CALLBACK (duty_change), &data);

	//Change status when click button
	g_signal_connect(data.f_b, "clicked", G_CALLBACK(f_or_b), &data);

	gtk_main();
	close(data.fd);

	return 0;
}