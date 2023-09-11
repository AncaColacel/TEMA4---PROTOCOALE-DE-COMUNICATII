#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3: add Authorization header
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }
    
    // Step 4 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i ++) {
            if (i == cookies_count - 1) {
                strcat(line, cookies[i]);
            } else {
                strcat(line, cookies[i]);
                strcat(line, "; ");
            }
        }
    }
    compute_message(message, line);


    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char* token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *elem = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    
   // Step 3: add headers
    strcpy(line, "Content-Type: application/json");
    compute_message(message, line);

    int lungime = 0;
    for (int i = 0; i < body_data_fields_count; i++) {
        lungime = lungime + strlen(body_data[i]);
    }
    sprintf(line, "Content-Length: %d", lungime);
    compute_message(message, line);

     if (token != NULL) {
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }

    // Step 4 (optional): add cookies
    if (cookies != NULL) {
        sprintf(line, "Cookies: ");
        for (int i = 0; i < cookies_count; i ++) {
            if (i == cookies_count - 1) {
                sprintf(elem, "%s", cookies[i]);
            } else {
                sprintf(elem, "%s;", cookies[i]);
            }
        }
       sprintf(line, "%s", elem);
    }
    // Step 5: add new line at end of header
    memset(line, 0, LINELEN);
    compute_message(message, line);
    // Step 6: add the actual payload data
    for (int i = 0; i < body_data_fields_count; i++) {
        strcpy(line, body_data[i]);
        compute_message(message, line);
    }

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3: add Authorization header
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }
    
    // Step 4 (optional): add headers 
    strcpy(line, "Content-Type: application/json");
    compute_message(message, line);

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}
