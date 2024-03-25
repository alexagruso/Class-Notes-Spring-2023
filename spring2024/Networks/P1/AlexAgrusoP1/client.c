#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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
  char first_name[100];
  char last_name[100];
  int score;
} student;

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

int main(int argc, char **args) {
  int client_socket = socket(PF_INET, SOCK_STREAM, 0);

  unsigned short port = (unsigned short)atoi(args[2]);

  struct hostent *host_name = gethostbyname(args[1]);

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = *((unsigned long *)(host_name->h_addr_list[0]));
  memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
  socklen_t addr_size = sizeof server_addr;

  int connect_response =
      connect(client_socket, (struct sockaddr *)&server_addr, addr_size);

  if (connect_response != 0) {
    printf("Connect Response: %d\n", connect_response);
    return EXIT_FAILURE;
  }

  uint32_t action;
  char message[64];

  do {
    print_actions();
    scanf("%d", &action);
    printf("\n");

    uint32_t nl_action = htonl(action);
    send(client_socket, &nl_action, sizeof nl_action, 0);

    if (action == ADD) {
      student new;

      printf("Enter id: ");
      scanf("%d", &new.id);

      printf("Enter first name: ");
      scanf("%s", new.first_name);

      printf("Enter last name: ");
      scanf("%s", new.last_name);

      printf("Enter score: ");
      scanf("%d", &new.score);

      send(client_socket, &new, sizeof(student), 0);

      printf("\nCreated new student.\n\n");

    } else if (action == QUERY_ID) {
      int id;
      printf("Enter id: ");
      scanf("%d", &id);

      send(client_socket, &id, sizeof(int), 0);

      student result;
      recv(client_socket, &result, sizeof(student), 0);

      if (result.id >= 0) {
        printf("\nFound:\n");
        print_student(result);
        printf("\n");
      } else {
        printf("\nNot Found\n\n");
      }
    } else if (action == QUERY_SCORE) {
      int score;
      printf("Enter score: ");
      scanf("%d", &score);

      send(client_socket, &score, sizeof(int), 0);

      int found;
      recv(client_socket, &found, sizeof(int), 0);

      student result[found];

      for (int i = 0; i < found; i++) {
        student current;
        recv(client_socket, &current, sizeof(student), 0);
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
      recv(client_socket, &student_count, sizeof(int), 0);

      for (int i = 0; i < student_count; i++) {
        recv(client_socket, &current, sizeof(student), 0);
        print_student(current);
      }
    } else if (action == DELETE) {
      int id;
      printf("Enter id: ");
      scanf("%d", &id);

      send(client_socket, &id, sizeof(int), 0);

      student result;
      recv(client_socket, &result, sizeof(student), 0);

      if (result.id >= 0) {
        printf("\nDeleted:\n");
        print_student(result);
        printf("\n");
      } else {
        printf("\nNot Found\n\n");
      }
    }
  } while (action != 6);

  close(client_socket);

  return EXIT_SUCCESS;
}