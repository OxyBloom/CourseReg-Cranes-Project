#include "database.h"
sqlite3 *db;
int logged_in_student_id = -1;
int logged_in_staff_id = -1;
void init_database(sqlite3 **db) {
    
    int rc = sqlite3_open("/home/david/coursereg.db", db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return;
    }

    const char *sql =
        "CREATE TABLE IF NOT EXISTS staff ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "name TEXT, "
        "position TEXT);"

        "CREATE TABLE IF NOT EXISTS students ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "name TEXT, "
        "reg_number TEXT UNIQUE NOT NULL, "
        "year INTEGER, "
        "department TEXT);"

        "CREATE TABLE IF NOT EXISTS courses ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "credit INTEGER NOT NULL, "
        "year INTEGER NOT NULL);"

        "CREATE TABLE IF NOT EXISTS enrollments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "student_id INTEGER, "
        "course_id INTEGER, "
        "enrollment_date TEXT, "
        "FOREIGN KEY (student_id) REFERENCES students(id), "
        "FOREIGN KEY (course_id) REFERENCES courses(id));"

        "CREATE TABLE IF NOT EXISTS course_stats ("
        "name TEXT, "
        "year INTEGER, "
        "enrollment_count INTEGER, "
        "UNIQUE (name, year));"

        "CREATE TABLE IF NOT EXISTS grades ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "student_id INTEGER, "
        "course_id INTEGER, "
        "grade TEXT, "
        "FOREIGN KEY (student_id) REFERENCES students(id), "
        "FOREIGN KEY (course_id) REFERENCES courses(id));"

        "CREATE TABLE IF NOT EXISTS notifications ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER, "
        "message TEXT, "
        "date TEXT);"

        "CREATE TABLE IF NOT EXISTS logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER, "
        "action TEXT, "
        "timestamp TEXT);";

    rc = sqlite3_exec(*db, sql, 0, 0, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(*db));
    }

    // sqlite3_close(db);
}

void close_database(sqlite3 **db) {
    if (*db) {
        sqlite3_close(*db);
        *db=NULL;
    }
}

int execute_sql(const char *sql) {
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}

int validate_login(const char *username, const char *password, char *user_type) {
    printf("Checking login for username: %s in table: %s\n", username, password);
    // printf("Student ID: %d\n",logged_in_student_id);
    // printf("Staff ID: %d\n",logged_in_staff_id);
    fflush(stdout);
    
    sqlite3_stmt *stmt;
    const char *sql;

    // Check Staff Login
    sql = "SELECT id FROM staff WHERE username = ? AND password = ?";

    // printf("Checking login for username: %s and password: %s\n", username,password);
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
        int result = 0;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            logged_in_staff_id = sqlite3_column_int(stmt, 0);
            // printf("Staff ID: %d",logged_in_staff_id);
            strcpy(user_type, "Staff Login");
            fflush(stdout);
            sqlite3_finalize(stmt);
            return 1;
        }
    }
    sqlite3_finalize(stmt);

    // Check Student Login
    sql = "SELECT id FROM students WHERE username = ? AND password = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
        // printf("STudent ID: %d",logged_in_student_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            logged_in_student_id = sqlite3_column_int(stmt, 0);  // Save Student ID
            // printf("STudent ID: %d",logged_in_student_id);
            strcpy(user_type, "Student Login");
            fflush(stdout);
            sqlite3_finalize(stmt);
            return 1;
        }
    }
    sqlite3_finalize(stmt);

    return 0;
}

int get_course_id_by_name(const char *course_name) {
    const char *sql = "SELECT id FROM courses WHERE name = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (get_course_id): %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, course_name, -1, SQLITE_STATIC);

    int course_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        course_id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return course_id;
}

GList* get_unregistered_courses_by_year(int student_id, int year) {
    const char *sql = 
        "SELECT name FROM courses "
        "WHERE year = ? "
        "AND id NOT IN (SELECT course_id FROM enrollments WHERE student_id = ?)";

    sqlite3_stmt *stmt;
    GList *course_list = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (get_unregistered_courses_by_year): %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, year);
    sqlite3_bind_int(stmt, 2, student_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *course_name = (const char *)sqlite3_column_text(stmt, 0);
        course_list = g_list_append(course_list, g_strdup(course_name));
    }

    sqlite3_finalize(stmt);
    return course_list;
}

GList* get_registered_courses_by_year(int student_id, int year) {
    const char *sql = 
        "SELECT name FROM courses "
        "WHERE year = ? "
        "AND id IN (SELECT course_id FROM enrollments WHERE student_id = ?)";

    sqlite3_stmt *stmt;
    GList *course_list = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (get_registered_courses_by_year): %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, year);
    sqlite3_bind_int(stmt, 2, student_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *course_name = (const char *)sqlite3_column_text(stmt, 0);
        course_list = g_list_append(course_list, g_strdup(course_name));
    }

    sqlite3_finalize(stmt);
    return course_list;
}

int enroll_student(int student_id, int course_id,int year) {
    const char *sql_check = "SELECT COUNT(*) FROM enrollments WHERE student_id = ? AND course_id = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql_check, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (check): %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_int(stmt, 2, course_id);

    if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
        printf("Student is already enrolled in this course.\n");
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_finalize(stmt);

    // Insert the enrollment
    const char *sql_insert = "INSERT INTO enrollments (student_id, course_id, enrollment_date) VALUES (?, ?, DATE('now'))";
    
    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (insert): %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_int(stmt, 2, course_id);
    const char *course_name = get_name_by_id(&course_id);

    char query[256];
    

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        snprintf(query, sizeof(query),
            "INSERT INTO course_stats (name, year, enrollment_count) "
            "VALUES ('%s', %d, 1) "
            "ON CONFLICT(name, year) "
            "DO UPDATE SET enrollment_count = enrollment_count + 1;", 
            course_name, year);

        if (sqlite3_exec(db, query, NULL, NULL, NULL) != SQLITE_OK) {
        g_print("Error updating course_stats: %s\n", sqlite3_errmsg(db));
        } else {
        g_print("\nEnrollment count updated successfully.\n");
        }
        printf("\nEnrollment successful!\n");
        sqlite3_finalize(stmt);
        return 1;
    }

    fprintf(stderr, "Failed to enroll student.\n");
    sqlite3_finalize(stmt);
    return 0;
}

GList* get_courses_by_year(int year) {
    GList *course_list = NULL;

    // Sample query (modify as per your DB setup)
    char query[256];
    snprintf(query, sizeof(query), "SELECT name, credit FROM courses WHERE year = %d;", year);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *course_name = (const char *)sqlite3_column_text(stmt, 0);
            int course_credit = sqlite3_column_int(stmt, 1);

            // Create a Course object
            Course *course = g_malloc(sizeof(Course));
            course->course_name = g_strdup(course_name);
            course->credit_units = course_credit;

            course_list = g_list_append(course_list, course);
        }
    } else {
        g_printerr("SQL Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return course_list;
}

GList* get_student_courses_by_year(int year) {
    GList *course_list = NULL;

    // Sample query (modify as per your DB setup)
    char query[256];
    snprintf(query, sizeof(query), "SELECT c.name, c.credit FROM courses c JOIN enrollments e ON c.id = e.course_id WHERE e.student_id = %d AND c.year = %d;", logged_in_student_id,year);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *course_name = (const char *)sqlite3_column_text(stmt, 0);
            int course_credit = sqlite3_column_int(stmt, 1);

            // Create a Course object
            Course *course = g_malloc(sizeof(Course));
            course->course_name = g_strdup(course_name);
            course->credit_units = course_credit;

            course_list = g_list_append(course_list, course);
        }
    } else {
        g_printerr("SQL Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return course_list;
}


GList* get_course_data_by_year(int year) {
    GList *course_list = NULL;

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT name, enrollment_count "
             "FROM course_stats "
             "WHERE year = %d;", year);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            CourseData *course = g_new(CourseData, 1);
            strncpy(course->name, (const char *)sqlite3_column_text(stmt, 0), sizeof(course->name));
            course->enrolled_students = sqlite3_column_int(stmt, 1);

            course_list = g_list_append(course_list, course);
        }
    }

    sqlite3_finalize(stmt);
    return course_list;
}

int get_year_by_id(const int *course_id){
    const char *sql = "SELECT year FROM courses WHERE id = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (get_course_year): %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // sqlite3_bind_text(stmt, 1, (const char *)course_id, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 1, *course_id);

    int course_year = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        course_year = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return course_year;
}

char* get_name_by_id(const int *course_id){
    const char *sql = "SELECT name FROM courses WHERE id = ?";
    sqlite3_stmt *stmt;
    char *course_name = NULL; // To hold the course name


    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (get_course_year): %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // sqlite3_bind_text(stmt, 1, (const char *)course_id, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 1, *course_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *name = sqlite3_column_text(stmt, 0);
        if (name) {
            course_name = g_strdup((const char *)name);  // Duplicates the string safely
        }
    }

    sqlite3_finalize(stmt);
    return course_name;
}

int delete_course(int student_id, int course_id) {
    const char *sql = 
        "DELETE FROM enrollments "
        "WHERE student_id = ? AND course_id = ?";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error (delete_course): %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, student_id);
    sqlite3_bind_int(stmt, 2, course_id);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        return 1; // Deletion successful
    }

    fprintf(stderr, "Failed to remove course.\n");
    sqlite3_finalize(stmt);
    return 0;
}






