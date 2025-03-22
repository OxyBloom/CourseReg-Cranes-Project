#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_NAME "attendance.db"

// Function Prototypes
static void activate(GtkApplication *app, gpointer user_data);
void login(GtkWidget *widget, gpointer data);
void show_dashboard();
void add_class(GtkWidget *widget, gpointer data);
void delete_class(GtkWidget *widget, gpointer data);
void export_attendance(GtkWidget *widget, gpointer data);
void start_attendance(GtkWidget *widget, gpointer data);
void manage_students(GtkWidget *widget, gpointer data);
void show_statistics(GtkWidget *widget, gpointer data);
void create_add_class_window();
void create_manage_students_window();
void add_user(const char *username, const char *password, const char *role);

// SQLite Database Initialization
void init_db();
int validate_login(const char *username, const char *password);
void execute_sql(const char *sql);

// Global Widgets
GtkWidget *username_entry;
GtkWidget *password_entry;
GtkWidget *login_window;

// Entry point
int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    add_user("admin1", "password123", "admin1");  
    
    init_db();
    
    app = gtk_application_new("com.example.Attendance", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}

// Initialize Database
void init_db() {
    const char *sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT, role TEXT);"
                      "CREATE TABLE IF NOT EXISTS classes (id INTEGER PRIMARY KEY, subject TEXT, semester TEXT, batch TEXT UNIQUE);"
                      "CREATE TABLE IF NOT EXISTS students (id INTEGER PRIMARY KEY, name TEXT, reg_no TEXT UNIQUE, email TEXT, branch TEXT, class_id INTEGER, FOREIGN KEY(class_id) REFERENCES classes(id));"
                      "CREATE TABLE IF NOT EXISTS attendance (id INTEGER PRIMARY KEY, student_id INTEGER, date TEXT, status TEXT, FOREIGN KEY(student_id) REFERENCES students(id));";
    execute_sql(sql);
}

// Helper function to execute SQL statements
void execute_sql(const char *sql) {
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open(DB_NAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    sqlite3_close(db);
}

// Login Function
void login(GtkWidget *widget, gpointer data) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    if (validate_login(username, password)) {
        gtk_widget_destroy(login_window);
        show_dashboard();
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(login_window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "Invalid username or password!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Show Dashboard
void show_dashboard() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Dashboard");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *add_class_button = gtk_button_new_with_label("Add Class");
    g_signal_connect(add_class_button, "clicked", G_CALLBACK(create_add_class_window), NULL);
    gtk_grid_attach(GTK_GRID(grid), add_class_button, 0, 0, 1, 1);

    GtkWidget *manage_students_button = gtk_button_new_with_label("Manage Students");
    g_signal_connect(manage_students_button, "clicked", G_CALLBACK(create_manage_students_window), NULL);
    gtk_grid_attach(GTK_GRID(grid), manage_students_button, 0, 1, 1, 1);

    gtk_widget_show_all(window);
}

// Validate Login Credentials
int validate_login(const char *username, const char *password) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int valid = 0;
    
    int rc = sqlite3_open(DB_NAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    const char *sql = "SELECT role FROM users WHERE username = ? AND password = ?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        valid = 1;
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return valid;
}

// Activate GUI
static void activate(GtkApplication *app, gpointer user_data) {
    login_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(login_window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(login_window), 400, 300);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(login_window), grid);
    
    username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Username");
    gtk_grid_attach(GTK_GRID(grid), username_entry, 0, 0, 1, 1);
    
    password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");
    gtk_grid_attach(GTK_GRID(grid), password_entry, 0, 1, 1, 1);
    
    GtkWidget *login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(login), NULL);
    gtk_grid_attach(GTK_GRID(grid), login_button, 0, 2, 1, 1);
    
    gtk_widget_show_all(login_window);
}

// GUI Form for Adding Classes
void create_add_class_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Add Class");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);
    
    GtkWidget *subject_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(subject_entry), "Subject");
    gtk_grid_attach(GTK_GRID(grid), subject_entry, 0, 0, 1, 1);
    
    GtkWidget *semester_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(semester_entry), "Semester");
    gtk_grid_attach(GTK_GRID(grid), semester_entry, 0, 1, 1, 1);
    
    GtkWidget *batch_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(batch_entry), "Batch");
    gtk_grid_attach(GTK_GRID(grid), batch_entry, 0, 2, 1, 1);
    
    GtkWidget *add_button = gtk_button_new_with_label("Add Class");
    gtk_grid_attach(GTK_GRID(grid), add_button, 0, 3, 1, 1);
    
    gtk_widget_show_all(window);
}

// GUI Form for Managing Students
void create_manage_students_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Manage Students");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);
    
    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Name");
    gtk_grid_attach(GTK_GRID(grid), name_entry, 0, 0, 1, 1);
    
    GtkWidget *reg_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(reg_entry), "Registration Number");
    gtk_grid_attach(GTK_GRID(grid), reg_entry, 0, 1, 1, 1);
    
    GtkWidget *email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Email");
    gtk_grid_attach(GTK_GRID(grid), email_entry, 0, 2, 1, 1);
    
    GtkWidget *branch_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(branch_entry), "Branch");
    gtk_grid_attach(GTK_GRID(grid), branch_entry, 0, 3, 1, 1);
    
    GtkWidget *add_button = gtk_button_new_with_label("Add Student");
    gtk_grid_attach(GTK_GRID(grid), add_button, 0, 4, 1, 1);
    
    gtk_widget_show_all(window);
}

void add_user(const char *username, const char *password, const char *role) {
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open(DB_NAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql = "INSERT INTO users (username, password, role) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, role, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Error inserting user: %s\n", sqlite3_errmsg(db));
    } else {
        printf("User added successfully!\n");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

