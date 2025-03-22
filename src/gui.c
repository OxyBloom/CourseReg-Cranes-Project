#include "gui.h"

int get_logged_in_student_id() {
    return logged_in_student_id;
}

int get_logged_in_staff_id() {
    return logged_in_staff_id;
}

void logout() {
    logged_in_student_id = -1;
    logged_in_staff_id = -1;
}

static void on_student_login_clicked(GtkWidget *widget, gpointer data) {
    build_login_window("Student Login");
}

static void on_staff_login_clicked(GtkWidget *widget, gpointer data) {
    build_login_window("Staff Login");
}

static void on_back_clicked(GtkWidget *widget, gpointer data) {
    show_main_page();
}

static void on_back_student_clicked(GtkWidget *widget, gpointer data) {
    show_dashboard("Student Login");
}

static void on_back_staff_clicked(GtkWidget *widget, gpointer data) {
    AddCourseData *newdata = (AddCourseData *)data;
    if (newdata) {
        g_free(newdata);  // Correctly free the entire structure
    }
    
    show_dashboard("Staff Login");
    
    GtkWidget *grid;
}

static void on_logout_clicked(GtkWidget *widget, gpointer data) {
    logout();
    show_main_page();
    
}

static void on_confirm_add_course_clicked(GtkWidget *widget, gpointer user_data) {
    // Extract widgets from grid
    // GtkWidget **widgets = (GtkWidget **)user_data;
    AddCourseData *data = (AddCourseData *)user_data;

    GtkWidget *grid = data->grid;
    GtkWidget **widgets = data->widgets;

    if (!widgets || !GTK_IS_ENTRY(widgets[0]) || !GTK_IS_ENTRY(widgets[1]) || !GTK_IS_COMBO_BOX_TEXT(widgets[2])) {
        show_error_message("Internal error: Invalid widget references.");
        return;
    }

    const char *course_name = gtk_entry_get_text(GTK_ENTRY(widgets[0]));
    const char *credit_str = gtk_entry_get_text(GTK_ENTRY(widgets[1]));
    const char *year_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widgets[2]));

    // Validation
    if (!course_name || !credit_str || !year_text) {
        show_error_message("All fields are required!");
        return;
    }
    // if (strlen(course_name) == 0 || strlen(credit_str) == 0 || year_text == NULL) {
    //     show_error_message("All fields are required!");
    //     return;
    // }

    // Convert credit to integer
    int credit = atoi(credit_str);
    int year = atoi(&year_text[strlen(year_text) - 1]); // Extract year number from text like "Year 1"

    // SQL insertion
    char sql[256];
    snprintf(sql, sizeof(sql),
             "INSERT INTO courses (name, credit, year) VALUES ('%s', %d, %d);",
             course_name, credit, year);
    char sql1[256];
    const int course_id = get_course_id_by_name(course_name);

    snprintf(sql1, sizeof(sql1),
            "INSERT OR IGNORE INTO course_stats (name, year, enrollment_count) VALUES ('%s', %d, 0);",
            course_name, year);

    if (execute_sql(sql) && execute_sql(sql1)) {
        show_success_message("Course added successfully!");
    } else {
        show_error_message("Failed to add course. It may already exist.");
    }
    gtk_entry_set_text(GTK_ENTRY(widgets[0]), "");
    gtk_entry_set_text(GTK_ENTRY(widgets[1]), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets[2]), -1);

    // g_object_set_data(G_OBJECT(grid), "widgets", widgets);
    // g_signal_connect(grid, "destroy", G_CALLBACK(g_free), widgets);
}


// Builds the main page with student/staff login options
static void show_main_page() {
    GtkWidget *grid;
    GtkWidget *btn_student_login;
    GtkWidget *btn_staff_login;

    if (main_window == NULL) {
        main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(main_window), "Course Registration System");
        gtk_window_set_default_size(GTK_WINDOW(main_window), 1000, 1000);
    } else {
        gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);
    }

    grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 250); // Adds padding
    gtk_container_add(GTK_CONTAINER(main_window), grid);

    btn_student_login = gtk_button_new_with_label("Student Login");
    gtk_widget_set_halign(btn_student_login, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(btn_student_login, GTK_ALIGN_CENTER);
    g_signal_connect(btn_student_login, "clicked", G_CALLBACK(on_student_login_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_student_login, 0, 0, 1, 1);


    btn_student_login = gtk_button_new_with_label("Student Login");
    g_signal_connect(btn_student_login, "clicked", G_CALLBACK(on_student_login_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_student_login, 0, 0, 1, 1);

    btn_staff_login = gtk_button_new_with_label("Staff Login");
    g_signal_connect(btn_staff_login, "clicked", G_CALLBACK(on_staff_login_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_staff_login, 1, 0, 1, 1);

    gtk_widget_show_all(main_window);
}

// Login Window Creation
static void build_login_window(const char *title) {
    GtkWidget *grid;
    // GtkWidget *entry_username, *entry_password;
    GtkWidget *btn_login, *btn_back;

    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 250);
    gtk_container_add(GTK_CONTAINER(main_window), grid);

    gtk_window_set_title(GTK_WINDOW(main_window), title);

    entry_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Username");
    gtk_grid_attach(GTK_GRID(grid), entry_username, 0, 0, 2, 1);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_grid_attach(GTK_GRID(grid), entry_password, 0, 1, 2, 1);

    btn_login = gtk_button_new_with_label("Login");
    g_signal_connect(btn_login, "clicked", G_CALLBACK(on_login), (gpointer)title);
    gtk_grid_attach(GTK_GRID(grid), btn_login, 0, 2, 1, 1);

    btn_back = gtk_button_new_with_label("Back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 1, 2, 1, 1);

    gtk_widget_show_all(main_window);
}
static void build_staff_dashboard(GtkWidget *grid, const char *user_type) {

    GtkWidget *btn_logout, *btn_add_course, *btn_view_catalogue, *btn_view_stats, *btn_remove_course;
    GtkWidget *label_welcome, *combo_year;
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    gtk_window_set_title(GTK_WINDOW(main_window), "Staff Dashboard");

    btn_logout = gtk_button_new_with_label("Logout");
    g_signal_connect(btn_logout, "clicked", G_CALLBACK(on_logout_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_logout, 0, 0, 1, 1);

    label_welcome = gtk_label_new(g_strdup_printf("Welcome, %s!", user_type));
    gtk_grid_attach(GTK_GRID(grid), label_welcome, 1, 0, 1, 1);

    btn_add_course = gtk_button_new_with_label("Add Course");
    g_signal_connect(btn_add_course, "clicked", G_CALLBACK(on_add_clicked), combo_year);
    gtk_widget_set_halign(btn_add_course, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_add_course, 0, 1, 1, 1);

    btn_remove_course = gtk_button_new_with_label("Remove Course");
    g_signal_connect(btn_remove_course, "clicked", G_CALLBACK(on_delete_clicked), combo_year);
    gtk_widget_set_halign(btn_remove_course, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_remove_course, 1, 1, 1, 1);

    btn_view_catalogue = gtk_button_new_with_label("View Catalogue");
    g_signal_connect(btn_view_catalogue, "clicked", G_CALLBACK(on_view_clicked), combo_year);
    gtk_widget_set_halign(btn_view_catalogue, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_view_catalogue, 2, 1, 1, 1);

    btn_view_stats = gtk_button_new_with_label("View Course Statistics");
    g_signal_connect(btn_view_stats, "clicked", G_CALLBACK(on_view_stats_clicked), combo_year);
    gtk_widget_set_halign(btn_view_stats, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_view_stats, 3, 1, 1, 1);


    combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 4");

    gtk_grid_attach(GTK_GRID(grid), combo_year, 0, 2, 4, 1);
}

static void build_student_dashboard(GtkWidget *grid, const char *user_type) {
    GtkWidget *btn_logout;
    GtkWidget *label_welcome;

    gtk_window_set_title(GTK_WINDOW(main_window), "Student Dashboard");

    btn_logout = gtk_button_new_with_label("Logout");
    g_signal_connect(btn_logout, "clicked", G_CALLBACK(on_logout_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_logout, 0, 0, 1, 1);

    label_welcome = gtk_label_new(g_strdup_printf("Welcome, %s!", user_type));
    gtk_grid_attach(GTK_GRID(grid), label_welcome, 1, 0, 1, 1);

    GtkWidget *label_year = gtk_label_new("Select Year:");
    GtkWidget *combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "4");

    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_year, 1, 1, 2, 1);
    // Enroll Button - Navigate to Enroll Page
    GtkWidget *btn_enroll = gtk_button_new_with_label("Enroll in a Course");
    g_signal_connect(btn_enroll, "clicked", G_CALLBACK(on_enroll_clicked), combo_year);
    gtk_grid_attach(GTK_GRID(grid), btn_enroll, 0, 2, 3, 1);

    // View Registered Courses Button
    GtkWidget *btn_view = gtk_button_new_with_label("View Registered Courses");
    g_signal_connect(btn_view, "clicked", G_CALLBACK(on_view_courses_clicked), combo_year);
    gtk_grid_attach(GTK_GRID(grid), btn_view, 0, 3, 3, 1);

    // Delete Course Button
    GtkWidget *btn_delete = gtk_button_new_with_label("Delete a Course");
    g_signal_connect(btn_delete, "clicked", G_CALLBACK(on_delete_course_clicked), combo_year);
    gtk_grid_attach(GTK_GRID(grid), btn_delete, 0, 4, 3, 1);

}

static void build_student_enroll_page(GtkWidget *grid) {
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *label_title = gtk_label_new("Enroll in a Course");
    gtk_grid_attach(GTK_GRID(grid), label_title, 0, 0, 2, 1);

    // Year selection combo box
    GtkWidget *label_year = gtk_label_new("Year:");
    gtk_widget_set_halign(label_year, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 1, 1, 1);
    GtkWidget *combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "4");
    gtk_grid_attach(GTK_GRID(grid), combo_year, 1, 1, 2, 1);

    // Course list combo box
    GtkWidget *label_course = gtk_label_new("Course:");
    gtk_widget_set_halign(label_course, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_course, 0, 2, 2, 1);
    GtkWidget *combo_unregistered_courses = gtk_combo_box_text_new();
    gtk_grid_attach(GTK_GRID(grid), combo_unregistered_courses, 1, 2, 2, 1);

    // Connect year selection to populate course list
    g_signal_connect(combo_year, "changed", G_CALLBACK(on_enroll_year_selected), combo_unregistered_courses);

    // Confirm Button
    GtkWidget *btn_confirm = gtk_button_new_with_label("Confirm Enrollment");
    ComboWidgets *combo_widgets = g_new(ComboWidgets, 1);
    combo_widgets->combo_year = combo_year;
    combo_widgets->combo_registered_courses = combo_unregistered_courses;
    // GtkWidget *btn_confirm = gtk_button_new_with_label("Confirm Enrollment");
    g_signal_connect(btn_confirm, "clicked", G_CALLBACK(on_confirm_enrollment_clicked), combo_widgets);
    gtk_grid_attach(GTK_GRID(grid), btn_confirm, 0, 3, 2, 1);

    GtkWidget *btn_back = gtk_button_new_with_label("Back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_student_clicked), NULL);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 0, 4, 2, 1);
}

static void build_student_view_page(GtkWidget *grid) {

    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    // Title
    GtkWidget *label_title = gtk_label_new("Registered Courses");
    gtk_widget_set_halign(label_title, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label_title, 0, 0, 2, 1);

    // Year Dropdown
    GtkWidget *label_year = gtk_label_new("Year:");
    gtk_widget_set_halign(label_year, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 1, 1, 1);

    GtkWidget *combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 4");
    gtk_grid_attach(GTK_GRID(grid), combo_year, 1, 1, 1, 1);

    // Course List (Scrollable)
    GtkWidget *label_cou = gtk_label_new("Course");
    gtk_widget_set_halign(label_cou, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_cou, 0, 2, 2, 1);
    GtkWidget *label_cu = gtk_label_new("Credit Units");
    gtk_widget_set_halign(label_cu, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), label_cu, 1, 2, 1, 1);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, 350, 250);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 3, 2, 1);

    GtkWidget *list_box = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), list_box);

    // Connect Year Dropdown to dynamically update Course List
    g_signal_connect(combo_year, "changed", G_CALLBACK(on_year_selected_student_view), list_box);

    // Back Button
    GtkWidget *btn_back = gtk_button_new_with_label("Back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_student_clicked), NULL);
    gtk_widget_set_halign(btn_back, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 0, 4, 2, 1);
    gtk_widget_show_all(grid);
}

static void build_student_delete_page(GtkWidget *grid) {

    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    GtkWidget *label_title = gtk_label_new("Delete a Course");
    gtk_grid_attach(GTK_GRID(grid), label_title, 0, 0, 2, 1);

    // Year selection combo box
    GtkWidget *label_year = gtk_label_new("Year:");
    gtk_widget_set_halign(label_year, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 1, 1, 1);
    GtkWidget *combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "4");
    gtk_grid_attach(GTK_GRID(grid), combo_year, 1, 1, 2, 1);

    // Course list combo box
    GtkWidget *label_course = gtk_label_new("Course:");
    gtk_widget_set_halign(label_course, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_course, 0, 2, 2, 1);
    GtkWidget *combo_registered_courses = gtk_combo_box_text_new();
    gtk_grid_attach(GTK_GRID(grid), combo_registered_courses, 1, 2, 2, 1);

    // Connect year selection to populate course list
    g_signal_connect(combo_year, "changed", G_CALLBACK(on_delete_year_selected), combo_registered_courses);

    // Confirm Button
    GtkWidget *btn_confirm = gtk_button_new_with_label("Confirm Deletion");
    ComboWidgets *combo_widgets = g_new(ComboWidgets, 1);
    combo_widgets->combo_year = combo_year;
    combo_widgets->combo_registered_courses = combo_registered_courses;

    g_signal_connect(btn_confirm, "clicked", G_CALLBACK(on_remove_clicked), combo_widgets);

    gtk_grid_attach(GTK_GRID(grid), btn_confirm, 0, 3, 2, 1);

    GtkWidget *btn_back = gtk_button_new_with_label("Back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_student_clicked), NULL);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 0, 4, 2, 1);
}

static void on_remove_clicked(GtkButton *button, gpointer data) {
    ComboWidgets *widgets = (ComboWidgets *)data;  // Correctly cast the data
    GtkWidget *combo_year = widgets->combo_year;
    GtkWidget *combo_registered_courses = widgets->combo_registered_courses;
    const gchar *selected_course = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_registered_courses));

    if (!selected_course || !g_utf8_validate(selected_course, -1, NULL)) {
        show_message("Please select a valid course to delete.");
        return;
    }

    const char *sql_get_id = "SELECT id FROM courses WHERE name = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql_get_id, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (get_course_id): %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, selected_course, -1, SQLITE_STATIC);

    int course_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        course_id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (course_id == -1) {
        show_message("Error: Could not find course ID.");
        return;
    }

    if (delete_course(get_logged_in_student_id(), course_id)) {
        show_message("Course deleted successfully.");
        // Clear the course list and refresh it
        gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combo_registered_courses));

        // Refresh the course list by triggering the year selection callback
        
        on_delete_year_selected(combo_year, combo_registered_courses);
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_year), -1); // Clear year selection
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_registered_courses), -1); // Clear course selection

    } else {
        show_message("Failed to delete the course.");
    }

    // gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combo_registered_courses));
}

static void build_add_page(GtkWidget *grid) {
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    // Title
    GtkWidget *label_title = gtk_label_new("Add a Course");
    gtk_widget_set_halign(label_title, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label_title, 0, 0, 2, 1);

    // Name of Course
    GtkWidget *label_name = gtk_label_new("Name of Course:");
    gtk_widget_set_halign(label_name, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_name, 0, 1, 1, 1);

    GtkWidget *entry_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry_name, 1, 1, 1, 1);

    // Credit
    GtkWidget *label_credit = gtk_label_new("Credit:");
    gtk_widget_set_halign(label_credit, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_credit, 0, 2, 1, 1);

    GtkWidget *entry_credit = gtk_entry_new();
    gtk_entry_set_input_purpose(GTK_ENTRY(entry_credit), GTK_INPUT_PURPOSE_NUMBER);
    gtk_grid_attach(GTK_GRID(grid), entry_credit, 1, 2, 1, 1);

    // Year (Drop-down)
    GtkWidget *label_year = gtk_label_new("Year:");
    gtk_widget_set_halign(label_year, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 3, 1, 1);

    GtkWidget *combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 4");
    gtk_grid_attach(GTK_GRID(grid), combo_year, 1, 3, 1, 1);

    // Confirm Button
    GtkWidget *btn_confirm = gtk_button_new_with_label("Confirm");

    // Pass widgets as user data
    // GtkWidget **widgets = g_malloc(3 * sizeof(GtkWidget *));
    // widgets[0] = entry_name;
    // widgets[1] = entry_credit;
    // widgets[2] = combo_year;

    // g_signal_connect(btn_confirm, "clicked", G_CALLBACK(on_confirm_add_course_clicked), widgets);
    AddCourseData *data = g_malloc(sizeof(AddCourseData));
    data->grid = grid;
    data->widgets[0] = entry_name;
    data->widgets[1] = entry_credit;
    data->widgets[2] = combo_year;

    g_signal_connect(btn_confirm, "clicked", G_CALLBACK(on_confirm_add_course_clicked), data);
    gtk_widget_set_halign(btn_confirm, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_confirm, 0, 4, 2, 1);

    // Back Button
    GtkWidget *btn_back = gtk_button_new_with_label("Back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_staff_clicked), data);
    gtk_widget_set_halign(btn_back, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 0, 5, 2, 1);

}

static void on_year_selected(GtkWidget *combo_year, gpointer data) {
    GtkWidget *combo_course = (GtkWidget *)data;

    // Clear existing entries
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combo_course));

    // Get selected year
    const char *selected_year = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_year));
    if (!selected_year) {
        show_error_message("Please select a year.");
        return;
    }

    // Map year text to year number
    int year = 0;
    if (strcmp(selected_year, "Year 1") == 0) year = 1;
    else if (strcmp(selected_year, "Year 2") == 0) year = 2;
    else if (strcmp(selected_year, "Year 3") == 0) year = 3;
    else if (strcmp(selected_year, "Year 4") == 0) year = 4;

    // Fetch course list for the selected year
    GList *courses = get_courses_by_year(year); // Ensure this function fetches correct data
    if (!courses) {
        show_error_message("No courses found for the selected year.");
        return;
    }

    // Add courses to combo box
    for (GList *l = courses; l != NULL; l = l->next) {
        Course *course = (Course *)l->data; // Correct casting
        if (course && g_utf8_validate(course->course_name, -1, NULL)) {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_course), course->course_name);
        } else {
            show_error_message("Invalid course data detected.");
        }
        // gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_course), (char *)l->data);
    }

    // Free the list after usage
    g_list_free_full(courses, (GDestroyNotify)g_free);
}

static void build_delete_page(GtkWidget *grid) {
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    // Title
    GtkWidget *label_title = gtk_label_new("Remove a Course");
    gtk_widget_set_halign(label_title, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label_title, 0, 0, 2, 1);

    // Year (Drop-down)
    GtkWidget *label_year = gtk_label_new("Year:");
    gtk_widget_set_halign(label_year, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 1, 1, 1);

    GtkWidget *combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 4");
    gtk_grid_attach(GTK_GRID(grid), combo_year, 1, 1, 1, 1);

    // Course (Drop-down)
    GtkWidget *label_course = gtk_label_new("Name of Course:");
    gtk_widget_set_halign(label_course, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_course, 0, 2, 1, 1);

    GtkWidget *combo_course = gtk_combo_box_text_new();
    gtk_grid_attach(GTK_GRID(grid), combo_course, 1, 2, 1, 1);

    // Connect Year Dropdown to dynamically update Course List
    g_signal_connect(combo_year, "changed", G_CALLBACK(on_year_selected), combo_course);

    // Confirm Button
    GtkWidget *btn_confirm = gtk_button_new_with_label("Confirm Deletion");
    GtkWidget **widgets = g_new(GtkWidget*, 2);  // Dynamically allocate
    widgets[0] = combo_year;
    widgets[1] = combo_course;

    g_signal_connect(btn_confirm, "clicked", G_CALLBACK(on_confirm_delete_course_clicked), widgets);

    gtk_widget_set_halign(btn_confirm, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_confirm, 0, 3, 2, 1);

    // Back Button
    GtkWidget *btn_back = gtk_button_new_with_label("Back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_staff_clicked), NULL);
    gtk_widget_set_halign(btn_back, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 0, 4, 2, 1);
}

void free_course_data(Course *course) {
    if (course) {
        g_free(course->course_name);
        g_free(course);
    }
}

static void on_confirm_delete_course_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **widgets = (GtkWidget **)data;
    GtkWidget *combo_year = widgets[0];
    GtkWidget *combo_course = widgets[1];

    const char *selected_course = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_course));
    if (!selected_course) {
        show_error_message("Please select a course to delete.");
        return;
    }

    // SQL to delete the course
    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM courses WHERE name = '%s';", selected_course);
    char sql1[256];
    snprintf(sql1, sizeof(sql1), "DELETE FROM course_stats WHERE name = '%s';", selected_course);
    char sql2[256];
    int course_id = get_course_id_by_name(selected_course);
    snprintf(sql2, sizeof(sql2), "DELETE FROM enrollments WHERE course_id = %d;", course_id);

    if (execute_sql(sql) && execute_sql(sql1) && execute_sql(sql2)) {
        show_success_message("Course successfully deleted!");
    } else {
        show_error_message("Failed to delete the course. Course may not exist.");
    }

    // Refresh the course list after deletion
    on_year_selected(combo_year, combo_course);
    // g_free(widgets);
}

static void on_year_selected_catalogue(GtkWidget *combo_year, gpointer data) {
    GtkWidget *list_box = (GtkWidget *)data;

    // Clear existing entries
    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Get selected year
    const char *selected_year = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_year));
    if (!selected_year) {
        show_error_message("Please select a year.");
        return;
    }

    int year = 0;
    if (strcmp(selected_year, "Year 1") == 0) year = 1;
    else if (strcmp(selected_year, "Year 2") == 0) year = 2;
    else if (strcmp(selected_year, "Year 3") == 0) year = 3;
    else if (strcmp(selected_year, "Year 4") == 0) year = 4;

    // Fetch course list for the selected year
    GList *courses = get_courses_by_year(year);
    if (!courses) {
        GtkWidget *no_courses_label = gtk_label_new("No courses found for the selected year.");
        gtk_list_box_insert(GTK_LIST_BOX(list_box), no_courses_label, -1);
        gtk_widget_show(no_courses_label);
        return;
    }

    // Add courses and credits to the list
    for (GList *l = courses; l != NULL; l = l->next) {
        Course *course = (Course *)l->data;

        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);

        GtkWidget *label_course = gtk_label_new(course->course_name);
        gtk_widget_set_halign(label_course, GTK_ALIGN_START);
        gtk_box_pack_start(GTK_BOX(row), label_course, FALSE, FALSE, 0);

        // Spacer to push credit units to the right
        GtkWidget *spacer = gtk_label_new(""); 
        gtk_box_pack_start(GTK_BOX(row), spacer, TRUE, TRUE, 0);

        char credit_text[10];
        snprintf(credit_text, sizeof(credit_text), "%d   ", course->credit_units);
        GtkWidget *label_credit = gtk_label_new(credit_text);
        gtk_widget_set_halign(label_credit, GTK_ALIGN_END);
        gtk_box_pack_end(GTK_BOX(row), label_credit, FALSE, FALSE, 0);

        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
        gtk_widget_show_all(row);
    }

    g_list_free_full(courses, g_free);
}

static void on_year_selected_student_view(GtkWidget *combo_year, gpointer data) {
    GtkWidget *list_box = (GtkWidget *)data;

    // Clear existing entries
    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Get selected year
    const char *selected_year = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_year));
    if (!selected_year) {
        show_error_message("Please select a year.");
        return;
    }

    int year = 0;
    if (strcmp(selected_year, "Year 1") == 0) year = 1;
    else if (strcmp(selected_year, "Year 2") == 0) year = 2;
    else if (strcmp(selected_year, "Year 3") == 0) year = 3;
    else if (strcmp(selected_year, "Year 4") == 0) year = 4;

    // Fetch course list for the selected year//////////////////////////////
    GList *courses = get_student_courses_by_year(year);
    if (!courses) {
        GtkWidget *no_courses_label = gtk_label_new("No courses registered for the selected year.");
        gtk_list_box_insert(GTK_LIST_BOX(list_box), no_courses_label, -1);
        gtk_widget_show(no_courses_label);
        return;
    }

    // Add courses and credits to the list
    for (GList *l = courses; l != NULL; l = l->next) {
        Course *course = (Course *)l->data;

        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);

        GtkWidget *label_course = gtk_label_new(course->course_name);
        gtk_widget_set_halign(label_course, GTK_ALIGN_START);
        gtk_box_pack_start(GTK_BOX(row), label_course, FALSE, FALSE, 0);

        // Spacer to push credit units to the right
        GtkWidget *spacer = gtk_label_new(""); 
        gtk_box_pack_start(GTK_BOX(row), spacer, TRUE, TRUE, 0);

        char credit_text[10];
        snprintf(credit_text, sizeof(credit_text), "%d   ", course->credit_units);
        GtkWidget *label_credit = gtk_label_new(credit_text);
        gtk_widget_set_halign(label_credit, GTK_ALIGN_END);
        gtk_box_pack_end(GTK_BOX(row), label_credit, FALSE, FALSE, 0);

        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
        gtk_widget_show_all(row);
    }

    g_list_free_full(courses, g_free);
}

static void build_view_page(GtkWidget *grid) {
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    // Title
    GtkWidget *label_title = gtk_label_new("Course Catalogue");
    gtk_widget_set_halign(label_title, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label_title, 0, 0, 2, 1);

    // Year Dropdown
    GtkWidget *label_year = gtk_label_new("Year:");
    gtk_widget_set_halign(label_year, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 1, 1, 1);

    GtkWidget *combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 4");
    gtk_grid_attach(GTK_GRID(grid), combo_year, 1, 1, 1, 1);

    // Course List (Scrollable)
    GtkWidget *label_cou = gtk_label_new("Course");
    gtk_widget_set_halign(label_cou, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_cou, 0, 2, 2, 1);
    GtkWidget *label_cu = gtk_label_new("Credit Units");
    gtk_widget_set_halign(label_cu, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), label_cu, 1, 2, 1, 1);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, 350, 250);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 3, 2, 1);

    GtkWidget *list_box = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), list_box);

    // Connect Year Dropdown to dynamically update Course List
    g_signal_connect(combo_year, "changed", G_CALLBACK(on_year_selected_catalogue), list_box);

    // Back Button
    GtkWidget *btn_back = gtk_button_new_with_label("Back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_staff_clicked), NULL);
    gtk_widget_set_halign(btn_back, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 0, 4, 2, 1);
    gtk_widget_show_all(grid);
}

static void on_year_selected_stats(GtkComboBoxText *combo, gpointer user_data) {
    GtkWidget *grid_stats = GTK_WIDGET(user_data);

    // Clear previous content from the grid
    GList *children = gtk_container_get_children(GTK_CONTAINER(grid_stats));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    int year = gtk_combo_box_get_active(GTK_COMBO_BOX(combo)) + 1;

    GList *course_list = get_course_data_by_year(year);
    if (!course_list) {
        GtkWidget *label_no_data = gtk_label_new("No data available for this year.");
        gtk_grid_attach(GTK_GRID(grid_stats), label_no_data, 0, 0, 2, 1);
    } else {
        int row = 0;
        for (GList *iter = course_list; iter != NULL; iter = iter->next) {
            CourseData *course = (CourseData *)iter->data;

            // Course Name Label
            GtkWidget *label_course = gtk_label_new(course->name);
            gtk_widget_set_halign(label_course, GTK_ALIGN_START);
            gtk_widget_set_margin_end(label_course, 10);
            gtk_grid_attach(GTK_GRID(grid_stats), label_course, 0, row, 1, 1);

            // Progress Bar
            GtkWidget *progress_bar = gtk_progress_bar_new();
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), course->enrolled_students / 100.0);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), 
                                      g_strdup_printf("  %d ", course->enrolled_students));
            gtk_widget_set_margin_start(progress_bar, 10);
            gtk_widget_set_margin_end(progress_bar, 10);
            gtk_grid_attach(GTK_GRID(grid_stats), progress_bar, 1, row, 1, 1);

            // Student Count Label
            GtkWidget *label_student_count = gtk_label_new(
                g_strdup_printf(" %d ", course->enrolled_students));
            gtk_widget_set_halign(label_student_count, GTK_ALIGN_END);
            gtk_grid_attach(GTK_GRID(grid_stats), label_student_count, 2, row, 1, 1);

            row++;
        }

        g_list_free_full(course_list, g_free);
    }

    gtk_widget_show_all(grid_stats);
}


static void build_stats_view_page(GtkWidget *grid) {
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    // Title
    GtkWidget *label_title = gtk_label_new("Student Enrollment Statistics");
    gtk_widget_set_halign(label_title, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label_title, 0, 0, 2, 1);

    // Year Dropdown
    GtkWidget *label_year = gtk_label_new("Year:");
    gtk_widget_set_halign(label_year, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 1, 1, 1);

    GtkWidget *combo_year = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 2");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 3");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_year), "Year 4");
    gtk_grid_attach(GTK_GRID(grid), combo_year, 1, 1, 1, 1);

    // Chart Grid for Statistics Display
    GtkWidget *grid_stats = gtk_grid_new();  // <-- Fixed: Changed from label to grid
    gtk_widget_set_halign(grid_stats, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), grid_stats, 0, 2, 2, 1);

    // Connect year selection to display stats
    g_signal_connect(combo_year, "changed", G_CALLBACK(on_year_selected_stats), grid_stats);

    // Back Button
    GtkWidget *btn_back = gtk_button_new_with_label("Back");
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_staff_clicked), NULL);
    gtk_widget_set_halign(btn_back, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), btn_back, 0, 3, 2, 1);

    gtk_widget_show_all(grid);
}


static void show_dashboard(const char *user_type) {
    GtkWidget *grid;

    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 250);
    gtk_container_add(GTK_CONTAINER(main_window), grid);

    if (user_type && g_strcmp0(user_type, "Staff Login") == 0) {
        build_staff_dashboard(grid, user_type);  // Refactored staff section
    } else if (user_type && g_strcmp0(user_type, "Student Login") == 0) {
        // build_course_management_window(main_window,"Student Dashboard");
        build_student_dashboard(grid, user_type);
    }

    gtk_widget_show_all(main_window);
}

static void on_login(GtkWidget *widget, gpointer data) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(entry_username));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
    // const char *user_type = (const char *)data;
    char user_type[20];
    if (validate_login(username, password, user_type)) {
        // printf("ID--->%d",get_logged_in_student_id());
        fflush(stdout);
        show_dashboard(user_type);  // Show Staff or Student Dashboard
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   "Invalid username or password.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Handlers for navigation (to be implemented later)
static void on_enroll_clicked(GtkWidget *widget, gpointer data) {
    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_container_add(GTK_CONTAINER(main_window), grid);
    build_student_enroll_page(grid);    
    gtk_widget_show_all(main_window);
}

static void on_add_clicked(GtkWidget *widget, gpointer data) {
    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_container_add(GTK_CONTAINER(main_window), grid);
    build_add_page(grid);    
    gtk_widget_show_all(main_window);
}

static void on_view_courses_clicked(GtkWidget *widget, gpointer data) {
    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_container_add(GTK_CONTAINER(main_window), grid);
    build_student_view_page(grid);    
    gtk_widget_show_all(main_window);
}

static void on_view_clicked(GtkWidget *widget, gpointer data) {
    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_container_add(GTK_CONTAINER(main_window), grid);
    build_view_page(grid);    
    gtk_widget_show_all(main_window);
}

static void on_view_stats_clicked(GtkWidget *widget, gpointer data) {
    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_container_add(GTK_CONTAINER(main_window), grid);
    build_stats_view_page(grid);    
    gtk_widget_show_all(main_window);
}

static void on_delete_course_clicked(GtkWidget *widget, gpointer data) {
    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_container_add(GTK_CONTAINER(main_window), grid);
    build_student_delete_page(grid);    
    gtk_widget_show_all(main_window);
}

static void on_delete_clicked(GtkWidget *widget, gpointer data) {
    gtk_container_foreach(GTK_CONTAINER(main_window), (GtkCallback)gtk_widget_destroy, NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_container_add(GTK_CONTAINER(main_window), grid);
    build_delete_page(grid);    
    gtk_widget_show_all(main_window);
}

void show_message(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message
    );
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// void show_success_message(const char *message) {
//     GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, 
//                                                GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
//                                                "%s", message);
//     gtk_dialog_run(GTK_DIALOG(dialog));
//     gtk_widget_destroy(dialog);
// }

void show_success_message(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    // ✅ Ensures GTK processes any pending UI updates before continuing
    while (gtk_events_pending()) gtk_main_iteration();
}

void show_error_message(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, 
                                               GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                               "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    while (gtk_events_pending()) gtk_main_iteration();
}


static void on_confirm_enrollment_clicked(GtkWidget *widget, gpointer data) {
    // GtkWidget *combo_unregistered_courses = GTK_WIDGET(data);
    ComboWidgets *widgets = (ComboWidgets *)data;  // Correctly cast the data
    GtkWidget *combo_year = widgets->combo_year;
    GtkWidget *combo_unregistered_courses = widgets->combo_registered_courses;
    const gchar *selected_course = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_unregistered_courses));

    if (selected_course == NULL) {
        show_message("Please select a course before confirming enrollment.");
        return;
    }

    int student_id = get_logged_in_student_id();
    int course_id = get_course_id_by_name(selected_course);
    int  year = get_year_by_id(&course_id);

    if (course_id == -1||year==-1) {
        show_message("Error retrieving course information.");
        return;
    }

    if (enroll_student(student_id, course_id,year)) {
        show_message("Enrollment successful!");
        gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combo_unregistered_courses));

        // Refresh the course list by triggering the year selection callback
        
        on_enroll_year_selected(combo_year, combo_unregistered_courses);
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_year), -1); // Clear year selection
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_unregistered_courses), -1); // Clear course selection
    } else {
        show_message("Enrollment failed. You may already be enrolled.");
    }
}

// Callback function to update course list based on selected year
static void on_enroll_year_selected(GtkWidget *widget, gpointer combo_unregistered_courses) {
    const gchar *selected_year = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));

    if (selected_year == NULL) {
        show_message("Please select a valid year.");
        return;
    }

    // Clear the existing course list
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combo_unregistered_courses));

    // Get student ID (assumes this function exists)
    int student_id = get_logged_in_student_id();

    // Fetch and populate courses for the selected year
    GList *courses = get_unregistered_courses_by_year(student_id, atoi(selected_year));
    for (GList *node = courses; node != NULL; node = node->next) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_unregistered_courses), node->data);
    }
    g_list_free_full(courses, g_free); // Clean up the list
}

static void on_delete_year_selected(GtkWidget *widget, gpointer combo_registered_courses) {
    const gchar *selected_year = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));

    if (selected_year == NULL) {
        show_message("Please select a valid year.");
        return;
    }

    // Fetch and populate courses for the selected year
    int year = atoi(selected_year + (g_ascii_strncasecmp(selected_year, "Year", 4) == 0 ? 5 : 0));
    if (year < 1 || year > 7) {
        show_message("Invalid year selected.");
        return;
    }

    // Clear the existing course list
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(combo_registered_courses));

    // Fetch and populate courses for the selected year
    int student_id = get_logged_in_student_id();
    GList *courses = get_registered_courses_by_year(student_id,atoi(selected_year));
    if (!courses) {
        show_message("No registered courses found for the selected year.");
        return;
    }

    for (GList *node = courses; node != NULL; node = node->next) {
        if(node->data){
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_registered_courses), node->data);
        }
    }

    g_list_free_full(courses, g_free); // Ensure memory cleanup
}

void initialize_gui(GtkApplication *app) {
    main_window = gtk_application_window_new(app);
    show_main_page();
}
