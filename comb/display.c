#include <gtk/gtk.h>
#include <libintl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

//Declare gtk pointer
GtkWidget* window, *data, *vbox;

//If click close program then quit
gboolean c_window(GtkWidget *window, gpointer data){
  gtk_main_quit();
  return FALSE;
}
//print the read values to the gtk window
int print_data(){
    
    char string[1024];
    
    //Open data file
    FILE* fp = fopen("data.txt","r");
    if(fp == NULL){
		printf("Open the data file is error!\n");
     	exit(0);
    }
    
    //Scan line by line and updata on display
    while(!feof(fp)){
        fscanf(fp,"%[^\n]%*c", string);
        gtk_label_set_text(GTK_LABEL(data),string);
    }
    
    fclose(fp);
    
    return 1;
    
}

int main (int argc, char *argv[]){
    
    //Initlize gtk lab
    gtk_init(&argc, &argv);

    while(1){
        
        //Set all gtk components
        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        data = gtk_label_new ("0");
        vbox = gtk_vbox_new(FALSE, 0);
        
        //set window font size and type
        PangoFontDescription *font_desc = pango_font_description_from_string("Sans");
        pango_font_description_set_size (font_desc, 27 * PANGO_SCALE);
        gtk_widget_modify_font (data, font_desc);
        //set window size
        gtk_widget_set_usize(window,700,350);
        gtk_window_set_title(GTK_WINDOW(window), "Motor Display");
        
        //Add window under vbox and add data under window
        gtk_container_add(GTK_CONTAINER (window), vbox);
        gtk_container_add(GTK_CONTAINER (data), window);
        
        //Click close will close program
        g_signal_connect(G_OBJECT (window), "delete-event",G_CALLBACK(c_window), NULL);
        //When CPU free call print_data function
        g_idle_add((GSourceFunc)print_data, NULL);
        
        //Show all the components under and include window
        gtk_widget_show_all(window);
        gtk_main();
    }
  
  return 0;
}