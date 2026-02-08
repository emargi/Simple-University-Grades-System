#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <sodium.h>

// constants
// characters + '\n' (fgets BUG) + '\0'
#define MAX_PASSWORD_LEN 26
#define MAX_NAME_LEN 26
#define MAX_EMAIL_LEN 21
#define MAX_USERNAME_LEN 16
#define MAX_SIGNIN_TRIES 3
#define MAX_SIGNIN_TRIES_PERIOD 15
#define MAX_CAPTCHA_RAND_NUM 30

#define PHONE_NUMBER_LEN 13
#define NATIONAL_NUMBER_LEN 12
#define DATE_LEN 12
#define STATUS_LEN 10
#define LESSON_CODE_LEN 7
#define STUDENT_NUMBER_LEN 12
#define PASSING_GRADE 10
#define MAX_GRADE 20
#define MIN_GRADE 0
#define SIGNIN_LIMIT_TIME 300
#define HASHED_PASSWORD_LEN crypto_pwhash_STRBYTES + 1

#define HEAD_PROFILES_FILE "department_head_profiles.txt"
#define STAFF_PROFILES_FILE "academic_staff_profiles.txt"
#define LESSONS_FILE "lessons.txt"
#define GRADES_FILE "grades.txt"
#define STUDENTS_FILE "students.txt"
#define SIGN_IN_FILE "sign_in.txt"
#define LIMITS_FILE "limits.txt"

const char USERNAME_CHARS[] = "abcdefghijklmnopqrstuvwxyz1234567890_";

// structures
struct user {
    char type[6]; // admin, head, staff
    char username[MAX_USERNAME_LEN];
};
typedef struct user User;

struct departmentHead {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char username[MAX_USERNAME_LEN];
    char hashedPassword[HASHED_PASSWORD_LEN];
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
    char hashedPassword[HASHED_PASSWORD_LEN];
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

struct signIn {
    char username[MAX_USERNAME_LEN];
    char status[STATUS_LEN];
    long long int time;
    struct signIn *next;
};

struct limit {
    char username[MAX_USERNAME_LEN];
    long long int releaseTime;
    struct limit *next;
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
int validateName(char name[]);
int validatePassword(char password[]);

void removeNewLine(char str[]);
void extractDateInfo(char date[], int *year, int *month, int *day);
void getTimeInfo(long long int time, int *year, int *month, int *day, int *hour, int *min, int *sec);
void hashPassword(char hashedPassword[], char password[]);
int verifyPassword(char hashedPassword[], char password[]);
void lockPasswordMem(char password[]);
void unlockPasswordMem(char password[]);

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
void getSignIns(struct signIn **start, struct signIn **end);
void setSignIns(struct signIn *start, struct signIn *end);
void freeSignIns(struct signIn *start, struct signIn *end);
void getLimits(struct limit **start, struct limit **end);
void setLimits(struct limit *start, struct limit *end);
void freeLimits(struct limit *start, struct limit *end);
struct departmentHead *getHeadByUsername(struct departmentHead *start, struct departmentHead *end, char username[]);
struct academicStaff *getStaffByUsername(struct academicStaff *start, struct academicStaff *end, char username[]);
struct student *getStudentByStudentNumber(struct student *start, struct student *end, char studentNumber[]);
struct lesson *getLessonByLessonCode(struct lesson *start, struct lesson *end, char lessonCode[]);
struct grade *getGradeByLessonCodeAndStudentNumber(struct grade *start, struct grade *end, char lessonCode[], char studentNumber[]);
float getAverageGradeByStudentNumber(struct grade *start, struct grade *end, char studentNumber[]);
int checkLimitByUsername(char username[]);
int checkUserByUsername(char username[]);
int checkStudentByStudentNumber(char studentNumber[]);
int checkLessonByLessonCode(char lessonCode[]);

void sortStudentsByAverageGrade(struct student **startStu, struct student **endStu, struct grade *startGra, struct grade *endGra, int numStu);
void sortGradesByStudentLastName(struct grade **startGra, struct grade **endGra, struct student *startStu, struct student *endStu, int numGra);

void captcha(void);

User loginPanel(void);
User loginAdmin(void);
User loginHead(void);
User loginStaff(void);

void resetPasswordMenu(void);

void mainMenu(User user);
void adminMainMenu(User user);
void headMainMenu(User user);
void staffMainMenu(User user);

void themeMenu(void);

void adminLog(User user);
void headLog(User user);
void staffLog(User user);

void searchStudent(void);
void studentAverageGrade(void);
void lessonAverageGrade(void);
void listUsers(void);
void listHeads(void);
void listStaff(void);
void listStudents(void);
void listLessons(void);
void listStudentGrades(void);
void listFailingStudents(void);
void listStudentsPassingLesson(void);
void listStudentsFailingLesson(void);
void listStudentsBornInTimePeriod(void);
void listStudentsOfMajor(void);
void listStudentsBornInCity(void);
void listFailingStudentsStudiedLesson(void);
void listLessonGrades(void);
void listEnabledLessons(void);
void listDeletedHeads(void);
void listDeletedStaff(void);
void listDeletedLessons(void);
void listStudentsSortedByAverageGrade(void);
void listLessonGradesSortedByStudentName(void);
void listSignIns(void);
void listSuccessfulSignIns(void);

void addNewHead(void);
void addNewStaff(void);
void addNewStudent(void);
void addNewLesson(void);
void addNewGrade(User user);
void addNewSignIn(char username[], char status[]);
void addNewLimit(char username[], long long int releaseTime);
void editLesson(void);
void editGrade(void);
void editHead(User user);
void editStaff(User user);
void editStudent(void);
void createBackup(void);
void useBackup(void);

int backupFile(char dirPath[], char filename[]);
int useBackupFile(char dirPath[], char filename[]);

void deleteUser(void);
int deleteHead(char username[]);
int deleteStaff(char username[]);
void deleteLesson(void);

// main function
void main() {
    // libsodium initialization
    if (sodium_init() < 0) {
        printf("[!] An external library is not installed.");
        exit(1);
    }

    captcha();
    User user = loginPanel();
    mainMenu(user);
}

// function definitions
void captcha(void) {
    system("cls");
    printf("[#] To use the program, you must solve the captcha:\n\n");

    srand(time(NULL));
    int num1, num2, answer;
    do {
        num1 = rand() % MAX_CAPTCHA_RAND_NUM + 1;
        num2 = rand() % MAX_CAPTCHA_RAND_NUM + 1;

        printf("[#] %d + %d\n[?] Enter result (0 to exit): ", num1, num2);

        char strNum[10];
        inputStringNumber(strNum, 10);
        answer = atoi(strNum);
        if (answer == 0)
            exit(0);

        if (answer != num1 + num2)
            printf("[!] Wrong answer. Try again\n\n");
    } while (answer != num1 + num2);
}

User loginPanel(void) {
    system("cls");

    printf("[#] Welcome to Student Management System\n");
    printf("[#] To use this program, you should first login to your account\n");
    printf("[#] The available roles to login as are listed below\n");
    printf("\n1. Admin\n2. Deparment Head\n3. Academic Staff");
    printf("\n\n9. Forgot password?\n0. Exit");
    printf("\n\n[?] Enter your choice (enter number): ");

    int choice = inputDigit();
    printf("\n");

    if (choice == 0)
        exit(1);

    if (choice == 1)
        return loginAdmin();

    if (choice == 2)
        return loginHead();

    if (choice == 3)
        return loginStaff();

    if (choice == 9)
        resetPasswordMenu();

    return loginPanel();
}

User loginAdmin(void) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char status[STATUS_LEN];
    int askAgain = 0;

    lockPasswordMem(password);

    do {
        printf("[?] Enter your username: ");
        inputString(username, MAX_USERNAME_LEN);

        if (checkLimitByUsername(username)) {
            printf("[!] This account is frozen. Try again a few minutes later\n\n");
            askAgain = 1;
            continue;
        }

        printf("[?] Enter password: ");
        inputPassword(password, MAX_PASSWORD_LEN);

        if (strcmp(username, "admin") != 0 || strcmp(password, "admin") != 0) {
            Beep(700, 400);
            printf("[!] Wrong information. Try again\n");
            strcpy(status, "failed");
            askAgain = 1;
        }
        else {
            strcpy(status, "success");
            askAgain = 0;
        }

        addNewSignIn(username, status);
    } while (askAgain);

    unlockPasswordMem(password);
    sodium_memzero(password, MAX_PASSWORD_LEN);

    User u;
    strcpy(u.type, "admin");
    strcpy(u.username, username);

    return u;
}

User loginHead(void) {
    struct departmentHead *start, *end, *user;
    getDepartmentHeads(&start, &end);

    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char hashedPassword[HASHED_PASSWORD_LEN];
    char status[STATUS_LEN];
    int correct;

    do {
        printf("[?] Enter username: ");
        inputUsername(username, MAX_USERNAME_LEN);

        if (checkLimitByUsername(username)) {
            printf("[!] This account is frozen. Try again a few minutes later\n\n");
            correct = 0;
            continue;
        }

        lockPasswordMem(password);
        printf("[?] Enter password: ");
        inputPassword(password, MAX_PASSWORD_LEN);

        user = getHeadByUsername(start, end, username);
        if (user != NULL && verifyPassword(user->hashedPassword, password)) {
            strcpy(status, "success");
            correct = 1;
        }
        else {
            Beep(700, 400);
            strcpy(status, "failed");
            correct = 0;
        }

        sodium_memzero(password, MAX_PASSWORD_LEN);
        unlockPasswordMem(password);
        addNewSignIn(username, status);
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

    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char hashedPassword[HASHED_PASSWORD_LEN];
    char status[STATUS_LEN];
    int correct;

    do {
        printf("[?] Enter username: ");
        inputUsername(username, MAX_USERNAME_LEN);

        if (checkLimitByUsername(username)) {
            printf("[!] This account is frozen. Try again a few minutes later\n\n");
            correct = 0;
            continue;
        }

        lockPasswordMem(password);
        printf("[?] Enter password: ");
        inputPassword(password, MAX_PASSWORD_LEN);
        
        user = getStaffByUsername(start, end, username);
        if (user != NULL && verifyPassword(user->hashedPassword, password)) {
            strcpy(status, "success");
            correct = 1;
        }
        else {
            Beep(700, 400);
            strcpy(status, "failed");
            correct = 0;
        }

        sodium_memzero(password, MAX_PASSWORD_LEN);
        unlockPasswordMem(password);
        addNewSignIn(username, status);
        if (!correct)
            printf("[!] Wrong information. Try again\n\n");
    } while (!correct);

    User u;
    strcpy(u.type, "staff");
    strcpy(u.username, user->username);

    freeAcademicStaff(start, end);
    return u;
}

void resetPasswordMenu(void) {
    system("cls");

    printf("[#] Welcome to reset password menu.\n");
    printf("[#] You can reset your password by answering the questions below:\n\n");

    char username[MAX_USERNAME_LEN];
    printf("[?] Enter username: ");
    inputUsername(username, MAX_USERNAME_LEN);

    struct departmentHead *startHead, *currHead, *endHead;
    getDepartmentHeads(&startHead, &endHead);
    currHead = getHeadByUsername(startHead, endHead, username);
    if (currHead != NULL) {
        char phoneNumber[PHONE_NUMBER_LEN];
        printf("[?] Enter your phone number: ");
        inputStringNumber(phoneNumber, PHONE_NUMBER_LEN);
        if (strcmp(phoneNumber, currHead->phoneNumber) == 0) {
            char hashedPassword[HASHED_PASSWORD_LEN];
            // save national number because hashPassword wipes it
            char temp[NATIONAL_NUMBER_LEN];
            strcpy(temp, currHead->nationalNumber);
            hashPassword(hashedPassword, temp);
            strcpy(currHead->hashedPassword, hashedPassword);
            setDepartmentHeads(&startHead, &endHead);
            printf("[#] Your national number is now set to your password\n");
        }
        else {
            Beep(700, 400);
            printf("[!] Wrong info. You can't reset your password");
        }
    }
    else {
        struct academicStaff *startStaff, *currStaff, *endStaff;
        getAcademicStaff(&startStaff, &endStaff);
        currStaff = getStaffByUsername(startStaff, endStaff, username);
        if (currStaff != NULL) {
            char email[MAX_EMAIL_LEN];
            printf("[?] Enter your email: ");
            inputString(email, MAX_EMAIL_LEN);
            if (strcmp(currStaff->email, email) == 0) {
                char hashedPassword[HASHED_PASSWORD_LEN];
                char temp[PHONE_NUMBER_LEN];
                strcpy(temp, currStaff->phoneNumber);
                hashPassword(hashedPassword, temp);
                strcpy(currStaff->hashedPassword, hashedPassword);
                setAcademicStaff(&startStaff, &endStaff);
                printf("[#] Your phone number is now set to your password");
            }
            else {
                Beep(700, 400);
                printf("[!] Wrong email. You can't reset your password");
            }
        }
        else {
            printf("[!] You can't reset this username's password");
        }
        freeAcademicStaff(startStaff, endStaff);
    }
    freeDepartmentHeads(startHead, endHead);

    Sleep(1000);
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
    printf("[#] The available actions are listed below:\n");
    printf("\n1. Add new department head\n2. Add new academic staff");
    printf("\n3. List users\n4. Delete user\n5. Log\n6. Create backup");
    printf("\n7. Use backup\n\n9. Theme menu\n0. Log out");
    printf("\n\n[?] Enter your choice (enter number): ");

    int choice = inputDigit();
    printf("\n");

    if (choice == 0) {
        // reset theme
        system("color 07");

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

    else if (choice == 9)
        themeMenu();

    mainMenu(user);
}

void headMainMenu(User user) {
    system("cls");
    printf("[#] The available actions are listed below:\n\n");
    printf("1. Add new lesson\n2. Add new grade\n3. Edit lesson");
    printf("\n4. Edit grade\n5. Delete lesson\n6. Log");
    printf("\n7. Account settings\n\n9. Theme menu\n0. Log out\n");

    printf("\n[?] Enter your choice (enter number): ");
    int digit = inputDigit();

    if (digit == 0) {
        system("color 07");

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

    else if (digit == 9)
        themeMenu();

    headMainMenu(user);
}

void staffMainMenu(User user) {
    system("cls");
    printf("[#] The available actions are listed below:\n");
    printf("\n1. Add student\n2. Edit student\n3. Add grade\n4. Log");
    printf("\n5. Account settings\n\n9. Theme menu\n0. Log out");
    printf("\n\n[?] Enter your choice (enter number): ");

    int digit = inputDigit();
    if (digit == 0) {
        // reset theme
        system("color 07");

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

    else if (digit == 4)
        staffLog(user);

    else if (digit == 5)
        editStaff(user);

    else if (digit == 9)
        themeMenu();

    staffMainMenu(user);
}

void themeMenu(void) {
    system("cls");

    printf("[#] Welcome to theme menu. in here you can change how the program looks\n");
    printf("[#] The available themes to apply, are listed below:\n");
    printf("\n1. Dark theme\n2. Light theme\n3. Hacker theme");
    printf("\n4. aqua theme (dark)\n5. aqua theme (light)\n\n0. Exit");
    printf("\n\n[?] Enter your choice (enter number): ");

    int choice = inputDigit();
    if (choice == 1)
        system("color 07"); // 0: black, 7: white

    else if (choice == 2)
        system("color f0");

    else if (choice == 3)
        system("color 0a");

    else if (choice == 4)
        system("color 0b");

    else if (choice == 5)
        system("color f3");

    // do not exit the function until user enters 0
    if (choice != 0)
        themeMenu();
}

void adminLog(User user) {
    system("cls");

    printf("[#] Welcome to admin log menu\n");
    printf("[#] The available actions here are listed below:\n");
    printf("\n1. List of department heads\n2. List of academic staff");
    printf("\n3. List of deleted department heads\n4. List of deleted academic staff");
    printf("\n5. List of all logins\n6. List of successful logins");
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

    else if (digit == 5)
        listSignIns();

    else if (digit == 6)
        listSuccessfulSignIns();

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
    printf("\n8. Grades of students in a specific lesson (sorted by last name)");
    printf("\n9. Average grade of a student\n10. Average grade of a lesson");
    printf("\n11. List of students sorted by their average grade");
    printf("\n12. List of students passed a specific lesson");
    printf("\n13. List of students failed a specific lesson");
    printf("\n14. List of failed students");
    printf("\n15. List of failed students that studied a specific lesson");
    printf("\n\n0. Return to the main menu");
    printf("\n\n[?] Enter your choice (enter number): ");

    char strNum[5];
    inputStringNumber(strNum, 5);
    int num = atoi(strNum);

    if (num == 1)
        searchStudent();

    else if (num == 2)
        listStudents();

    else if (num == 3)
        listLessons();

    else if (num == 4)
        listDeletedLessons();

    else if (num == 5)
        listEnabledLessons();

    else if (num == 6)
        listStudentGrades();

    else if (num == 7)
        listLessonGrades();

    else if (num == 8)
        listLessonGradesSortedByStudentName();

    else if (num == 9)
        studentAverageGrade();

    else if (num == 10)
        lessonAverageGrade();

    else if (num == 11)
        listStudentsSortedByAverageGrade();

    else if (num == 12)
        listStudentsPassingLesson();

    else if (num == 13)
        listStudentsFailingLesson();

    else if (num == 14)
        listFailingStudents();

    else if (num == 15)
        listFailingStudentsStudiedLesson();

    if (num != 0)
        headLog(user);
}

void staffLog(User user) {
    system("cls");

    printf("[#] Welcome to your log menu\n");
    printf("[#] The available actions here are listed below:\n");
    printf("\n1. Search for a student\n2. List of students born in a specific period of time");
    printf("\n3. List of students of a specific major\n4. List of students born in a specific city");
    printf("\n\n0. Return to the main menu\n\n");
    printf("[?] Enter your choice (enter number): ");

    int digit = inputDigit();

    if (digit == 1)
        searchStudent();

    else if (digit == 2)
        listStudentsBornInTimePeriod();

    else if (digit == 3)
        listStudentsOfMajor();

    else if (digit == 4)
        listStudentsBornInCity();

    if (digit != 0)
        staffLog(user);
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

void studentAverageGrade(void) {
    char stuNum[STUDENT_NUMBER_LEN];
    printf("[?] Enter student number: ");
    inputStringNumber(stuNum, STUDENT_NUMBER_LEN);

    struct student *startStu, *currStu, *endStu;
    getStudents(&startStu, &endStu);

    currStu = getStudentByStudentNumber(startStu, endStu, stuNum);
    if (currStu != NULL) {
        printf("[#] Student found: %s %s\n", currStu->firstName, currStu->lastName);
        struct grade *startGra, *currGra, *endGra;
        getGrades(&startGra, &endGra);
        float avg;
        avg = getAverageGradeByStudentNumber(startGra, endGra, currStu->studentNumber);

        printf("[#] Student's average grade: %6.3f", avg);
        freeGrades(startGra, endGra);
    }
    else
        printf("[!] Student not found");

    freeStudents(startStu, endStu);
    printf("\n\nPress any key to continue...");
    getch();
}

void lessonAverageGrade(void) {
    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    struct lesson *startLes, *currLes, *endLes;
    getLessons(&startLes, &endLes);

    currLes = getLessonByLessonCode(startLes, endLes, lessonCode);
    if (currLes != NULL) {
        printf("[#] Lesson found: %s - %s\n", currLes->type, currLes->name);

        struct grade *start, *curr, *end;
        getGrades(&start, &end);

        curr = start;
        int num = 0;
        float avg = 0;
        while (curr != NULL) {
            if (strcmp(curr->lessonCode, currLes->code) == 0) {
                avg += curr->grade;
                num++;
            }

            curr = curr->next;
        }

        if (num != 0)
            avg /= num;

        printf("[#] Lesson's average grade: %6.3f", avg);
        freeGrades(start, end);
    }
    else
        printf("[!] Lesson not found");

    freeLessons(startLes, endLes);
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

void listStudents(void) {
    struct student *start, *curr, *end;
    getStudents(&start, &end);

    if (start != NULL) {
        printf("+----+------------+----------------------+------------------------+-------------+\n");
        printf("| No | Student No | Last name            | Major                  | National No |\n");
        printf("+----+------------+----------------------+------------------------+-------------+\n");
    }

    curr = start;
    int num = 1;
    while (curr != NULL) {
        printf("| %-2d | %-10s ", num, curr->studentNumber);
        printf("| %-22s | %-15s ", curr->lastName, curr->major);
        printf("| %-10s  |\n", curr->nationalNumber);
        printf("+----+------------+----------------------+------------------------+-------------+\n");

        curr = curr->next;
        num++;
    }

    freeStudents(start, end);

    printf("\n\nPress any key to continue...");
    getch();
}

void listLessons(void) {
    struct lesson *start, *curr, *end;
    getLessons(&start, &end);

    if (start != NULL) {
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");
        printf("| No | Code  | Type                 | Name                 | Units | Status   |\n");
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");
    }

    int num = 1;
    curr = start;
    while (curr != NULL) {
        printf("| %-2d | %-5s ", num, curr->code);
        printf("| %-20s | %-20s ", curr->type, curr->name);
        printf("| %-5d | %-8s |\n", curr->units, curr->status);
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");

        num++;
        curr = curr->next;
    }

    freeLessons(start, end);
    printf("\n\nPress any key to continue...");
    getch();
}

void listStudentGrades(void) {
    struct student *startStu, *currStu, *endStu;
    getStudents(&startStu, &endStu);

    char stuNum[STUDENT_NUMBER_LEN];
    printf("[?] Enter student number: ");
    inputStringNumber(stuNum, STUDENT_NUMBER_LEN);

    currStu = startStu;
    while (currStu != NULL) {
        if (strcmp(currStu->studentNumber, stuNum) == 0)
            break;

        currStu = currStu->next;
    }

    if (currStu != NULL) {
        printf("[#] Student found: %s %s\n", currStu->firstName, currStu->lastName);
        printf("[#] Available grades of the student are listed below:\n");

        struct grade *startGra, *currGra, *endGra;
        getGrades(&startGra, &endGra);

        struct lesson *startLes, *currLes, *endLes;
        getLessons(&startLes, &endLes);

        printf("+----+-------------+----------------------+--------+------------------+\n");
        printf("| No | Lesson Code | Lesson name          | Grade  | Set date         | \n");
        printf("+----+-------------+----------------------+--------+------------------+\n");

        currGra = startGra;
        int num = 1;
        while (currGra != NULL) {
            if (strcmp(currGra->studentNumber, currStu->studentNumber) != 0) {
                currGra = currGra->next;
                continue;
            }

            currLes = getLessonByLessonCode(startLes, endLes, currGra->lessonCode);
            if (currLes != NULL) {
                printf("| %-2d | %-11s ", num, currLes->code);
                printf("| %-20s | %06.3f ", currLes->name, currGra->grade);
                int year, month, day, hour, min, sec;
                getTimeInfo(currGra->time, &year, &month, &day, &hour, &min, &sec);
                printf("| %04d/%02d/%02d ", year, month, day);
                printf("%02d:%02d |\n", hour, min);
                printf("+----+-------------+----------------------+--------+------------------+\n");
                num++;
            }

            currGra = currGra->next;
        }

        freeGrades(startGra, endGra);
        freeLessons(startLes, endLes);
    }

    else
        printf("[!] Student not found");

    freeStudents(startStu, endStu);
    printf("\n\nPress any key to continue...");
    getch();
}

void listFailingStudents(void) {
    struct student *startStu, *currStu, *endStu;
    getStudents(&startStu, &endStu);

    struct grade *startGra, *endGra;
    getGrades(&startGra, &endGra);

    printf("+----+------------+----------------------+----------------------+-----------+\n");
    printf("| No | Student No | First name           | Last name            | Avg grade |\n");
    printf("+----+------------+----------------------+----------------------+-----------+\n");

    currStu = startStu;
    int num = 0;
    while (currStu != NULL) {
        float avg;
        avg = getAverageGradeByStudentNumber(startGra, endGra, currStu->studentNumber);

        if (avg < PASSING_GRADE) {
            num++;
            printf("| %-2d | %-10s ", num, currStu->studentNumber);
            printf("| %-20s | %-20s ", currStu->firstName, currStu->lastName);
            printf("| %-6.3f     |\n", avg);
            printf("+----+------------+----------------------+----------------------+-----------+\n");
        }

        currStu = currStu->next;
    }

    freeStudents(startStu, endStu);
    freeGrades(startGra, endGra);
    printf("\n\nPress any key to continue...");
    getch();
}

void listStudentsPassingLesson(void) {
    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    struct lesson *startLes, *currLes, *endLes;
    getLessons(&startLes, &endLes);
    currLes = getLessonByLessonCode(startLes, endLes, lessonCode);

    if (currLes != NULL) {
        printf("[#] Lesson found: %s - %s\n", currLes->type, currLes->name);
        printf("[#] The students who passed this lesson are listed below:\n");

        struct student *startStu, *currStu, *endStu;
        getStudents(&startStu, &endStu);

        struct grade *startGra, *currGra, *endGra;
        getGrades(&startGra, &endGra);

        printf("+----+------------+----------------------+----------------------+--------+\n");
        printf("| No | Student No | First name           | Last name            | Grade  |\n");
        printf("+----+------------+----------------------+----------------------+--------+\n");
        currStu = startStu;
        int num = 0;
        while (currStu != NULL) {
            currGra = getGradeByLessonCodeAndStudentNumber(startGra, endGra, currLes->code, currStu->studentNumber);
            if (currGra != NULL) {
                if (currGra->grade >= PASSING_GRADE) {
                    num++;
                    printf("| %-2d | %-10s ", num, currStu->studentNumber);
                    printf("| %-20s | %-20s ", currStu->firstName, currStu->lastName);
                    printf("| %-6.3f |\n", currGra->grade);
                    printf("+----+------------+----------------------+----------------------+--------+\n");
                }
            }
            currStu = currStu->next;
        }

        freeStudents(startStu, endStu);
        freeGrades(startGra, endGra);
    }
    else
        printf("[!] Lesson not found");

    freeLessons(startLes, endLes);
    printf("\n\nPress any key to continue...");
    getch();
}

void listStudentsFailingLesson(void) {
    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    struct lesson *startLes, *currLes, *endLes;
    getLessons(&startLes, &endLes);
    currLes = getLessonByLessonCode(startLes, endLes, lessonCode);

    if (currLes != NULL) {
        printf("[#] Lesson found: %s - %s\n", currLes->type, currLes->name);
        printf("[#] The students who failed this lesson are listed below:\n");

        struct student *startStu, *currStu, *endStu;
        getStudents(&startStu, &endStu);

        struct grade *startGra, *currGra, *endGra;
        getGrades(&startGra, &endGra);

        printf("+----+------------+----------------------+----------------------+--------+\n");
        printf("| No | Student No | First name           | Last name            | Grade  |\n");
        printf("+----+------------+----------------------+----------------------+--------+\n");
        currStu = startStu;
        int num = 0;
        while (currStu != NULL) {
            currGra = getGradeByLessonCodeAndStudentNumber(startGra, endGra, currLes->code, currStu->studentNumber);
            if (currGra != NULL) {
                if (currGra->grade < PASSING_GRADE) {
                    num++;
                    printf("| %-2d | %-10s ", num, currStu->studentNumber);
                    printf("| %-20s | %-20s ", currStu->firstName, currStu->lastName);
                    printf("| %-6.3f |\n", currGra->grade);
                    printf("+----+------------+----------------------+----------------------+--------+\n");
                }
            }
            currStu = currStu->next;
        }

        freeStudents(startStu, endStu);
        freeGrades(startGra, endGra);
    }
    else
        printf("[!] Lesson not found");

    freeLessons(startLes, endLes);
    printf("\n\nPress any key to continue...");
    getch();
}

void listStudentsBornInTimePeriod(void) {
    char strDate1[DATE_LEN];
    char strDate2[DATE_LEN];

    printf("[?] Enter first date (sample: 1404/01/01): ");
    inputString(strDate1, DATE_LEN);
    printf("[?] Enter second date (sample: 1404/01/01): ");
    inputString(strDate2, DATE_LEN);

    int year1, year2, month1, month2, day1, day2;
    extractDateInfo(strDate1, &year1, &month1, &day1);
    extractDateInfo(strDate2, &year2, &month2, &day2);

    struct student *start, *curr, *end;
    getStudents(&start, &end);

    curr = start;
    int num = 0;
    while (curr != NULL) {
        int year, month, day;
        extractDateInfo(curr->birthDate, &year, &month, &day);

        if (year >= year1 && year <= year2 && month >= month1 && month <= month2
            && day >= day1 && day <= day2) {
                num++;
                printf("\n[#] Index: %d\n", num);
                printf("[#] Name: %s %s\n", curr->firstName, curr->lastName);
                printf("[#] Major: %s\n", curr->major);
                printf("[#] Student Number: %s\n", curr->studentNumber);
                printf("[#] National number: %s\n", curr->nationalNumber);
                printf("[#] Birth date: %s\n", curr->birthDate);
                printf("[#] Birth city: %s\n", curr->birthCity);
                printf("[#] Email: %s\n", curr->email);
                printf("[#] Phone number: %s\n", curr->phoneNumber);
        }
        curr = curr->next;
    }

    freeStudents(start, end);
    printf("\n\nPress any key to continue...");
    getch();
}

void listStudentsOfMajor(void) {
    char major[MAX_NAME_LEN];
    printf("[?] Enter major: ");
    inputString(major, MAX_NAME_LEN);

    struct student *start, *curr, *end;
    getStudents(&start, &end);

    curr = start;
    int num = 0;
    while (curr != NULL) {
        if (strcmp(curr->major, major) == 0) {
            num++;
            printf("\n[#] Index: %d\n", num);
            printf("[#] Name: %s %s\n", curr->firstName, curr->lastName);
            printf("[#] Major: %s\n", curr->major);
            printf("[#] Student Number: %s\n", curr->studentNumber);
            printf("[#] National number: %s\n", curr->nationalNumber);
            printf("[#] Birth date: %s\n", curr->birthDate);
            printf("[#] Birth city: %s\n", curr->birthCity);
            printf("[#] Email: %s\n", curr->email);
            printf("[#] Phone number: %s\n", curr->phoneNumber);
        }

        curr = curr->next;
    }

    freeStudents(start, end);
    printf("\n\nPress any key to continue...");
    getch();
}

void listStudentsBornInCity(void) {
    char city[MAX_NAME_LEN];
    printf("[?] Enter city: ");
    inputString(city, MAX_NAME_LEN);

    struct student *start, *curr, *end;
    getStudents(&start, &end);

    curr = start;
    int num = 0;
    while (curr != NULL) {
        if (strcmp(curr->birthCity, city) == 0) {
            num++;
            printf("\n[#] Index: %d\n", num);
            printf("[#] Name: %s %s\n", curr->firstName, curr->lastName);
            printf("[#] Major: %s\n", curr->major);
            printf("[#] Student Number: %s\n", curr->studentNumber);
            printf("[#] National number: %s\n", curr->nationalNumber);
            printf("[#] Birth date: %s\n", curr->birthDate);
            printf("[#] Birth city: %s\n", curr->birthCity);
            printf("[#] Email: %s\n", curr->email);
            printf("[#] Phone number: %s\n", curr->phoneNumber);
        }

        curr = curr->next;
    }

    freeStudents(start, end);
    printf("\n\nPress any key to continue...");
    getch();
}

void listFailingStudentsStudiedLesson(void) {
    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    struct lesson *startLes, *currLes, *endLes;
    getLessons(&startLes, &endLes);
    currLes = getLessonByLessonCode(startLes, endLes, lessonCode);

    if (currLes != NULL) {
        printf("[#] Lesson found: %s - %s\n", currLes->type, currLes->name);
        printf("[#] The students who are failed and stuied this lesson are listed below:\n");

        struct student *startStu, *currStu, *endStu;
        getStudents(&startStu, &endStu);

        struct grade *startGra, *currGra, *endGra;
        getGrades(&startGra, &endGra);

        printf("+----+------------+----------------------+--------------------+--------+-----------+\n");
        printf("| No | Student No | First name           | Last name          | Grade  | Avg grade |\n");
        printf("+----+------------+----------------------+--------------------+--------+-----------+\n");

        currStu = startStu;
        int num = 0;
        while (currStu != NULL) {
            float avg;
            avg = getAverageGradeByStudentNumber(startGra, endGra, currStu->studentNumber);
            currGra = getGradeByLessonCodeAndStudentNumber(startGra, endGra, currLes->code, currStu->studentNumber);

            if (avg < PASSING_GRADE && currGra != NULL) {
                num++;
                printf("| %-2d | %-10s ", num, currStu->studentNumber);
                printf("| %-20s | %-20s ", currStu->firstName, currStu->lastName);
                printf("| %-6.3f | %-6.3f    |\n", currGra->grade, avg);
                printf("+----+------------+----------------------+--------------------+--------+-----------+\n");
            }

            currStu = currStu->next;
        }

        freeStudents(startStu, endStu);
        freeGrades(startGra, endGra);
    }

    freeLessons(startLes, endLes);
    printf("\n\nPress any key to continue...");
    getch();
}

void listLessonGrades(void) {
    struct lesson *startLes, *currLes, *endLes;
    getLessons(&startLes, &endLes);

    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    currLes = startLes;
    while (currLes != NULL) {
        if (strcmp(currLes->code, lessonCode) == 0)
            break;

        currLes = currLes->next;
    }

    if (currLes != NULL) {
        printf("[#] Lesson found: %s", currLes->name);
        printf("[#] The available grades of the lesson are listed below:\n");

        struct grade *startGra, *currGra, *endGra;
        getGrades(&startGra, &endGra);

        struct student *startStu, *currStu, *endStu;
        getStudents(&startStu, &endStu);

        printf("+----+------------+----------------------+----------------------+--------+\n");
        printf("| No | Student No | First name           | Last name            | Grade  |\n");
        printf("+----+------------+----------------------+----------------------+--------+\n");

        currGra = startGra;
        int num = 1;
        while (currGra != NULL) {
            if (strcmp(currGra->lessonCode, currLes->code) != 0) {
                currGra = currGra->next;
                continue;
            }

            currStu = getStudentByStudentNumber(startStu, endStu, currGra->studentNumber);
            if (currStu != NULL) {
                printf("| %-2d | %-10s ", num, currStu->studentNumber);
                printf("| %-20s | %-20s ", currStu->firstName, currStu->lastName);
                printf("| %-6.3f |\n", currGra->grade);
                printf("+----+------------+----------------------+----------------------+--------+\n");
                num++;
            }

            currGra = currGra->next;
        }
        freeGrades(startGra, endGra);
        freeStudents(startStu, endStu);
    }

    else
        printf("[!] Lesson not found");

    freeLessons(startLes, endLes);
    printf("\n\nPress any key to continue...");
    getch();
}

void listEnabledLessons(void) {
    struct lesson *start, *curr, *end;
    getLessons(&start, &end);

    if (start != NULL) {
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");
        printf("| No | Code  | Type                 | Name                 | Units | Status   |\n");
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");
    }

    int num = 1;
    curr = start;
    while (curr != NULL) {
        if (strcmp(curr->status, "deleted") == 0) {
            curr = curr->next;
            continue;
        }

        printf("| %-2d | %-5s ", num, curr->code);
        printf("| %-20s | %-20s ", curr->type, curr->name);
        printf("| %-5d | %-8s |\n", curr->units, curr->status);
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");

        num++;
        curr = curr->next;
    }

    freeLessons(start, end);
    printf("\n\nPress any key to continue...");
    getch();
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

void listDeletedLessons(void) {
    struct lesson *start, *curr, *end;
    getLessons(&start, &end);

    if (start != NULL) {
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");
        printf("| No | Code  | Type                 | Name                 | Units | Status   |\n");
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");
    }

    int num = 1;
    curr = start;
    while (curr != NULL) {
        if (strcmp(curr->status, "enabled") == 0) {
            curr = curr->next;
            continue;
        }

        printf("| %-2d | %-5s ", num, curr->code);
        printf("| %-20s | %-20s ", curr->type, curr->name);
        printf("| %-5d | %-8s |\n", curr->units, curr->status);
        printf("+----+-------+----------------------+----------------------+-------+----------+\n");

        num++;
        curr = curr->next;
    }

    freeLessons(start, end);
    printf("\n\nPress any key to continue...");
    getch();
}

void listStudentsSortedByAverageGrade(void) {
    struct student *startStu, *currStu, *endStu;
    getStudents(&startStu, &endStu);

    struct grade *startGra, *currGra, *endGra;
    getGrades(&startGra, &endGra);

    int numStu = 0;
    currStu = startStu;
    while (currStu != NULL) {
        numStu++;
        currStu = currStu->next;
    }

    sortStudentsByAverageGrade(&startStu, &endStu, startGra, endGra, numStu);

    if (startStu != NULL) {
        printf("+----+------------+----------------------+----------------------+-----------+\n");
        printf("| No | Student No | First name           | Last name            | Avg Grade |\n");
        printf("+----+------------+----------------------+----------------------+-----------+\n");
    }

    currStu = startStu;
    int num = 0;
    while (currStu != NULL) {
        num++;
        float avg;
        avg = getAverageGradeByStudentNumber(startGra, endGra, currStu->studentNumber);

        printf("| %-2d | %-10s ", num, currStu->studentNumber);
        printf("| %-20s | %-20s ", currStu->firstName, currStu->lastName);
        printf("| %-6.3f    |\n", avg);
        printf("+----+------------+----------------------+----------------------+-----------+\n");

        currStu = currStu->next;
    }

    freeStudents(startStu, endStu);
    freeGrades(startGra, endGra);
    printf("\n\nPress any key to continue...");
    getch();
}

void listLessonGradesSortedByStudentName(void) {
    char lessonCode[LESSON_CODE_LEN];
    printf("[?] Enter lesson code: ");
    inputStringNumber(lessonCode, LESSON_CODE_LEN);

    struct lesson *startLes, *currLes, *endLes;
    getLessons(&startLes, &endLes);

    currLes = startLes;
    while (currLes != NULL) {
        if (strcmp(currLes->code, lessonCode) == 0)
            break;

        currLes = currLes->next;
    }

    if (currLes != NULL) {
        printf("[#] Lesson found: %s\n", currLes->name);
        printf("[#] Available grades are sorted and listed below:\n");

        struct grade *startGra, *currGra, *endGra;
        getGrades(&startGra, &endGra);

        struct student *startStu, *currStu, *endStu;
        getStudents(&startStu, &endStu);

        struct grade *firstGra, *lastGra;

        firstGra = NULL;
        lastGra = NULL;
        currGra = startGra;
        int num = 0;
        while (currGra != NULL) {
            if (strcmp(currGra->lessonCode, currLes->code) != 0) {
                currGra = currGra->next;
                continue;
            }

            currStu = getStudentByStudentNumber(startStu, endStu, currGra->studentNumber);
            if (currStu != NULL) {
                num++;
                if (firstGra == NULL) {
                    firstGra = currGra;
                    lastGra = firstGra;
                }
                else {
                    lastGra->next = currGra;
                    lastGra = currGra;
                }
            }

            currGra = currGra->next;
        }

        printf("%d\n", num);
        sortGradesByStudentLastName(&firstGra, &lastGra, startStu, endStu, num);

        printf("+----+------------+----------------------+----------------------+--------+\n");
        printf("| No | Student No | First name           | Last name            | Grade  |\n");
        printf("+----+------------+----------------------+----------------------+--------+\n");

        currGra = firstGra;
        num = 0;
        while (currGra != NULL) {
            num++;
            currStu = getStudentByStudentNumber(startStu, endStu, currGra->studentNumber);
            printf("| %-2d | %-10s ", num, currStu->studentNumber);
            printf("| %-20s | %-20s ", currStu->firstName, currStu->lastName);
            printf("| %-6.3f |\n", currGra->grade);
            printf("+----+------------+----------------------+----------------------+--------+\n");
            currGra = currGra->next;
        }

        freeStudents(startStu, endStu);
        freeGrades(startGra, endGra);
    }

    else
        printf("[!] Lesson not found");

    freeLessons(startLes, endLes);
    printf("\n\nPress any key to continue...");
    getch();
}

void listSignIns(void) {
    struct signIn *start, *curr, *end;
    getSignIns(&start, &end);

    printf("[#] List of all logins to the system is as below:\n");
    printf("+--------+------------+------------------+---------------------+\n");
    printf("| Number | Status     | Username         | Time                |\n");
    printf("+--------+------------+------------------+---------------------+\n");

    curr = start;
    int num = 0;
    while (curr != NULL) {
        num++;
        printf("| %-6d | %-10s | %-16s ", num, curr->status, curr->username);
        int year, month, day, hour, min, sec;
        getTimeInfo(curr->time, &year, &month, &day, &hour, &min, &sec);
        printf("| %04d/%02d/%02d %02d:%02d:%02d |\n", year, month, day, hour, min, sec);
        printf("+--------+------------+------------------+---------------------+\n");

        curr = curr->next;
    }

    freeSignIns(start, end);
}

void listSuccessfulSignIns(void) {
    struct signIn *start, *curr, *end;
    getSignIns(&start, &end);

    printf("[#] List of all logins to the system is as below:\n");
    printf("+--------+------------+------------------+---------------------+\n");
    printf("| Number | Status     | Username         | Time                |\n");
    printf("+--------+------------+------------------+---------------------+\n");

    curr = start;
    int num = 0;
    while (curr != NULL) {
        if (strcmp(curr->status, "success") != 0) {
            curr = curr->next;
            continue;
        }

        num++;
        printf("| %-6d | %-10s | %-16s ", num, curr->status, curr->username);
        int year, month, day, hour, min, sec;
        getTimeInfo(curr->time, &year, &month, &day, &hour, &min, &sec);
        printf("| %04d/%02d/%02d %02d:%02d:%02d |\n", year, month, day, hour, min, sec);
        printf("+--------+------------+------------------+---------------------+\n");

        curr = curr->next;
    }

    freeSignIns(start, end);
}

void sortStudentsByAverageGrade(struct student **startStu, struct student **endStu, struct grade *startGra, struct grade *endGra, int numStu) {
    struct student *prev, *curr, *next;

    int i;
    for (i = 0; i < numStu; i++) {
        prev = NULL;
        curr = *startStu;
        while (curr != NULL && curr->next != NULL) {
            next = curr->next;
            float currAvg;
            currAvg = getAverageGradeByStudentNumber(startGra, endGra, curr->studentNumber);
            float nextAvg;
            nextAvg = getAverageGradeByStudentNumber(startGra, endGra, next->studentNumber);

            if (currAvg > nextAvg) {
                if (curr == *startStu)
                    *startStu = next;
                if (next == *endStu)
                    *endStu = curr;

                curr->next = next->next;
                next->next = curr;
                if (prev != NULL)
                    prev->next = next;

                prev = next;
            }
            else {
                prev = curr;
                curr = next;
            }
        }
    }
}

void sortGradesByStudentLastName(struct grade **startGra, struct grade **endGra, struct student *startStu, struct student *endStu, int numGra) {
    struct grade *prev, *curr, *next;
    struct student *currStu, *nextStu;

    int i;
    for (i = 0; i < numGra; i++) {
        prev = NULL;
        curr = *startGra;
        while (curr != NULL && curr->next != NULL) {
            next = curr->next;
            currStu = getStudentByStudentNumber(startStu, endStu, curr->studentNumber);
            nextStu = getStudentByStudentNumber(startStu, endStu, next->studentNumber);

            if (stricmp(currStu->lastName, nextStu->lastName) > 0) {
                if (curr == *startGra)
                    *startGra = next;

                if (next == *endGra)
                    *endGra = curr;

                curr->next = next->next;
                next->next = curr;
                if (prev != NULL)
                    prev->next = next;

                prev = next;
            }
            else {
                prev = curr;
                curr = next;
            }
        }
    }
}

void addNewHead() {
    char strNum[5];
    int num;
    printf("[?] Enter the number of department heads you want to add: ");
    inputStringNumber(strNum, 5);
    num = atoi(strNum);

    int i;
    struct departmentHead *newHead, *curr, *lastHead;
    lastHead = NULL;
    newHead = NULL;

    for (i = 0; i < num; i++) {
        curr = malloc(sizeof(struct departmentHead));
        printf("[#] Enter the information of user #%d:\n", i + 1);

        int correct;
        do {
            printf("[?] Enter first name: ");
            inputString(curr->firstName, MAX_NAME_LEN);
            correct = validateName(curr->firstName);
            if (!correct)
                printf("[!] Wrong characters. Try with english characters and space\n");
        } while (!correct);

        do {
            printf("[?] Enter last name: ");
            inputString(curr->lastName, MAX_NAME_LEN);
            correct = validateName(curr->lastName);
            if (!correct)
                printf("[!] Wrong characters. Try with english characters and space\n");
        } while (!correct);

        printf("[?] Enter group name: ");
        inputString(curr->groupName, MAX_NAME_LEN);

        do {
            printf("[?] Enter join date (sample: 2026/01/01): ");
            inputString(curr->joinDate, DATE_LEN);
            correct = validateDate(curr->joinDate);
            if (!correct)
                printf("[!] Wrong date format. Try again\n");
        } while (!correct);

        do {
            printf("[?] Enter national number: ");
            inputStringNumber(curr->nationalNumber, NATIONAL_NUMBER_LEN);
            if (strlen(curr->nationalNumber) != NATIONAL_NUMBER_LEN - 2) {
                printf("[!] Wrong national number. Try again\n");
                correct = 0;
            }
            else
                correct = 1;
        } while (!correct);

        do {
            printf("[?] Enter phone number (sample: 09123456789): ");
            inputStringNumber(curr->phoneNumber, PHONE_NUMBER_LEN);
            correct = validatePhoneNumber(curr->phoneNumber);
            if (!correct)
                printf("[!] Wrong phone number. Try again\n");
        } while (!correct);

        do {
            printf("[?] Enter email (sample: admin@yahoo.com): ");
            inputString(curr->email, MAX_EMAIL_LEN);
            correct = validateEmail(curr->email);
            if (!correct)
                printf("[!] Wrong email. Try again\n");
        } while (!correct);

        int usernameTaken;
        do {
            printf("[?] Enter username: ");
            inputUsername(curr->username, MAX_USERNAME_LEN);
            usernameTaken = checkUserByUsername(curr->username);
            if (usernameTaken)
                printf("[!] This username is already taken. Try another one\n");
        } while (usernameTaken);

        int same;
        char password1[MAX_PASSWORD_LEN], password2[MAX_PASSWORD_LEN];
        char hashedPassword[HASHED_PASSWORD_LEN];
        lockPasswordMem(password1);
        lockPasswordMem(password2);
        do {
            int strongPassword;
            do {
                printf("[?] Enter password: ");
                inputPassword(password1, MAX_PASSWORD_LEN);
                strongPassword = validatePassword(password1);
                if (!strongPassword)
                    printf("[!] This password is weak. Try another one\n");
            } while (!strongPassword);

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
        hashPassword(hashedPassword, password1);
        sodium_memzero(password2, sizeof(password2));
        strcpy(curr->hashedPassword, hashedPassword);
        unlockPasswordMem(password1);
        unlockPasswordMem(password2);

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
    char strNum[5];
    int num;
    printf("[?] Enter the number of academic staff you want to add: ");
    inputStringNumber(strNum, 5);
    num = atoi(strNum);

    int i;
    struct academicStaff *newStaff, *curr, *lastStaff;
    newStaff = NULL;
    lastStaff = NULL;
    for (i = 0; i < num; i++) {
        curr = malloc(sizeof(struct academicStaff));
        printf("\n[#] Enter the information of staff #%d:\n", i + 1);

        int correct;
        do {
            printf("[?] Enter first name: ");
            inputString(curr->firstName, MAX_NAME_LEN);
            correct = validateName(curr->firstName);
            if (!correct)
                printf("[!] Wrong characters. Try with english characters and space\n");
        } while (!correct);

        do {
            printf("[?] Enter last name: ");
            inputString(curr->lastName, MAX_NAME_LEN);
            correct = validateName(curr->lastName);
            if (!correct)
                printf("[!] Wrong characters. Try with english characters and space\n");
        } while (!correct);

        do {
            printf("[?] Enter the date of join (sample: 2026/01/01): ");
            inputString(curr->joinDate, DATE_LEN);
            correct = validateDate(curr->joinDate);
            if (!correct)
            printf("[!] Wrong date. Try again\n");
        } while (!correct);

        char strNumber[10];
        printf("[?] Enter rank: ");
        inputStringNumber(strNumber, 10);
        curr->rank = atoi(strNumber);

        do {
            printf("[?] Enter phone number (sample: 09123456789): ");
            inputStringNumber(curr->phoneNumber, PHONE_NUMBER_LEN);
            correct = validatePhoneNumber(curr->phoneNumber);
            if (!correct)
            printf("[!] Wrong phone number format. Try again\n");
        } while (!correct);

        do {
            printf("[?] Enter email (sample: test@yahoo.com): ");
            inputString(curr->email, MAX_EMAIL_LEN);
            correct = validateEmail(curr->email);
            if (!correct)
            printf("[!] Wrong email. Try again\n");
        } while (!correct);

        int usernameTaken;
        do {
            printf("[?] Enter username: ");
            inputUsername(curr->username, MAX_USERNAME_LEN);
            usernameTaken = checkUserByUsername(curr->username);
            if (usernameTaken)
                printf("[!] This username is already taken. Try another one\n");
        } while (usernameTaken);

        char password1[MAX_PASSWORD_LEN], password2[MAX_PASSWORD_LEN];
        char hashedPassword[HASHED_PASSWORD_LEN];
        lockPasswordMem(password1);
        lockPasswordMem(password2);
        do {
            int strongPassword;
            do {
                printf("[?] Enter password: ");
                inputPassword(password1, MAX_PASSWORD_LEN);
                strongPassword = validatePassword(password1);
                if (!strongPassword)
                    printf("[!] This password is weak. Try another one\n");
            } while (!strongPassword);

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
        hashPassword(hashedPassword, password1);
        sodium_memzero(password2, sizeof(password2));
        strcpy(curr->hashedPassword, hashedPassword);
        unlockPasswordMem(password1);
        unlockPasswordMem(password2);

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

        int isCorrect;
        do {
            printf("[?] Enter first name: ");
            inputString(curr->firstName, MAX_NAME_LEN);
            isCorrect = validateName(curr->firstName);
            if (!isCorrect)
                printf("[!] Wrong characters. Try with english characters and space\n");
        } while (!isCorrect);

        do {
            printf("[?] Enter last name: ");
            inputString(curr->lastName, MAX_NAME_LEN);
            isCorrect = validateName(curr->lastName);
            if (!isCorrect)
                printf("[!] Wrong characters. Try with english characters and space\n");
        } while (!isCorrect);

        do {
            printf("[?] Enter national number: ");
            inputStringNumber(curr->nationalNumber, NATIONAL_NUMBER_LEN);
            if (strlen(curr->nationalNumber) != NATIONAL_NUMBER_LEN - 2) {
                printf("[!] Wrong national number. Try again\n");
                isCorrect = 0;
            }
            else
                isCorrect = 1;
        } while (!isCorrect);

        do {
            printf("[?] Enter birth date: ");
            inputString(curr->birthDate, DATE_LEN);
            isCorrect = validateDate(curr->birthDate);
            if (!isCorrect)
                printf("[!] Wrong date format. Try again\n");
        } while (!isCorrect);

        printf("[?] Enter birth city name: ");
        inputString(curr->birthCity, MAX_NAME_LEN);

        printf("[?] Enter major: ");
        inputString(curr->major, MAX_NAME_LEN);

        do {
            printf("[?] Enter student number: ");
            inputStringNumber(curr->studentNumber, STUDENT_NUMBER_LEN);
            if (strlen(curr->studentNumber) != STUDENT_NUMBER_LEN - 2) {
                printf("[!] Wrong student number. Try again\n");
                isCorrect = 0;
            }
            else if (checkStudentByStudentNumber(curr->studentNumber)) {
                printf("[!] This student number already exists. Try another one\n");
                isCorrect = 0;
            }
            else
                isCorrect = 1;
        } while (!isCorrect);

        do {
            printf("[?] Enter phone number: ");
            inputStringNumber(curr->phoneNumber, PHONE_NUMBER_LEN);
            isCorrect = validatePhoneNumber(curr->phoneNumber);
            if (!isCorrect)
                printf("[!] Wrong phone number. Try again\n");
        } while (!isCorrect);

        do {
            printf("[?] Enter email: ");
            inputString(curr->email, MAX_EMAIL_LEN);
            isCorrect = validateEmail(curr->email);
            if (!isCorrect)
                printf("[!] Wrong email format. Try again\n");
        } while (!isCorrect);

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
    char strNum[5];
    int num;
    printf("\n[?] How many lessons do you want to add (enter number): ");
    inputStringNumber(strNum, 5);
    num = atoi(strNum);

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

        int correct;
        do {
            printf("[?] Enter code: ");
            inputStringNumber(curr->code, LESSON_CODE_LEN);
            if (checkLessonByLessonCode(curr->code)) {
                printf("[!] A lesson with this code already exists. Try another one\n");
                correct = 0;
            }
            else
                correct = 1;
        } while (!correct);

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

        int isCorrect;
        do {
            printf("[?] Enter lesson code: ");
            inputStringNumber(curr->lessonCode, LESSON_CODE_LEN);
            if (checkLessonByLessonCode(curr->lessonCode))
                isCorrect = 1;
            else {
                printf("[!] There is no lesson with this lesson code. Try another one\n");
                isCorrect = 0;
            }
        } while (!isCorrect);

        do {
            printf("[?] Enter student number: ");
            inputStringNumber(curr->studentNumber, STUDENT_NUMBER_LEN);
            if (strlen(curr->studentNumber) != STUDENT_NUMBER_LEN - 2) {
                printf("[!] Wrong student number. Try again\n");
                isCorrect = 0;
            }
            else if (!checkStudentByStudentNumber(curr->studentNumber)) {
                printf("[!] There is no student with this student number. Try another one\n");
                isCorrect = 0;
            }
            else
                isCorrect = 1;
        } while (!isCorrect);

        struct grade *startGra, *temp, *endGra;
        getGrades(&startGra, &endGra);
        temp = getGradeByLessonCodeAndStudentNumber(startGra, endGra, curr->lessonCode, curr->studentNumber);
        if (temp != NULL) {
            printf("[!] There's already a grade stored for this student in this lesson.");
            printf("Try again\n\n");
            free(curr);
            freeGrades(startGra, endGra);
            i--;
            continue;
        }
        freeGrades(startGra, endGra);

        char strNumber[10];
        do {
            printf("[?] Enter grade (0-20): ");
            inputStringFloat(strNumber, 10);
            curr->grade = atof(strNumber);
            if (curr->grade < MIN_GRADE || curr->grade > MAX_GRADE) {
                printf("[!] Wrong grade. Try again\n");
                isCorrect = 0;
            }
            else
                isCorrect = 1;
        } while (!isCorrect);

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

void addNewSignIn(char username[], char status[]) {
    struct signIn *start, *curr, *end;
    getSignIns(&start, &end);

    curr = malloc(sizeof(struct signIn));
    curr->time = time(NULL);
    strcpy(curr->username, username);
    strcpy(curr->status, status);
    curr->next = NULL;

    if (start == NULL) {
        start = curr;
        end = start;
    }
    else {
        end->next = curr;
        end = curr;
    }

    // look if user has exceeded maximum
    // sign-in tries count
    long long int now = time(NULL);
    curr = start;
    int count = 0;
    while (curr != NULL) {
        long long int timePassed = now - curr->time;
        if (strcmp(curr->username, username) == 0 && timePassed <= MAX_SIGNIN_TRIES_PERIOD)
            count++;

        curr = curr->next;
    }

    if (count >= MAX_SIGNIN_TRIES)
        addNewLimit(username, now + SIGNIN_LIMIT_TIME);

    setSignIns(start, end);
    freeSignIns(start, end);
}

void addNewLimit(char username[], long long int releaseTime) {
    struct limit *start, *curr, *end;
    getLimits(&start, &end);

    curr = malloc(sizeof(struct limit));
    curr->releaseTime = releaseTime;
    strcpy(curr->username, username);
    curr->next = NULL;

    if (start == NULL) {
        start = curr;
        end = start;
    }
    else {
        end->next = curr;
        end = curr;
    }

    setLimits(start, end);
    freeLimits(start, end);
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
        float tempGrade = atof(strNumber);
        if (tempGrade < MIN_GRADE || tempGrade > MAX_GRADE)
            printf("[!] Wrong grade. grade not changed");
        else {
            curr->grade = tempGrade;
            setGrades(start, end);
            printf("[#] Grade changed");
        }
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
        char oldPassword[MAX_PASSWORD_LEN];
        char newHashedPassword[HASHED_PASSWORD_LEN];
        lockPasswordMem(oldPassword);
        printf("[?] Enter old password: ");
        inputPassword(oldPassword, MAX_PASSWORD_LEN);

        if (verifyPassword(head->hashedPassword, oldPassword)) {
            char password1[MAX_PASSWORD_LEN], password2[MAX_PASSWORD_LEN];
            lockPasswordMem(password1);
            lockPasswordMem(password2);

            printf("[?] Enter new password: ");
            inputPassword(password1, MAX_PASSWORD_LEN);
            printf("[?] Enter password again: ");
            inputPassword(password2, MAX_PASSWORD_LEN);

            if (strcmp(password1, password2) == 0) {
                if (validatePassword(password1)) {
                    hashPassword(newHashedPassword, password1);
                    sodium_memzero(password2, sizeof(password2));

                    strcpy(head->hashedPassword, newHashedPassword);
                    setDepartmentHeads(&start, &end);
                    printf("[#] Password changed");
                }
                else
                    printf("[!] This password is weak\n");
            }
            else {
                printf("[!] Passwords do not match");
            }
            unlockPasswordMem(password1);
            unlockPasswordMem(password2);
        }
        else {
            printf("[!] Password wrong");
        }

        Sleep(1000);
    }
    else if (digit == 2) {
        char tempEmail[MAX_EMAIL_LEN];
        printf("[?] Enter new email: ");
        inputString(tempEmail, MAX_EMAIL_LEN);
        if (!validateEmail(tempEmail))
            printf("[!] Wrong email format. email not changed");
        else {
            strcpy(head->email, tempEmail);
            setDepartmentHeads(&start, &end);
            printf("[#] Email changed");
        }
        Sleep(1000);
    }
    else if (digit == 3) {
        char tempPhoneNumber[PHONE_NUMBER_LEN];
        printf("[?] Enter new phone number: ");
        inputStringNumber(tempPhoneNumber, PHONE_NUMBER_LEN);
        if (!validatePhoneNumber(tempPhoneNumber))
            printf("[!] Wrong phone number. Phone number not changed");
        else {
            strcpy(head->phoneNumber, tempPhoneNumber);
            setDepartmentHeads(&start, &end);
            printf("[#] Phone number changed");
        }
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
        char oldPassword[MAX_PASSWORD_LEN];
        char newHashedPassword[HASHED_PASSWORD_LEN];
        printf("[?] Enter old password: ");
        inputPassword(oldPassword, MAX_PASSWORD_LEN);
        if (verifyPassword(staff->hashedPassword, oldPassword)) {
            char password1[MAX_PASSWORD_LEN], password2[MAX_PASSWORD_LEN];
            lockPasswordMem(password1);
            lockPasswordMem(password2);

            printf("[?] Enter new password: ");
            inputPassword(password1, MAX_PASSWORD_LEN);
            printf("[?] Enter password again: ");
            inputPassword(password2, MAX_PASSWORD_LEN);

            if (strcmp(password1, password2) == 0) {
                if (validatePassword(password1)) {
                    hashPassword(newHashedPassword, password1);
                    sodium_memzero(password2, sizeof(password2));

                    strcpy(staff->hashedPassword, newHashedPassword);
                    setAcademicStaff(&start, &end);
                    printf("[#] Password changed");
                }
                else
                    printf("[!] This password is weak");
            }
            else {
                printf("[!] Passwords do not match");
            }
            unlockPasswordMem(password1);
            unlockPasswordMem(password2);
        }
        else {
            printf("[!] Password wrong");
        }

        Sleep(1000);
    }
    else if (digit == 2) {
        char tempEmail[MAX_EMAIL_LEN];
        printf("[?] Enter new email: ");
        inputString(tempEmail, MAX_EMAIL_LEN);
        if (!validateEmail(tempEmail))
            printf("[!] Wrong email format. email not changed");
        else {
            strcpy(staff->email, tempEmail);
            setAcademicStaff(&start, &end);
            printf("[#] Email changed");
        }
        Sleep(1000);
    }
    else if (digit == 3) {
        char tempPhoneNumber[PHONE_NUMBER_LEN];
        printf("[?] Enter new phone number: ");
        inputStringNumber(tempPhoneNumber, PHONE_NUMBER_LEN);
        if (!validatePhoneNumber(tempPhoneNumber))
            printf("[!] Wrong phone number. phone number not changed");
        else {
            strcpy(staff->phoneNumber, tempPhoneNumber);
            setAcademicStaff(&start, &end);
            printf("[#] Phone number changed");
        }
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
            char tempPhoneNumber[PHONE_NUMBER_LEN];
            printf("[?] Enter new phone number: ");
            inputStringNumber(tempPhoneNumber, PHONE_NUMBER_LEN);
            if (!validatePhoneNumber(tempPhoneNumber))
                printf("[!] wrong phone number. phone number not changed");
            else {
                strcpy(curr->phoneNumber, tempPhoneNumber);
                setStudents(start, end);
                printf("[#] Changes saved");
            }
            Sleep(1000);
        }
        else if (digit == 3) {
            char tempEmail[MAX_EMAIL_LEN];
            printf("[?] Enter new email: ");
            inputString(tempEmail, MAX_EMAIL_LEN);
            if (!validateEmail(tempEmail))
                printf("[!] Wrong email format. email not changed");
            else {
                strcpy(curr->email, tempEmail);
                setStudents(start, end);
                printf("[#] Changes saved");
            }
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
    char answer[7];
    inputString(answer, 7);
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
    char answer[7];
    inputString(answer, 7);

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
        char answer[7];
        printf("[?] Do you want to delete it (yes/no): ");
        inputString(answer, 7);

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
    printf("[?] Enter destination path (sample: D:\\Backup_2026_01_01): ");
    inputString(path, 110);

    if (backupFile(path, HEAD_PROFILES_FILE)) {
        backupFile(path, STAFF_PROFILES_FILE);
        backupFile(path, STUDENTS_FILE);
        backupFile(path, LESSONS_FILE);
        backupFile(path, GRADES_FILE);
        backupFile(path, SIGN_IN_FILE);
        backupFile(path, LIMITS_FILE);
        printf("[#] copied files successfully");
    }
    else {
        printf("[!] You entered wrong path");
    }

    Sleep(1000);
}

void useBackup(void) {
    char path[110];
    printf("[?] Enter backup directory path (sample: D:\\Backup_2026_01_01): ");
    inputString(path, 110);

    if (useBackupFile(path, HEAD_PROFILES_FILE)) {
        useBackupFile(path, STAFF_PROFILES_FILE);
        useBackupFile(path, STUDENTS_FILE);
        useBackupFile(path, LESSONS_FILE);
        useBackupFile(path, GRADES_FILE);
        useBackupFile(path, SIGN_IN_FILE);
        useBackupFile(path, LIMITS_FILE);
        printf("[#] copied files successfully");
    }
    else {
        printf("[!] You entered wrong path");
    }

    Sleep(1000);
}

int backupFile(char dirPath[], char filename[]) {
    char path[200];
    strcpy(path, dirPath);
    strcat(path, "\\");
    strcat(path, filename);

    FILE *sf, *df; // source file, destination file
    df = fopen(path, "w");
    if (df == NULL)
        return 0;

    sf = fopen(filename, "r");
    if (sf != NULL) {
        char ch = fgetc(sf);
        while (ch != EOF) {
            fputc(ch, df);
            ch = fgetc(sf);
        }

        fclose(sf);
        fclose(df);
    }

    return 1;
}

int useBackupFile(char dirPath[], char filename[]) {
    char path[200];
    strcpy(path, dirPath);
    strcat(path, "\\");
    strcat(path, filename);

    FILE *sf, *df;
    sf = fopen(path, "r");
    if (sf == NULL)
        return 0;

    df = fopen(filename, "w");
    if (df != NULL) {
        char ch = fgetc(sf);
        while (ch != EOF) {
            fputc(ch, df);
            ch = fgetc(sf);
        }

        fclose(sf);
        fclose(df);
    }

    return 1;
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

        fgets(curr->hashedPassword, HASHED_PASSWORD_LEN, fp);
        removeNewLine(curr->hashedPassword);

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
        fprintf(fp, "%s\n", curr->hashedPassword);
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

        fgets(curr->hashedPassword, HASHED_PASSWORD_LEN, fp);
        removeNewLine(curr->hashedPassword);

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
        fprintf(fp, "%s\n", curr->hashedPassword);
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

float getAverageGradeByStudentNumber(struct grade *start, struct grade *end, char studentNumber[]) {
    struct grade *curr;
    curr = start;
    int num = 0;
    float avg = 0;
    while (curr != NULL) {
        if (strcmp(curr->studentNumber, studentNumber) == 0) {
            avg += curr->grade;
            num++;
        }

        curr = curr->next;
    }

    if (num != 0)
        avg /= num;

    return avg;
}

int checkLimitByUsername(char username[]) {
    struct limit *start, *curr, *end;
    getLimits(&start, &end);

    curr = start;
    long long int now = time(NULL);
    while (curr != NULL) {
        if (strcmp(curr->username, username) == 0 && curr->releaseTime >= now) {
            freeLimits(start, end);
            return 1;
        }

        curr = curr->next;
    }

    freeLimits(start, end);
    return 0;
}

int checkUserByUsername(char username[]) {
    if (strcmp(username, "admin") == 0)
        return 1;

    int found = 0;

    struct departmentHead *startHead, *currHead, *endHead;
    getDepartmentHeads(&startHead, &endHead);

    currHead = startHead;
    while (currHead != NULL) {
        if (strcmp(currHead->status, "deleted") != 0
            && strcmp(currHead->username, username) == 0) {
                found = 1;
                break;
        }

        currHead = currHead->next;
    }
    freeDepartmentHeads(startHead, endHead);

    if (found)
        return 1;

    struct academicStaff *startStaff, *currStaff, *endStaff;
    getAcademicStaff(&startStaff, &endStaff);

    currStaff = startStaff;
    while (currStaff != NULL) {
        if (strcmp(currStaff->status, "deleted") != 0
            && strcmp(currStaff->username, username) == 0) {
                found = 1;
                break;
        }

        currStaff = currStaff->next;
    }
    freeAcademicStaff(startStaff, endStaff);

    return found;
}

int checkStudentByStudentNumber(char studentNumber[]) {
    struct student *start, *curr, *end;
    getStudents(&start, &end);
    curr = getStudentByStudentNumber(start, end, studentNumber);
    int found = 0;
    if (curr != NULL)
        found = 1;

    freeStudents(start, end);
    return found;
}

int checkLessonByLessonCode(char lessonCode[]) {
    struct lesson *start, *curr, *end;
    getLessons(&start, &end);
    curr = getLessonByLessonCode(start, end, lessonCode);
    int found = 0;
    if (curr != NULL)
        found = 1;

    freeLessons(start, end);
    return found;
}

void freeGrades(struct grade *start, struct grade *end) {
    end = start;
    while (end != NULL) {
        end = start->next;
        free(start);
        start = end;
    }
}

void getSignIns(struct signIn **start, struct signIn **end) {
    FILE *fp;
    fp = fopen(SIGN_IN_FILE, "r");
    if (fp == NULL) {
        fp = fopen(SIGN_IN_FILE, "w");
        fclose(fp);
        fp = fopen(SIGN_IN_FILE, "r");
        if (fp == NULL) {
            printf("[!] Something went wrong while reading a file");
            exit(1);
        }
    }

    *start = NULL;
    *end = NULL;
    struct signIn *curr;
    while (!feof(fp)) {
        curr = malloc(sizeof(struct signIn));
        if (fgets(curr->username, MAX_USERNAME_LEN, fp) == NULL) {
            free(curr);
            break;
        }
        removeNewLine(curr->username);

        fgets(curr->status, STATUS_LEN, fp);
        removeNewLine(curr->status);

        char strNum[30];
        fgets(strNum, 30, fp);
        removeNewLine(strNum);
        curr->time = atoll(strNum);

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

void setSignIns(struct signIn *start, struct signIn *end) {
    FILE *fp;
    fp = fopen(SIGN_IN_FILE, "w");
    if (fp == NULL) {
        printf("[!] Something went wrong while writing to a file");
        exit(1);
    }

    struct signIn *curr;
    curr = start;
    while (curr != NULL) {
        fprintf(fp, "%s\n", curr->username);
        fprintf(fp, "%s\n", curr->status);
        fprintf(fp, "%lld\n", curr->time);
        curr = curr->next;
    }

    fclose(fp);
}

void freeSignIns(struct signIn *start, struct signIn *end) {
    end = start;
    while (end != NULL) {
        end = start->next;
        free(start);
        start = end;
    }
}

void getLimits(struct limit **start, struct limit **end) {
    FILE *fp;
    fp = fopen(LIMITS_FILE, "r");
    if (fp == NULL) {
        fp = fopen(LIMITS_FILE, "w");
        fclose(fp);
        fp = fopen(LIMITS_FILE, "w");
        if (fp == NULL) {
            printf("[Error] Something went wrong while writing to a file");
            exit(1);
        }
    }

    *start = NULL;
    *end = NULL;
    struct limit *curr;
    while (!feof(fp)) {
        curr = malloc(sizeof(struct limit));
        if (fgets(curr->username, MAX_USERNAME_LEN, fp) == NULL) {
            free(curr);
            break;
        }
        removeNewLine(curr->username);

        char strNum[30];
        fgets(strNum, 30, fp);
        removeNewLine(strNum);
        curr->releaseTime = atoll(strNum);

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

void setLimits(struct limit *start, struct limit *end) {
    FILE *fp;
    fp = fopen(LIMITS_FILE, "w");
    if (fp == NULL) {
        printf("[Error] Something went wrong while writing to a file");
        exit(1);
    }

    struct limit *curr;
    curr = start;
    while (curr != NULL) {
        fprintf(fp, "%s\n", curr->username);
        fprintf(fp, "%lld\n", curr->releaseTime);

        curr = curr->next;
    }

    fclose(fp);
}

void freeLimits(struct limit *start, struct limit *end) {
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
        if (ch == '\n' || ch == '\r') {
            if (n_chars != 0)
                break;
        }

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
        if (ch == '\n' || ch == '\r') {
            if (n_chars != 0)
                break;
        }

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
        if (ch == '\n' || ch == '\r') {
            if (n_chars != 0)
                break;
        }

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
        if (ch == '\n' || ch == '\r') {
            if (n_chars != 0)
                break;
        }

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
        if (ch == '\n' || ch == '\r') {
            if (n_chars != 0)
                break;
        }

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
    int i;
    int len = strlen(date);
    int slashCount = 0;
    for (i = 0; i < len; i++) {
        char ch = date[i];
        if (!isdigit(ch) && ch != '/')
            return 0;

        if (ch == '/')
            slashCount++;
    }

    if (slashCount != 2)
        return 0;

    int year, month, day;
    extractDateInfo(date, &year, &month, &day);

    if (year < 1000)
        return 0;

    if (month < 1 || month > 12)
        return 0;

    if (day < 1 || day > 31)
        return 0;
}

int validatePhoneNumber(char phoneNumber[]) {
    if (strlen(phoneNumber) != PHONE_NUMBER_LEN - 2)
        return 0;

    if (phoneNumber[0] != '0' || phoneNumber[1] != '9')
        return 0;

    return 1;
}

int validateEmail(char email[]) {
    int len = strlen(email);
    int i;
    int at = 0;
    int dot = 0;
    for (i = 0; i < len; i++) {
        char ch = email[i];
        if (!isalnum(ch) && ch != '@' && ch != '.' && ch != '_' && ch != '-')
            return 0;

        if (ch == '@')
            at++;

        if (ch == '.')
            dot++;
    }

    if (at != 1)
        return 0;

    if (dot < 1)
        return 0;

    for (i = 0; email[i] != '@'; i++);

    if (len - i < 3)
        return 0;

    i++;
    if (!isalnum(email[i]))
        return 0;

    for (; i < len; i++) {
        char ch = email[i];
        if (i == len - 1 && ch == '.')
            return 0;

        if (ch == '.') {
            if (email[i+1] == '.')
                return 0;
        }
    }

    return 1;
}

int validateName(char name[]) {
    int i;
    int len = strlen(name);
    for (i = 0; i < len; i++) {
        if (!isalpha(name[i]))
            return 0;
    }

    return 1;
}

int validatePassword(char password[]) {
    // validate password by a score system based on password characters
    int hasLower = 0, hasUpper = 0, hasNum = 0, hasPunct = 0;
    int i;
    int len = strlen(password);
    for (i = 0; i < len; i++) {
        char ch = password[i];
        if (isalpha(ch) && islower(ch))
            hasLower = 1;
        else if (isalpha(ch) && isupper(ch))
            hasUpper = 1;
        else if (isdigit(ch))
            hasNum = 1;
        else if (ispunct(ch))
            hasPunct = 1;
    }

    int score = hasLower + hasUpper + hasNum + hasPunct;
    score += len;
    return (score > 8);
}

void removeNewLine(char str[]) {
    int len = strlen(str);
    if (str[len-1] == '\n')
        str[len-1] = '\0';
}

void extractDateInfo(char date[], int *year, int *month, int *day) {
    int i = 0;
    *year = 0;
    for (; date[i] != '/'; i++)
        *year = *year * 10 + (date[i] - '0');

    i++;
    *month = 0;
    for(; date[i] != '/'; i++)
        *month = *month * 10 + (date[i] - '0');

    i++;
    *day = 0;
    for(; date[i] != '\0'; i++)
        *day = *day * 10 + (date[i] - '0');
}

void getTimeInfo(long long int time, int *year, int *month, int *day, int *hour, int *min, int *sec) {
    struct tm *local;
    local = localtime(&time);
    *year = local->tm_year + 1900; // tm_year is counted from 1900
    *month = local->tm_mon + 1; // tm_mon : 0 to 11
    *day = local->tm_mday;
    *hour = local->tm_hour;
    *min = local->tm_min;
    *sec = local->tm_sec;
}

void hashPassword(char hashedPassword[], char password[]) {
    int error = 0;
    if (crypto_pwhash_str(
        hashedPassword, password, strlen(password),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            printf("[Error] Out of memory!\n");
            error = 1;
    }

    // wipe the data in memory to avoid leak from memory dumps
    sodium_memzero(password, MAX_PASSWORD_LEN);

    if (error)
        exit(1);
}

int verifyPassword(char hashedPassword[], char password[]) {
    // cmp = 0 if password is verified
    int cmp = crypto_pwhash_str_verify(hashedPassword, password, strlen(password));
    sodium_memzero(password, MAX_PASSWORD_LEN);

    return (cmp == 0);
}

void lockPasswordMem(char password[]) {
    // lock the memory to prevent from swapping to disk
    sodium_mlock(password, MAX_PASSWORD_LEN);
}

void unlockPasswordMem(char password[]) {
    // unlock the memory
    sodium_munlock(password, MAX_PASSWORD_LEN);
}
