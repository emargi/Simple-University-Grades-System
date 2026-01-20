#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

// constants
// characters + '\n' (fgets BUG) + '\0'
#define MAX_PASSWORD_LEN 26
#define MAX_NAME_LEN 26
#define MAX_EMAIL_LEN 21
#define MAX_USERNAME_LEN 16

#define PHONE_NUMBER_LEN 13
#define NATIONAL_NUMBER_LEN 12
#define DATE_LEN 12
#define STATUS_LEN 10

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
    char status[STATUS_LEN];
    struct departmentHead *next;
};

struct academicStaff {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char email[MAX_EMAIL_LEN];
    char phoneNumber[PHONE_NUMBER_LEN];
    char joinDate[DATE_LEN];
    char status[STATUS_LEN];
    int rank;
    struct academicStaff *next;
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
void freeDepartmentHeads(struct departmentHead *start, struct departmentHead *end);
void getAcademicStaff(struct academicStaff **start, struct academicStaff **end);
void setAcademicStaff(struct academicStaff **start, struct academicStaff **end);
void freeAcademicStaff(struct academicStaff *start, struct academicStaff *end);

User loginPanel(void);
User loginAdmin(void);
User loginHead(void);
User loginStaff(void);

void mainMenu(User user);
void adminMainMenu(User user);
void headMainMenu(User user);
void staffMainMenu(User user);

void listUsers(void);
void listHeads(void);
void listStaff(void);

void addNewHead(void);
void addNewStaff(void);
void createBackup(void);
void useBackup(void);

void deleteUser(void);
int deleteHead(char username[]);
int deleteStaff(char username[]);

// main function
void main() {
    printf("Welcome to Student Management System");

    User user = loginPanel();
    mainMenu(user);
}

// function definitions
User loginPanel(void) {
    Sleep(1000);
    system("cls");

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
    system("cls");
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
        printf("\n3. List users\n4. Delete user\n5. Log\n6. Create backup");
        printf("\n7. Use backup\n\n0. Log out");
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

    else if (choice == 1) {
        addNewHead();
    }

    else if (choice == 2) {
        addNewStaff();
    }

    else if (choice == 3) {
        listUsers();
    }

    else if (choice == 4) {
        deleteUser();
    }

    else if (choice == 5) {
        // ...
    }

    else if (choice == 6) {
        createBackup();
    }

    else if (choice == 7) {
        useBackup();
    }

    mainMenu(user);
}

void headMainMenu(User user) {
    // ...
}

void staffMainMenu(User user) {
    // ...
}

void listUsers(void) {
    system("cls");

    printf("[#] List of all available users:\n\n");
    listHeads();
    printf("\n");
    listStaff();

    printf("\n\nPress any key to continue...");
    getch();
}

void listHeads(void) {
    struct departmentHead *start, *end;
    getDepartmentHeads(&start, &end);

    if (start != NULL) {
        printf("[#] List of department heads:\n");
    }

    int num = 1;
    struct departmentHead *curr;
    curr = start;
    while (curr != NULL) {
        if (strcmp(curr->status, "deleted") == 0) {
            curr = curr->next;
            continue;
        }

        char name[50];
        strcpy(name, curr->firstName);
        strcat(name, " ");
        strcat(name, curr->lastName);

        printf("\n[#] Index: %d\n", num);
        printf("[#] Name: %s\n", name);
        printf("[#] Join date: %s\n", curr->joinDate);
        printf("[#] Academic group: %s\n", curr->groupName);
        printf("[#] Username: %s\n", curr->username);
        printf("[#] National number: %s\n", curr->nationalNumber);
        printf("[#] Phone number: %s\n", curr->phoneNumber);
        printf("[#] Email: %s\n", curr->email);

        curr = curr->next;
        num++;
    }
}

void listStaff(void) {
    struct academicStaff *start, *end;
    getAcademicStaff(&start, &end);

    if (start != NULL) {
        printf("[#] List of academic staff:\n");
    }

    int num = 1;
    struct academicStaff *curr = start;
    while (curr != NULL) {
        if (strcmp(curr->status, "deleted") == 0) {
            curr = curr->next;
            continue;
        }

        char name[50];
        strcpy(name, curr->firstName);
        strcat(name, " ");
        strcat(name, curr->lastName);

        printf("\n[#] Index: %d\n", num);
        printf("[#] Name: %s\n", name);
        printf("[#] Join date: %s\n", curr->joinDate);
        printf("[#] Rank: %d\n", curr->rank);
        printf("[#] Username: %s\n", curr->username);
        printf("[#] Phone number: %s\n", curr->phoneNumber);
        printf("[#] Email: %s\n", curr->email);

        curr = curr->next;
        num++;
    }
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

        strcpy(curr->status, "enabled");

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
    freeDepartmentHeads(start, end);

    printf("\n[#] New department head successfully added!\n");
    Sleep(1000);
}

void addNewStaff(void) {
    int num;
    printf("[?] Enter the number of academic staff you want to add: ");
    scanf("%d", &num);
    
    int i;
    struct academicStaff *newStaff, *curr, *lastStaff;
    newStaff = NULL;
    lastStaff = NULL;
    for (i = 0; i < num; i++) {
        curr = malloc(sizeof(struct academicStaff));
        printf("\n[#] Enter the information of staff #%d:\n", i + 1);
        
        printf("[?] Enter first name: ");
        inputString(curr->firstName, MAX_NAME_LEN);
        
        printf("[?] Enter last name: ");
        inputString(curr->lastName, MAX_NAME_LEN);
        
        int correct;
        char date[DATE_LEN];
        do {
            printf("[?] Enter the date of join (sample: 2026/01/01): ");
            inputString(date, DATE_LEN);
            correct = validateDate(date);
            if (!correct)
            printf("[!] Wrong date. Try again\n");
        } while (!correct);
        strcpy(curr->joinDate, date);
        
        char strNumber[10];
        printf("[?] Enter rank: ");
        inputStringNumber(strNumber, 10);
        curr->rank = atoi(strNumber);
        
        char phoneNumber[PHONE_NUMBER_LEN];
        do {
            printf("[?] Enter phone number (sample: 09123456789): ");
            inputStringNumber(phoneNumber, PHONE_NUMBER_LEN);
            correct = validatePhoneNumber(phoneNumber);
            if (!correct)
            printf("[!] Wrong phone number format. Try again\n");
        } while (!correct);
        strcpy(curr->phoneNumber, phoneNumber);
        
        char email[MAX_EMAIL_LEN];
        do {
            printf("[?] Enter email (sample: test@yahoo.com): ");
            inputString(email, MAX_EMAIL_LEN);
            correct = validateEmail(email);
            if (!correct)
            printf("[!] Wrong email. Try again\n");
        } while (!correct);
        strcpy(curr->email, email);
        
        printf("[?] Enter username: ");
        inputUsername(curr->username, MAX_USERNAME_LEN);
        
        char password1[MAX_PASSWORD_LEN], password2[MAX_PASSWORD_LEN];
        do {
            printf("[?] Enter password: ");
            inputPassword(password1, MAX_PASSWORD_LEN);
            printf("[?] Enter password again: ");
            inputPassword(password2, MAX_PASSWORD_LEN);
            if (strcmp(password1, password2) != 0) {
                correct = 0;
                printf("[!] Passwords do not match. Try again\n");
            }
            else {
                correct = 1;
            }
        } while (!correct);
        strcpy(curr->password, password1);
        
        strcpy(curr->status, "enabled");
        
        curr->next = NULL;
        
        if (newStaff == NULL) {
            newStaff = curr;
            lastStaff = newStaff;
        }
        else {
            lastStaff->next = curr;
            lastStaff = curr;
        }
    }
    
    struct academicStaff *start, *end;
    getAcademicStaff(&start, &end);
    if (start == NULL) {
        start = newStaff;
        end = lastStaff;
    }
    else {
        end->next = newStaff;
        end = lastStaff;
    }
    
    setAcademicStaff(&start, &end);
    freeAcademicStaff(start, end);
    
    printf("\n[#] New academic staff added");
    Sleep(1000);
}

void deleteUser(void) {
    char username[MAX_USERNAME_LEN];
    printf("[?] Enter username (Enter 0 to ccancel): ");
    inputString(username, MAX_USERNAME_LEN);

    if (strcmp(username, "0") != 0) {
        int res = deleteHead(username);
        if (!res) {
            res = deleteStaff(username);
        }

        if (!res) {
            printf("[!] User not found");
            Sleep(1000);
        }
    }
}

int deleteHead(char username[]) {
    struct departmentHead *start, *curr, *end;
    getDepartmentHeads(&start, &end);
    curr = start;
    while (curr != NULL) {
        if (strcmp(curr->status, "deleted") == 0) {
            curr = curr->next;
            continue;
        }
        
        if (strcmp(curr->username, username) == 0)
            break;

        curr = curr->next;
    }

    if (curr == NULL)
        return 0;

    printf("[#] User found: Department Head, %s %s\n", curr->firstName, curr->lastName);
    printf("[??] Do you want to delete this user (yes/no): ");
    char answer[5];
    inputString(answer, 5);
    if (strcmp(answer, "yes") != 0) {
        freeDepartmentHeads(start, end);
        printf("[!] Operation canceled");
        return 1;
    }

    strcpy(curr->status, "deleted");
    setDepartmentHeads(&start, &end);
    freeDepartmentHeads(start, end);

    printf("[#] User deleted");
    return 1;
}

int deleteStaff(char username[]) {
    struct academicStaff *start, *curr, *end;
    getAcademicStaff(&start, &end);
    curr = start;
    while (curr != NULL) {
        if (strcmp(curr->status, "deleted") == 0) {
            curr = curr->next;
            continue;
        }

        if (strcmp(curr->username, username) == 0)
            break;

        curr = curr->next;
    }

    if (curr == NULL) {
        freeAcademicStaff(start, end);
        return 0;
    }

    printf("[#] User found: Academic staff, %s %s\n", curr->firstName, curr->lastName);
    printf("[??] Do you want to delete this user (yes/no): ");
    char answer[5];
    inputString(answer, 5);

    if (strcmp(answer, "yes") != 0) {
        freeAcademicStaff(start, end);
        printf("[!] Operation canceled");
        return 0;
    }

    strcpy(curr->status, "deleted");
    setAcademicStaff(&start, &end);
    freeAcademicStaff(start, end);

    printf("[#] User deleted");
    return 1;
}

void createBackup(void) {
    char path[110];
    printf("[?] Enter destination path (sample: D:\\backups): ");
    inputString(path, 110);

    char dest[150];
    strcpy(dest, path);
    strcat(dest, "\\");
    strcat(dest, HEAD_PROFILES_FILE);

    FILE *fd;
    fd = fopen(dest, "w");
    if (fd == NULL) {
        printf("[!] Wrong path");
        Sleep(1000);
    }
    else {
        FILE *fs;
        fs = fopen(HEAD_PROFILES_FILE, "r");
        if (fs == NULL) {
            printf("[Error] Something went wrong");
            fclose(fd);
            exit(1);
        }

        char ch = fgetc(fs);
        while (ch != EOF) {
            fputc(ch, fd);
            ch = fgetc(fs);
        }

        fclose(fs);
        fclose(fd);
        printf("[#] Department head information: backup done\n");
    }

    strcpy(dest, path);
    strcat(dest, "\\");
    strcat(dest, STAFF_PROFILES_FILE);

    fd = fopen(dest, "w");
    if (fd == NULL) {
        printf("[!] Wrong path");
        Sleep(1000);
    }
    else {
        FILE *fs;
        fs = fopen(STAFF_PROFILES_FILE, "r");
        if (fs == NULL) {
            printf("[Error] Something went wrong");
            fclose(fd);
            exit(1);
        }

        char ch = fgetc(fs);
        while (ch != EOF) {
            fputc(ch, fd);
            ch = fgetc(fs);
        }

        fclose(fd);
        fclose(fs);
        printf("[#] Academic staff information: backup done");
    }
}

void useBackup(void) {
    // ...
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

        fgets(curr->status, STATUS_LEN, fp);
        removeNewLine(curr->status);

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
        fprintf(fp, "%s\n", curr->status);
        curr = curr->next;
    }

    fclose(fp);
}

void freeDepartmentHeads(struct departmentHead *start, struct departmentHead *end) {
    end = start;
    while (end != NULL) {
        end = start->next;
        free(start);
        start = end;
    }
}

void getAcademicStaff(struct academicStaff **start, struct academicStaff **end) {
    FILE *fp;
    fp = fopen(STAFF_PROFILES_FILE, "r");
    if (fp == NULL) {
        // maybe file is not created yet
        // create it, then read it again
        fp = fopen(STAFF_PROFILES_FILE, "w");
        fclose(fp);
        fp = fopen(STAFF_PROFILES_FILE, "r");
        if (fp == NULL) {
            printf("[Error] Something went wrong while reading a file.\n");
            exit(1);
        }
    }

    *start = NULL;
    *end = NULL;

    struct academicStaff *curr;
    while (!feof(fp)) {
        curr = malloc(sizeof(struct academicStaff));
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

        fgets(curr->email, MAX_EMAIL_LEN, fp);
        removeNewLine(curr->email);

        fgets(curr->phoneNumber, PHONE_NUMBER_LEN, fp);
        removeNewLine(curr->phoneNumber);

        fgets(curr->joinDate, DATE_LEN, fp);
        removeNewLine(curr->joinDate);

        fgets(curr->status, STATUS_LEN, fp);
        removeNewLine(curr->status);

        char strNumber[12];
        fgets(strNumber, 12, fp);
        removeNewLine(strNumber);
        curr->rank = atoi(strNumber);

        curr->next = NULL;

        if (*start == NULL) {
            *start = curr;
            *end = *start;
        }
        else {
            (*end)->next = curr;
            *end = curr;
        }
    }

    fclose(fp);
}

void setAcademicStaff(struct academicStaff **start, struct academicStaff **end) {
    FILE *fp;
    fp = fopen(STAFF_PROFILES_FILE, "w");
    if (fp == NULL) {
        printf("[Error] Something went wrong while opening a file");
        exit(1);
    }

    struct academicStaff *curr = *start;
    while (curr != NULL) {
        fprintf(fp, "%s\n", curr->firstName);
        fprintf(fp, "%s\n", curr->lastName);
        fprintf(fp, "%s\n", curr->username);
        fprintf(fp, "%s\n", curr->password);
        fprintf(fp, "%s\n", curr->email);
        fprintf(fp, "%s\n", curr->phoneNumber);
        fprintf(fp, "%s\n", curr->joinDate);
        fprintf(fp, "%s\n", curr->status);
        fprintf(fp, "%d\n", curr->rank);
        curr = curr->next;
    }

    fclose(fp);
}

void freeAcademicStaff(struct academicStaff *start, struct academicStaff *end) {
    end = start;
    while (end != NULL) {
        end = start->next;
        free(start);
        start = end;
    }
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
