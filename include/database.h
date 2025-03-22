#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>


// Database functions

extern sqlite3 *db;
typedef struct {
    char *course_name;
    int credit_units;
} Course;

typedef struct {
    char name[100];
    int enrolled_students;
} CourseData;

extern int logged_in_student_id;
extern int logged_in_staff_id;
// int get_logged_in_student_id();
// int get_logged_in_staff_id();
void init_database(sqlite3 **db);
void close_database(sqlite3 **db);
int validate_login(const char *username, const char *password, char *user_type);
int execute_sql(const char *sql);
int get_course_id_by_name(const char *course_name);
GList* get_courses_by_year(int year);
int get_year_by_id(const int *course_id);
char* get_name_by_id(const int *course_id);
int enroll_student(int student_id, int course_id,int year);
GList* get_unregistered_courses_by_year(int student_id, int year) ;
GList* get_registered_courses_by_year(int student_id, int year);
GList* get_course_data_by_year(int year);
GList* get_student_courses_by_year(int year);
int delete_course(int student_id, int course_id);
bool create_user(const char *username, const char *password, const char *email, const char *role);
bool authenticate_user(const char *username, const char *password, char *role_out);

#endif // DATABASE_H
