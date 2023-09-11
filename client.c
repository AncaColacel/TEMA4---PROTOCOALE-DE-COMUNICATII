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
#include <json-c/json.h>

#define MAXBUF 20

// aceasta metoda va verfifica daca un string contine spatiu
// si va intoarce 0 daca nu contine
int verifica_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {
            return 1;
        }
    }
    return 0;
}

// aceasta metoda verifica daca un string nu contine cifre
int verifica_cifre(char* str) {
    int lungime = strlen(str);
    for (int i = 0; i < lungime; i++) {
        if ((str[i] >= '0' && str[i] <= '9')) {
            // nu contine doar litere
            return 0;
        }
    }
    // contine doar litere
    return 1; 
}

// aceasta metoda verifica daca un string contine doar cifre
int contineDoarCifre(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;  
        }
    }
    return 1;  
}


int main(int argc, char *argv[])
{
    char *message;
    char *response;
    char **cookie_final;
    char *token_final;
    char comanda[BUFLEN];
    int sockfd;
    char user[BUFLEN], parola[BUFLEN];
    char title[BUFLEN], author[BUFLEN], genre[BUFLEN], publisher[BUFLEN];
    // int page_count;
    char ID[20];
    char *contentType = "application/json";
    // dezactivare bufferizare
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
     setvbuf(stdin, NULL, _IONBF, BUFSIZ);
    // alocare memorie stringuri
    cookie_final = calloc(5, sizeof(char **));
    cookie_final[0] = calloc(200, sizeof(char *));
    token_final = calloc(300, sizeof(char *));

citire_comenzi:
    while (1) {
        printf("**********************************************\n");
        fgets(comanda, MAXBUF, stdin);
        if (strncmp(comanda, "register", 8) == 0) {
            printf("username=");
            fgets(user, BUFLEN, stdin);
            user[strcspn(user, "\n")] = 0;
            printf("password=");
            fgets(parola, BUFLEN, stdin);
            parola[strcspn(parola, "\n")] = 0;
            // veriric ca userul si parola sa nu contina spatii
            if (verifica_string(user) == 1 || verifica_string(parola) == 1) {
                printf("Ati introdus un user sau parola cu caractere neacceptate!\n");
                goto citire_comenzi;
            }
            // formare un obiect JSON
            json_object *json_obj = json_object_new_object();
            json_object_object_add(json_obj, "username", json_object_new_string(user));
            json_object_object_add(json_obj, "password", json_object_new_string(parola));
            // formare un buffer JSON din obiectul JSON
            const char *json_str = json_object_to_json_string(json_obj);
            char *contentType = "application/json";
            sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request((char *)"34.254.242.81", (char *)"/api/v1/tema/auth/register", contentType, (char **)&json_str, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            if (response == NULL)
            {
                perror("receive_from_server");
                exit(EXIT_FAILURE);
            }
            char *firstLine = strtok(response, "\r\n");
            if (strcmp(firstLine, "HTTP/1.1 201 Created") == 0)
            {
                printf("201 - OK - Utilizator înregistrat cu succes!\n");
                free(message);
            }
            else
            {
                printf("Credentiale deja folosite :(. Mai incearca!\n");
            }
        }

        if (strncmp(comanda, "exit", 4) == 0)
        {
            printf("**********************************************\n");
            printf("S-a introdus exit\n");
            close_connection(sockfd);
            exit(0);
        }
            if (strncmp(comanda, "login", 5) == 0) {
                // verific valoarea cookie-ului ca sa nu ajung sa ma loghez de 2 ori
                if (strlen(cookie_final[0]) == 0) {
                    // introducere credentiale
                    printf("username=");
                    fgets(user, BUFLEN, stdin);
                    user[strcspn(user, "\n")] = 0;
                    printf("password=");
                    fgets(parola, BUFLEN, stdin);
                    parola[strcspn(parola, "\n")] = 0;
                    // verific ca userul si parola sa nu aiba spatii
                    if (verifica_string(user) == 1 || verifica_string(parola) == 1) {
                        printf("Ati introdus un user sau parola cu caractere neacceptate!\n");
                        goto citire_comenzi;
                    }
                    // formare un obiect JSON
                    json_object *json_obj = json_object_new_object();
                    json_object_object_add(json_obj, "username", json_object_new_string(user));
                    json_object_object_add(json_obj, "password", json_object_new_string(parola));
                    // formare un buffer JSON din obiectul JSON
                    const char *json_str = json_object_to_json_string(json_obj);
                    char *contentType = "application/json";
                    sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                    message = compute_post_request((char *)"34.254.242.81", (char *)"/api/v1/tema/auth/login", contentType, (char **)&json_str, 1, NULL, 0, NULL);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);
                    if (response == NULL)
                    {
                        perror("receive_from_server");
                        exit(EXIT_FAILURE);
                    }
                    // caut pozitia in care incepe mesajul JSON
                    // ma folosesc de faptul ca linia este de tip json si incepe cu {
                    char *json_start = strstr(response, "{");
                    // extrag lungimea mesajului JSON din randul "Content-Length"
                    char *content_length_start = strstr(response, "Content-Length");
                    // obtin lungimea mesajului json adaugand la inceputul liniei textul de dinainte
                    // de numarul efectiv si fac conversie
                    int content_length = atoi(content_length_start + strlen("Content-Length:"));
                    char json_msg[content_length + 1];
                    strncpy(json_msg, json_start, content_length);
                    json_msg[content_length] = '\0';
                    if (strncmp(json_msg, "{\"error\":\"Credentials are not good!\"}", 37) == 0)
                    {
                        printf("Credentialele introduse nu se potrivesc :(. Mai incearca!\n");
                    }
                    if (strncmp(json_msg, "{\"error\":\"No account with this username!\"}", 42) == 0)
                    {
                        printf("Nu exista cont pentru acest user!\n");
                    }
                    else if ((strncmp(json_msg, "{\"error\":\"Credentials are not good!\"}", 37) != 0) && (strncmp(json_msg, "{\"error\":\"No account with this username!\"}", 42) != 0))
                    {
                        printf("200 - OK - Bun venit!\n");
                    }
                    // imi salvez cookie-ul ca sa pot demonstra ca sunt autentificata
                    // la biblioteca
                    char *set_cookie = strstr(response, "Set-Cookie: ");
                    if (set_cookie != NULL)
                    {
                        // sar peste partea "Set-Cookie"
                        set_cookie = set_cookie + strlen("Set-Cookie: ");
                        // caut finalul de linie
                        char *end_of_line = strchr(set_cookie, '\r');
                        if (end_of_line != NULL)
                        {
                            // imi scot informatia cookie care ma intereseaza
                            char cookie_string[end_of_line - set_cookie + 1];
                            strncpy(cookie_string, set_cookie, end_of_line - set_cookie);
                            cookie_string[end_of_line - set_cookie] = '\0';
                            strcpy(cookie_final[0], cookie_string);
                        }
                    }
                }
                else {
                    printf("Sunteti deja logat!\n");
                }
            }
        // introducere comanda enter_library
        if (strncmp(comanda, "enter_library", 13) == 0)
        {
            sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request((char *)"34.254.242.81", (char *)"/api/v1/tema/library/access", NULL, cookie_final, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            if (response == NULL)
            {
                perror("receive_from_server");
                exit(EXIT_FAILURE);
            }

            // caut pozitia in care incepe mesajul JSON
            // caut șirul "token"
            char *token_start = strstr(response, "\"token\":\"");
            if (token_start != NULL)
            {
                // sar peste șirul "token"
                token_start = token_start + strlen("\"token\":\"");
                // caut  următorul caracter "
                char *token_end = strchr(token_start, '"');
                if (token_end != NULL)
                {
                    // calculez lungimea token-ului
                    int token_len = token_end - token_start;
                    // aloc memorie pentru token
                    char token[token_len + 1];
                    strncpy(token, token_start, token_len);
                    token[token_len] = '\0';
                    strcpy(token_final, token);
                }
            }
            // caut pozitia in care incepe mesajul JSON
            // ma folosesc de faptul ca linia este de tip json si incepe cu {
            char *json_start = strstr(response, "{");
            // extrag lungimea mesajului JSON din randul "Content-Length"
            char *content_length_start = strstr(response, "Content-Length");
            // obtin lungimea mesajului json adaugand la inceputul liniei textul de dinainte
            // de numarul efectiv si fac conversie
            int content_length = atoi(content_length_start + strlen("Content-Length:"));
            char json_msg[content_length + 1];
            strncpy(json_msg, json_start, content_length);
            json_msg[content_length] = '\0';
            if (strncmp(json_msg, "{\"error\":\"You are not logged in!\"}", 37) == 0)
            {
                printf("Nu sunteti logat!\n");
            }
            else if (strncmp(json_msg, "{\"error\":\"You are not logged in!\"}", 37) != 0)
            {
                printf("Bine ati venit! Ati primit acces la biblioteca.\n");
            }
        }
        // introducere comanda get_books
        if (strncmp(comanda, "get_books", 9) == 0)
        {
            sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request((char *)"34.254.242.81", (char *)"/api/v1/tema/library/books", NULL, NULL, 0, token_final);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            if (response == NULL)
            {
                perror("receive_from_server");
                exit(EXIT_FAILURE);
            }
            // caut pozitia in care incepe mesajul JSON
            // ma folosesc de faptul ca linia este de tip json si incepe cu { 
            char *json_start = strstr(response, "{");
            if (json_start == NULL) {
                printf("Nu exista nicio carte introdusa\n");
            }
            else {
                // extrag lungimea mesajului JSON din randul "Content-Length"
                // obtin lungimea mesajului json adaugand la inceputul liniei textul de dinainte
                char *content_length_start = strstr(response, "Content-Length");
                // de numarul efectiv si fac conversie
                int content_length = atoi(content_length_start + strlen("Content-Length:"));
                char json_msg[content_length + 1];
                strncpy(json_msg, json_start, content_length);
                json_msg[content_length] = '\0';
                if (strncmp(json_msg, "{\"error\":\"Error when decoding tokenn!\"}", 42) == 0)
                {
                    printf("Nu aveti acces la biblioteca!\n");
                }
                else {
                    // caut pozitia in care incepe mesajul JSON
                    // ma folosesc de faptul ca linia este de tip json si incepe cu {
                    char *json_start = strstr(response, "{");
                    // extrag lungimea mesajului JSON din randul "Content-Length"
                    char *content_length_start = strstr(response, "Content-Length");
                    // obtin lungimea mesajului json adaugand la inceputul liniei textul de dinainte
                    // de numarul efectiv si fac conversie
                    int content_length = atoi(content_length_start + strlen("Content-Length:"));
                    char json_msg[content_length + 1];
                    strncpy(json_msg, json_start, content_length);
                    json_msg[content_length] = '\0';
                    printf("%s\n", json_msg);
                }
                //parsare_json1(json_msg);
            }
        }
        // adaug o verificare suplimentara deoarece primele 8 caractere se potrivesc
        // si cu comanda get_books si intra pe ambele ramuri
        if ((strncmp(comanda, "get_books", 9) != 0) && (strncmp(comanda, "get_book", 8) == 0))
        {
            printf("id=");
            fgets(ID, 20, stdin);
            ID[strcspn(ID, "\n")] = 0;
            
            if (contineDoarCifre(ID) == 0) {
                printf("ID non-numeric introdus!\n");
                // ca sa nu mai trec prin restul verificarilor inutil
                goto citire_comenzi;
            }

            else {
                char url[50];
                // ii adaug in adresa int-ul citit
                sprintf(url, "/api/v1/tema/library/books/%s", ID);
                sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request((char *)"34.254.242.81", url, NULL, NULL, 0, token_final);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                if (response == NULL)
                {
                    perror("receive_from_server");
                    exit(EXIT_FAILURE);
                }
                // caut pozitia in care incepe mesajul JSON
                // ma folosesc de faptul ca linia este de tip json si incepe cu {
                char *json_start = strstr(response, "{");
                // extrag lungimea mesajului JSON din randul "Content-Length"
                char *content_length_start = strstr(response, "Content-Length");
                // obtin lungimea mesajului json adaugand la inceputul liniei textul de dinainte
                // de numarul efectiv si fac conversie
                int content_length = atoi(content_length_start + strlen("Content-Length:"));
                char json_msg[content_length + 1];
                strncpy(json_msg, json_start, content_length);
                json_msg[content_length] = '\0';
                //printf("%s\n", json_msg);
                if (strncmp(json_msg, "{\"error\":\"No book was found!\"}", 30) == 0) {
                    printf("ID incorect! Nu exista nicio carte cu acest ID.\n");
                }
                if (strncmp(json_msg, "{\"error\":\"Error when decoding tokenn!\"}", 42) == 0) {
                    printf("Nu aveti acces la biblioteca!\n");
                }
                else if ((strncmp(json_msg, "{\"error\":\"No book was found!\"}", 30) != 0) && (strncmp(json_msg, "{\"error\":\"Error when decoding tokenn!\"}", 42) != 0)) {
                    // caut pozitia in care incepe mesajul JSON
                    // ma folosesc de faptul ca linia este de tip json si incepe cu {
                    char *json_start = strstr(response, "{");
                    // extrag lungimea mesajului JSON din randul "Content-Length"
                    char *content_length_start = strstr(response, "Content-Length");
                    // obtin lungimea mesajului json adaugand la inceputul liniei textul de dinainte
                    // de numarul efectiv si fac conversie
                    int content_length = atoi(content_length_start + strlen("Content-Length:"));
                    char json_msg[content_length + 1];
                    strncpy(json_msg, json_start, content_length);
                    json_msg[content_length] = '\0';
                    printf("%s\n", json_msg);
                    //parsare_json2(json_msg);
                }
            }
        }
        if (strncmp(comanda, "add_book", 8) == 0)
        {
            // introducere info
            printf("title=");
            fgets(title, BUFLEN, stdin);
            title[strcspn(title, "\n")] = 0;

            printf("author=");
            fgets(author, BUFLEN, stdin);
            author[strcspn(author, "\n")] = 0;
            if (verifica_cifre(author) == 0) {
                printf("Autor incorect! Stringul autor trebuie sa contina doar litere\n");
                goto citire_comenzi;

            }
            else {

                printf("genre=");
                fgets(genre, BUFLEN, stdin);
                genre[strcspn(genre, "\n")] = 0;


                printf("publisher=");
                fgets(publisher, BUFLEN, stdin);
                publisher[strcspn(publisher, "\n")] = 0;
                
                char page_count[20];
                printf("page_count=");
                fgets(page_count, 20, stdin);
                page_count[strcspn(page_count, "\n")] = 0;
                if (contineDoarCifre(page_count) == 0) {
                    printf("ID non-numeric introdus!\n");
                    // ca sa nu mai trec prin restul verificarilor inutil
                    goto citire_comenzi;
                }
                else {
                    json_object *json_obj = json_object_new_object();
                    json_object_object_add(json_obj, "title", json_object_new_string(title));
                    json_object_object_add(json_obj, "author", json_object_new_string(author));
                    json_object_object_add(json_obj, "genre", json_object_new_string(genre));
                    json_object_object_add(json_obj, "page_count", json_object_new_int(atoi(page_count)));
                    json_object_object_add(json_obj, "publisher", json_object_new_string(publisher));
                    // formare un buffer JSON din obiectul JSON
                    const char *json_str = json_object_to_json_string(json_obj);
                    sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                    message = compute_post_request((char *)"34.254.242.81", (char *)"/api/v1/tema/library/books", contentType, (char **)&json_str, 1, NULL, 0, token_final);
                    //printf("%s\n", message);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);
                    // caut pozitia in care incepe mesajul JSON
                    // ma folosesc de faptul ca linia este de tip json si incepe cu {
                    char *json_start = strstr(response, "{");
                    // extrag lungimea mesajului JSON din randul "Content-Length"
                    char *content_length_start = strstr(response, "Content-Length");
                    // obtin lungimea mesajului json adaugand la inceputul liniei textul de dinainte
                    // de numarul efectiv si fac conversie
                    int content_length = atoi(content_length_start + strlen("Content-Length:"));
                    char json_msg[content_length + 1];
                    strncpy(json_msg, json_start, content_length);
                    json_msg[content_length] = '\0';
                    if (strncmp(json_msg, "{\"error\":\"Error when decoding tokenn!\"}", 42) == 0) {
                        printf("Nu aveti acces la biblioteca!\n");
                    }
                    else {
                        printf("Carte adaugata cu succes!\n");
                    }
                    if (response == NULL)
                    {
                        perror("receive_from_server");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
        if (strncmp(comanda, "delete_book", 11) == 0) {
            printf("id=");
            fgets(ID, 20, stdin);
            ID[strcspn(ID, "\n")] = 0;
            if (contineDoarCifre(ID) == 0) {
                printf("ID non-numeric introdus!\n");
                // ca sa nu mai trec prin restul verificarilor inutil
                goto citire_comenzi;
            }
            else {
                char url[50];
                // ii adaug in adresa int-ul citit
                sprintf(url, "/api/v1/tema/library/books/%s", ID);
                sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_delete_request((char *)"34.254.242.81", url, NULL, token_final);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                if (response == NULL)
                {
                    perror("receive_from_server");
                    exit(EXIT_FAILURE);
                }
                // caut pozitia in care incepe mesajul JSON
                // ma folosesc de faptul ca linia este de tip json si incepe cu {
                char *json_start = strstr(response, "{");
                // extrag lungimea mesajului JSON din randul "Content-Length"
                char *content_length_start = strstr(response, "Content-Length");
                // obtin lungimea mesajului json adaugand la inceputul liniei textul de dinainte
                // de numarul efectiv si fac conversie
                int content_length = atoi(content_length_start + strlen("Content-Length:"));
                char json_msg[content_length + 1];
                strncpy(json_msg, json_start, content_length);
                json_msg[content_length] = '\0';
                if (strncmp(json_msg, "{\"error\":\"No book was deleted!\"}", 30) == 0)
                {
                    printf("ID incorect! Nu exista nicio carte cu acest ID pentru a fi stearsa.\n");
                }
                else if (strncmp(json_msg, "{\"error\":\"Error when decoding tokenn!\"}", 42) == 0)
                {
                    printf("Nu aveti acces la biblioteca!\n");
                }
                else if ((strncmp(json_msg, "{\"error\":\"No book was deleted!\"}", 30) != 0) && (strncmp(json_msg, "{\"error\":\"Error when decoding tokenn!\"}", 42) != 0)){
                    printf("Cartea cu ID-ul %s a fost stearsa!\n", ID);
                }
            }
        }

        // introducere comanda logout
        if (strncmp(comanda, "logout", 6) == 0)
        {
            sockfd = open_connection((char *)"34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request((char *)"34.254.242.81", (char *)"/api/v1/tema/library/access", NULL, cookie_final, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("Delogare cont!\n");
            if (response == NULL)
            {
                perror("receive_from_server");
                exit(EXIT_FAILURE);
            }
            // memset(cookie_final[0], 0, sizeof(cookie_final[0]));
            free(cookie_final[0]);
            cookie_final[0] = (char *)calloc(200, sizeof(char *));
            free(token_final);
            token_final = calloc(300, sizeof(char *));
        }
    }
}
