/* (c) 2010 Virgoptrex. Feel Free to use. Leave credits intact */

#include <gtk/gtk.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <gtk/gtk.h>
#include <gtk/gtkcontainer.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define LOCAL_SERVER_PORT 1500
#define LOCAL_SERVER_PORT2 1501
#define MAX_MSG 40000
#define sBuffer 16
#define dataSize 57600
#define dataSize_command 255
int size_of_buffer[sBuffer];
GdkPixbuf *pixbuf;
GError *err = NULL;
GtkWidget *image = NULL;

 //GtkWidget *window;
GtkWidget *aspect_frame;
GtkWidget *drawing_area;
GtkImage *im = NULL;

int rowstride = 960;
int width = 320;
int height = 240;


gchar data1[57600];
double data1_command[255];
int data1_send[255];
gchar* comData;
uint8_t data2[57600];
uint8_t data3[57600];
uint8_t data4[57600];
uint8_t data5[57600];
uint8_t* pixbufdata;



int i = 0;
int i_command = 0;
int count = 0;
int count_command = 0;
GtkWidget *window;
GtkWidget *table;


int done = 0;
int done_command = 0;
int try1 = 0;
int try1_command = 0;




int sd, rc, n, cliLen, flags;
struct sockaddr_in cliAddr, servAddr;
struct sockaddr_in cliAddr_command, servAddr_command;


int sd_command, rc_command, n_command, cliLen_command, flags_command;


GtkWidget *window;
GtkWidget *vbox;
GtkWidget *button;


GdkPixbuf *buffer_to_pixbuf(const gchar*, gsize, GError**);



gint t=0;

     gboolean
     expose_event_callback (GtkWidget *widget, GdkEventExpose *event, gpointer data)
     {




     //gtk_widget_queue_draw(GTK_WIDGET(widget));

     //g_object_ref_sink (widget->window);

     // gdk_drawable_ref (widget->window);


/*
       gdk_draw_arc (widget->window,
                     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                     TRUE,
                     0, 0, widget->allocation.width, widget->allocation.height,
                     0, 64 * 18*t);
                     */

      //gdk_drawable_unref (widget->window);


        // gtk_widget_show (window);




       return TRUE;
     }


static void callback( GtkWidget *widget,
                      gpointer   data )
{
    g_print ("Hello again - %s was pressed\n", (char *) data);
}




static gboolean
time_handler(GtkWidget *widget)
{



  //if (t<20)
  //{ t++; }
  //else if (t >=20)
  //{ t=0; }

           // if (!(try1 > 0))
          //  {
            //g_object_ref (image);
            /*
            pixbuf = gdk_pixbuf_new_from_data(pixbufdata,
  											 GDK_COLORSPACE_RGB,
  											 FALSE,
  											 8,
  											 320,
  											 240,
  											 960,
  											 NULL,
  											 NULL);



            pixbuf = gdk_pixbuf_scale_simple(pixbuf,
                                    width,
                                    height,
                                    GDK_INTERP_BILINEAR);

*/


             pixbuf = buffer_to_pixbuf(comData,57600,&err);

            gtk_container_remove(GTK_CONTAINER(window), image);
          //  }
            //sleep(1);
            image  = (GtkWidget*)gtk_image_new_from_pixbuf( pixbuf );
            gtk_container_add(GTK_CONTAINER(window), image);
        //     while (g_main_context_iteration (NULL, FALSE));
            gtk_widget_show_all(window);
            try1++;


       //  widget  = gtk_image_new_from_pixbuf( pixbuf );
       //  gtk_widget_set_size_request (image, 320, 240);

       //  gtk_widget_show (widget);


/*
        gtk_image_set_from_pixbuf(im,pixbuf);
        image = (GtkWidget*)im;
        gtk_container_remove(GTK_CONTAINER(window),image);
        gtk_container_add (GTK_CONTAINER (window), image);
        gtk_widget_show_all(window);
         //gtk_image_set_from_pixbuf((GtkImage*)im,pixbuf);
*/

 //printf("hello %d\n",t);
   return TRUE;



}



GdkPixbuf *buffer_to_pixbuf (const gchar *buf, gsize count, GError **err)
{
        GdkPixbufLoader *loader;
        GdkPixbuf *pixbuf;

        loader = gdk_pixbuf_loader_new ();
        if (gdk_pixbuf_loader_write (loader, buf, count, err) &&
            gdk_pixbuf_loader_close (loader, err)) {
                pixbuf = g_object_ref (gdk_pixbuf_loader_get_pixbuf (loader));
                g_object_unref (loader);
                return pixbuf;
        } else {
                return NULL;
        }
}






void *videoThread(void)
{



//////////////////////////////////////////////////Video Socket///////////////////////////////////
  /* socket creation */
  sd=socket(AF_INET, SOCK_DGRAM, 0);
  if(sd<0) {
    printf("%s: cannot open socket \n","prg");
    exit(1);
  }

  /* bind local server port */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(LOCAL_SERVER_PORT);
  rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
  if(rc<0) {
    printf("%s: cannot bind port number %d \n",
	   "prg", LOCAL_SERVER_PORT);
    exit(1);
  }

  printf("%s: waiting for data on port UDP %u\n",
	   "prg",LOCAL_SERVER_PORT);

/* BEGIN jcs 3/30/05 */

  flags = 0;

  //////////////////////////////////////////////////Video Socket Drone 1 ///////////////////////////////////
 //





    //DATA Thread

while(1){
   // printf("vid.");
 int recData = 0;


    while (recData < 1)
    {


    switch(recData)
    {
        case 0:
            cliLen = sizeof(cliAddr);
            n = recvfrom(sd, data1, dataSize, flags,
            (struct sockaddr *) &cliAddr, &cliLen);


             if(n<0) {
                printf("%s: cannot receive data \n","prg");

            //continue;
            }
            else{
               // printf("Got Packet %d\n",recData);
                recData++;

/*
                printf("1: %s: from %s:UDP%u : %d \n",
                "prg",inet_ntoa(cliAddr.sin_addr),
                ntohs(cliAddr.sin_port),data1[20000]);
                */

                // sleep(1);
                 //sendto(sd,"yes",3,flags,(struct sockaddr *)&cliAddr,cliLen);





            }

           // int i = 0;
            /*
            for (i; i<57600; i++)
            {
            // if (data1[i] == 0)
                    printf("data1[%d] = %d\n",i,data1[i]);

            }
            */


            break;
        case 1:
            cliLen = sizeof(cliAddr);
            n = recvfrom(sd, data2, dataSize, flags,
            (struct sockaddr *) &cliAddr, &cliLen);



              if(n<0) {
                printf("1: %s: cannot receive data \n","prg");
            //continue;
            }
             else{
              //  printf("Got Packet %d\n",recData);
                recData++;
            /*
                 printf("2: %s: from %s:UDP%u : %d \n",
                argv[0],inet_ntoa(cliAddr.sin_addr),
                ntohs(cliAddr.sin_port),data2[57599]);
            */
                 //sleep(1);
                // sendto(sd,data1_send,sizeof(data1_send),flags,(struct sockaddr *)&cliAddr,cliLen);




            }

            break;
        case 2:
            cliLen = sizeof(cliAddr);
            n = recvfrom(sd, data3, dataSize, flags,
            (struct sockaddr *) &cliAddr, &cliLen);


              if(n<0) {
                printf("%s: cannot receive data \n","prg");
            //continue;
            }
             else{
               // printf("Got Packet %d\n",recData);
                recData++;

                /*
                 printf("3: %s: from %s:UDP%u : %d \n",
                argv[0],inet_ntoa(cliAddr.sin_addr),
                ntohs(cliAddr.sin_port),data3[57599]);
                */
                 sleep(1);
                 sendto(sd,"yes",3,flags,(struct sockaddr *)&cliAddr,cliLen);



            }

            break;
        case 3:
            cliLen = sizeof(cliAddr);
            n = recvfrom(sd, data4, dataSize, flags,
            (struct sockaddr *) &cliAddr, &cliLen);


              if(n<0) {
                printf("%s: cannot receive data \n","prg");
            //continue;
            }
            else{
              //  printf("Got Packet %d\n",recData);
                recData++;


                 printf("4: %s: from %s:UDP%u : %d \n",
                "prg",inet_ntoa(cliAddr.sin_addr),
                ntohs(cliAddr.sin_port),data4[57599]);


                 sleep(1);
                 sendto(sd,"yes",3,flags,(struct sockaddr *)&cliAddr,cliLen);


            }
            break;
        case 4:
            cliLen = sizeof(cliAddr);
            n = recvfrom(sd, data5, dataSize, flags,
            (struct sockaddr *) &cliAddr, &cliLen);


              if(n<0) {
                printf("%s: cannot receive data \n","prg");
            //continue;
            }
            else{
              //  printf("Got Packet %d\n",recData);
                recData++;

                /*
                 printf("5: %s: from %s:UDP%u : %d \n",
                argv[0],inet_ntoa(cliAddr.sin_addr),
                ntohs(cliAddr.sin_port),data5[0]);
                */
                 sleep(1);
                 sendto(sd,"yes",3,flags,(struct sockaddr *)&cliAddr,cliLen);


            }
            break;

        default:
            printf("Default case reached\n");
            break;

    }




    }



    memcpy(comData, data1,57600 *sizeof(uint8_t));
   // memcpy(pixbufdata + 57600, data2,57600 *sizeof(uint8_t));
    //memcpy(pixbufdata + 115200, data3,57600 *sizeof(uint8_t));
   // memcpy(pixbufdata + 172800, data4,57600 *sizeof(uint8_t));
    //memcpy(pixbufdata + 230400, data5,57600 *sizeof(uint8_t));
    done++;

   ;


}
}

void *gtkThread(int argc, char* argv[]){
   //GTK Thread

   while(!(done >0))
   {
  // printf(".");
  // sleep(1);
  ;
   }
   gtk_init(&argc, &argv);






    /*
    if(n<0) {
      printf("%s: cannot receive data \n",argv[0]);
      //continue;
    }
    */

    /* print received message */
    /*
    printf("%s: from %s:UDP%u : %d \n",
	   argv[0],inet_ntoa(cliAddr.sin_addr),
	   ntohs(cliAddr.sin_port),data4[0]);



/* BEGIN jcs 3/30/05 */
/*
    sleep(1);
    sendto(sd,"yes",3,flags,(struct sockaddr *)&cliAddr,cliLen);
*/
  //}
/*
    ////////////////////////////// GET BUFFER ////////////////////////////
     gchar* buf[size_of_buffer[0]];
     memset(buf,0x0,size_of_buffer[0]);



    cliLen = sizeof(cliAddr);
     n = recvfrom(sd, buf, size_of_buffer[0], flags,
		 (struct sockaddr *) &cliAddr, &cliLen);

    printf("Got this from buffer: ");
    printf((int)&buf[0]);
    printf("\n");

    sleep(1);
    sendto(sd,"yes",3,flags,(struct sockaddr *)&cliAddr,cliLen);

  } // new end loop
    //pixbuf = buffer_to_pixbuf(buf,size_of_buffer[0],&err);



 */






  image  = (GtkWidget*)gtk_image_new_from_pixbuf( pixbuf );



  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 230, 150);
  gtk_window_set_title(GTK_WINDOW(window), "Red Rock");
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(window), 2);


  //////////////////////////////////////////////////////////////// buttons ///////////////////////////

   /* Create a 2x2 table */
    table = gtk_table_new (4, 4, TRUE);


     /* Put the table in the main window */
    gtk_container_add (GTK_CONTAINER (window), table);


   /* Create first button */
    button = gtk_button_new_with_label ("Left");

    /* When the button is clicked, we call the "callback" function
     * with a pointer to "button 1" as its argument */
    g_signal_connect (button, "clicked",
	              G_CALLBACK (callback), (gpointer) "Left");

     gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 1, 0, 1);

    // gtk_widget_show (button);

    /* Create second button */

    button = gtk_button_new_with_label ("Up");

    /* When the button is clicked, we call the "callback" function
     * with a pointer to "button 2" as its argument */
    g_signal_connect (button, "clicked",
                      G_CALLBACK (callback), (gpointer) "Up");
    /* Insert button 2 into the upper right quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE (table), button, 1, 3, 0, 1);

    /* Create third button */

    button = gtk_button_new_with_label ("Right");

    /* When the button is clicked, we call the "callback" function
     * with a pointer to "button 2" as its argument */
    g_signal_connect (button, "clicked",
                      G_CALLBACK (callback), (gpointer) "Right");
    /* Insert button 2 into the upper right quadrant of the table */
    gtk_table_attach_defaults (GTK_TABLE (table), button, 3, 4, 0, 1);



   // gtk_widget_show (button);




    /* Create "Quit" button */
   // button = gtk_button_new_with_label ("Quit");

    /* When the button is clicked, we call the "delete-event" function
     * and the program exits */
     //g_signal_connect (button, "clicked",
                      //G_CALLBACK (delete_event), NULL);

    /* Insert the quit button into the both
     * lower quadrants of the table */
//    gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 2, 1, 2);







  /////////////// buttons ////////////////////////////////////////////////////







  //vbox = gtk_vbox_new(FALSE, 5);
  //gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 5);
  //gtk_container_add(GTK_CONTAINER(window), vbox);





  //gtk_container_add(GTK_CONTAINER(window), image);
  gtk_table_attach_defaults (GTK_TABLE (table), image, 0, 4, 1, 4);


 //  g_signal_connect (G_OBJECT (image), "expose_event",
       //                  G_CALLBACK (expose_event_callback), NULL);
    g_timeout_add(100, (GSourceFunc) time_handler, (gpointer)image);



    g_signal_connect_swapped(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(window));


 // gtk_widget_show (table);
  gtk_widget_show_all(window);
  gtk_main();






}




void *dataThread(){

 ///////////////////////////////////////////////////////// Commands and Data Comm 1////////////////////////

 sd_command=socket(AF_INET, SOCK_DGRAM, 0);
  if(sd_command<0) {
    printf("%s: cannot open socket \n","prg");
    exit(1);
  }

  /* bind local server port */
  servAddr_command.sin_family = AF_INET;
  servAddr_command.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr_command.sin_port = htons(LOCAL_SERVER_PORT2);
  rc_command = bind (sd_command, (struct sockaddr *) &servAddr_command,sizeof(servAddr_command));
  if(rc_command<0) {
    printf("%s: cannot bind port number %d \n",
	   "prg", LOCAL_SERVER_PORT2);
    exit(1);
  }

  printf("%s: waiting for data on port UDP %u\n",
	   "prg",LOCAL_SERVER_PORT2);



  int recData = 0;


   while(1){
  //  printf(".");
   recData = 0;


    while (recData < 1)
    {


    switch(recData)
    {
        case 0:
            cliLen_command = sizeof(cliAddr_command);
            n_command = recvfrom(sd_command, data1_command, dataSize_command, flags_command,
            (struct sockaddr *) &cliAddr_command, &cliLen_command);
             if(n<0) {
                printf("%s: cannot receive data \n","prg");

            //continue;
            }
            else{
               // printf("Got connection!\n");
               // printf("Got Packet %d\n",recData);
                recData++;
                printf("Time: %f\n",data1_command[10]);

/*
                printf("1: %s: from %s:UDP%u : %d \n",
                "prg",inet_ntoa(cliAddr.sin_addr),
                ntohs(cliAddr.sin_port),data1[20000]);
                */
                 data1_send[0] = 1
                 ;
                 sleep(1);
                 sendto(sd_command,data1_send,sizeof(data1_send),flags,(struct sockaddr *)&cliAddr_command,cliLen_command);



                break;

            }
    }
    }
   }


/* BEGIN jcs 3/30/05 */

  flags_command = 0;

  ///////////////////////////////////////////////////////// Commands and Data Comm 1//////////////////////

}


int main( int argc,
          char *argv[] )
{

    pixbufdata = malloc(288000 * sizeof(uint8_t));
    comData = malloc(57600 * sizeof(uint8_t));



//////////////////////////////////////////////////////////////////////////////////////////////
//Threads///

    pthread_t video_pth; //id
    pthread_t gtk_pth; //id
    pthread_t data_pth; //id

    pthread_create(&gtk_pth,NULL,dataThread,"Data stuff.. \n");
    pthread_create(&video_pth,NULL,videoThread,"Getting Pixbuf Data.\n");

    pthread_create(&gtk_pth,NULL,gtkThread(argc,argv),"GTK stuff.. \n");





    return 0;
}
