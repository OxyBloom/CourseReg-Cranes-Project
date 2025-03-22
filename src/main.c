#include <gtk/gtk.h>
#include "database.h"
#include "gui.h" // Include the header for GUI functions

// sqlite3 *db;
GtkWidget *entry_username;
GtkWidget *entry_password;
static void activate(GtkApplication *app, gpointer user_data) {
    init_database(&db);
    initialize_gui(app); // Initialize GUI from the module
}

int main(int argc, char *argv[]) {
    // if (!connect_db()) {
    //     return 1;  // Exit if database connection fails
    // }
    
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.CourseReg", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    close_database(&db);
    return status;
}
