#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>

// constants
#define MAX_PASSWORD_LEN 26
#define MAX_NAME_LEN 26
#define MAX_EMAIL_LEN 21
#define MAX_USERNAME_LEN 16

const char USERNAME_CHARS[] = "abcdefghijklmnopqrstuvwxyz1234567890_";

// structures
struct user {
    char type[6]; // admin, head, staff
    char name[MAX_NAME_LEN]; // + '\0'
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char email[MAX_EMAIL_LEN];
};
typedef struct user User;

// function prototypes
int inputDigit(void);
void inputString(char input[], int size);
void inputPassword(char password[], int size);

int validateChar(char ch, const char acceptable[]);

User loginPanel(void);
User loginAdmin(void);
User loginHead(void);
User loginStaff(void);

// main function
void main() {
    printf("Welcome to Student Management System\n\n");

    User user = loginPanel();
}

// function definitions
User loginPanel(void) {
    printf("[#] To use this program, you should first login to your account\n");
    int choice = -1;
    do {
        printf("[#] The available roles to login as are listed below\n");
        printf("\n1. Admin\n2. Deparment Head\n3. Academic Staff\n\n0. Exit");
        printf("\n\n[?] Enter your choice (enter number): ");
    
        choice = inputDigit();
        printf("\n");
        if (choice < 0 || choice > 3)
            printf("[Error] Wrong choiceber. Try again\n");
    } while (choice < 0 || choice > 3);

    if (choice == 0)
        exit(1);

    if (choice == 1)
        return loginAdmin();

    if (choice == 2)
        return loginHead();

    return loginStaff();
}

User loginAdmin(void) {
    char username[MAX_USERNAME_LEN];
    int askAgain = 0;

    do {
        printf("[?] Enter your username: ");
        inputString(username, MAX_USERNAME_LEN);
        if (strcmp(username, "admin") != 0) {
            printf("[!] Username not found. Try again\n");
            askAgain = 1;
        }
        else {
            askAgain = 0;
        }
    } while (askAgain);

    char password[MAX_PASSWORD_LEN];
    do {
        printf("[?] Enter your password: ");
        inputPassword(password, MAX_PASSWORD_LEN);
        if (strcmp(password, "admin") != 0) {
            printf("[!] Wrong password. Try again\n");
            askAgain = 1;
        }
        else {
            askAgain = 0;
        }
    } while (askAgain);

    User u;
    strcpy(u.type, "admin");
    strcpy(u.name, "admin");
    strcpy(u.username, username);
    strcpy(u.password, password);
    strcpy(u.email, "admin@admin");
    return u;
}

User loginHead(void) {
    User u;
    return u;
}

User loginStaff(void) {
    User u;
    return u;
}

int inputDigit(void) {
    char input;
    while (1) {
        input = _getch();
        if (isdigit(input))
            break;
    }
    printf("%c", input);

    int digit = input - '0';
    return digit;
}

void inputString(char input[], int size) {
    // clear the input (BUG FIX)
    fflush(stdin);

    int n_chars = 0;
    while (n_chars < size) {
        char ch = getch();
        if (ch == '\n' || ch == '\r')
            break;

        if (ch == '\b') {
            if (n_chars == 0)
                continue;

            printf("\b \b");
            n_chars--;
            continue;
        }

        if (validateChar(ch, USERNAME_CHARS)) {
            putchar(ch);
            input[n_chars] = ch;
            n_chars++;
        }
    }
    putchar('\n');
    input[n_chars] = '\0';
}

void inputPassword(char password[], int size) {
    // clear the input (BUG FIX)
    fflush(stdin);

    int n_chars = 0;
    while (n_chars < size) {
        char ch = getch();
        if (ch == '\n' || ch == '\r')
            break;

        if (ch == '\b') {
            if (n_chars == 0)
                continue;

            printf("\b \b");
            n_chars--;
            continue;
        }

        if (ch >= 32 && ch <= 126) {
            putchar('*');
            password[n_chars] = ch;
            n_chars++;
        }
    }
    putchar('\n');
    password[n_chars] = '\0';
}

int validateChar(char ch, const char acceptable[]) {
    int found = 0;
    int i;
    for (i = 0; acceptable[i] != '\0'; i++) {
        if (ch == acceptable[i]) {
            found = 1;
            break;
        }
    }
    return found;
}
