#include "GladeGui.h"

#include "GladeSearchpath.h"

#include <config.h>
#include <i18n.h>

#include <stdlib.h>

GladeGui::GladeGui(GladeSearchpath* gladeSearchPath, string glade, string mainWnd)
{
	XOJ_INIT_TYPE(GladeGui);

	this->window = NULL;
	this->gladeSearchPath = gladeSearchPath;


#if !GTK3_ENABLED
	// IF GTK2, load .xml files instead of .glade files

	// about.glade => about.xml
	if (glade.substr(glade.size() - 6) == ".glade")
	{
		glade = glade.substr(0, glade.size() - 6) + ".xml";
	}

#endif

	string filename = this->gladeSearchPath->findFile("", glade);

#if GTK3_ENABLED

	GError* error = NULL;
	builder = gtk_builder_new();

	if (!gtk_builder_add_from_file(builder, filename.c_str(), &error))
	{
		g_warning("Couldn't load builder file: %s", error->message);
		GtkWidget* dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
												   GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s",
												   FC(_F("Error loading glade file \"{1}\" (try to load \"{2}\")")
															% glade % filename));

		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(GTK_WIDGET(dialog));

		g_error_free(error);

		exit(-1);
	}

#else

	this->xml = glade_xml_new(filename.c_str(), NULL, NULL);
	if (!this->xml)
	{
		GtkWidget* dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
												   GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s",
												   FC(_F("Error loading glade file \"{1}\" (try to load \"{2}\")")
															% glade % filename));
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(GTK_WIDGET(dialog));
		exit(-1);
	}
#endif

	this->window = get(mainWnd);
}

GladeGui::~GladeGui()
{
	XOJ_CHECK_TYPE(GladeGui);

#if GTK3_ENABLED
	// TODO: this causes a segfault... do we need this?
	//gtk_widget_destroy(this->window);
	g_object_unref(builder);
#else
	gtk_widget_destroy(this->window);
	g_object_unref(this->xml);
#endif

	XOJ_RELEASE_TYPE(GladeGui);
}

GtkWidget* GladeGui::get(string name)
{
	XOJ_CHECK_TYPE(GladeGui);

#if GTK3_ENABLED
	GtkWidget* w = GTK_WIDGET(gtk_builder_get_object(builder, name.c_str()));
#else
	GtkWidget* w = glade_xml_get_widget(xml, name.c_str());
#endif
	if (w == NULL)
	{
		g_warning("GladeGui::get: Could not find glade Widget: \"%s\"", name);
	}
	return w;
}

GtkWidget* GladeGui::loadIcon(string filename)
{
	XOJ_CHECK_TYPE(GladeGui);

	GdkPixbuf* icon = loadIconPixbuf(filename);
	if (icon == NULL)
	{
		return gtk_image_new();
	}

	GtkWidget* w = gtk_image_new_from_pixbuf(icon);

	g_object_unref(icon);

	return w;
}

GdkPixbuf* GladeGui::loadIconPixbuf(string filename)
{
	XOJ_CHECK_TYPE(GladeGui);

	if (filename == "")
	{
		return NULL;
	}

	string pathname = this->gladeSearchPath->findFile("pixmaps", filename);

	if (pathname == "")
	{
		g_warning("GladeGui::get: Couldn't find pixmap file: %s", filename.c_str());
		return NULL;
	}

	GError* error = NULL;
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(pathname.c_str(), &error);
	if (pixbuf == NULL)
	{
		g_error("Failed to load pixbuf file: %s: %s\n", pathname.c_str(), error->message);
		g_error_free(error);
	}

	return pixbuf;
}

GtkWidget* GladeGui::getWindow()
{
	XOJ_CHECK_TYPE(GladeGui);

	return this->window;
}

GladeSearchpath* GladeGui::getGladeSearchPath()
{
	XOJ_CHECK_TYPE(GladeGui);

	return this->gladeSearchPath;
}

GladeGui::operator GdkWindow* ()
{
	XOJ_CHECK_TYPE(GladeGui);

	return gtk_widget_get_window(GTK_WIDGET(getWindow()));
}

GladeGui::operator GtkWindow* ()
{
	XOJ_CHECK_TYPE(GladeGui);

	return GTK_WINDOW(getWindow());
}
