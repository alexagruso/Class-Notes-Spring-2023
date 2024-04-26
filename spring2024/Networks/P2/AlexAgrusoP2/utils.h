#ifndef PROJECT_UTILS
#define PROJECT_UTILS

#include <sys/types.h>
#define MAX_STUDENTS 100
#define MAX_NAME_LENGTH 100
#define MAX_MESSAGE_LENGTH 64

enum actions {
  ADD = 1,
  QUERY_ID = 2,
  QUERY_SCORE = 3,
  QUERY_ALL = 4,
  DELETE = 5,
  EXIT = 6,
};

typedef struct student {
  int id;
  char first_name[MAX_NAME_LENGTH];
  char last_name[MAX_NAME_LENGTH];
  int score;
} student;

void load_students(const char *file, student *students, int *student_count);
void write_students(const char *file, student *students, int student_count);

void print_help();
void print_student(student student);
void print_actions();

#endif // PROJECT_UTILS
