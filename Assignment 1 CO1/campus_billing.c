
#include <stdio.h>
#include <string.h>

#define SERVICE_PERCENT 5.0
#define CONCESSION_PERCENT 10.0
#define WARNING_LIMIT 1000.0

/* Function to calculate service charge */
double calculateService(double subtotal)
{
    return subtotal * SERVICE_PERCENT / 100.0;
}

int main(void)
{
    char facility_id[30];
    char department[30];

    double units, rate;
    double subtotal, service_charge;
    double concession = 0.0;
    double final_bill;

    int eligible;

    printf("========================================\n");
    printf("       SMART CAMPUS UTILITY BILLING\n");
    printf("========================================\n");

    /* Get facility details */
    printf("Enter Facility ID: ");
    if (scanf("%29s", facility_id) != 1)
    {
        printf("Error: Invalid facility ID.\n");
        return 1;
    }

    printf("Enter Department Code: ");
    if (scanf("%29s", department) != 1)
    {
        printf("Error: Invalid department code.\n");
        return 1;
    }

    /* Get and validate units */
    printf("Enter Units Consumed: ");
    if (scanf("%lf", &units) != 1 || units < 0)
    {
        printf("Error: Units must be non-negative.\n");
        return 1;
    }

    /* Get and validate rate */
    printf("Enter Rate Per Unit (Rs.): ");
    if (scanf("%lf", &rate) != 1 || rate < 0)
    {
        printf("Error: Rate must be non-negative.\n");
        return 1;
    }

    /* Check concession eligibility */
    eligible = (strcmp(department, "ELEC") == 0);

    /* Calculate bill */
    subtotal = units * rate;
    service_charge = calculateService(subtotal);

    final_bill = subtotal + service_charge;

    /* Apply concession if eligible */
    if (eligible)
    {
        concession =
            final_bill * CONCESSION_PERCENT / 100.0;

        final_bill = final_bill - concession;
    }

    /* Display bill summary */
    printf("\n========================================\n");
    printf("              BILL SUMMARY\n");
    printf("========================================\n");

    printf("Facility ID       : %s\n", facility_id);
    printf("Department        : %s\n", department);
    printf("Units Consumed    : %.2f\n", units);
    printf("Rate Per Unit     : Rs. %.2f\n", rate);

    printf("----------------------------------------\n");

    printf("Subtotal          : Rs. %.2f\n", subtotal);
    printf("Service Charge    : Rs. %.2f\n", service_charge);
    printf("Concession        : Rs. %.2f\n", concession);

    printf("----------------------------------------\n");

    printf("FINAL BILL        : Rs. %.2f\n", final_bill);

    /* Check warning limit */
    if (final_bill > WARNING_LIMIT)
    {
        printf("\nWARNING: Bill exceeds Rs. %.2f\n",
               WARNING_LIMIT);
    }
    else
    {
        printf("\nBill is within the warning limit.\n");
    }

    printf("========================================\n");
    printf("       Thank you!\n");
    printf("========================================\n");

    return 0;
}
