#include "utils.h"

#include <stdio.h>

void load_students(const char *file, student *students, int *student_count) {
  FILE *student_file = fopen(file, "r");

  int position = 0;
  char *line;
  size_t length;

  while (getline(&line, &length, student_file) != -1) {
    sscanf(line, "%d %s %s %d", &students[position].id,
           students[position].first_name, students[position].last_name,
           &students[position].score);

    position++;
  }

  fclose(student_file);

  *student_count = position;
}

void write_students(const char *file, student *students, int student_count) {
  FILE *student_file = fopen(file, "w+");

  for (int i = 0; i < student_count; i++) {
    fprintf(student_file, "%d %s %s %d\n", students[i].id,
            students[i].first_name, students[i].last_name, students[i].score);
  }
}

void print_help() {
  fprintf(stderr, "command format:\n");
  fprintf(stderr, "  <client|server> <hostname> <port>\n");
}

void print_student(student student) {
  printf("%d: %s %s, %d\n", student.id, student.first_name, student.last_name,
         student.score);
}

void print_actions() {
  printf("\n");
  printf("1.) Add an entry\n");
  printf("2.) Query entries by ID\n");
  printf("3.) Query entries by score\n");
  printf("4.) Query all entries\n");
  printf("5.) Delete entry\n");
  printf("6.) Exit program\n");
  printf("> ");
}
