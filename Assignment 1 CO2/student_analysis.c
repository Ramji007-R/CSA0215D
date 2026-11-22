
#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100
#define MIN_ATTENDANCE 75.0
#define MIN_MARK 40.0
#define WARNING_ATTENDANCE 70.0

/* Display the performance category */
void displayPerformance(double mark)
{
    if (mark >= 90)
        printf("Excellent");
    else if (mark >= 75)
        printf("Good");
    else if (mark >= 60)
        printf("Average");
    else
        printf("Needs Improvement");
}

/* Check examination eligibility */
int isEligible(double attendance, double mark)
{
    if (attendance >= MIN_ATTENDANCE)
    {
        if (mark >= MIN_MARK)
            return 1;
        else
            return 0;
    }
    else
    {
        return 0;
    }
}

/* Display all student records */
void displayStudents(int n, char id[][30],
                     double attendance[], double marks[])
{
    int i;

    printf("\n--------------- STUDENT RECORDS ---------------\n");
    printf("%-15s %-15s %-12s %-22s %-12s\n",
           "Student ID", "Attendance", "Mark", "Performance", "Eligibility");

    for (i = 0; i < n; i++)
    {
        printf("%-15s %-14.2f %-12.2f ",
               id[i], attendance[i], marks[i]);

        displayPerformance(marks[i]);

        if (isEligible(attendance[i], marks[i]))
            printf("       Eligible\n");
        else
            printf("       Not Eligible\n");
    }
}

/* Display class statistics */
void displayStatistics(int n, double marks[],
                       double attendance[])
{
    int i;
    int highestIndex = 0;
    int lowestIndex = 0;
    int eligibleCount = 0;
    int aboveAverage = 0;
    int belowAverage = 0;

    double total = 0.0;
    double average;

    for (i = 0; i < n; i++)
    {
        total += marks[i];

        if (marks[i] > marks[highestIndex])
            highestIndex = i;

        if (marks[i] < marks[lowestIndex])
            lowestIndex = i;

        if (isEligible(attendance[i], marks[i]))
            eligibleCount++;
    }

    average = total / n;

    for (i = 0; i < n; i++)
    {
        if (marks[i] > average)
            aboveAverage++;
        else if (marks[i] < average)
            belowAverage++;
    }

    printf("\n--------------- CLASS STATISTICS ---------------\n");
    printf("Total Students       : %d\n", n);
    printf("Total Marks          : %.2f\n", total);
    printf("Class Average        : %.2f\n", average);
    printf("Highest Mark         : %.2f\n", marks[highestIndex]);
    printf("Lowest Mark          : %.2f\n", marks[lowestIndex]);
    printf("Above Average        : %d\n", aboveAverage);
    printf("Below Average        : %d\n", belowAverage);
    printf("Eligible Students    : %d\n", eligibleCount);
}

/* Search student using linear search */
void searchStudent(int n, char id[][30],
                   double attendance[], double marks[])
{
    char searchID[30];
    int i;
    int found = 0;

    printf("\nEnter Student ID to search: ");

    if (scanf("%29s", searchID) != 1)
    {
        printf("Invalid input.\n");
        return;
    }

    for (i = 0; i < n; i++)
    {
        if (strcmp(id[i], searchID) == 0)
        {
            printf("\nStudent Found!\n");
            printf("Student ID : %s\n", id[i]);
            printf("Attendance : %.2f%%\n", attendance[i]);
            printf("Mark       : %.2f\n", marks[i]);

            printf("Performance: ");
            displayPerformance(marks[i]);

            printf("\nEligibility: ");

            if (isEligible(attendance[i], marks[i]))
                printf("Eligible\n");
            else
                printf("Not Eligible\n");

            found = 1;
            break;
        }
    }

    if (!found)
        printf("Student ID not found.\n");
}

/* Display attendance improvement warnings */
void displayAttendanceWarnings(int n, char id[][30],
                               double attendance[])
{
    int i;
    int warningCount = 0;

    printf("\n---------- ATTENDANCE WARNINGS ----------\n");

    for (i = 0; i < n; i++)
    {
        if (attendance[i] < WARNING_ATTENDANCE)
            continue;

        if (attendance[i] < MIN_ATTENDANCE)
        {
            printf("%s : %.2f%% - Attendance needs improvement.\n",
                   id[i], attendance[i]);
            warningCount++;
        }
    }

    if (warningCount == 0)
        printf("No students in the attendance warning range.\n");
}

/* Display merit list using bubble sort */
void displayMeritList(int n, char id[][30], double marks[])
{
    int i, j;
    int swapped;

    char tempID[30];
    double tempMark;

    char sortedID[MAX_STUDENTS][30];
    double sortedMarks[MAX_STUDENTS];

    /* Copy original records */
    for (i = 0; i < n; i++)
    {
        strcpy(sortedID[i], id[i]);
        sortedMarks[i] = marks[i];
    }

    /* Bubble sort in descending order */
    for (i = 0; i < n - 1; i++)
    {
        swapped = 0;

        for (j = 0; j < n - i - 1; j++)
        {
            if (sortedMarks[j] < sortedMarks[j + 1])
            {
                tempMark = sortedMarks[j];
                sortedMarks[j] = sortedMarks[j + 1];
                sortedMarks[j + 1] = tempMark;

                strcpy(tempID, sortedID[j]);
                strcpy(sortedID[j], sortedID[j + 1]);
                strcpy(sortedID[j + 1], tempID);

                swapped = 1;
            }
        }

        if (!swapped)
            break;
    }

    printf("\n--------------- MERIT LIST ---------------\n");
    printf("%-10s %-20s %-10s\n", "Rank", "Student ID", "Mark");

    for (i = 0; i < n; i++)
    {
        printf("%-10d %-20s %-10.2f\n",
               i + 1, sortedID[i], sortedMarks[i]);
    }

    printf("\nTie Rule: Students with equal marks retain their original order.\n");
}

/* Check duplicate student IDs using nested loops */
void checkDuplicateIDs(int n, char id[][30])
{
    int i, j;
    int duplicateFound = 0;

    for (i = 0; i < n; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (strcmp(id[i], id[j]) == 0)
            {
                printf("Duplicate ID found: %s\n", id[i]);
                duplicateFound = 1;
                break;
            }
        }

        if (duplicateFound)
            break;
    }

    if (!duplicateFound)
        printf("All student IDs are unique.\n");
}

int main(void)
{
    char id[MAX_STUDENTS][30];
    double attendance[MAX_STUDENTS];
    double marks[MAX_STUDENTS];

    int n, i;
    int choice;

    printf("============================================\n");
    printf(" STUDENT ATTENDANCE AND PERFORMANCE SYSTEM\n");
    printf("============================================\n");

    /* Validate number of students using do-while */
    do
    {
        printf("Enter number of students (1-%d): ", MAX_STUDENTS);

        if (scanf("%d", &n) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            return 1;
        }

        if (n < 1 || n > MAX_STUDENTS)
            printf("Student count must be between 1 and %d.\n",
                   MAX_STUDENTS);

    } while (n < 1 || n > MAX_STUDENTS);

    /* Read student records */
    for (i = 0; i < n; i++)
    {
        int j;
        int duplicate;

        printf("\nEnter details for Student %d\n", i + 1);

        /* Read unique student ID */
        do
        {
            duplicate = 0;

            printf("Student ID: ");

            if (scanf("%29s", id[i]) != 1)
            {
                printf("Invalid ID input.\n");
                return 1;
            }

            for (j = 0; j < i; j++)
            {
                if (strcmp(id[i], id[j]) == 0)
                {
                    duplicate = 1;
                    printf("ID already exists. Enter a different ID.\n");
                    break;
                }
            }

        } while (duplicate);

        /* Validate attendance */
        printf("Attendance Percentage: ");

        if (scanf("%lf", &attendance[i]) != 1 ||
            attendance[i] < 0 || attendance[i] > 100)
        {
            printf("Invalid attendance. Enter a value from 0 to 100.\n");
            return 1;
        }

        /* Validate internal marks */
        printf("Internal Mark (0-100): ");

        if (scanf("%lf", &marks[i]) != 1 ||
            marks[i] < 0 || marks[i] > 100)
        {
            printf("Invalid mark. Enter a value from 0 to 100.\n");
            return 1;
        }
    }

    /* Repeating menu using while loop */
    while (1)
    {
        printf("\n=============== MAIN MENU ===============\n");
        printf("1. Display Student Records\n");
        printf("2. Display Class Statistics\n");
        printf("3. Search Student by ID\n");
        printf("4. Display Merit List\n");
        printf("5. Attendance Improvement Warnings\n");
        printf("6. Check Duplicate IDs\n");
        printf("7. Exit\n");
        printf("=========================================\n");

        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid menu input. Exiting program.\n");
            break;
        }

        switch (choice)
        {
            case 1:
                displayStudents(n, id, attendance, marks);
                break;

            case 2:
                displayStatistics(n, marks, attendance);
                break;

            case 3:
                searchStudent(n, id, attendance, marks);
                break;

            case 4:
                displayMeritList(n, id, marks);
                break;

            case 5:
                displayAttendanceWarnings(n, id, attendance);
                break;

            case 6:
                checkDuplicateIDs(n, id);
                break;

            case 7:
                printf("\nThank you for using the system!\n");
                return 0;

            default:
                printf("Invalid choice! Select an option from 1 to 7.\n");
        }
    }

    return 0;
}
