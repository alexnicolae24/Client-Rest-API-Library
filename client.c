#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>

#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define IP_SERVER "34.254.242.81"
#define PORT_SERVER 8080

struct User
{
  char username[50];
  char password[50];
  JSON_Value *val;
  JSON_Object *obj;
};

struct Book
{
  int id;
  char title[100];
  char author[100];
  char genre[100];
  int page_count;
  char publisher[100];
  JSON_Value *val;
  JSON_Object *obj;
};

int commandToint(char command[25])
{
  if (strcmp(command, "register") == 0)
    return 1;
  else if (strcmp(command, "login") == 0)
    return 2;
  else if (strcmp(command, "enter_library") == 0)
    return 3;
  else if (strcmp(command, "get_books") == 0)
    return 4;
  else if (strcmp(command, "get_book") == 0)
    return 5;
  else if (strcmp(command, "add_book") == 0)
    return 6;
  else if (strcmp(command, "delete_book") == 0)
    return 7;
  else if (strcmp(command, "logout") == 0)
    return 8;
  return 0;
}

int main(int argc, char *argv[])
{
  struct User u;
  struct Book b;
  char command[25], sess_cookie[25], tok[BUFLEN];
  char *users[1], *cookies[1], *books[1];
  int logged = 0, in_lib = 0;
  int sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0); //deschiderea unei conexiuni cu serverul
  while (1)
  {
    //Citirea comenzii de la stdin
    fgets(command, 25, stdin);
    command[strlen(command) - 1] = '\0';

    //Conditia de iesire din bucla
    if (strcmp(command, "exit") == 0)
    {
      break;
    }
    else
    {
      //Deschiderea unei conexiuni cu serverul
      sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
      //Transformarea comenzii in numarul intreg corespunzator
      int n = commandToint(command);
      switch (n)
      {
      case 0:
        break;

      case 1:
        //Introducerea credentialelor de la tastatura
        printf("username=");
        scanf("%s", u.username);
        printf("password=");
        scanf("%s", u.password);
        //Parsarea datelor
        u.val = json_value_init_object();
        u.obj = json_value_get_object(u.val);
        json_object_set_string(u.obj, "username", u.username);
        json_object_set_string(u.obj, "password", u.password);
        users[0] = json_serialize_to_string(u.val);
        //Efectuarea cererii de tip POST
        char *mess1 = compute_post_request(IP_SERVER, "/api/v1/tema/auth/register", "application/json", users, 1, NULL, 0, NULL);
        send_to_server(sockfd, mess1);
        char *res1 = receive_from_server(sockfd);
        //Pastrarea cookie-ului care verifica daca utilizatorul este inregistrat
        char *ptr1 = strstr(res1, "error");
        //Verificare daca utilizatorul exista deja
        if (ptr1 != NULL)
        {
          printf("Username deja folosit! Schimba username-ul.\n");
        }
        else
        {
          printf("Gata! Acum esti inregistrat.\n");
        }
        break;

      case 2:
        //Verificarea daca utilizatorul este logat cu alt cont
        if (logged == 0)
        {
          //Setarea variabilei de logare
          logged = 1;
          //Introducerea credentialelor de tastatura
          printf("username=");
          scanf("%s", u.username);
          printf("password=");
          scanf("%s", u.password);
          //Parsarea datelor
          u.val = json_value_init_object();
          u.obj = json_value_get_object(u.val);
          json_object_set_string(u.obj, "username", u.username);
          json_object_set_string(u.obj, "password", u.password);
          users[0] = json_serialize_to_string(u.val);
          //Efectuarea cererii de tip POST
          char *mess2 = compute_post_request(IP_SERVER, "/api/v1/tema/auth/login", "application/json", users, 1, NULL, 0, NULL);
          send_to_server(sockfd, mess2);
          char *res2 = receive_from_server(sockfd);
          //Pastrarea cookie-ului care verifica daca utilizatorul este logat
          char *ptr2 = strstr(res2, "Set-Cookie: ");
          //Verificare daca utilizatorul se logheaza sau nu
          if (ptr2 == NULL)
          {
            printf("Logare esuata!\n");
            logged = 0;
            in_lib = 0;
          }
          else
          {
            //Extragerea cookie-ului din campul "Set-Cookie: " al cererii
            printf("Logare reusita!\n");
            strtok(ptr2, ";");
            ptr2 += strlen("Set-Cookie: ");
            strcpy(sess_cookie, ptr2);
            cookies[0] = ptr2;
          }
        }
        else
        {
          printf("Esti deja logat!Delogheaza-te daca vrei sa te loghezi cu alt cont!\n");
        }
        break;

      case 3:
        //Verificarea daca utilizatorul este logat
        if (logged == 1)
        {
          in_lib = 1;
          //Efectuarea cererii de tip GET
          char *mess3 = compute_get_request(IP_SERVER, "/api/v1/tema/library/access", NULL, cookies, 1, tok);
          send_to_server(sockfd, mess3);
          char *res3 = receive_from_server(sockfd);
          //Verificare daca utilizatorul a accesat bibilioteca
          char *ptr3 = strstr(res3, "token");
          if (ptr3 == NULL)
          {
            printf("Nu s-a putut accesa biblioteca!\n");
            in_lib = 0;
          }
          else
          {
            //Extragerea tokenului de acces in biblioteca
            memset(tok, 0, BUFLEN);
            ptr3 += 8;
            strcpy(tok, ptr3);
            tok[strlen(tok) - 2] = '\0';
            printf("Ai intrat in biblioteca!\n");
          }
        }
        else
        {
          printf("Mai intai logheaza-te!.\n");
        }
        break;

      case 4:
        //Verificare daca utilizatorul a accesat biblioteca
        if (in_lib == 1)
        {
          //Efectuare unei cereri de tip GET
          char *mess4 = compute_get_request(IP_SERVER, "/api/v1/tema/library/books", NULL, cookies, 1, tok);
          send_to_server(sockfd, mess4);
          char *res4 = receive_from_server(sockfd);
          //Extragerea cartior prezente in biblioteca
          char *ptr4 = strstr(res4, "[");
          //Afisarea cartilor din biblioteca
          printf("%s\n", ptr4);
        }
        else
        {
          printf("Mai intai acceseaza biblioteca!\n");
        }
        break;

      case 5:
              //Verificare daca utilizatorul a accesat biblioteca
              if (in_lib == 1) {
                b.id = 0;
                //Introducere id-ului cartii care se doreste a fi luata
                printf("id=");
                char id_str[100];
                scanf("%s", id_str);
                b.id = atoi(id_str);
                while (b.id <= 0){
                  printf("Id-ul are formatul gresit!Scrie alt id!\n");
                  printf("id=");
                  scanf("%s", id_str);
                  b.id = atoi(id_str);
                }
               //Configurarea caii de acces spre cartea cu id-ul dat
               char path[BUFLEN];
               sprintf(path,"/api/v1/tema/library/books/%d",b.id);
               //Efectuarea unei cereri de tip GET pentru calea de acces                  
               char *mess5 = compute_get_request(IP_SERVER, path, NULL, cookies, 1, tok);
               send_to_server(sockfd, mess5);
               char *res5 = receive_from_server(sockfd);
               //Verificare daca exista carti cu id-ul cerut in cerere
               if (strstr(res5, "{") == NULL) {
                printf("Nu exista nicio carte cu acest id!\n");
               } 
               else {
                printf("%s\n", strstr(res5, "{"));
               }
              } 
              else {
                printf("Mai intai acceseaza biblioteca!\n");
              }
            break;

      case 6:
        //Verificare daca utilizatorul a accesat biblioteca
        if (in_lib == 1)
        {
          //Introducerea caracterisiticilor cartii si verificarea formatarii
          printf("title=");
          fgets(b.title, sizeof(b.title), stdin);
          b.title[strlen(b.title) - 1] = '\0';
          if (strcmp(b.title, "") == 0)
          {
            printf("Nu se respecta formatarea!\n");
            printf("title=");
            fgets(b.title, sizeof(b.title), stdin);
          }

          printf("author=");
          scanf("%s", b.author);
          if (strcmp(b.author, "") == 0)
          {
            printf("Nu se respecta formatarea!\n");
            printf("author=");
            scanf("%s", b.author);
          }

          printf("genre=");
          scanf("%s", b.genre);
          if (strcmp(b.genre, "") == 0)
          {
            printf("Nu se respecta formatarea!\n");
            printf("genre=");
            scanf("%s", b.genre);
          }

          printf("publisher=");
          scanf("%s", b.publisher);
          if (strcmp(b.publisher, "") == 0)
          {
            printf("Nu se respecta formatarea!\n");
            printf("publisher=");
            scanf("%s", b.publisher);
          }

          printf("page_count=");
          char pg[100];
          scanf("%s", pg);
          b.page_count = atoi(pg);

          while (b.page_count <= 0)
          {
            printf("Nu se respecta formatarea!\n");
            printf("page_count=");
            scanf("%s", pg);
            b.page_count = atoi(pg);
          }

          //Transformarea numarului de pagini din string in int
          char pages[BUFLEN];
          sprintf(pages, "%d", b.page_count);
          //Parsarea datelor
          b.val = json_value_init_object();
          b.obj = json_value_get_object(b.val);
          json_object_set_string(b.obj, "title", b.title);
          json_object_set_string(b.obj, "author", b.author);
          json_object_set_string(b.obj, "genre", b.genre);
          json_object_set_string(b.obj, "page_count", pages);
          json_object_set_string(b.obj, "publisher", b.publisher);
          books[0] = json_serialize_to_string(b.val);
          //Efectuarea unei cereri de tip post
          char *mess6 = compute_post_request(IP_SERVER, "/api/v1/tema/library/books", "application/json", books, 1, NULL, 0, tok);
          send_to_server(sockfd, mess6);
          receive_from_server(sockfd);
          printf("Carte adaugata cu succes!\n");
        }
        else
        {
          printf("Mai intai acceseaza biblioteca!\n");
        }
        break;

      case 7:
        //Verificare daca utilizatorul a accesat biblioteca
        if (in_lib == 1)
        {
          b.id = 0;
          //Introducere id-ului cartii care se doreste a fi stearsa
          printf("id=");
          char id_str[100];
          scanf("%s", id_str);
          b.id = atoi(id_str);
          while (b.id <= 0){
            printf("Id-ul are formatul gresit!Scrie alt id!\n");
            printf("id=");
            scanf("%s", id_str);
            b.id = atoi(id_str);
          }
          //Configurarea caii de acces spre cartea cu id-ul dat
          char path[BUFLEN];
          sprintf(path, "/api/v1/tema/library/books/%d", b.id);
          //Efectuarea unei cereri de tip DELETE
          char *mess7 = compute_delete_request(IP_SERVER, path, NULL, cookies, 1, tok);
          send_to_server(sockfd, mess7);
          char *res7 = receive_from_server(sockfd);
          //Verificarea pentru eroare
          char *ptr7 = strstr(res7, "error");
          if (ptr7 != NULL)
          {
            printf("Nu s-a gasit nicio carte cu acest id!\n");
          }
          else
            printf("Carte stearsa cu succes!\n");
        }
        else
        {
          printf("Mai intai acceseaza biblioteca!\n");
        }
        break;

      case 8:
        //Verificarea daca utilizatorul este logat
        if (logged == 1)
        {
          //Efectuarea unei cereri de tip GET
          char *mess8 = compute_get_request(IP_SERVER, "/api/v1/tema/auth/logout", NULL, cookies, 1, tok);
          send_to_server(sockfd, mess8);
          receive_from_server(sockfd);
          //Resetarea variabilelor
          logged = 0;
          in_lib = 0;
          printf("Ai fost deconectat!\n");
        }
        else
        {
          printf("Mai intai logheaza-te!\n");
        }
        break;
      }
      close_connection(sockfd);
    }
  }
  return 0;
}