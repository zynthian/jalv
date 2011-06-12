/*
  Copyright 2007-2011 David Robillard <http://drobilla.net>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <gtk/gtk.h>

#include "jalv_internal.h"

static void destroy(GtkWidget* widget,
                    gpointer   data)
{
	gtk_main_quit();
}

void
jalv_init(int* argc, char*** argv)
{
	GError* error = NULL;
	gtk_init_with_args(
		argc, argv,
		"PLUGIN_URI [JACK_UUID] - Run an LV2 plugin as a Jack application",
		NULL, NULL, &error);
}

LilvNode*
jalv_native_ui_type(Jalv* jalv)
{
	return lilv_new_uri(jalv->world,
	                    "http://lv2plug.in/ns/extensions/ui#GtkUI");
}

int
jalv_open_ui(Jalv*         jalv,
             SuilInstance* instance)
{
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
	g_signal_connect(window, "destroy",
	                 G_CALLBACK(destroy), NULL);
    
	gtk_container_set_border_width(GTK_CONTAINER(window), 8);

	if (instance) {
		GtkWidget* widget = (GtkWidget*)suil_instance_get_widget(instance);
		gtk_container_add(GTK_CONTAINER(window), widget);
	} else {
		GtkWidget* button = gtk_button_new_with_label("Close");
    
		g_signal_connect_swapped(button, "clicked",
		                         G_CALLBACK(gtk_widget_destroy),
		                         window);
    
		gtk_container_add(GTK_CONTAINER(window), button);
	}

	// TODO: Check UI properties for resizable
	gtk_window_set_resizable(GTK_WINDOW(window), false);

	gtk_widget_show_all(window);
    
	gtk_main();
	sem_post(jalv->done);
	return 0;
}