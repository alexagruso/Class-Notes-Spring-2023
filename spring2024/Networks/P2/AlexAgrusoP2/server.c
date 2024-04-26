#include "utils.h"

#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

student students[MAX_STUDENTS];
int student_count = 0;

int main(int argc, char **args) {
  if (argc < 2) {
    print_help();
    return EXIT_FAILURE;
  }

  int new_socket;

  if ((new_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "ERROR: failed to create socket");
    return EXIT_FAILURE;
  }

  struct sockaddr_in server;

  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(args[2]));
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(new_socket, (struct sockaddr *)&server, sizeof(server))) {
    fprintf(stderr, "ERROR: failed to bind to socket");
    return EXIT_FAILURE;
  }

  struct sockaddr_in client;
  uint client_address_size = sizeof(client);

  int action;

  load_students("students.txt", students, &student_count);

  do {
    recvfrom(new_socket, &action, sizeof(action), 0, (struct sockaddr *)&client,
             &client_address_size);

    if (action == ADD) {
      student new;
      recvfrom(new_socket, &new, sizeof(student), 0, (struct sockaddr *)&client,
               &client_address_size);

      students[student_count] = new;
      student_count++;
    } else if (action == QUERY_ID) {
      int id;
      recvfrom(new_socket, &id, sizeof(id), 0, (struct sockaddr *)&client,
               &client_address_size);

      student result;
      int found = 0;

      for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) {
          result = students[i];
          found = 1;
        }
      }

      if (!found) {
        result.id = -1;
      }

      sendto(new_socket, &result, sizeof(result), 0, (struct sockaddr *)&client,
             client_address_size);
    } else if (action == QUERY_SCORE) {
      int score;
      recvfrom(new_socket, &score, sizeof(score), 0, (struct sockaddr *)&client,
               &client_address_size);

      student result[student_count];
      int found = 0;

      for (int i = 0; i < student_count; i++) {
        if (students[i].score >= score) {
          result[found] = students[i];
          found++;
        }
      }

      sendto(new_socket, &found, sizeof(found), 0, (struct sockaddr *)&client,
             client_address_size);

      for (int i = 0; i < found; i++) {
        sendto(new_socket, &result[i], sizeof(student), 0,
               (struct sockaddr *)&client, client_address_size);
      }
    } else if (action == QUERY_ALL) {
      sendto(new_socket, &student_count, sizeof(student_count), 0,
             (struct sockaddr *)&client, client_address_size);

      for (int i = 0; i < student_count; i++) {
        sendto(new_socket, &students[i], sizeof(student), 0,
               (struct sockaddr *)&client, client_address_size);
      }
    } else if (action == DELETE) {
      int id;
      recvfrom(new_socket, &id, sizeof(id), 0, (struct sockaddr *)&client,
               &client_address_size);

      student result;
      int found = 0;

      for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) {
          result = students[i];
          found = 1;

          students[i] = students[student_count - 1];
          student_count--;
        }
      }

      if (!found) {
        result.id = -1;
      }

      sendto(new_socket, &result, sizeof(student), 0,
             (struct sockaddr *)&client, client_address_size);
    }
  } while (action != EXIT);

  write_students("students.txt", students, student_count);
  printf("Exiting program.\n");

  close(new_socket);

  return EXIT_SUCCESS;
}
