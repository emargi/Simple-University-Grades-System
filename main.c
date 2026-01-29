#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

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
#define LESSON_CODE_LEN 7
#define STUDENT_NUMBER_LEN 12

#define HEAD_PROFILES_FILE "department_head_profiles.txt"
#define STAFF_PROFILES_FILE "academic_staff_profiles.txt"
#define LESSONS_FILE "lessons.txt"
#define GRADES_FILE "grades.txt"
#define STUDENTS_FILE "students.txt"

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

struct student {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char nationalNumber[NATIONAL_NUMBER_LEN];
    char birthDate[DATE_LEN];
    char birthCity[MAX_NAME_LEN];
    char major[MAX_NAME_LEN];
    char studentNumber[STUDENT_NUMBER_LEN];
    char phoneNumber[PHONE_NUMBER_LEN];
    char email[MAX_EMAIL_LEN];
    struct student *next;
};

struct lesson {
    char name[MAX_NAME_LEN];
    char type[MAX_NAME_LEN];
    char code[LESSON_CODE_LEN];
    char status[STATUS_LEN];
    int units;
    struct lesson *next;
};

struct grade {
    char studentNumber[STUDENT_NUMBER_LEN];
    char lessonCode[LESSON_CODE_LEN];
    char username[MAX_USERNAME_LEN];
    float grade;
    long long int time;
    struct grade *next;
};

// function prototypes
int inputDigit(void);
void inputString(char input[], int size);
void inputStringNumber(char strNumber[], int size);
void inputStringFloat(char strFloat[], int size);
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
void getStudents(struct student **start, struct student **end);
void setStudents(struct student *start, struct student *end);
void freeStudents(struct student *start, struct student *end);
void getLessons(struct lesson **start, struct lesson **end);
void setLessons(struct lesson *start, struct lesson *end);
void freeLessons(struct lesson *start, struct lesson *end);
void getGrades(struct grade **start, struct grade **end);
void setGrades(struct grade *start, struct grade *end);
void freeGrades(struct grade *start, struct grade *end);
struct departmentHead *getHeadByUsername(struct departmentHead *start, struct departmentHead *end, char username[]);
struct academicStaff *getStaffByUsername(struct academicStaff *start, struct academicStaff *end, char username[]);
struct student *getStudentByStudentNumber(struct student *start, struct student *end, char studentNumber[]);
struct lesson *getLessonByLessonCode(struct lesson *start, struct lesson *end, char lessonCode[]);
struct grade *getGradeByLessonCodeAndStudentNumber(struct grade *start, struct grade *end, char lessonCode[], char studentNumber[]);

User loginPanel(void);
User loginAdmin(void);
User loginHead(void);
User loginStaff(void);

void mainMenu(User user);
void adminMainMenu(User user);
void headMainMenu(User user);
void staffMainMenu(User user);

void adminLog(User user);
void headLog(User user);

void searchStudent(void);
void listUsers(void);
void listHeads(void);
void listStaff(void);
void listDeletedHeads(void);
void listDeletedStaff(void);

void addNewHead(void);
void addNewStaff(void);
void addNewStudent(void);
void addNewLesson(void);
void addNewGrade(User user);
void editLesson(void);
void editGrade(void);
void editHead(User user);
void editStaff(User user);
void editStudent(void);
void createBackup(void);
void useBackup(void);

void deleteUser(void);
int deleteHead(char username[]);
int deleteStaff(char username[]);
void deleteLesson(void);

// main function
void main() {
    printf("Welcome to Student Management System");
    Sleep(1000);

    User user = loginPanel();
    mainMenu(user);
}

// function definitions
User loginPanel(void) {
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
    struct departmentHead *start, *end, *user;
    getDepartmentHeads(&start, &end);

    int correct;
    do {
        char username[MAX_USERNAME_LEN];
        printf("[?] Enter username: ");
        inputUsername(username, MAX_USERNAME_LEN);

        char password[MAX_PASSWORD_LEN];
        printf("[?] Enter password: ");
        inputPassword(password, MAX_PASSWORD_LEN);

        user = getHeadByUsername(start, end, username);
        if (user != NULL) {
            if (strcmp(user->password, password) != 0)
                correct = 0;
            else
                correct = 1;
        }
        else {
            correct = 0;
        }

        if (!correct)
            printf("[!] Wrong information. Try again\n\n");
    } while (!correct);

    User u;
    strcpy(u.type, "head");
    strcpy(u.username, user->username);

    freeDepartmentHeads(start, end);
    return u;
}

User loginStaff(void) {
    struct academicStaff *start, *end, *user;
    getAcademicStaff(&start, &end);

    int correct;
    do {
        char username[MAX_USERNAME_LEN];
        printf("[?] Enter username: ");
        inputUsername(username, MAX_USERNAME_LEN);

        char password[MAX_PASSWORD_LEN];
        printf("[?] Enter password: ");
        inputPassword(password, MAX_PASSWORD_LEN);

        user = getStaffByUsername(start, end, username);
        if (user != NULL) {
            if (strcmp(user->password, password) != 0)
                correct = 0;
            else
                correct = 1;
        }
        else {
            correct = 0;
        }

        if (!correct)
            printf("[!] Wrong information. Try again\n\n");
    } while (!correct);

    User u;
    strcpy(u.type, "staff");
    strcpy(u.username, user->username);

    freeAcademicStaff(start, end);
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

    else if (choice == 1)
        addNewHead();

    else if (choice == 2)
        addNewStaff();

    else if (choice == 3)
        listUsers();

    else if (choice == 4)
        deleteUser();

    else if (choice == 5)
        adminLog(user);

    else if (choice == 6)
        createBackup();

    else if (choice == 7)
        useBackup();

    mainMenu(user);
}

void headMainMenu(User user) {
    system("cls");
    printf("[#] The available actions are listed below:\n\n");
    printf("1. Add new lesson\n2. Add new grade\n3. Edit lesson");
    printf("\n4. Edit grade\n5. Delete lesson\n6. Log");
    printf("\n7. Account settings\n\n0. Log out\n");

    printf("\n[?] Enter your choice (enter number): ");
    int digit = inputDigit();

    if (digit == 0) {
        user = loginPanel();
        mainMenu(user);
        exit(0);
    }

    if (digit == 1)
        addNewLesson();

    else if (digit == 2)
        addNewGrade(user);

    else if (digit == 3)
        editLesson();

    else if (digit == 4)
        editGrade();

    else if (digit == 5)
        deleteLesson();

    else if (digit == 6)
        headLog(user);

    else if (digit == 7)
        editHead(user);

    headMainMenu(user);
}

void staffMainMenu(User user) {
    system("cls");
    printf("[#] The available actions are listed below:\n");
    printf("\n1. Add student\n2. Edit student\n3. Add grade\n4. Log");
    printf("\n5. Account settings\n\n0. Log out");
    printf("\n\n[?] Enter your choice (enter number): ");

    int digit = inputDigit();
    if (digit == 0) {
        User user = loginPanel();
        mainMenu(user);
        exit(0);
    }

    else if (digit == 1)
        addNewStudent();

    else if (digit == 2)
        editStudent();

    else if (digit == 3)
        addNewGrade(user);

    else if (digit == 5)
        editStaff(user);

    staffMainMenu(user);
}

void adminLog(User user) {
    system("cls");

    printf("[#] Welcome to admin log menu\n");
    printf("[#] The available actions here are listed below:\n");
    printf("\n1. List of department heads\n2. List of academic staff");
    printf("\n3. List of deleted department heads\n4. List of deleted academic staff");
    printf("\n5. List of successfull logins\n6. List of unsuccessful logins");
    printf("\n\n0. Return to the main menu\n\n");
    printf("[?] Enter your choice (enter number): ");
    int digit = inputDigit();

    if (digit == 0) {
        mainMenu(user);
        exit(0);
    }

    if (digit == 1)
        listHeads();

    else if (digit == 2)
        listStaff();

    else if (digit == 3)
        listDeletedHeads();

    else if (digit == 4)
        listDeletedStaff();

    printf("\n\nPress any key to continue...");
    getch();
    adminLog(user);
}

void headLog(User user) {
    system("cls");

    printf("[#] Welcome to department head log menu\n");
    printf("[#] The available actions here are listed below:\n");
    printf("\n1. Search for a student\n2. Brief list of students");
    printf("\n3. Brief list of lessons\n4. list of deleted lessons");
    printf("\n5. list of enabled lessons\n6. Grades of a specific student");
    printf("\n7. Grades of students in a specific lesson");
    printf("\n8. Grades of students in a specific lesson (sorted)");
    printf("\n9. Average grade of a student\n10. Average grade of a lesson");
    printf("\n11. List of students sorted by their average grade");
    printf("\n12. List of students passed a specific lesson");
    printf("\n13. List of students failed a specific lesson");
    printf("\n14. List of failed students");
    printf("\n15. List of failed students that passed a specific lesson");
    printf("\n\n0. Return to the main menu");
    printf("\n\n[?] Enter your choice (enter number): ");

    int digit = inputDigit();

    if (digit == 1)
        searchStudent();

    headLog(user);
}

void searchStudent(void) {
    printf("[#] The available options you can search with is:\n");
    printf("\n1. First name\n2. Last name\n3. Student number");
    printf("\n\n0. Return to log menu");
    printf("\n\n[?] Enter your choice (enter number): ");
    int digit = inputDigit();

    if (digit <= 3 && digit >= 1) {
        struct student *start, *curr, *end;
        getStudents(&start, &end);
        char input[50];
        if (digit == 1) {
            printf("[?] Enter first name: ");
            inputString(input, MAX_NAME_LEN);
        }
        else if (digit == 2) {
            printf("[?] Enter last name: ");
            inputString(input, MAX_NAME_LEN);
        }
        else {
            printf("[?] Enter student number: ");
            inputStringNumber(input, STUDENT_NUMBER_LEN);
        }

        curr = start;
        while (curr != NULL) {
            if (digit == 1 && strcmp(input, curr->firstName) == 0)
                break;

            if (digit == 2 && strcmp(input, curr->lastName) == 0)
                break;

            if (digit == 3 && strcmp(input, curr->studentNumber) == 0)
                break;

            curr = curr->next;
        }

        if (curr != NULL) {
            printf("[#] Student found:\n[#] Name: %s %s\n", curr->firstName, curr->lastName);
            printf("[#] National number: %s\n", curr->nationalNumber);
            printf("[#] Birth date: %s\n[#] Birth city: %s\n", curr->birthDate, curr->birthCity);
            printf("[#] Major: %s\n[#] Student number: %s\n", curr->major, curr->studentNumber);
            printf("[#] Phone number: %s\n[#] Email: %s\n", curr->phoneNumber, curr->email);
            printf("\n\nPress any key to continue...");
            getch();
        }
        else {
            printf("[#] No results");
            Sleep(1000);
        }

        freeStudents(start, end);
        searchStudent();
    }
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
        printf("[#] List of brief infromation of department heads:\n");
        printf("+----+----------------------+---------------------------+-----------------+------------+\n");
        printf("| No | Ac. Group            | Last Name                 | Username        | Status     |\n");
        printf("+----+----------------------+---------------------------+-----------------+------------+\n");
    }

    int num = 1;
    struct departmentHead *curr;
    curr = start;
    while (curr != NULL) {
        printf("| %-02d | %-20s | %-25s |",  num, curr->groupName, curr->lastName);
        printf(" %-15s | %-10s |\n", curr->username, curr->status);
        printf("+----+----------------------+---------------------------+-----------------+------------+\n");
        num++;
        curr = curr->next;
    }

    if (start != NULL) {
        printf("[#] List of full information of department heads:\n");
    }

    curr = start;
    num = 1;
    while (curr != NULL) {
        printf("\n[#] Index: %d\n", num);
        printf("[#] Name: %s %s\n", curr->firstName, curr->lastName);
        printf("[#] Join date: %s\n", curr->joinDate);
        printf("[#] Academic group: %s\n", curr->groupName);
        printf("[#] Username: %s\n", curr->username);
        printf("[#] National number: %s\n", curr->nationalNumber);
        printf("[#] Phone number: %s\n", curr->phoneNumber);
        printf("[#] Email: %s\n", curr->email);
        printf("[#] Status: %s\n", curr->status);

        curr = curr->next;
        num++;
    }

    freeDepartmentHeads(start, end);
}

void listStaff(void) {
    struct academicStaff *start, *end;
    getAcademicStaff(&start, &end);

    if (start != NULL) {
        printf("[#] List of brief information of academic staff:\n");
        printf("+----+------+---------------------------+-----------------+------------+\n");
        printf("| No | Rank | Last Name                 | Username        | Status     |\n");
        printf("+----+------+---------------------------+-----------------+------------+\n");
    }

    int num = 1;
    struct academicStaff *curr = start;
    while (curr != NULL) {
        printf("| %-2d | %-4d | %-25s |", num, curr->rank, curr->lastName);
        printf(" %-15s | %-10s |\n", curr->username, curr->status);
        printf("+----+------+---------------------------+-----------------+------------+\n");
        num++;
        curr = curr->next;
    }

    if (start != NULL) {
        printf("[#] List of full information of academic staff:\n");
    }

    curr = start;
    num = 1;
    while (curr != NULL) {
        printf("\n[#] Index: %d\n", num);
        printf("[#] Name: %s %s\n", curr->firstName, curr->lastName);
        printf("[#] Join date: %s\n", curr->joinDate);
        printf("[#] Rank: %d\n", curr->rank);
        printf("[#] Username: %s\n", curr->username);
        printf("[#] Phone number: %s\n", curr->phoneNumber);
        printf("[#] Email: %s\n", curr->email);
        printf("[#] Status: %s\n", curr->status);

        curr = curr->next;
        num++;
    }

    freeAcademicStaff(start, end);
}

void listDeletedHeads(void) {
    struct departmentHead *start, *end;
    getDepartmentHeads(&start, &end);

    if (start != NULL)
        printf("[#] List of deleted department heads:\n");

    else
        printf("[#] No deleted department heads found");

    struct departmentHead *curr;
    curr = start;
    int num = 1;
    while (curr != NULL) {
        if (strcmp(curr->status, "deleted") != 0) {
            curr = curr->next;
            continue;
        }

        printf("\n[#] Index: %d\n", num);
        printf("[#] Name: %s %s\n", curr->firstName, curr->lastName);
        printf("[#] Join date: %s\n", curr->joinDate);
        printf("[#] Academic group: %s\n", curr->groupName);
        printf("[#] Username: %s\n", curr->username);
        printf("[#] National number: %s\n", curr->nationalNumber);
        printf("[#] Phone number: %s\n", curr->phoneNumber);
        printf("[#] Email: %s\n", curr->email);
        printf("[#] Status: %s\n", curr->status);

        curr = curr->next;
        num++;
    }

    freeDepartmentHeads(start, end);
}

void listDeletedStaff(void) {
    struct academicStaff *start, *end;
    getAcademicStaff(&start, &end);

    if (start != NULL)
        printf("[#] List of deleted academic staff:\n");

    else
        printf("[#] No deleted academic staff found");

    struct academicStaff *curr;
    curr = start;
    int num = 1;
    while (curr != NULL) {
        if (strcmp(curr->status, "deleted") != 0) {
            curr = curr->next;
            continue;
        }

        printf("\n[#] Index: %d\n", num);
        printf("[#] Name: %s %s\n", curr->firstName, curr->lastName);
        printf("[#] Join date: %s\n", curr->joinDate);
        printf("[#] Rank: %d\n", curr->rank);
        printf("[#] Username: %s\n", curr->username);
        printf("[#] Phone number: %s\n", curr->phoneNumber);
        printf("[#] Email: %s\n", curr->email);
        printf("[#] Status: %s\n", curr->status);

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

void addNewStudent(void) {
    char strNum[5];
    printf("[?] Enter number of students you want to enter: ");
    inputStringNumber(strNum, 5);
    int num = atoi(strNum);

    struct student *first, *curr, *last;
    first = NULL;
    last = NULL;
    int i;
    for (i = 0; i < num; i++) {
        curr = malloc(sizeof(struct student));
        printf("\n[#] Enter the information of student #%d:\n", i + 1);

        printf("[?] Enter first name: ");
        inputString(curr->firstName, MAX_NAME_LEN);

        printf("[?] Enter last name: ");
        inputString(curr->lastName, MAX_NAME_LEN);

        printf("[?] Enter national number: ");
        inputStringNumber(curr->nationalNumber, NATIONAL_NUMBER_LEN);

        printf("[?] Enter birth date: ");
        inputString(curr->birthDate, DATE_LEN);

        printf("[?] Enter birth city name: ");
        inputString(curr->birthCity, MAX_NAME_LEN);

        printf("[?] Enter major: ");
        inputString(curr->major, MAX_NAME_LEN);

        printf("[?] Enter student number: ");
        inputStringNumber(curr->studentNumber, MAX_NAME_LEN);

        printf("[?] Enter phone number: ");
        inputStringNumber(curr->phoneNumber, PHONE_NUMBER_LEN);

        printf("[?] Enter email: ");
        inputString(curr->email, MAX_EMAIL_LEN);

        curr->next = NULL;
        if (first == NULL) {
            first = curr;
            last = first;
        }
        else {
            last->next = curr;
            last = curr;
        }
    }

    struct student *start, *end;
    getStudents(&start, &end);
    if (start == NULL) {
        start = first;
        end = last;
    }
    else {
        end->next = first;
        end = last;
    }

    setStudents(start, end);
    freeStudents(start, end);

    printf("[#] Students added");
    Sleep(1000);
}

void addNewLesson(void) {
    int num;
    printf("\n[?] How many lessons do you want to add (enter number): ");
    scanf("%d", &num);

    struct lesson *start, *curr, *end;
    start = NULL;
    end = NULL;

    int i;
    for (i = 0; i < num; i++) {
        curr = malloc(sizeof(struct lesson));
        printf("[#] Enter the information of lesson #%d:\n", i + 1);

        printf("[?] Enter name: ");
        inputString(curr->name, MAX_NAME_LEN);

        char strNumber[5];
        printf("[?] Enter units: ");
        inputStringNumber(strNumber, 5);
        curr->units = atoi(strNumber);

        printf("[?] Enter type: ");
        inputString(curr->type, MAX_NAME_LEN);

        printf("[?] Enter code: ");
        inputStringNumber(curr->code, LESSON_CODE_LEN);

        strcpy(curr->status, "enabled");
        curr->next = NULL;

        if (start == NULL) {
            start = curr;
            end = start;
        }
        else {
            end->next = curr;
            end = curr;
        }
    }

    struct lesson *first, *last;
    getLessons(&first, &last);
    if (first == NULL) {
        first = start;
        last = end;
    }
    else {
        last->next = start;
        last = end;
    }

    setLessons(first, last);
    freeLessons(first, last);

    printf("[#] Lessons added");
    Sleep(1000);
}

void addNewGrade(User user) {
    char strNum[5];
    printf("[?] Enter the number of grades you want to add: ");
    inputStringNumber(strNum, 5);
    int num = atoi(strNum);

    struct grade *first, *curr, *last;
    first = NULL;
    last = NULL;
    int i;
    for (i = 0; i < num; i++) {
        curr = malloc(sizeof(struct grade));
        printf("[#] Enter the information of grade #%d:\n", i + 1);

        printf("[?] Enter lesson code: ");
        inputStringNumber(curr->lessonCode, LESSON_CODE_LEN);

        printf("[?] Enter student number: ");
        inputStringNumber(curr->studentNumber, STUDENT_NUMBER_LEN);

        char strNumber[10];
        printf("[?] Enter grade: ");
        inputStringFloat(strNumber, 10);
        curr->grade = atof(strNumber);

        strcpy(curr->username, user.username);
        curr->time = time(NULL);
        curr->next = NULL;

        if (first == NULL) {
            first = curr;
            last = curr;
        }
        else {
            last->next = curr;
            last = curr;
        }
    }

    struct grade *start, *end;
    getGrades(&start, &end);
    if (start == NULL) {
        start = first;
        end = last;
    }
    else {
        end->next = first;
        end = last;
    }

    setGrades(start, end);
    freeGrades(start, end);

    printf("[#] Grades added");
    Sleep(1000);
}

void editLesson(void) {
    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    struct lesson *start, *end, *curr;
    getLessons(&start, &end);
    curr = getLessonByLessonCode(start, end, lessonCode);
    if (curr == NULL) {
        printf("[!] Lesson not found");
        Sleep(1000);
    }
    else {
        printf("[#] Lesson found:\nName: %s\n", curr->name);
        printf("Type: %s\nUnits: %d\n", curr->type, curr->units);
        printf("[#] The available options to edit are listed below:\n\n");
        printf("1. Lesson name\n2. Lesson type\n3. Units\n\n0. Cancel\n");
        printf("[?] Enter your choice (enter number): ");

        int digit = inputDigit();
        if (digit == 1) {
            printf("\n[?] Enter new name: ");
            inputString(curr->name, MAX_NAME_LEN);
            setLessons(start, end);
            printf("[#] Changes, submitted");
            Sleep(1000);
        }
        else if (digit == 2) {
            printf("\n[?] Enter new type: ");
            inputString(curr->type, MAX_NAME_LEN);
            setLessons(start, end);
            printf("[#] Changes, submitted");
            Sleep(1000);
        }
        else if (digit == 3) {
            char strNumber[5];
            printf("\n[?] Enter new number of units: ");
            inputStringNumber(strNumber, 5);
            curr->units = atoi(strNumber);
            setLessons(start, end);
            printf("[#] Changes, submitted");
            Sleep(1000);
        }
    }
    freeLessons(start, end);
}

void editGrade(void) {
    char studentNumber[STUDENT_NUMBER_LEN];
    printf("[?] Enter student number: ");
    inputStringNumber(studentNumber, STUDENT_NUMBER_LEN);

    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    struct grade *start, *end, *curr;
    getGrades(&start, &end);
    curr = getGradeByLessonCodeAndStudentNumber(start, end, lessonCode, studentNumber);
    if (curr == NULL) {
        printf("[!] Grade not found");
        Sleep(1000);
    }
    else {
        printf("[#] Grade found: %.2f\n", curr->grade);
        char strNumber[10];
        printf("[?] Enter new grade: ");
        inputStringFloat(strNumber, 10);
        curr->grade = atof(strNumber);
        setGrades(start, end);
        printf("[#] Grade changed");
        Sleep(1000);
    }

    freeGrades(start, end);
}

void editHead(User user) {
    struct departmentHead *start, *end, *head;
    getDepartmentHeads(&start, &end);
    head = getHeadByUsername(start, end, user.username);
    if (head == NULL) {
        printf("[Error] Something went wrong.");
        exit(1);
    }

    printf("[#] Your account settings is as below:\n\n");
    printf("[#] Account type: Department Head\n");
    printf("[#] Academic group name: %s\n", head->groupName);
    printf("[#] Name: %s %s\n", head->firstName, head->lastName);
    printf("[#] Username: %s\n[#] Email: %s\n", head->username, head->email);
    printf("[#] National number: %s\n", head->nationalNumber);
    printf("[#] Phone number: %s\n\n", head->phoneNumber);

    printf("[#] The editable options are listed below:\n");
    printf("1. Password\n2. Email\n3. Phone number\n\n0. Cancel\n\n");
    printf("[?] Enter your choice (enter number): ");

    int digit = inputDigit();
    if (digit == 0) {
        printf("[#] Operation canceled");
        Sleep(1000);
    }
    else if (digit == 1) {
        char old_password[MAX_PASSWORD_LEN];
        printf("[?] Enter old password: ");
        inputPassword(old_password, MAX_PASSWORD_LEN);
        if (strcmp(head->password, old_password) == 0) {
            char password1[MAX_PASSWORD_LEN], password2[MAX_PASSWORD_LEN];
            printf("[?] Enter new password: ");
            inputPassword(password1, MAX_PASSWORD_LEN);
            printf("[?] Enter password again: ");
            inputPassword(password2, MAX_PASSWORD_LEN);

            if (strcmp(password1, password2) == 0) {
                strcpy(head->password, password1);
                setDepartmentHeads(&start, &end);
                printf("[#] Password changed");
            }
            else {
                printf("[!] Passwords do not match");
            }
        }
        else {
            printf("[!] Password wrong");
        }

        Sleep(1000);
    }
    else if (digit == 2) {
        printf("[?] Enter new email: ");
        inputString(head->email, MAX_EMAIL_LEN);
        setDepartmentHeads(&start, &end);
        printf("[#] Email changed");
        Sleep(1000);
    }
    else if (digit == 3) {
        printf("[?] Enter new phone number: ");
        inputStringNumber(head->phoneNumber, PHONE_NUMBER_LEN);
        setDepartmentHeads(&start, &end);
        printf("[#] Phone number changed");
        Sleep(1000);
    }

    freeDepartmentHeads(start, end);
}

void editStaff(User user) {
    struct academicStaff *start, *end, *staff;
    getAcademicStaff(&start, &end);
    staff = getStaffByUsername(start, end, user.username);
    if (staff == NULL) {
        printf("[Error] Something went wrong.");
        exit(1);
    }

    printf("[#] Your account settings is as below:\n\n");
    printf("[#] Account type: Academic Staff\n");
    printf("[#] Name: %s %s\n", staff->firstName, staff->lastName);
    printf("[#] Username: %s\n[#] Email: %s\n", staff->username, staff->email);
    printf("[#] Rank: %d\n", staff->rank);
    printf("[#] Phone number: %s\n\n", staff->phoneNumber);

    printf("[#] The editable options are listed below:\n");
    printf("1. Password\n2. Email\n3. Phone number\n\n0. Cancel\n\n");
    printf("[?] Enter your choice (enter number): ");

    int digit = inputDigit();
    if (digit == 0) {
        printf("[#] Operation canceled");
        Sleep(1000);
    }
    else if (digit == 1) {
        char old_password[MAX_PASSWORD_LEN];
        printf("[?] Enter old password: ");
        inputPassword(old_password, MAX_PASSWORD_LEN);
        if (strcmp(staff->password, old_password) == 0) {
            char password1[MAX_PASSWORD_LEN], password2[MAX_PASSWORD_LEN];
            printf("[?] Enter new password: ");
            inputPassword(password1, MAX_PASSWORD_LEN);
            printf("[?] Enter password again: ");
            inputPassword(password2, MAX_PASSWORD_LEN);

            if (strcmp(password1, password2) == 0) {
                strcpy(staff->password, password1);
                setAcademicStaff(&start, &end);
                printf("[#] Password changed");
            }
            else {
                printf("[!] Passwords do not match");
            }
        }
        else {
            printf("[!] Password wrong");
        }

        Sleep(1000);
    }
    else if (digit == 2) {
        printf("[?] Enter new email: ");
        inputString(staff->email, MAX_EMAIL_LEN);
        setAcademicStaff(&start, &end);
        printf("[#] Email changed");
        Sleep(1000);
    }
    else if (digit == 3) {
        printf("[?] Enter new phone number: ");
        inputStringNumber(staff->phoneNumber, PHONE_NUMBER_LEN);
        setAcademicStaff(&start, &end);
        printf("[#] Phone number changed");
        Sleep(1000);
    }

    freeAcademicStaff(start, end);
}

void editStudent(void) {
    char studentNumber[STUDENT_NUMBER_LEN];
    printf("[?] Enter student number: ");
    inputStringNumber(studentNumber, STUDENT_NUMBER_LEN);

    struct student *start, *curr, *end;
    getStudents(&start, &end);
    curr = getStudentByStudentNumber(start, end, studentNumber);

    if (curr == NULL) {
        printf("[#] Student not found");
        Sleep(1000);
    }
    else {
        printf("[#] Student found:\n[#] Name: %s %s\n", curr->firstName, curr->lastName);
        printf("[#] National number: %s\n", curr->nationalNumber);
        printf("[#] Birth date: %s\n[#] Birth city: %s\n", curr->birthDate, curr->birthCity);
        printf("[#] Major: %s\n[#] Student number: %s\n", curr->major, curr->studentNumber);
        printf("[#] Phone number: %s\n[#] Email: %s\n", curr->phoneNumber, curr->email);

        printf("\n[#] The available options to edit are listed below:\n");
        printf("\n1. Major\n2. Phone number\n3. Email\n\n0. Cancel");
        printf("\n\n[?] Enter your choice (enter number): ");
        int digit = inputDigit();

        if (digit == 0) {
            printf("[#] Operation canceled");
            Sleep(1000);
        }
        else if (digit == 1) {
            printf("[?] Enter new major: ");
            inputString(curr->major, MAX_NAME_LEN);
            setStudents(start, end);
            printf("[#] Changes saved");
            Sleep(1000);
        }
        else if (digit == 2) {
            printf("[?] Enter new phone number: ");
            inputStringNumber(curr->phoneNumber, PHONE_NUMBER_LEN);
            setStudents(start, end);
            printf("[#] Changes saved");
            Sleep(1000);
        }
        else if (digit == 3) {
            printf("[?] Enter new email: ");
            inputString(curr->email, PHONE_NUMBER_LEN);
            setStudents(start, end);
            printf("[#] Changes saved");
            Sleep(1000);
        }
    }

    freeStudents(start, end);
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

void deleteLesson(void) {
    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    struct lesson *start, *end, *curr;
    getLessons(&start, &end);
    curr = getLessonByLessonCode(start, end, lessonCode);
    if (curr == NULL) {
        printf("[!] Lesson not found");
        Sleep(1000);
    }
    else {
        printf("[#] Lesson found: %s - %s\n", curr->type, curr->name);
        char answer[5];
        printf("[?] Do you want to delete it (yes/no): ");
        inputString(answer, 5);

        if (strcmp(answer, "yes") == 0) {
            strcpy(curr->status, "deleted");
            setLessons(start, end);
            printf("[#] Lesson deleted");
            Sleep(1000);
        }
        else {
            printf("[#] Operation canceled");
            Sleep(1000);
        }
    }

    freeLessons(start, end);
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

struct departmentHead *getHeadByUsername(struct departmentHead *start, struct departmentHead *end, char username[]) {
    struct departmentHead *curr;
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

    return curr;
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

struct academicStaff *getStaffByUsername(struct academicStaff *start, struct academicStaff *end, char username[]) {
    struct academicStaff *curr;
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

    return curr;
}

void freeAcademicStaff(struct academicStaff *start, struct academicStaff *end) {
    end = start;
    while (end != NULL) {
        end = start->next;
        free(start);
        start = end;
    }
}

void getStudents(struct student **start, struct student **end) {
    FILE *fp;
    fp = fopen(STUDENTS_FILE, "r");
    if (fp == NULL) {
        fp = fopen(STUDENTS_FILE, "w");
        fclose(fp);
        fp = fopen(STUDENTS_FILE, "r");
        if (fp == NULL) {
            printf("[Error] Something went wrong while reading a file");
            exit(1);
        }
    }

    struct student *curr;
    *start = NULL;
    *end = NULL;
    while (!feof(fp)) {
        curr = malloc(sizeof(struct student));

        if (fgets(curr->firstName, MAX_NAME_LEN, fp) == NULL) {
            free(curr);
            break;
        }
        removeNewLine(curr->firstName);

        fgets(curr->lastName, MAX_NAME_LEN, fp);
        removeNewLine(curr->lastName);

        fgets(curr->nationalNumber, NATIONAL_NUMBER_LEN, fp);
        removeNewLine(curr->nationalNumber);

        fgets(curr->birthDate, DATE_LEN, fp);
        removeNewLine(curr->birthDate);

        fgets(curr->birthCity, MAX_NAME_LEN, fp);
        removeNewLine(curr->birthCity);

        fgets(curr->major, MAX_NAME_LEN, fp);
        removeNewLine(curr->major);

        fgets(curr->studentNumber, STUDENT_NUMBER_LEN, fp);
        removeNewLine(curr->studentNumber);

        fgets(curr->phoneNumber, PHONE_NUMBER_LEN, fp);
        removeNewLine(curr->phoneNumber);

        fgets(curr->email, MAX_EMAIL_LEN, fp);
        removeNewLine(curr->email);

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

void setStudents(struct student *start, struct student *end) {
    FILE *fp;
    fp = fopen(STUDENTS_FILE, "w");
    if (fp == NULL) {
        printf("[Error] Something went wrong while writing to a file");
        exit(1);
    }

    struct student *curr;
    curr = start;
    while (curr != NULL) {
        fprintf(fp, "%s\n", curr->firstName);
        fprintf(fp, "%s\n", curr->lastName);
        fprintf(fp, "%s\n", curr->nationalNumber);
        fprintf(fp, "%s\n", curr->birthDate);
        fprintf(fp, "%s\n", curr->birthCity);
        fprintf(fp, "%s\n", curr->major);
        fprintf(fp, "%s\n", curr->studentNumber);
        fprintf(fp, "%s\n", curr->phoneNumber);
        fprintf(fp, "%s\n", curr->email);
        curr = curr->next;
    }

    fclose(fp);
}

struct student *getStudentByStudentNumber(struct student *start, struct student *end, char studentNumber[]) {
    struct student *curr;
    curr = start;
    while (curr != NULL) {
        if (strcmp(curr->studentNumber, studentNumber) == 0)
            break;

        curr = curr->next;
    }

    return curr;
}

void freeStudents(struct student *start, struct student *end) {
    end = start;
    while (end != NULL) {
        end = start->next;
        free(start);
        start = end;
    }
}

void getLessons(struct lesson **start, struct lesson **end) {
    FILE *fp;
    fp = fopen(LESSONS_FILE, "r");
    if (fp == NULL) {
        fp = fopen(LESSONS_FILE, "w");
        fclose(fp);
        fp = fopen(LESSONS_FILE, "r");
        if (fp == NULL) {
            printf("[Error] Something went wrong while reading a file");
            exit(1);
        }
    }

    *start = NULL;
    *end = NULL;
    struct lesson *curr;
    while (!feof(fp)) {
        curr = malloc(sizeof(struct lesson));
        if (fgets(curr->name, MAX_NAME_LEN, fp) == NULL) {
            free(curr);
            break;
        }
        removeNewLine(curr->name);

        fgets(curr->type, MAX_NAME_LEN, fp);
        removeNewLine(curr->type);

        fgets(curr->code, LESSON_CODE_LEN, fp);
        removeNewLine(curr->code);

        char strNumber[5];
        fgets(strNumber, 5, fp);
        removeNewLine(strNumber);
        curr->units = atoi(strNumber);

        fgets(curr->status, STATUS_LEN, fp);
        removeNewLine(curr->status);

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

void setLessons(struct lesson *start, struct lesson *end) {
    FILE *fp;
    fp = fopen(LESSONS_FILE, "w");
    if (fp == NULL) {
        printf("[Error] Something went wrong while write to afile");
        exit(1);
    }

    struct lesson *curr;
    curr = start;
    while (curr != NULL) {
        fprintf(fp, "%s\n", curr->name);
        fprintf(fp, "%s\n", curr->type);
        fprintf(fp, "%s\n", curr->code);
        fprintf(fp, "%d\n", curr->units);
        fprintf(fp, "%s\n", curr->status);
        curr = curr->next;
    }

    fclose(fp);
}

struct lesson *getLessonByLessonCode(struct lesson *start, struct lesson *end, char lessonCode[]) {
    struct lesson *curr;
    curr = start;
    while (curr != NULL) {
        if (strcmp(curr->status, "deleted") == 0) {
            curr = curr->next;
            continue;
        }

        if (strcmp(curr->code, lessonCode) == 0)
            break;

        curr = curr->next;
    }

    return curr;
}

void freeLessons(struct lesson *start, struct lesson *end) {
    end = start;
    while (end != NULL) {
        end = start->next;
        free(start);
        start = end;
    }
}

void getGrades(struct grade **start, struct grade **end) {
    FILE *fp;
    fp = fopen(GRADES_FILE, "r");
    if (fp == NULL) {
        fp = fopen(GRADES_FILE, "w");
        fclose(fp);
        fp = fopen(GRADES_FILE, "r");
        if (fp == NULL) {
            printf("[Error] Something went wrong while reading a file");
            exit(1);
        }
    }

    *start = NULL;
    *end = NULL;
    struct grade *curr;
    while (!feof(fp)) {
        curr = malloc(sizeof(struct grade));

        if (fgets(curr->studentNumber, STUDENT_NUMBER_LEN, fp) == NULL) {
            free(curr);
            break;
        }
        removeNewLine(curr->studentNumber);

        fgets(curr->lessonCode, LESSON_CODE_LEN, fp);
        removeNewLine(curr->lessonCode);

        fgets(curr->username, MAX_USERNAME_LEN, fp);
        removeNewLine(curr->username);

        char strNumber[30];
        fgets(strNumber, 30, fp);
        removeNewLine(strNumber);
        curr->grade = atof(strNumber);

        fgets(strNumber, 30, fp);
        removeNewLine(strNumber);
        curr->time = atoll(strNumber);

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

void setGrades(struct grade *start, struct grade *end) {
    FILE *fp;
    fp = fopen(GRADES_FILE, "w");
    if (fp == NULL) {
        printf("[Error] Something went wrong while writing to a file");
        exit(1);
    }

    struct grade *curr;
    curr = start;
    while (curr != NULL) {
        fprintf(fp, "%s\n", curr->studentNumber);
        fprintf(fp, "%s\n", curr->lessonCode);
        fprintf(fp, "%s\n", curr->username);
        fprintf(fp, "%f\n", curr->grade);
        fprintf(fp, "%lld\n", curr->time);
        curr = curr->next;
    }

    fclose(fp);
}

struct grade *getGradeByLessonCodeAndStudentNumber(struct grade *start, struct grade *end, char lessonCode[], char studentNumber[]) {
    struct grade *curr;
    curr = start;
    while (curr != NULL) {
        if (strcmp(curr->lessonCode, lessonCode) != 0) {
            curr = curr->next;
            continue;
        }

        if (strcmp(curr->studentNumber, studentNumber) == 0)
            break;

        curr = curr->next;
    }

    return curr;
}

void freeGrades(struct grade *start, struct grade *end) {
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
    printf("%c\n", input);
    
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

void inputStringFloat(char strNumber[], int size) {
    fflush(stdin);

    int decimalPoint = 0;
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
            if (strNumber[n_chars] == '.')
                decimalPoint = 0;
        }

        if (isdigit(ch) || ch == '.') {
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
