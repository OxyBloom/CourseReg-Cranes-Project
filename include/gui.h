// // #ifndef GUI_H
// // #define GUI_H

// // 

// // // Function to initialize the GUI
// // void activate(GtkApplication *app, gpointer user_data);

// // // GUI Functions
// // void create_login_window(GtkWidget *window);
// // void create_admin_panel(GtkWidget *window);

// // #endif // GUI_H


// #ifndef GUI_H
// #define GUI_H

// #include <gtk/gtk.h>

// extern GtkBuilder *builder;

// void on_login_button_clicked(GtkWidget *widget, gpointer data);

// #endif // GUI_H

#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include <glib.h>

#include "database.h"

static GtkWidget *main_window = NULL;
static GtkWidget *content_box=NULL; // New global variable
extern GtkWidget *entry_username;
extern GtkWidget *entry_password;
typedef struct {
    GtkWidget *grid;
    GtkWidget *widgets[3];
} AddCourseData;

typedef struct {
    GtkWidget *combo_year;
    GtkWidget *combo_registered_courses;
} ComboWidgets;


// Function Prototypes
static void show_main_page();
static void build_login_window(const char *title);
static void show_dashboard(const char *user_type);
// static void show_dashboard(GtkWidget *widget, gpointer data)
static void on_login(GtkWidget *widget, gpointer data);
void initialize_gui(GtkApplication *app);
static void on_logout_clicked(GtkWidget *widget, gpointer data);
static void on_back_clicked(GtkWidget *widget, gpointer data);
static void on_staff_login_clicked(GtkWidget *widget, gpointer data);
static void on_student_login_clicked(GtkWidget *widget, gpointer data);

static void build_student_enroll_page(GtkWidget *grid);
static void build_student_view_page(GtkWidget *parent_window);
static void build_student_delete_page(GtkWidget *parent_window);
// void on_back_clicked(GtkWidget *widget, gpointer data);
static void on_delete_course_clicked(GtkWidget *widget, gpointer data) ;
static void on_view_courses_clicked(GtkWidget *widget, gpointer data);
static void on_enroll_clicked(GtkWidget *widget, gpointer data);
static void build_course_management_window(GtkWidget *parent_window,const char *title);

static void on_back_student_clicked(GtkWidget *widget, gpointer data);
static void on_back_staff_clicked(GtkWidget *widget, gpointer data);
static void build_student_dashboard(GtkWidget *grid, const char *user_type);
static void build_staff_dashboard(GtkWidget *grid, const char *user_type);

static void on_add_clicked(GtkWidget *widget, gpointer data);
static void on_year_selected(GtkWidget *combo_year, gpointer data);
static void build_delete_page(GtkWidget *grid);
static void on_delete_clicked(GtkWidget *widget, gpointer data);
// static void on_year_selected_view(GtkComboBoxText *combo_year, GtkWidget *grid_course_table);
static void build_view_page(GtkWidget *grid);
static void on_view_clicked(GtkWidget *widget, gpointer data);
static void on_view_stats_clicked(GtkWidget *widget, gpointer data);
static void build_stats_view_page(GtkWidget *grid);
// static void on_year_selected_stats(GtkComboBoxText *combo_year, GtkWidget *grid_chart);
static void on_year_selected_stats(GtkComboBoxText *combo, gpointer user_data);
static void on_enroll_year_selected(GtkWidget *widget, gpointer combo_unregistered_courses);
static void on_confirm_enrollment_clicked(GtkWidget *widget, gpointer data);

void show_message(const char *message);
void show_success_message(const char *message);
void show_error_message(const char *message);

static void on_confirm_add_course_clicked(GtkWidget *widget, gpointer user_data);
static void on_confirm_delete_course_clicked(GtkWidget *widget, gpointer data);
static void on_year_selected_catalogue(GtkWidget *combo_year, gpointer data);
static void on_year_selected_student_view(GtkWidget *combo_year, gpointer data);
static void on_remove_clicked(GtkButton *button, gpointer combo_registered_courses);
static void on_delete_year_selected(GtkWidget *widget, gpointer combo_registered_courses);

#endif // GUI_H
