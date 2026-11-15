# University Academic & Grade Management System

A console application written in C for managing academic affairs and student grades at a university.

> **Project status: educational.**
> This is my final project for a *Fundamentals of Programming* university course. I built it to practice C: structs, linked lists, file I/O, input validation, and basic security ideas such as password hashing. It is **not** production software and has **not** been security-audited. Please do not use it to store real student data.

## What to expect (and what not to)

**This repository is a good fit if you want to:**
- See a mid-sized C project that uses linked lists and plain-text file storage.
- Look at an example of password hashing with [libsodium](https://libsodium.gitbook.io/doc/) in a console app.
- Browse code written by a first-year student as a learning exercise.

**It is not:**
- A real or secure student information system.
- Cross-platform. It uses Windows-only headers and calls (`conio.h`, `windows.h`, `system("cls")`, `Beep`).
- Well-structured by professional standards. Everything lives in a single `main.c` file, and there are no automated tests.

## Known limitations

These are deliberate trade-offs of a course project, listed here so nobody is surprised:

- **Demo admin account.** The built-in admin account uses fixed credentials that are written in the source code. They are for demonstration only and would never be acceptable in a real system.
- **Simple password reset.** Resetting a password only requires answering a personal-information question (phone number or email), and the new password is derived from personal data. This is convenient for a demo but weak by real-world standards.
- **Plain-text storage.** Data is stored in `.txt` files. Passwords are hashed and salted, but the rest of the data (names, phone numbers, grades, and so on) is not encrypted.
- **Windows only.** Tested only with MinGW's `gcc` on Windows.

## Features

- Three user roles: admin, department head, and academic staff
- Management of students, lessons, and grades, with report tables
- Masked password input (asterisks)
- Input filtering and validation for every field
- Login that does not reveal whether the username or the password was wrong
- Password hashing with salt (libsodium), and wiping of plain-text passwords from memory after use
- Rejection of weak passwords when setting a new one
- Login attempt log, with tables of successful and failed sign-ins
- Temporary account lock after repeated failed logins (brute-force protection)
- CAPTCHA on login and an error sound after wrong credentials
- Backup and restore of data files
- Selectable color theme
- Clean output formatting

## Rules

- Usernames may contain only lowercase English letters, digits, and underscores (`_`).

## Building

> Only tested with **MinGW** on Windows.

1. Download and extract `libsodium-1.0.21-stable-mingw.tar.gz` (available from the libsodium GitHub releases).
2. Copy the extracted `libsodium-win64` directory into the project's root directory (next to `main.c`).
3. From the root directory, run:

```bash
gcc main.c -o main.exe -Ilibsodium-win64/include -Llibsodium-win64/lib -lsodium -static
```

A pre-compiled `.exe` is also available in this repository's **Releases** section.

## Running

Run the compiled `main.exe` **from a regular terminal or by double-clicking it, not from inside VS Code or another IDE's built-in terminal**. The program relies on Windows console features (clearing the screen, colors, hidden password input) that may not work correctly in embedded terminals.

## Language note

The program's interface is in English. A Persian README is available as [`README-fa.md`](README.md).

## Feedback

I'm still learning, so constructive suggestions are welcome. Feel free to open an issue.
