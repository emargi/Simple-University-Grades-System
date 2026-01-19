#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>

// constants
// characters + '\n' (fgets BUG) + '\0'
#define MAX_PASSWORD_LEN 26
#define MAX_NAME_LEN 26
#define MAX_EMAIL_LEN 21
#define MAX_USERNAME_LEN 16

#define PHONE_NUMBER_LEN 13
#define NATIONAL_NUMBER_LEN 12
#define DATE_LEN 12

#define HEAD_PROFILES_FILE "department_head_profiles.txt"
#define STAFF_PROFILES_FILE "academic_staff_profiles.txt"

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

struct departmentHead {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char groupName[MAX_NAME_LEN];
    char email[MAX_EMAIL_LEN];
    char phoneNumber[PHONE_NUMBER_LEN];
    char nationalNumber[NATIONAL_NUMBER_LEN];
    char joinDate[DATE_LEN]; // xxxx/xx/xx
    struct departmentHead *next;
};

// function prototypes
int inputDigit(void);
void inputString(char input[], int size);
void inputStringNumber(char strNumber[], int size);
void inputUsername(char username[], int size);
void inputPassword(char password[], int size);

int validateChar(char ch, const char acceptable[]);
int validateDate(char date[]);
int validatePhoneNumber(char phoneNumber[]);
int validateEmail(char email[]);

void removeNewLine(char str[]);

void getDepartmentHeads(struct departmentHead **start, struct departmentHead **end);
void setDepartmentHeads(struct departmentHead **start, struct departmentHead **end);

User loginPanel(void);
User loginAdmin(void);
User loginHead(void);
User loginStaff(void);

void mainMenu(User user);
void adminMainMenu(User user);
void headMainMenu(User user);
void staffMainMenu(User user);

void addNewHead(void);

// main function
void main() {
    printf("Welcome to Student Management System\n\n");

    User user = loginPanel();
    mainMenu(user);
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
            printf("[Error] Wrong number. Try again\n");
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

void mainMenu(User user) {
    if (strcmp(user.type, "admin") == 0)
        adminMainMenu(user);

    else if (strcmp(user.type, "head") == 0)
        headMainMenu(user);

    else
        staffMainMenu(user);
}

void adminMainMenu(User user) {
    int choice = -1;
    do {
        printf("[#] The available actions are listed below:\n");
        printf("\n1. Add new department head\n2. Add new academic staff");
        printf("\n3. List users\n4. Delete user\n5. Log\n6. Backup\n\n0. Log out");
        printf("\n\n[?] Enter your choice (enter number): ");

        choice = inputDigit();
        printf("\n");
        if (choice < 0 || choice > 6) {
            printf("[Error] Wrong number. Try again\n");
        }
    } while (choice < 0 || choice > 6);

    if (choice == 0) {
        user = loginPanel();
        mainMenu(user);
    }

    if (choice == 1) {
        addNewHead();
    }

    mainMenu(user);
}

void headMainMenu(User user) {
    // ...
}

void staffMainMenu(User user) {
    // ...
}

void addNewHead() {
    int num;
    printf("[?] Enter the number of department heads you want to add: ");
    scanf("%d", &num);

    int i;
    struct departmentHead *newHead, *curr, *lastHead;
    lastHead = NULL;
    newHead = NULL;

    for (i = 0; i < num; i++) {
        curr = malloc(sizeof(struct departmentHead));
        printf("[#] Enter the information of user #%d:\n", i + 1);

        printf("[?] Enter first name: ");
        inputString(curr->firstName, MAX_NAME_LEN);
    
        printf("[?] Enter last name: ");
        inputString(curr->lastName, MAX_NAME_LEN);
    
        printf("[?] Enter group name: ");
        inputString(curr->groupName, MAX_NAME_LEN);
    
        int correct;
        char date[DATE_LEN];
        do {
            printf("[?] Enter join date (sample: 2026/01/01): ");
            inputString(date, DATE_LEN);
            correct = validateDate(date);
            if (!correct)
                printf("[!] Wrong date format. Try again\n");
        } while (!correct);
        strcpy(curr->joinDate, date);
    
        char nationalNumber[NATIONAL_NUMBER_LEN];
        do {
            printf("[?] Enter national number: ");
            inputStringNumber(nationalNumber, NATIONAL_NUMBER_LEN);
            if (strlen(nationalNumber) != NATIONAL_NUMBER_LEN - 2) {
                printf("[!] Wrong national number. Try again\n");
                correct = 0;
            }
            else
                correct = 1;
        } while (!correct);
        strcpy(curr->nationalNumber, nationalNumber);
    
        char phoneNumber[PHONE_NUMBER_LEN];
        do {
            printf("[?] Enter phone number (sample: 09123456789): ");
            inputStringNumber(phoneNumber, PHONE_NUMBER_LEN);
            correct = validatePhoneNumber(phoneNumber);
            if (!correct)
                printf("[!] Wrong phone number. Try again\n");
        } while (!correct);
        strcpy(curr->phoneNumber, phoneNumber);
    
        char email[MAX_EMAIL_LEN];
        do {
            printf("[?] Enter email (sample: admin@yahoo.com): ");
            inputString(email, MAX_EMAIL_LEN);
            correct = validateEmail(email);
            if (!correct)
                printf("[!] Wrong email. Try again\n");
        } while (!correct);
        strcpy(curr->email, email);
    
        printf("[?] Enter username: ");
        inputUsername(curr->username, MAX_USERNAME_LEN);
    
        int same;
        char password1[MAX_PASSWORD_LEN], password2[MAX_PASSWORD_LEN];
        do {
            printf("[?] Enter password: ");
            inputPassword(password1, MAX_PASSWORD_LEN);
            printf("[?] Enter password again: ");
            inputPassword(password2, MAX_PASSWORD_LEN);
    
            if (strcmp(password1, password2) != 0) {
                printf("[!] Passwords do not match. Try again\n");
                same = 0;
            }
            else {
                same = 1;
            }
        } while (same == 0);
        strcpy(curr->password, password1);

        curr->next = NULL;
        if (lastHead == NULL) {
            newHead = curr;
            lastHead = newHead;
        }
        else {
            lastHead->next = curr;
            lastHead = curr;
        }
    }

    struct departmentHead *start, *end;
    getDepartmentHeads(&start, &end);
    if (start == NULL) {
        start = newHead;
        end = lastHead;
    }
    else {
        end->next = newHead;
        end = lastHead;
    }

    setDepartmentHeads(&start, &end);

    struct departmentHead *temp;
    curr = start;
    while (curr != NULL) {
        temp = curr->next;
        free(curr);
        curr = temp;
    }

    printf("\n[#] New department head successfully added!\n");
}

void getDepartmentHeads(struct departmentHead **start, struct departmentHead **end) {
    *start = NULL;
    *end = NULL;

    FILE *fp;
    fp = fopen(HEAD_PROFILES_FILE, "r");
    if (fp == NULL) {
        // maybe file is not created yet
        // create it, then read it again
        fp = fopen(HEAD_PROFILES_FILE, "w");
        fclose(fp);
        fp = fopen(HEAD_PROFILES_FILE, "r");
        if (fp == NULL) {
            printf("[Error] Something went wrong while reading a file");
            exit(1);
        }
    }

    struct departmentHead *curr;

    while (!feof(fp)) {
        curr = malloc(sizeof(struct departmentHead));
        // BUG: fgets gets '\n' at the end of the line

        if (fgets(curr->firstName, MAX_NAME_LEN, fp) == NULL) {
            free(curr);
            break;
        }
        removeNewLine(curr->firstName);

        fgets(curr->lastName, MAX_NAME_LEN, fp);
        removeNewLine(curr->lastName);

        fgets(curr->username, MAX_USERNAME_LEN, fp);
        removeNewLine(curr->username);

        fgets(curr->password, MAX_PASSWORD_LEN, fp);
        removeNewLine(curr->password);

        fgets(curr->groupName, MAX_NAME_LEN, fp);
        removeNewLine(curr->groupName);

        fgets(curr->email, MAX_EMAIL_LEN, fp);
        removeNewLine(curr->email);

        fgets(curr->phoneNumber, PHONE_NUMBER_LEN, fp);
        removeNewLine(curr->phoneNumber);

        fgets(curr->nationalNumber, NATIONAL_NUMBER_LEN, fp);
        removeNewLine(curr->nationalNumber);

        fgets(curr->joinDate, DATE_LEN, fp);
        removeNewLine(curr->joinDate);

        curr->next = NULL;

        if (*start == NULL) {
            *start = curr;
            *end  = *start;
        }
        else {
            (*end)->next = curr;
            *end = curr;
        }
    }

    fclose(fp);
}

void setDepartmentHeads(struct departmentHead **start, struct departmentHead **end) {
    FILE *fp;
    fp = fopen(HEAD_PROFILES_FILE, "w");
    if (fp == NULL) {
        printf("[Error] Something went wrong while opening a file.");
        exit(1);
    }

    struct departmentHead *curr = *start;
    while (curr != NULL) {
        fprintf(fp, "%s\n", curr->firstName);
        fprintf(fp, "%s\n", curr->lastName);
        fprintf(fp, "%s\n", curr->username);
        fprintf(fp, "%s\n", curr->password);
        fprintf(fp, "%s\n", curr->groupName);
        fprintf(fp, "%s\n", curr->email);
        fprintf(fp, "%s\n", curr->phoneNumber);
        fprintf(fp, "%s\n", curr->nationalNumber);
        fprintf(fp, "%s\n", curr->joinDate);
        curr = curr->next;
    }

    fclose(fp);
}

int inputDigit(void) {
    char input;
    while (1) {
        input = getch();
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
    while (n_chars < size - 2) {
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

        if (isprint(ch)) {
            putchar(ch);
            input[n_chars] = ch;
            n_chars++;
        }
    }
    putchar('\n');
    input[n_chars] = '\0';
}

void inputStringNumber(char strNumber[], int size) {
    fflush(stdin);

    int n_chars = 0;
    while (n_chars < size - 2) {
        char ch = getch();
        if (ch == '\n' || ch == '\r')
            break;

        if (ch == '\b') {
            if (n_chars == 0)
                continue;

            printf("\b \b");
            n_chars--;
        }

        if (isdigit(ch)) {
            putchar(ch);
            strNumber[n_chars] = ch;
            n_chars++;
        }
    }
    putchar('\n');
    strNumber[n_chars] = '\0';
}

void inputUsername(char input[], int size) {
    // clear the input (BUG FIX)
    fflush(stdin);

    int n_chars = 0;
    while (n_chars < size - 2) {
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
    while (n_chars < size - 2) {
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

int validateDate(char date[]) {
    return 1;
}

int validatePhoneNumber(char phoneNumber[]) {
    return 1;
}

int validateEmail(char email[]) {
    return 1;
}

void removeNewLine(char str[]) {
    int len = strlen(str);
    if (str[len-1] == '\n')
        str[len-1] = '\0';
}