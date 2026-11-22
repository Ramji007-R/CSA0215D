/*
 SIMATS ENGINEERING - C PROGRAMMING ASSIGNMENT 4 (CO4)
 Command-Line Library Inventory and Membership Management System

 Build: gcc -std=c11 -Wall -Wextra -pedantic library.c -o library

 Data files created in the working directory:
 books.txt, members.txt, borrowing.txt
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#define ID 16
#define TITLE 80
#define NAME 60
#define EMAIL 80
#define PHONE 16
#define CITY 40
#define POSTAL 10
#define DATE 11
#define CODE 12
#define LINE 256
#define MAX_RECORDS 1000

#define BOOK_FILE "books.txt"
#define MEMBER_FILE "members.txt"
#define BORROW_FILE "borrowing.txt"

struct Address { char city[CITY], postalCode[POSTAL]; };
struct Contact { char email[EMAIL], phone[PHONE]; struct Address address; };
struct Member { char id[ID], name[NAME]; struct Contact contact; };

enum IdentifierType { NUMERIC_ID = 1, TEXT_ID = 2 };
union IdentifierValue { int accessionNumber; char shortCode[CODE]; };

struct Book {
    char id[ID], title[TITLE], author[TITLE];
    int year, available;
    enum IdentifierType identifierType;
    union IdentifierValue identifier;
};

struct BorrowRecord {
    char memberId[ID], bookId[ID], issueDate[DATE];
    int returned;
};

/* File-scope variable; it has static storage duration. */
int totalOperations = 0;

/* ---------- Input and validation ---------- */

static void stripNewline(char *s) {
    if (s) s[strcspn(s, "\r\n")] = '\0';
}

static int readLine(const char *prompt, char *out, size_t size) {
    size_t len;
    int ch;
    if (!out || size < 2) return 0;
    printf("%s", prompt);
    if (!fgets(out, (int)size, stdin)) return 0;
    len = strlen(out);
    if (len && out[len - 1] == '\n') {
        out[--len] = '\0';
        if (len && out[len - 1] == '\r') out[--len] = '\0';
    } else if (!feof(stdin)) {
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        puts("Input too long. Please try again.");
        return 0;
    }
    if (!out[0]) { puts("Input cannot be empty."); return 0; }
    if (strchr(out, '|') || strchr(out, '\t')) {
        puts("The characters '|' and tab are not allowed.");
        return 0;
    }
    return 1;
}

static int readRequired(const char *prompt, char *out, size_t size) {
    while (!readLine(prompt, out, size)) {
        if (feof(stdin)) return 0;
    }
    return 1;
}

static int toLong(const char *s, long low, long high, long *value) {
    char *end;
    long n;
    if (!s || !*s || !value) return 0;
    errno = 0;
    n = strtol(s, &end, 10);
    if (errno || end == s || *end || n < low || n > high) return 0;
    *value = n;
    return 1;
}

static int readInt(const char *prompt, int low, int high, int *value) {
    char s[LINE];
    long n;
    while (1) {
        if (!readRequired(prompt, s, sizeof s)) return 0;
        if (toLong(s, low, high, &n)) { *value = (int)n; return 1; }
        printf("Enter a whole number from %d to %d.\n", low, high);
    }
}

static int validDate(const char *s) {
    int i;
    if (!s || strlen(s) != 10 || s[4] != '-' || s[7] != '-') return 0;
    for (i = 0; i < 10; i++)
        if (i != 4 && i != 7 && !isdigit((unsigned char)s[i])) return 0;
    return 1;
}

/* ---------- Dynamic memory ---------- */

static int reserve(void **array, size_t itemSize, size_t *capacity,
                   size_t needed) {
    size_t next;
    void *temp;
    if (needed <= *capacity) return 1;
    if (needed > MAX_RECORDS || itemSize == 0) return 0;
    next = *capacity ? *capacity : 4;
    while (next < needed) {
        if (next > MAX_RECORDS / 2) { next = MAX_RECORDS; break; }
        next *= 2;
    }
    temp = realloc(*array, next * itemSize);
    if (!temp) return 0;
    *array = temp;
    *capacity = next;
    return 1;
}

/* ---------- Book file and operations ---------- */

static int findBook(const struct Book *a, size_t n, const char *id) {
    size_t i;
    for (i = 0; i < n; i++) if (strcmp(a[i].id, id) == 0) return (int)i;
    return -1;
}

static int loadBooks(struct Book **a, size_t *n, size_t *cap) {
    FILE *f = fopen(BOOK_FILE, "r");
    char line[LINE * 2];
    if (!f) return 1;
    while (fgets(line, sizeof line, f)) {
        char *v[7], *p;
        int k = 0;
        long year, available, type, number;
        struct Book b;
        stripNewline(line);
        p = strtok(line, "|");
        while (p && k < 7) { v[k++] = p; p = strtok(NULL, "|"); }
        if (k != 7 || !toLong(v[3], 1450, 2100, &year) ||
            !toLong(v[4], 0, 1, &available) ||
            !toLong(v[5], 1, 2, &type)) continue;
        memset(&b, 0, sizeof b);
        if (strlen(v[0]) >= ID || strlen(v[1]) >= TITLE ||
            strlen(v[2]) >= TITLE) continue;
        strcpy(b.id, v[0]); strcpy(b.title, v[1]); strcpy(b.author, v[2]);
        b.year = (int)year; b.available = (int)available;
        b.identifierType = (enum IdentifierType)type;
        if (type == NUMERIC_ID) {
            if (!toLong(v[6], 0, INT_MAX, &number)) continue;
            b.identifier.accessionNumber = (int)number;
        } else {
            if (strlen(v[6]) >= CODE) continue;
            strcpy(b.identifier.shortCode, v[6]);
        }
        if (!reserve((void **)a, sizeof **a, cap, *n + 1)) {
            fclose(f); return 0;
        }
        (*a)[(*n)++] = b;
    }
    fclose(f);
    return 1;
}

static int saveBooks(const struct Book *a, size_t n) {
    FILE *f = fopen(BOOK_FILE, "w");
    size_t i;
    if (!f) { perror("books.txt"); return 0; }
    for (i = 0; i < n; i++) {
        if (a[i].identifierType == NUMERIC_ID)
            fprintf(f, "%s|%s|%s|%d|%d|%d|%d\n", a[i].id, a[i].title,
                    a[i].author, a[i].year, a[i].available,
                    a[i].identifierType, a[i].identifier.accessionNumber);
        else
            fprintf(f, "%s|%s|%s|%d|%d|%d|%s\n", a[i].id, a[i].title,
                    a[i].author, a[i].year, a[i].available,
                    a[i].identifierType, a[i].identifier.shortCode);
    }
    if (fclose(f)) { perror("books.txt"); return 0; }
    return 1;
}

static void displayBook(const struct Book *b) {
    if (!b) return;
    printf("ID: %s\nTitle: %s\nAuthor: %s\nYear: %d\nStatus: %s\n",
           b->id, b->title, b->author, b->year,
           b->available ? "Available" : "Issued");
    if (b->identifierType == NUMERIC_ID)
        printf("Accession number: %d\n", b->identifier.accessionNumber);
    else printf("Short code: %s\n", b->identifier.shortCode);
}

static void listBooks(const struct Book *a, size_t n) {
    size_t i;
    if (!n) { puts("No books found."); return; }
    printf("%-12s %-28s %-22s %-6s %s\n",
           "BOOK ID", "TITLE", "AUTHOR", "YEAR", "STATUS");
    for (i = 0; i < n; i++)
        printf("%-12s %-28.28s %-22.22s %-6d %s\n",
               a[i].id, a[i].title, a[i].author, a[i].year,
               a[i].available ? "Available" : "Issued");
}

static int addBooks(struct Book **a, size_t *n, size_t *cap, int count) {
    int i;
    if (!reserve((void **)a, sizeof **a, cap, *n + (size_t)count)) {
        puts("Memory allocation failed."); return 0;
    }
    for (i = 0; i < count; i++) {
        struct Book b;
        char kind[LINE];
        int year;
        memset(&b, 0, sizeof b);
        printf("\nBook %d of %d\n", i + 1, count);
        if (!readRequired("Book ID: ", b.id, sizeof b.id) ||
            !readRequired("Title: ", b.title, sizeof b.title) ||
            !readRequired("Author: ", b.author, sizeof b.author) ||
            !readInt("Publication year (1450-2100): ", 1450, 2100, &year))
            return 0;
        if (findBook(*a, *n, b.id) >= 0) {
            puts("Duplicate book ID. Enter this record again.");
            i--; continue;
        }
        b.year = year; b.available = 1;
        while (1) {
            if (!readRequired("Identifier type (N=numeric, T=text): ",
                              kind, sizeof kind)) return 0;
            if (kind[0] == 'N' || kind[0] == 'n') {
                if (!readInt("Accession number: ", 0, INT_MAX,
                             &b.identifier.accessionNumber)) return 0;
                b.identifierType = NUMERIC_ID; break;
            }
            if (kind[0] == 'T' || kind[0] == 't') {
                if (!readRequired("Short code: ", b.identifier.shortCode,
                                  sizeof b.identifier.shortCode)) return 0;
                b.identifierType = TEXT_ID; break;
            }
            puts("Enter N or T.");
        }
        (*a)[(*n)++] = b;
    }
    if (!saveBooks(*a, *n)) return 0;
    totalOperations++;
    puts("Book record(s) saved.");
    return 1;
}

static int updateBook(struct Book *a, size_t n, const char *id, int status) {
    int index = findBook(a, n, id);
    struct Book *ptr;
    if (index < 0) { puts("Book not found."); return 0; }
    ptr = &a[index];
    ptr->available = status; /* structure pointer and -> operator */
    totalOperations++;
    return 1;
}

static int removeBook(struct Book *a, size_t *n, const char *id) {
    int index = findBook(a, *n, id);
    size_t i;
    if (index < 0) { puts("Book not found."); return 0; }
    for (i = (size_t)index; i + 1 < *n; i++) a[i] = a[i + 1];
    (*n)--;
    totalOperations++;
    return 1;
}

/* ---------- Member file and operations ---------- */

static int findMember(const struct Member *a, size_t n, const char *id) {
    size_t i;
    for (i = 0; i < n; i++) if (strcmp(a[i].id, id) == 0) return (int)i;
    return -1;
}

static int loadMembers(struct Member **a, size_t *n, size_t *cap) {
    FILE *f = fopen(MEMBER_FILE, "r");
    char line[LINE * 2];
    if (!f) return 1;
    while (fgets(line, sizeof line, f)) {
        char *v[6], *p;
        int k = 0;
        struct Member m;
        stripNewline(line);
        p = strtok(line, "|");
        while (p && k < 6) { v[k++] = p; p = strtok(NULL, "|"); }
        if (k != 6 || strlen(v[0]) >= ID || strlen(v[1]) >= NAME ||
            strlen(v[2]) >= EMAIL || strlen(v[3]) >= PHONE ||
            strlen(v[4]) >= CITY || strlen(v[5]) >= POSTAL) continue;
        memset(&m, 0, sizeof m);
        strcpy(m.id, v[0]); strcpy(m.name, v[1]);
        strcpy(m.contact.email, v[2]); strcpy(m.contact.phone, v[3]);
        strcpy(m.contact.address.city, v[4]);
        strcpy(m.contact.address.postalCode, v[5]);
        if (!reserve((void **)a, sizeof **a, cap, *n + 1)) {
            fclose(f); return 0;
        }
        (*a)[(*n)++] = m;
    }
    fclose(f);
    return 1;
}

static int saveMembers(const struct Member *a, size_t n) {
    FILE *f = fopen(MEMBER_FILE, "w");
    size_t i;
    if (!f) { perror("members.txt"); return 0; }
    for (i = 0; i < n; i++)
        fprintf(f, "%s|%s|%s|%s|%s|%s\n", a[i].id, a[i].name,
                a[i].contact.email, a[i].contact.phone,
                a[i].contact.address.city, a[i].contact.address.postalCode);
    if (fclose(f)) { perror("members.txt"); return 0; }
    return 1;
}

static int addMembers(struct Member **a, size_t *n, size_t *cap, int count) {
    int i;
    if (!reserve((void **)a, sizeof **a, cap, *n + (size_t)count)) {
        puts("Memory allocation failed."); return 0;
    }
    for (i = 0; i < count; i++) {
        struct Member m;
        memset(&m, 0, sizeof m);
        printf("\nMember %d of %d\n", i + 1, count);
        if (!readRequired("Member ID: ", m.id, sizeof m.id) ||
            !readRequired("Full name: ", m.name, sizeof m.name) ||
            !readRequired("Email: ", m.contact.email, sizeof m.contact.email) ||
            !readRequired("Phone: ", m.contact.phone, sizeof m.contact.phone) ||
            !readRequired("City: ", m.contact.address.city,
                          sizeof m.contact.address.city) ||
            !readRequired("Postal code: ", m.contact.address.postalCode,
                          sizeof m.contact.address.postalCode)) return 0;
        if (findMember(*a, *n, m.id) >= 0) {
            puts("Duplicate member ID. Enter this record again.");
            i--; continue;
        }
        (*a)[(*n)++] = m;
    }
    if (!saveMembers(*a, *n)) return 0;
    totalOperations++;
    puts("Member record(s) saved.");
    return 1;
}

static void listMembers(const struct Member *a, size_t n) {
    size_t i;
    if (!n) { puts("No members found."); return; }
    printf("%-12s %-22s %-28s %-15s %s\n",
           "MEMBER ID", "NAME", "EMAIL", "PHONE", "CITY");
    for (i = 0; i < n; i++)
        printf("%-12s %-22.22s %-28.28s %-15s %s\n",
               a[i].id, a[i].name, a[i].contact.email,
               a[i].contact.phone, a[i].contact.address.city);
}

/* ---------- Borrowing history ---------- */

static int loadBorrowing(struct BorrowRecord **a, size_t *n, size_t *cap) {
    FILE *f = fopen(BORROW_FILE, "r");
    char line[LINE];
    if (!f) return 1;
    while (fgets(line, sizeof line, f)) {
        char *v[4], *p;
        int k = 0;
        long returned;
        struct BorrowRecord b;
        stripNewline(line);
        p = strtok(line, "|");
        while (p && k < 4) { v[k++] = p; p = strtok(NULL, "|"); }
        if (k != 4 || !toLong(v[3], 0, 1, &returned) ||
            !validDate(v[2]) || strlen(v[0]) >= ID || strlen(v[1]) >= ID)
            continue;
        memset(&b, 0, sizeof b);
        strcpy(b.memberId, v[0]); strcpy(b.bookId, v[1]);
        strcpy(b.issueDate, v[2]); b.returned = (int)returned;
        if (!reserve((void **)a, sizeof **a, cap, *n + 1)) {
            fclose(f); return 0;
        }
        (*a)[(*n)++] = b;
    }
    fclose(f);
    return 1;
}

static int saveBorrowing(const struct BorrowRecord *a, size_t n) {
    FILE *f = fopen(BORROW_FILE, "w");
    size_t i;
    if (!f) { perror("borrowing.txt"); return 0; }
    for (i = 0; i < n; i++)
        fprintf(f, "%s|%s|%s|%d\n", a[i].memberId, a[i].bookId,
                a[i].issueDate, a[i].returned);
    if (fclose(f)) { perror("borrowing.txt"); return 0; }
    return 1;
}

static int addBorrow(struct BorrowRecord **a, size_t *n, size_t *cap,
                     const char *memberId, const char *bookId, const char *date) {
    struct BorrowRecord b;
    if (!reserve((void **)a, sizeof **a, cap, *n + 1)) {
        puts("Could not allocate borrowing record."); return 0;
    }
    memset(&b, 0, sizeof b);
    strcpy(b.memberId, memberId); strcpy(b.bookId, bookId);
    strcpy(b.issueDate, date); b.returned = 0;
    (*a)[(*n)++] = b;
    if (!saveBorrowing(*a, *n)) { (*n)--; return 0; }
    totalOperations++;
    puts("Borrowing record saved.");
    return 1;
}

static void memberSummary(const struct BorrowRecord *a, size_t n,
                          const char *memberId) {
    size_t i;
    int found = 0, returned = 0, pending = 0;
    printf("Borrowing summary for %s\n", memberId);
    for (i = 0; i < n; i++) {
        if (strcmp(a[i].memberId, memberId) == 0) {
            found = 1;
            printf("%s | %s | %s\n", a[i].bookId, a[i].issueDate,
                   a[i].returned ? "Returned" : "Not returned");
            if (a[i].returned) returned++; else pending++;
        }
    }
    if (!found) puts("No borrowing history.");
    printf("Total: %d | Returned: %d | Not returned: %d\n",
           returned + pending, returned, pending);
}


static int markReturned(struct BorrowRecord *records, size_t count,
                        struct Book *books, size_t bookCount,
                        const char *memberId, const char *bookId) {
    size_t i;
    int found = 0;
    for (i = 0; i < count; i++) {
        if (strcmp(records[i].memberId, memberId) == 0 &&
            strcmp(records[i].bookId, bookId) == 0 &&
            records[i].returned == 0) {
            records[i].returned = 1;
            found = 1;
            break;
        }
    }
    if (!found) {
        puts("No active borrowing record was found for this member and book.");
        return 0;
    }
    if (!saveBorrowing(records, count)) return 0;
    if (!updateBook(books, bookCount, bookId, 1) ||
        !saveBooks(books, bookCount)) return 0;
    totalOperations++;
    puts("Book marked as returned.");
    return 1;
}

static void demonstrateStorageClasses(void) {
    auto int localValue = 10;
    static int callCount = 0;
    extern int totalOperations;
    register int i;

    callCount++;
    printf("Storage classes: auto=%d, static-call=%d, extern-total=%d\n",
           localValue, callCount, totalOperations);
    for (i = 0; i < 1; i++) {
        /* register is a hint; it does not guarantee a CPU register. */
    }
}

/* ---------- Command-line interface ---------- */

static void usage(const char *p) {
    printf("Library Management System\n"
           "Commands:\n"
           "  %s add <count>\n"
           "  %s list\n"
           "  %s search <book_id>\n"
           "  %s update <book_id> <0|1>\n"
           "  %s remove <book_id>\n"
           "  %s add-member <count>\n"
           "  %s members\n"
           "  %s borrow <member_id> <book_id> <YYYY-MM-DD>\n"
           "  %s member-summary <member_id>\n",
           p,p,p,p,p,p,p,p,p);
}

int main(int argc, char *argv[]) {
    struct Book *books = NULL;
    struct Member *members = NULL;
    struct BorrowRecord *borrows = NULL;
    size_t nb = 0, cb = 0, nm = 0, cm = 0, nr = 0, cr = 0;
    long count, status;
    int result = 0;
    static int runCounter = 0; /* static local retains value between calls */
    runCounter++;
    demonstrateStorageClasses();

    if (!loadBooks(&books, &nb, &cb) ||
        !loadMembers(&members, &nm, &cm) ||
        !loadBorrowing(&borrows, &nr, &cr)) {
        puts("Unable to load records (possibly insufficient memory).");
        result = 1;
        goto cleanup;
    }

    if (argc < 2) { usage(argv[0]); result = 1; goto cleanup; }

    if (strcmp(argv[1], "add") == 0 && argc == 3) {
        if (!toLong(argv[2], 1, MAX_RECORDS, &count) ||
            !addBooks(&books, &nb, &cb, (int)count)) result = 1;
    } else if (strcmp(argv[1], "list") == 0 && argc == 2) {
        listBooks(books, nb);
    } else if (strcmp(argv[1], "search") == 0 && argc == 3) {
        int index = findBook(books, nb, argv[2]);
        if (index < 0) puts("Book not found.");
        else displayBook(&books[index]);
    } else if (strcmp(argv[1], "update") == 0 && argc == 4) {
        if (!toLong(argv[3], 0, 1, &status) ||
            !updateBook(books, nb, argv[2], (int)status) ||
            !saveBooks(books, nb)) result = 1;
        else puts("Book availability updated.");
    } else if (strcmp(argv[1], "remove") == 0 && argc == 3) {
        if (!removeBook(books, &nb, argv[2]) || !saveBooks(books, nb))
            result = 1;
        else puts("Book removed.");
    } else if (strcmp(argv[1], "add-member") == 0 && argc == 3) {
        if (!toLong(argv[2], 1, MAX_RECORDS, &count) ||
            !addMembers(&members, &nm, &cm, (int)count)) result = 1;
    } else if (strcmp(argv[1], "members") == 0 && argc == 2) {
        listMembers(members, nm);
    } else if (strcmp(argv[1], "borrow") == 0 && argc == 5) {
        int bi = findBook(books, nb, argv[3]);
        if (findMember(members, nm, argv[2]) < 0) {
            puts("Unknown member ID."); result = 1;
        } else if (bi < 0) {
            puts("Unknown book ID."); result = 1;
        } else if (!validDate(argv[4])) {
            puts("Date must use YYYY-MM-DD."); result = 1;
        } else if (!books[bi].available) {
            puts("Book is already issued."); result = 1;
        } else if (addBorrow(&borrows, &nr, &cr, argv[2], argv[3], argv[4])) {
            updateBook(books, nb, argv[3], 0);
            if (!saveBooks(books, nb)) result = 1;
        } else result = 1;
    } else if (strcmp(argv[1], "member-summary") == 0 && argc == 3) {
        if (findMember(members, nm, argv[2]) < 0)
            puts("Unknown member ID.");
        else memberSummary(borrows, nr, argv[2]);
    } else if (strcmp(argv[1], "return") == 0 && argc == 4) {
        if (findMember(members, nm, argv[2]) < 0) {
            puts("Unknown member ID."); result = 1;
        } else if (!markReturned(borrows, nr, books, nb, argv[2], argv[3])) {
            result = 1;
        }
    } else {
        usage(argv[0]);
        result = 1;
    }

    (void)runCounter;
cleanup:
    free(books);
    free(members);
    free(borrows);
    books = NULL; members = NULL; borrows = NULL;
    return result;
}
