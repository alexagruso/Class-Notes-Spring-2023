#include "utils.h"

#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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

  struct hostent *hostnm = gethostbyname(args[1]);
  struct sockaddr_in server;

  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(args[2]));
  server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

  uint server_address_size = sizeof(server);

  int action;

  do {
    print_actions();
    scanf("%d", &action);
    printf("\n");

    sendto(new_socket, &action, sizeof(action), 0, (struct sockaddr *)&server,
           server_address_size);

    if (action == ADD) {
      student new_student;

      printf("Enter id: ");
      scanf("%d", &new_student.id);

      printf("Enter first name: ");
      scanf("%s", new_student.first_name);

      printf("Enter last name: ");
      scanf("%s", new_student.last_name);

      printf("Enter score: ");
      scanf("%d", &new_student.score);

      sendto(new_socket, &new_student, sizeof(new_student), 0,
             (struct sockaddr *)&server, server_address_size);

      printf("\nCreated new student.\n\n");

    } else if (action == QUERY_ID) {
      int id;
      printf("Enter id: ");
      scanf("%d", &id);

      sendto(new_socket, &id, sizeof(id), 0, (struct sockaddr *)&server,
             server_address_size);

      student result;
      recvfrom(new_socket, &result, sizeof(result), 0,
               (struct sockaddr *)&server, &server_address_size);

      if (result.id >= 0) {
        printf("\nFound:\n");
        print_student(result);
        printf("\n");
      } else {
        printf("\nNot Found\n\n");
      }
    } else if (action == QUERY_SCORE) {
      int score;
      printf("Enter minimum score: ");
      scanf("%d", &score);

      sendto(new_socket, &score, sizeof(score), 0, (struct sockaddr *)&server,
             server_address_size);

      int found;
      recvfrom(new_socket, &found, sizeof(found), 0, (struct sockaddr *)&server,
               &server_address_size);

      student result[found];

      for (int i = 0; i < found; i++) {
        student current;
        recvfrom(new_socket, &current, sizeof(current), 0,
                 (struct sockaddr *)&server, &server_address_size);
        result[i] = current;
      }

      if (found >= 1) {
        printf("\nFound:\n");
        for (int i = 0; i < found; i++) {
          print_student(result[i]);
        }
      } else {
        printf("\nNot Found\n\n");
      }
    } else if (action == QUERY_ALL) {
      printf("All students:\n");
      student current;

      int student_count;
      recvfrom(new_socket, &student_count, sizeof(student_count), 0,
               (struct sockaddr *)&server, &server_address_size);

      for (int i = 0; i < student_count; i++) {
        recvfrom(new_socket, &current, sizeof(current), 0,
                 (struct sockaddr *)&server, &server_address_size);
        print_student(current);
      }
    } else if (action == DELETE) {
      int id;
      printf("Enter id: ");
      scanf("%d", &id);

      sendto(new_socket, &id, sizeof(id), 0, (struct sockaddr *)&server,
             server_address_size);

      student result;
      recvfrom(new_socket, &result, sizeof(result), 0,
               (struct sockaddr *)&server, &server_address_size);

      if (result.id >= 0) {
        printf("\nDeleted:\n");
        print_student(result);
        printf("\n");
      } else {
        printf("\nNot Found\n\n");
      }
    }
  } while (action != EXIT);

  close(new_socket);

  return EXIT_SUCCESS;
}
