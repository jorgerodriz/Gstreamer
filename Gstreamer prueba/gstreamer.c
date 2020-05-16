/* --------------------------------------------------------------------- *
 * Librerias y cabeceras
 * --------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <gst/gst.h>
#include <ctype.h>
#include <gst/check/gstcheck.h>


GstElement *queue_aud;
GstElement *queue_img;

int estado = 0;

/* --------------------------------------------------------------------- *
 *  Bus message handler Caleb tal cual no entiendo nada
 * --------------------------------------------------------------------- */
static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  GMainLoop *loop = (GMainLoop *) data;
  char *src = GST_MESSAGE_SRC_NAME(msg);
  
  switch (GST_MESSAGE_TYPE (msg)) {

  case GST_MESSAGE_EOS:
    g_print ("..[bus].. (%s) :: End of stream\n", src);
    g_main_loop_quit (loop);
    break;

  case GST_MESSAGE_ERROR: {
    gchar  *debug;
    GError *error;
    
    gst_message_parse_error (msg, &error, &debug);
    g_free (debug);
    
    g_printerr ("..[bus].. (%s) :: Error: %s\n", src, error->message);
    g_error_free (error);

    estado = -1;  // error multimedia
    g_main_loop_quit (loop);
    break;
  }
  case GST_MESSAGE_ELEMENT:
{
if(gst_structure_has_name(gst_message_get_structure(msg),"barcode")){
 const GstStructure *structure;

 structure =gst_message_get_structure(msg);
GstClockTime time;
gst_structure_get_clock_time(structure,"timestamp", &time);
const gchar *codigo;
codigo =gst_structure_get_string(structure,"symbol");
g_print("Barcode:%s", codigo);
    }
  }
  default: {
    g_print(" %lu \n", GST_MESSAGE_TIMESTAMP(msg));
    g_print ("..[bus].. %15s :: %-15s\n", src, GST_MESSAGE_TYPE_NAME(msg));
    break;
  }
  }

  return TRUE;
}


/* --------------------------------------------------------------------- *
  Esta función nos permite crear pads de forma dinamica
 * --------------------------------------------------------------------- */

/* Esta función nos permite crear pads de forma dinamica*/

static void
on_pad_added (GstElement *element,
              GstPad     *pad,
              gpointer    data)
{
  GstPad *sinkpad;

  gchar *name = gst_pad_get_name(pad);
  char *caps = gst_caps_to_string(gst_pad_get_current_caps(pad));

  g_print ("...Dynamic pad created: %s, capabilities: %s\n", name, caps);

  if (g_str_has_prefix(caps, "audio")) {
    /* Conecta Audio */
    sinkpad = gst_element_get_static_pad (queue_aud, "sink");
    
  } else if  (g_str_has_prefix(caps, "video")) {
    /* Conecta Video*/
    sinkpad = gst_element_get_static_pad (queue_img, "sink");
  }

  if (sinkpad != NULL) {
    /*Conecta */
    gst_pad_link (pad, sinkpad);

    /*Elimina */
    gst_object_unref (sinkpad);
  }

}

/* --------------------------------------------------------------------- *
 Main
 * --------------------------------------------------------------------- */
int main(int argc, char *argv[]) {


/* --------------------------------------------------------------------- *
 Comprobar argumentos
 * --------------------------------------------------------------------- */

  int c;
char *filename;

  
    
  opterr = 0;  // no es necesario declararla, la exporta getopt

  while ((c = getopt (argc, argv, "h:i:f:t:b:")) != -1) {
    g_print("¿ESTA ENTRANDO EN EL BUCLE?\n");
    switch (c)
      {
      case 'h':
	// ayuda
	g_print("Funcionalidad ayuda no implementada\n");
        break;

      case 'i':
	// tiempo inicio intervalo de vídeo a procesar
	g_print("Funcionalidad intervalo no implementada\n");
        break;

      case 'f':
	// tiempo inicio intervalo de vídeo a procesar
	g_print("Funcionalidad intervalo no implementada\n");
        break;

      case 't':
	// umbral inferior para efectos de vídeo
	g_print("Funcionalidad efectos no implementada\n");
        break;

      case 'b':
	// umbral superior para efectos de vídeo
	g_print("Funcionalidad efectos no implementada\n");
        break;

      case '?':  //NO LO ENTIENDO
	// getopt devuelve '?' si encuentra una opción desconocida
	// o si falta el argumento para una opción que lo requiere
	// La opción conflictiva queda almacenada en optopt
        if ((optopt == 'i') ||(optopt == 'f') || (optopt == 't') ||(optopt == 'b')) {
	  // falta argumento para opción que lo requiere
          fprintf (stderr, "Error: la opción -%c requiere un argumento\n", optopt);
	}
	// error: opción desconocida
        else if (isprint(optopt)==0)
          fprintf (stderr, "Error: argumento `-%c' no válido\n", optopt);
        else
          fprintf (stderr, "Error: argumento `\\x%x' no válido.\n", optopt);
        return 1;
	
      default:
        fprintf (stderr,
                 "Error: argumento %d no válido\n", optind);
        
        return 1;
      }
  }

 filename = argv[optind];
  
  // getopt recoloca los argumentos no procesados al final
  // el primero será el nombre del fichero de entrada (es correcto)
  // si hay algún otro: error argumento desconocido
int index = optind+1;

  for(index ; index < argc; index++) {
    printf ("Error: argumento %s no válido\n", argv[index]);
    return 1;
  }

/* --------------------------------------------------------------------- *
 * Inicializar Gstreamer
 * --------------------------------------------------------------------- */

  GMainLoop *loop;

  /* Check input arguments */
  if (argc != 2) {
    g_printerr ("Usage: %s <Ogg/Theora filename>\n", argv[0]);
    return -1;  // error argumentos
  }


  puts("GStreamer test - init!\n"); /* prints GStreamer test! */

  gst_init(&argc, &argv);  /* inits the GStreamer library */
  loop = g_main_loop_new (NULL, FALSE);

/* --------------------------------------------------------------------- *
 * Creación del Pipeline
 * --------------------------------------------------------------------- */


/* Creamos el pipeline */
  GstElement *pipeline = gst_pipeline_new ("mi_pipeline");


   /* Creamos los elementos*/
  GstElement *source = gst_element_factory_make("filesrc", "fuente_fichero");
  GstElement *decode = gst_element_factory_make("decodebin", "decode");
  GstElement *aud_sink = gst_element_factory_make("alsasink", "audio-output");
  GstElement *img_sink = gst_element_factory_make("ximagesink", "image-output");
  GstElement *zbar = gst_element_factory_make("zbar","barcode");
GstElement *videoconvert = gst_element_factory_make("videoconvert","convertidor");



  queue_aud = gst_element_factory_make("queue", "queue-aud");
  queue_img = gst_element_factory_make("queue", "queue-img");


 /* Comprobación de errores de los elementos*/

  if (!pipeline || !source ||!decode
     || !queue_img || !zbar ||!videoconvert  ||!img_sink || !queue_aud || !aud_sink  || !img_sink) 
    {
      g_printerr ("One element could not be created. Exiting.\n");
      return -1;   // error multimedia
    }

 /* Introducimos los valores de cada elemento*/

g_object_set (G_OBJECT (source), "location", argv[1], NULL);   /* NO LO ENTIENDO*/
g_object_set (G_OBJECT (decode), "decode", "decodebin", NULL);  /*NO LO ENTIENDO*/


/* --------------------------------------------------------------------- *
 * Nose que es esto ni para que sirve algo del bus
 * --------------------------------------------------------------------- */

/* Bus message handling NO LO ENTIENDO */
  GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

/* --------------------------------------------------------------------- *
 * Organizar elementos dentro del Pipeline
 * --------------------------------------------------------------------- */

  /* Añadir los elementos al pipeline*/
  gst_bin_add_many(GST_BIN(pipeline), source, decode, queue_img, zbar,videoconvert, img_sink, queue_aud,  aud_sink, NULL);


  /* Unimos los elementos */
 gst_element_link (source, decode);  /*Unimos fuente fichero con el demux*/
 gst_element_link_many(queue_aud, aud_sink, NULL); /* Unimos salida del audio con el sumidero de audio*/
 gst_element_link_many(queue_img, zbar,videoconvert, img_sink, NULL); /* Unimos la salida de video con el componente zbar, con el sumidero*/

 /* Realizamos las uniones del demux mediante la función auxiliar que nos crea pads dinámicos */
 g_signal_connect (decode, "pad-added", G_CALLBACK (on_pad_added), NULL);


/* --------------------------------------------------------------------- *
 * Cambio de estado
 * --------------------------------------------------------------------- */


 /* Poner el pipeline a funcionar*/

  g_print("Play: %s\n", argv[1]); 
  gst_element_set_state (pipeline, GST_STATE_PLAYING);


  /* Iterate (NOSE NI LO QUE ES) */

  g_print ("Run...\n");
  g_main_loop_run (loop);


/* --------------------------------------------------------------------- *
 * Final
 * --------------------------------------------------------------------- */

  g_print ("Returned, stopping playback\n");

  /* Volver al estado por defecto de todo el pipeline*/

  gst_element_set_state (pipeline, GST_STATE_NULL);


  /* Liberar memoria */

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  puts("\nGStreamer test - end!");


 /* prints GStreamer test! */

  return estado;
}




/* --------------------------------------------------------------------- *
 *Imprimir ayuda en h, intentar entender bus,entener case ?, buscar sobre iterate, comprobar código (victor)
 * --------------------------------------------------------------------- */

