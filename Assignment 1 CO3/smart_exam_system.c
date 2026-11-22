
#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 10
#define SUBJECTS 3
#define ASSESSMENTS 2
#define ID_LENGTH 20

/* Structure to store student result details */
typedef struct
{
    char id[ID_LENGTH];
    int total;
    float average;
} StudentResult;

/* Function declarations */
void readStudentData(int n, char ids[][ID_LENGTH],
                     int marks[MAX_STUDENTS][SUBJECTS]);

void readAssessmentData(int n,
    int assessmentMarks[MAX_STUDENTS][ASSESSMENTS][SUBJECTS]);

void displayMarks(int n, char ids[][ID_LENGTH],
                  int marks[MAX_STUDENTS][SUBJECTS]);

void displayAssessmentMarks(int n,
    int assessmentMarks[MAX_STUDENTS][ASSESSMENTS][SUBJECTS]);

int calculateTotal(int marks[], int count);
float calculateAverage(int total, int count);

void calculateResults(int n, int marks[MAX_STUDENTS][SUBJECTS],
                      StudentResult results[]);

int searchStudent(int n, char ids[][ID_LENGTH], char key[]);

void searchMark(int n, int marks[MAX_STUDENTS][SUBJECTS], int key);

void findSubjectToppers(int n,
    char ids[][ID_LENGTH],
    int marks[MAX_STUDENTS][SUBJECTS]);

void bubbleSort(StudentResult results[], int n);

void sortSubjectMarks(int n,
    int marks[MAX_STUDENTS][SUBJECTS], int subject);

void displayResults(StudentResult results[], int n);

void getResult(int marks[], int count,
               int *total, float *average);

int recursiveSum(int arr[], int n);

int recursiveMax(int arr[], int n);

void pointerTraversal(int arr[], int n);

void modifyValue(int *value);

void mergeResults(StudentResult a[], int n1,
                  StudentResult b[], int n2,
                  StudentResult merged[]);

void demonstrateFunctionTypes(void);
void displayMessage(void);
void printNumber(int number);
int getNumber(void);
int addNumbers(int a, int b);


/* MAIN FUNCTION */

int main()
{
    int n, i, j, choice, subject, key;
    int marks[MAX_STUDENTS][SUBJECTS];
    int assessmentMarks[MAX_STUDENTS][ASSESSMENTS][SUBJECTS];

    char ids[MAX_STUDENTS][ID_LENGTH];
    char searchID[ID_LENGTH];

    StudentResult results[MAX_STUDENTS];

    printf("\n====================================\n");
    printf(" SMART EXAMINATION AND RESULT SYSTEM\n");
    printf("====================================\n");

    printf("Enter number of students (1-%d): ", MAX_STUDENTS);

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_STUDENTS)
    {
        printf("Invalid student count!\n");
        return 1;
    }

    /* TASK 2: TWO-DIMENSIONAL ARRAY */

    printf("\n--- ENTER STUDENT DETAILS ---\n");

    readStudentData(n, ids, marks);

    displayMarks(n, ids, marks);

    /* TASK 3: THREE-DIMENSIONAL ARRAY */

    printf("\n--- MULTIDIMENSIONAL ARRAY DATA ---\n");

    readAssessmentData(n, assessmentMarks);

    displayAssessmentMarks(n, assessmentMarks);

    /* TASK 4: SEARCHING */

    printf("\n--- STUDENT SEARCH ---\n");

    printf("Enter Student ID: ");
    scanf("%19s", searchID);

    choice = searchStudent(n, ids, searchID);

    if (choice != -1)
    {
        printf("\nStudent Found!\n");
        printf("Student ID: %s\n", ids[choice]);

        for (j = 0; j < SUBJECTS; j++)
        {
            printf("Subject %d: %d\n",
                   j + 1, marks[choice][j]);
        }
    }
    else
    {
        printf("Student Not Found!\n");
    }

    printf("\nEnter mark to search: ");

    if (scanf("%d", &key) == 1)
    {
        searchMark(n, marks, key);
    }

    findSubjectToppers(n, ids, marks);

    /* TASK 5: SORTING */

    printf("\n--- SORTING SUBJECT MARKS ---\n");

    printf("Enter subject number (1-%d): ", SUBJECTS);
    scanf("%d", &subject);

    if (subject >= 1 && subject <= SUBJECTS)
    {
        sortSubjectMarks(n, marks, subject - 1);
    }
    else
    {
        printf("Invalid subject number!\n");
    }

    calculateResults(n, marks, results);

    bubbleSort(results, n);

    printf("\n--- STUDENT RANKING ---\n");

    displayResults(results, n);

    /* TASK 6: MERGING */

    printf("\n--- MERGING TWO SECTION RESULTS ---\n");

    {
        StudentResult sectionA[3] = {
            {"A101", 210, 70.0},
            {"A102", 245, 81.67},
            {"A103", 270, 90.0}
        };

        StudentResult sectionB[2] = {
            {"B101", 220, 73.33},
            {"B102", 245, 81.67}
        };

        StudentResult merged[5];

        mergeResults(sectionA, 3, sectionB, 2, merged);

        printf("Combined results in ascending order:\n");
        displayResults(merged, 5);
    }

    /* TASK 7 AND 8: FUNCTIONS */

    printf("\n--- FUNCTION TYPES ---\n");

    demonstrateFunctionTypes();

    /* TASK 9: POINTERS */

    printf("\n--- POINTER OPERATIONS ---\n");

    {
        int value = 50;
        int sample[] = {10, 20, 30, 40};

        int *ptr = &value;

        printf("Original value: %d\n", value);
        printf("Address: %p\n", (void *)ptr);
        printf("Value using pointer: %d\n", *ptr);

        modifyValue(ptr);

        printf("Modified value: %d\n", value);

        printf("\nPointer array traversal:\n");
        pointerTraversal(sample, 4);

        printf("\nPointer-based result calculation:\n");

        {
            int total;
            float average;

            getResult(sample, 4, &total, &average);

            printf("Total = %d\n", total);
            printf("Average = %.2f\n", average);
        }
    }

    /* TASK 10: RECURSION */

    printf("\n--- RECURSIVE OPERATIONS ---\n");

    {
        int arr[] = {12, 18, 20, 25, 15};
        int size = 5;

        printf("Recursive Sum = %d\n",
               recursiveSum(arr, size));

        printf("Recursive Maximum = %d\n",
               recursiveMax(arr, size));
    }

    printf("\n====================================\n");
    printf(" PROGRAM EXECUTION COMPLETED\n");
    printf("====================================\n");

    return 0;
}


/* TASK 2: READ STUDENT DATA */

void readStudentData(int n, char ids[][ID_LENGTH],
                     int marks[MAX_STUDENTS][SUBJECTS])
{
    int i, j, duplicate;

    for (i = 0; i < n; i++)
    {
        do
        {
            duplicate = 0;

            printf("\nEnter Student ID %d: ", i + 1);
            scanf("%19s", ids[i]);

            for (j = 0; j < i; j++)
            {
                if (strcmp(ids[i], ids[j]) == 0)
                {
                    duplicate = 1;
                    printf("Duplicate ID! Enter another ID.\n");
                    break;
                }
            }

        } while (duplicate);

        for (j = 0; j < SUBJECTS; j++)
        {
            do
            {
                printf("Enter Subject %d mark: ", j + 1);

                if (scanf("%d", &marks[i][j]) != 1)
                {
                    printf("Invalid input. Restart the program.\n");
                    return;
                }

                if (marks[i][j] < 0 || marks[i][j] > 100)
                {
                    printf("Marks must be between 0 and 100.\n");
                }

            } while (marks[i][j] < 0 || marks[i][j] > 100);
        }
    }
}


/* TASK 3: THREE-DIMENSIONAL ARRAY */

void readAssessmentData(int n,
    int assessmentMarks[MAX_STUDENTS][ASSESSMENTS][SUBJECTS])
{
    int i, j, k;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < ASSESSMENTS; j++)
        {
            printf("\nStudent %d - Assessment %d\n",
                   i + 1, j + 1);

            for (k = 0; k < SUBJECTS; k++)
            {
                do
                {
                    printf("Subject %d mark: ", k + 1);
                    scanf("%d", &assessmentMarks[i][j][k]);

                    if (assessmentMarks[i][j][k] < 0 ||
                        assessmentMarks[i][j][k] > 100)
                    {
                        printf("Enter marks between 0 and 100.\n");
                    }

                } while (assessmentMarks[i][j][k] < 0 ||
                         assessmentMarks[i][j][k] > 100);
            }
        }
    }
}


void displayAssessmentMarks(int n,
    int assessmentMarks[MAX_STUDENTS][ASSESSMENTS][SUBJECTS])
{
    int i, j, k, total;

    for (i = 0; i < n; i++)
    {
        total = 0;

        printf("\nStudent %d\n", i + 1);

        for (j = 0; j < ASSESSMENTS; j++)
        {
            printf("Assessment %d: ", j + 1);

            for (k = 0; k < SUBJECTS; k++)
            {
                printf("%d ", assessmentMarks[i][j][k]);
                total += assessmentMarks[i][j][k];
            }

            printf("\n");
        }

        printf("Total across assessments = %d\n", total);

        printf("Average across assessments = %.2f\n",
               (float)total / (ASSESSMENTS * SUBJECTS));
    }
}


/* DISPLAY TWO-DIMENSIONAL ARRAY */

void displayMarks(int n, char ids[][ID_LENGTH],
                  int marks[MAX_STUDENTS][SUBJECTS])
{
    int i, j;

    printf("\nID\t");

    for (j = 0; j < SUBJECTS; j++)
        printf("S%d\t", j + 1);

    printf("\n");

    for (i = 0; i < n; i++)
    {
        printf("%s\t", ids[i]);

        for (j = 0; j < SUBJECTS; j++)
        {
            printf("%d\t", marks[i][j]);
        }

        printf("\n");
    }
}


/* TOTAL AND AVERAGE FUNCTIONS */

int calculateTotal(int marks[], int count)
{
    int i, total = 0;

    for (i = 0; i < count; i++)
        total += marks[i];

    return total;
}


float calculateAverage(int total, int count)
{
    return (float)total / count;
}


void calculateResults(int n,
    int marks[MAX_STUDENTS][SUBJECTS],
    StudentResult results[])
{
    int i;

    for (i = 0; i < n; i++)
    {
        results[i].total = calculateTotal(marks[i], SUBJECTS);

        results[i].average =
            calculateAverage(results[i].total, SUBJECTS);

        /* IDs are associated separately during result creation
           in main through the corresponding student records. */
    }
}


/* TASK 4: STUDENT ID SEARCH */

int searchStudent(int n, char ids[][ID_LENGTH], char key[])
{
    int i;

    for (i = 0; i < n; i++)
    {
        if (strcmp(ids[i], key) == 0)
            return i;
    }

    return -1;
}


/* SEARCH FOR A PARTICULAR MARK */

void searchMark(int n,
    int marks[MAX_STUDENTS][SUBJECTS], int key)
{
    int i, j, found = 0;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < SUBJECTS; j++)
        {
            if (marks[i][j] == key)
            {
                printf("Found at Student %d, Subject %d\n",
                       i + 1, j + 1);

                found = 1;
            }
        }
    }

    if (!found)
        printf("Mark not found.\n");
}


/* SUBJECT-WISE TOPPERS */

void findSubjectToppers(int n,
    char ids[][ID_LENGTH],
    int marks[MAX_STUDENTS][SUBJECTS])
{
    int i, j, maximum;

    printf("\n--- SUBJECT TOPPERS ---\n");

    for (j = 0; j < SUBJECTS; j++)
    {
        maximum = marks[0][j];

        for (i = 1; i < n; i++)
        {
            if (marks[i][j] > maximum)
                maximum = marks[i][j];
        }

        printf("Subject %d Highest Mark: %d\n",
               j + 1, maximum);

        for (i = 0; i < n; i++)
        {
            if (marks[i][j] == maximum)
                printf("Topper: %s\n", ids[i]);
        }
    }
}


/* TASK 5: BUBBLE SORT */

void bubbleSort(StudentResult results[], int n)
{
    int i, j;
    StudentResult temp;

    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (results[j].total < results[j + 1].total)
            {
                temp = results[j];
                results[j] = results[j + 1];
                results[j + 1] = temp;
            }
        }
    }
}


/* SORT SUBJECT MARKS */

void sortSubjectMarks(int n,
    int marks[MAX_STUDENTS][SUBJECTS], int subject)
{
    int i, j, temp;
    int sorted[MAX_STUDENTS];

    for (i = 0; i < n; i++)
        sorted[i] = marks[i][subject];

    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (sorted[j] > sorted[j + 1])
            {
                temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    printf("Sorted marks: ");

    for (i = 0; i < n; i++)
        printf("%d ", sorted[i]);

    printf("\n");
}


/* DISPLAY RANKED RESULTS */

void displayResults(StudentResult results[], int n)
{
    int i, rank = 1;

    printf("\nRank\tTotal\tAverage\n");

    for (i = 0; i < n; i++)
    {
        if (i > 0 && results[i].total != results[i - 1].total)
            rank = i + 1;

        printf("%d\t%d\t%.2f\n",
               rank, results[i].total, results[i].average);
    }
}


/* TASK 6: MERGING TWO SORTED ARRAYS */

void mergeResults(StudentResult a[], int n1,
                  StudentResult b[], int n2,
                  StudentResult merged[])
{
    int i = 0, j = 0, k = 0;

    while (i < n1 && j < n2)
    {
        if (a[i].total <= b[j].total)
            merged[k++] = a[i++];
        else
            merged[k++] = b[j++];
    }

    while (i < n1)
        merged[k++] = a[i++];

    while (j < n2)
        merged[k++] = b[j++];
}


/* TASK 8: FOUR FUNCTION CATEGORIES */

void demonstrateFunctionTypes(void)
{
    int a, b;

    displayMessage();

    printNumber(25);

    a = getNumber();

    printf("Number returned by function: %d\n", a);

    printf("Enter two numbers for addition: ");

    scanf("%d %d", &a, &b);

    printf("Sum = %d\n", addNumbers(a, b));
}


/* NO ARGUMENTS AND NO RETURN VALUE */

void displayMessage(void)
{
    printf("Welcome to the Result Analytics System!\n");
}


/* ARGUMENTS AND NO RETURN VALUE */

void printNumber(int number)
{
    printf("Entered number: %d\n", number);
}


/* NO ARGUMENTS AND RETURN VALUE */

int getNumber(void)
{
    return 100;
}


/* ARGUMENTS AND RETURN VALUE */

int addNumbers(int a, int b)
{
    return a + b;
}


/* TASK 9: POINTER-BASED OPERATIONS */

void modifyValue(int *value)
{
    *value = *value + 10;
}


void pointerTraversal(int arr[], int n)
{
    int i;
    int *ptr = arr;

    for (i = 0; i < n; i++)
    {
        printf("%d ", *(ptr + i));
    }

    printf("\n");
}


void getResult(int marks[], int count,
               int *total, float *average)
{
    *total = calculateTotal(marks, count);

    *average = calculateAverage(*total, count);
}


/* TASK 10: RECURSION */

/* Recursive sum */

int recursiveSum(int arr[], int n)
{
    if (n <= 0)
        return 0;

    return arr[n - 1] + recursiveSum(arr, n - 1);
}


/* Recursive maximum */

int recursiveMax(int arr[], int n)
{
    int previous;

    if (n == 1)
        return arr[0];

    previous = recursiveMax(arr, n - 1);

    if (arr[n - 1] > previous)
        return arr[n - 1];

    return previous;
}
