/**
 * =====================================================================================
 *  PUBLIC TRANSPORT SEAT RESERVATION AND PASSENGER MANAGEMENT SYSTEM
 *  Module: Bus Schedule & Seat Reservation Console Application in C
 *  
 *  Features:
 *  - Select travel date (YYYY-MM-DD or DD/MM/YYYY)
 *  - Select time slot: Morning, Afternoon, Evening, Night (or All)
 *  - Display every bus running on that date and respected time slot
 *  - Display realistic 40-seat bus layout with aisle (2x2)
 *  - Passenger registration & seat booking
 *  - Booking ID generation and printable ticket display
 *  - Booking lookup and reservation cancellation (seat released)
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define TOTAL_SEATS 40
#define MAX_BUSES 12
#define MAX_BOOKINGS 100

/* Time slot enumeration */
typedef enum {
    SLOT_MORNING = 1,    /* 06:00 AM - 11:59 AM */
    SLOT_AFTERNOON = 2,  /* 12:00 PM - 04:59 PM */
    SLOT_EVENING = 3,    /* 05:00 PM - 08:59 PM */
    SLOT_NIGHT = 4,      /* 09:00 PM - 05:59 AM */
    SLOT_ALL = 5
} TimeSlot;

/* Bus Structure */
typedef struct {
    int id;
    char busNumber[20];
    char busName[50];
    char routeFrom[50];
    char routeTo[50];
    TimeSlot slot;
    char departureTime[15];
    char arrivalTime[15];
    double fare;
    char busType[30];
    int seats[TOTAL_SEATS + 1]; /* 0: available, 1: booked (1-indexed) */
} Bus;

/* Passenger Structure */
typedef struct {
    char name[60];
    int age;
    char gender[15];
    char mobile[20];
} Passenger;

/* Booking Record Structure */
typedef struct {
    char bookingId[20];
    int busId;
    char busNumber[20];
    char busName[50];
    char route[100];
    char travelDate[25];
    char departureTime[15];
    char timeSlotName[20];
    int seatNumber;
    double fare;
    Passenger passenger;
    char bookingDate[30];
    char status[20]; /* "Confirmed" or "Cancelled" */
} Booking;

/* Global Data */
Bus busFleet[MAX_BUSES];
int totalBuses = 0;

Booking bookingHistory[MAX_BOOKINGS];
int totalBookings = 0;

/* Function Declarations */
void initFleet(void);
void initSampleBookings(void);
const char* getSlotName(TimeSlot slot);
const char* getSlotTimeRange(TimeSlot slot);
void displayHeader(void);
void searchBusesByDateAndSlot(void);
void displayBusList(Bus buses[], int count, const char* travelDate, TimeSlot slot);
void displaySeatLayout(Bus *bus);
void bookSeat(void);
void viewBookings(void);
void cancelBooking(void);
void adminDashboard(void);
void clearInputBuffer(void);
void trimNewline(char *str);

/* Helper to clear stdin */
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Helper to strip trailing newline */
void trimNewline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
    }
}

/* Convert slot enum to display string */
const char* getSlotName(TimeSlot slot) {
    switch (slot) {
        case SLOT_MORNING:   return "Morning";
        case SLOT_AFTERNOON: return "Afternoon";
        case SLOT_EVENING:   return "Evening";
        case SLOT_NIGHT:     return "Night";
        default:             return "All Slots";
    }
}

const char* getSlotTimeRange(TimeSlot slot) {
    switch (slot) {
        case SLOT_MORNING:   return "06:00 AM - 11:59 AM";
        case SLOT_AFTERNOON: return "12:00 PM - 04:59 PM";
        case SLOT_EVENING:   return "05:00 PM - 08:59 PM";
        case SLOT_NIGHT:     return "09:00 PM - 05:59 AM";
        default:             return "Full Day (24 Hours)";
    }
}

/* Initialize Bus Fleet with realistic multi-slot schedules */
void initFleet(void) {
    totalBuses = 0;

    /* --- MORNING BUSES --- */
    busFleet[totalBuses++] = (Bus){
        101, "SKY-101-EXP", "SkyRoute Sunrise Express",
        "Metro Central Terminal", "Ocean Bay Coastal Station",
        SLOT_MORNING, "07:00 AM", "11:15 AM", 22.00, "Executive AC Seater", {0}
    };
    busFleet[totalBuses++] = (Bus){
        102, "SKY-102-EXP", "Coastal Breeze Superfast",
        "Metro Central Terminal", "Ocean Bay Coastal Station",
        SLOT_MORNING, "09:30 AM", "01:45 PM", 25.00, "Luxury Air-Suspension", {0}
    };
    busFleet[totalBuses++] = (Bus){
        103, "SKY-103-EXP", "Northway Morning Express",
        "Metro Central Terminal", "North Valley Terminal",
        SLOT_MORNING, "11:00 AM", "03:15 PM", 20.00, "Semi-Sleeper Coach", {0}
    };

    /* --- AFTERNOON BUSES --- */
    busFleet[totalBuses++] = (Bus){
        201, "SKY-201-EXP", "Skyline Midday Cruiser",
        "Metro Central Terminal", "Ocean Bay Coastal Station",
        SLOT_AFTERNOON, "01:15 PM", "05:30 PM", 25.00, "Air-Suspension Luxury", {0}
    };
    busFleet[totalBuses++] = (Bus){
        202, "SKY-202-EXP", "Highland Afternoon Swift",
        "Metro Central Terminal", "Highland Terminal",
        SLOT_AFTERNOON, "03:45 PM", "08:00 PM", 28.00, "AC Air-Sleeper", {0}
    };

    /* --- EVENING BUSES --- */
    busFleet[totalBuses++] = (Bus){
        301, "SKY-301-EXP", "Golden Coast Sunset Express",
        "Metro Central Terminal", "Ocean Bay Coastal Station",
        SLOT_EVENING, "05:30 PM", "09:45 PM", 27.00, "Luxury Sleeper / Seater", {0}
    };
    busFleet[totalBuses++] = (Bus){
        302, "SKY-302-EXP", "Metro Twilight Shuttle",
        "Metro Central Terminal", "Airport Express Hub",
        SLOT_EVENING, "07:15 PM", "09:30 PM", 18.00, "City Express AC", {0}
    };
    busFleet[totalBuses++] = (Bus){
        303, "SKY-303-EXP", "Pacific Shore Evening Runner",
        "Metro Central Terminal", "Ocean Bay Coastal Station",
        SLOT_EVENING, "08:30 PM", "12:45 AM", 25.00, "Air-Suspension Coach", {0}
    };

    /* --- NIGHT BUSES --- */
    busFleet[totalBuses++] = (Bus){
        401, "SKY-401-EXP", "Night Owl AC Sleeper Coach",
        "Metro Central Terminal", "Ocean Bay Coastal Station",
        SLOT_NIGHT, "10:15 PM", "02:30 AM", 30.00, "Luxury AC Sleeper", {0}
    };
    busFleet[totalBuses++] = (Bus){
        402, "SKY-402-EXP", "Starline Midnight Express",
        "Metro Central Terminal", "South Port Terminal",
        SLOT_NIGHT, "11:45 PM", "04:15 AM", 32.00, "Multi-Axle Super Sleeper", {0}
    };
    busFleet[totalBuses++] = (Bus){
        403, "SKY-403-EXP", "Dawn Star Early Highway Rider",
        "Metro Central Terminal", "Ocean Bay Coastal Station",
        SLOT_NIGHT, "04:30 AM", "08:45 AM", 24.00, "Executive Seater", {0}
    };
}

/* Pre-fill initial sample demo bookings */
void initSampleBookings(void) {
    totalBookings = 0;

    /* Pre-book seats on Bus 102 */
    busFleet[1].seats[5] = 1;
    busFleet[1].seats[6] = 1;
    bookingHistory[totalBookings++] = (Booking){
        "PTS-104921", 102, "SKY-102-EXP", "Coastal Breeze Superfast",
        "Metro Central -> Ocean Bay", "2026-11-25", "09:30 AM", "Morning",
        5, 25.00, {"Dr. Sarah Jenkins", 34, "Female", "9876543210"},
        "2026-11-20 09:15 AM", "Confirmed"
    };

    /* Pre-book seat on Bus 201 */
    busFleet[3].seats[14] = 1;
    bookingHistory[totalBookings++] = (Booking){
        "PTS-295014", 201, "SKY-201-EXP", "Skyline Midday Cruiser",
        "Metro Central -> Ocean Bay", "2026-11-25", "01:15 PM", "Afternoon",
        14, 25.00, {"Alex Rivera", 28, "Male", "9812345678"},
        "2026-11-20 10:40 AM", "Confirmed"
    };

    /* Pre-book seat on Bus 401 */
    busFleet[8].seats[22] = 1;
    bookingHistory[totalBookings++] = (Booking){
        "PTS-401928", 401, "SKY-401-EXP", "Night Owl AC Sleeper Coach",
        "Metro Central -> Ocean Bay", "2026-11-25", "10:15 PM", "Night",
        22, 30.00, {"David Miller", 39, "Male", "9898981234"},
        "2026-11-21 03:20 PM", "Confirmed"
    };
}

/* Banner Header */
void displayHeader(void) {
    printf("\n========================================================================================\n");
    printf("        PUBLIC TRANSPORT SEAT RESERVATION & PASSENGER MANAGEMENT SYSTEM        \n");
    printf("             SkyRoute Inter-City Coach Network - Academic Project (C)          \n");
    printf("========================================================================================\n");
}

/* Count available seats on a bus */
int getAvailableSeatCount(Bus *bus) {
    int available = 0;
    for (int i = 1; i <= TOTAL_SEATS; i++) {
        if (bus->seats[i] == 0) {
            available++;
        }
    }
    return available;
}

/* Find bus by its ID */
Bus* findBusById(int id) {
    for (int i = 0; i < totalBuses; i++) {
        if (busFleet[i].id == id) {
            return &busFleet[i];
        }
    }
    return NULL;
}

/* 
 * CORE REQUIREMENT:
 * Select Date -> Select Time Slot (Morning / Afternoon / Evening / Night)
 * -> Display every bus that runs on that date and respected time slot
 */
void searchBusesByDateAndSlot(void) {
    char travelDate[30];
    int slotChoice;
    TimeSlot selectedSlot;

    displayHeader();
    printf("\n>>> BUS SCHEDULE SEARCH BY DATE & TIME SLOT <<<\n\n");

    /* 1. Enter Date */
    printf("Enter Travel Date (e.g., 2026-11-25 or DD-MM-YYYY): ");
    if (fgets(travelDate, sizeof(travelDate), stdin) == NULL) return;
    trimNewline(travelDate);

    if (strlen(travelDate) == 0) {
        strcpy(travelDate, "2026-11-25");
        printf("[Defaulted to Date: 2026-11-25]\n");
    }

    /* 2. Select Time Slot */
    printf("\nSelect Desired Time Slot:\n");
    printf("  [1] Morning   (06:00 AM - 11:59 AM)\n");
    printf("  [2] Afternoon (12:00 PM - 04:59 PM)\n");
    printf("  [3] Evening   (05:00 PM - 08:59 PM)\n");
    printf("  [4] Night     (09:00 PM - 05:59 AM)\n");
    printf("  [5] All Slots (Show All Buses on this Date)\n");
    printf("Enter choice (1-5): ");

    if (scanf("%d", &slotChoice) != 1 || slotChoice < 1 || slotChoice > 5) {
        printf("\n[!] Invalid slot selection! Defaulting to All Slots.\n");
        selectedSlot = SLOT_ALL;
        clearInputBuffer();
    } else {
        selectedSlot = (TimeSlot)slotChoice;
        clearInputBuffer();
    }

    /* 3. Filter and Show every bus matching date & time slot */
    Bus matchingBuses[MAX_BUSES];
    int matchCount = 0;

    for (int i = 0; i < totalBuses; i++) {
        if (selectedSlot == SLOT_ALL || busFleet[i].slot == selectedSlot) {
            matchingBuses[matchCount++] = busFleet[i];
        }
    }

    displayBusList(matchingBuses, matchCount, travelDate, selectedSlot);

    /* Offer next action */
    if (matchCount > 0) {
        printf("\nOptions:\n");
        printf("  [1] Select a Bus to View Seat Map & Reserve a Seat\n");
        printf("  [2] Return to Main Menu\n");
        printf("Enter choice: ");
        int opt;
        if (scanf("%d", &opt) == 1 && opt == 1) {
            clearInputBuffer();
            bookSeat();
        } else {
            clearInputBuffer();
        }
    }
}

/* Display formatted bus table */
void displayBusList(Bus buses[], int count, const char* travelDate, TimeSlot slot) {
    printf("\n---------------------------------------------------------------------------------------------------------------------\n");
    printf(" SCHEDULED BUSES FOR DATE: %s | TIME SLOT: %s (%s)\n", travelDate, getSlotName(slot), getSlotTimeRange(slot));
    printf("---------------------------------------------------------------------------------------------------------------------\n");
    printf("%-7s | %-13s | %-28s | %-10s | %-9s | %-12s | %-7s\n",
           "Bus ID", "Coach No", "Service Name & Type", "Slot", "Dep Time", "Seats Avail", "Fare");
    printf("---------------------------------------------------------------------------------------------------------------------\n");

    if (count == 0) {
        printf("  No buses scheduled for this date and time slot combination.\n");
    } else {
        for (int i = 0; i < count; i++) {
            int avail = getAvailableSeatCount(&buses[i]);
            char seatStr[20];
            sprintf(seatStr, "%d / %d", avail, TOTAL_SEATS);

            printf("%-7d | %-13s | %-28s | %-10s | %-9s | %-12s | $%.2f\n",
                   buses[i].id,
                   buses[i].busNumber,
                   buses[i].busName,
                   getSlotName(buses[i].slot),
                   buses[i].departureTime,
                   seatStr,
                   buses[i].fare);
            printf("        Route: %s -> %s\n", buses[i].routeFrom, buses[i].routeTo);
            printf("        Coach Type: %s | Estimated Arrival: %s\n", buses[i].busType, buses[i].arrivalTime);
            printf("---------------------------------------------------------------------------------------------------------------------\n");
        }
    }
    printf("Total buses found: %d\n", count);
}

/* Display 40-seat coach layout in ASCII */
void displaySeatLayout(Bus *bus) {
    printf("\n=========================================================================\n");
    printf(" COACH SEAT MAP (40 SEATS): %s (%s)\n", bus->busName, bus->busNumber);
    printf(" Departure: %s | Slot: %s | Fare: $%.2f\n", bus->departureTime, getSlotName(bus->slot), bus->fare);
    printf(" Legend: [ 05 ] = Available (Green) | [ XX ] = Booked (Grey)\n");
    printf("=========================================================================\n");
    printf("                         [ FRONT WINDSHIELD ]                            \n");
    printf("      [ Entrance Door ]                       [ Driver Cabin: (O) ]      \n");
    printf("-------------------------------------------------------------------------\n");

    /* 10 rows: Left pair (Seat 1, 2) | AISLE | Right pair (Seat 3, 4) */
    for (int r = 1; r <= 10; r++) {
        int s1 = (r - 1) * 4 + 1;
        int s2 = (r - 1) * 4 + 2;
        int s3 = (r - 1) * 4 + 3;
        int s4 = (r - 1) * 4 + 4;

        char str1[10], str2[10], str3[10], str4[10];

        if (bus->seats[s1] == 1) strcpy(str1, "[ XX ]"); else sprintf(str1, "[ %02d ]", s1);
        if (bus->seats[s2] == 1) strcpy(str2, "[ XX ]"); else sprintf(str2, "[ %02d ]", s2);
        if (bus->seats[s3] == 1) strcpy(str3, "[ XX ]"); else sprintf(str3, "[ %02d ]", s3);
        if (bus->seats[s4] == 1) strcpy(str4, "[ XX ]"); else sprintf(str4, "[ %02d ]", s4);

        printf(" Row %02d:   %s  %s      || AISLE ||      %s  %s\n", r, str1, str2, str3, str4);
    }
    printf("-------------------------------------------------------------------------\n");
    printf("                     [ REAR EMERGENCY EXIT (X) ]                         \n");
    printf("=========================================================================\n");
    printf("Available Seats: %d / %d | Booked Seats: %d / %d\n",
           getAvailableSeatCount(bus), TOTAL_SEATS,
           TOTAL_SEATS - getAvailableSeatCount(bus), TOTAL_SEATS);
}

/* Book a Seat on selected bus */
void bookSeat(void) {
    int busId;
    Bus *selectedBus = NULL;

    printf("\n>>> SEAT RESERVATION <<<\n");
    printf("Enter Bus ID to book (e.g., 101, 102, 201, 301, 401): ");
    if (scanf("%d", &busId) != 1) {
        printf("[!] Invalid input.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    selectedBus = findBusById(busId);
    if (selectedBus == NULL) {
        printf("[!] Error: Bus ID %d not found in scheduled fleet!\n", busId);
        return;
    }

    /* Display bus layout */
    displaySeatLayout(selectedBus);

    if (getAvailableSeatCount(selectedBus) == 0) {
        printf("\n[!] Sorry, this bus is completely full!\n");
        return;
    }

    int seatNum;
    printf("\nEnter Seat Number to Reserve (1 - 40): ");
    if (scanf("%d", &seatNum) != 1 || seatNum < 1 || seatNum > TOTAL_SEATS) {
        printf("[!] Invalid seat number! Must be between 1 and 40.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    /* Check availability */
    if (selectedBus->seats[seatNum] == 1) {
        printf("\n[X] Error: Seat #%d is ALREADY BOOKED! Please pick another seat.\n", seatNum);
        return;
    }

    /* Collect Passenger Details */
    Passenger p;
    char travelDate[30];

    printf("\nEnter Travel Date (e.g. 2026-11-25): ");
    fgets(travelDate, sizeof(travelDate), stdin);
    trimNewline(travelDate);
    if (strlen(travelDate) == 0) strcpy(travelDate, "2026-11-25");

    printf("Enter Passenger Full Name: ");
    fgets(p.name, sizeof(p.name), stdin);
    trimNewline(p.name);

    printf("Enter Passenger Age: ");
    if (scanf("%d", &p.age) != 1 || p.age < 1 || p.age > 120) {
        printf("[!] Invalid age entered. Defaulting to 25.\n");
        p.age = 25;
    }
    clearInputBuffer();

    printf("Enter Passenger Gender (Male/Female/Other): ");
    fgets(p.gender, sizeof(p.gender), stdin);
    trimNewline(p.gender);

    printf("Enter 10-Digit Mobile Number: ");
    fgets(p.mobile, sizeof(p.mobile), stdin);
    trimNewline(p.mobile);

    /* Mark seat as booked */
    selectedBus->seats[seatNum] = 1;

    /* Generate Booking Record */
    int randId = 100000 + rand() % 900000;
    Booking b;
    sprintf(b.bookingId, "PTS-%d", randId);
    b.busId = selectedBus->id;
    strcpy(b.busNumber, selectedBus->busNumber);
    strcpy(b.busName, selectedBus->busName);
    sprintf(b.route, "%s -> %s", selectedBus->routeFrom, selectedBus->routeTo);
    strcpy(b.travelDate, travelDate);
    strcpy(b.departureTime, selectedBus->departureTime);
    strcpy(b.timeSlotName, getSlotName(selectedBus->slot));
    b.seatNumber = seatNum;
    b.fare = selectedBus->fare;
    b.passenger = p;
    strcpy(b.status, "Confirmed");

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(b.bookingDate, sizeof(b.bookingDate), "%Y-%m-%d %I:%M %p", t);

    bookingHistory[totalBookings++] = b;

    /* Display Printable Boarding Pass */
    printf("\n");
    printf("****************************************************************************************\n");
    printf("                      OFFICIAL TRANSIT BOARDING PASS / TICKET                           \n");
    printf("****************************************************************************************\n");
    printf(" BOOKING REFERENCE ID : %-15s   STATUS: CONFIRMED (PAID)\n", b.bookingId);
    printf(" Journey Date         : %-15s   Time Slot: %s\n", b.travelDate, b.timeSlotName);
    printf(" Coach Number         : %-15s   Bus Name : %s\n", b.busNumber, b.busName);
    printf(" Route                : %s\n", b.route);
    printf(" Departure Time       : %-15s   Fare Paid: $%.2f\n", b.departureTime, b.fare);
    printf("----------------------------------------------------------------------------------------\n");
    printf(" PASSENGER DETAILS:\n");
    printf(" Name                 : %-25s Age/Gender: %d yrs / %s\n", b.passenger.name, b.passenger.age, b.passenger.gender);
    printf(" Mobile Number        : %-25s Assigned Seat: SEAT #%02d\n", b.passenger.mobile, b.seatNumber);
    printf("----------------------------------------------------------------------------------------\n");
    printf(" BARCODE VERIFICATION : *%s*           [ Valid Transit Document ]\n", b.bookingId);
    printf("****************************************************************************************\n");
    printf("[*] Seat #%d successfully reserved! Ticket printed.\n", seatNum);
}

/* View all bookings or search by ID/Mobile */
void viewBookings(void) {
    char query[40];
    displayHeader();
    printf("\n>>> VIEW / SEARCH BOOKINGS <<<\n");
    printf("Enter Booking ID (e.g. PTS-104921) or Mobile Number (or press Enter to view ALL): ");
    fgets(query, sizeof(query), stdin);
    trimNewline(query);

    printf("\n---------------------------------------------------------------------------------------------------------------------\n");
    printf("%-12s | %-10s | %-20s | %-12s | %-7s | %-11s | %-10s | %-10s\n",
           "Booking ID", "Coach", "Passenger Name", "Mobile", "Seat", "Date", "Slot", "Status");
    printf("---------------------------------------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < totalBookings; i++) {
        Booking *b = &bookingHistory[i];
        int matches = 1;

        if (strlen(query) > 0) {
            if (strstr(b->bookingId, query) == NULL &&
                strstr(b->passenger.mobile, query) == NULL &&
                strstr(b->passenger.name, query) == NULL) {
                matches = 0;
            }
        }

        if (matches) {
            found++;
            printf("%-12s | %-10s | %-20s | %-12s | #%-5d | %-11s | %-10s | %-10s\n",
                   b->bookingId,
                   b->busNumber,
                   b->passenger.name,
                   b->passenger.mobile,
                   b->seatNumber,
                   b->travelDate,
                   b->timeSlotName,
                   b->status);
        }
    }

    if (found == 0) {
        printf("  No booking records match your query '%s'.\n", query);
    }
    printf("---------------------------------------------------------------------------------------------------------------------\n");
    printf("Total records found: %d\n", found);
}

/* Cancel a booking and release the seat back to available pool */
void cancelBooking(void) {
    char targetId[30];
    displayHeader();
    printf("\n>>> CANCEL RESERVATION <<<\n");
    printf("Enter Booking Reference ID to Cancel (e.g., PTS-104921): ");
    fgets(targetId, sizeof(targetId), stdin);
    trimNewline(targetId);

    int foundIndex = -1;
    for (int i = 0; i < totalBookings; i++) {
        if (strcmp(bookingHistory[i].bookingId, targetId) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("\n[!] Error: Booking ID '%s' was not found!\n", targetId);
        return;
    }

    Booking *b = &bookingHistory[foundIndex];

    if (strcmp(b->status, "Cancelled") == 0) {
        printf("\n[!] This booking is ALREADY CANCELLED!\n");
        return;
    }

    printf("\nFound Reservation:\n");
    printf("  Booking ID     : %s\n", b->bookingId);
    printf("  Passenger Name : %s\n", b->passenger.name);
    printf("  Coach Number   : %s (%s)\n", b->busNumber, b->busName);
    printf("  Travel Date    : %s (Time Slot: %s)\n", b->travelDate, b->timeSlotName);
    printf("  Assigned Seat  : Seat #%d\n", b->seatNumber);
    printf("  Total Fare     : $%.2f\n", b->fare);

    printf("\nAre you sure you want to cancel this reservation? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (confirm == 'y' || confirm == 'Y') {
        /* Update booking status */
        strcpy(b->status, "Cancelled");

        /* Release the seat back to open pool */
        Bus *bus = findBusById(b->busId);
        if (bus != NULL) {
            bus->seats[b->seatNumber] = 0;
        }

        printf("\n[OK] Reservation %s successfully CANCELLED!\n", b->bookingId);
        printf("[OK] Seat #%d on Coach %s has been RELEASED and is now available for other passengers.\n",
               b->seatNumber, b->busNumber);
    } else {
        printf("\n[i] Cancellation aborted. Reservation remains Confirmed.\n");
    }
}

/* Admin Dashboard */
void adminDashboard(void) {
    char username[30], password[30];
    displayHeader();
    printf("\n>>> ADMIN PORTAL LOGIN <<<\n");
    printf("Demo Credentials: Username = admin | Password = admin123\n\n");

    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    trimNewline(username);

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    trimNewline(password);

    if (strcmp(username, "admin") != 0 || strcmp(password, "admin123") != 0) {
        printf("\n[!] Authentication failed! Invalid username or password.\n");
        return;
    }

    int totalCapacity = totalBuses * TOTAL_SEATS;
    int bookedSeatsCount = 0;
    int confirmedBookings = 0;
    int cancelledBookings = 0;
    double totalRevenue = 0.0;

    for (int i = 0; i < totalBuses; i++) {
        for (int s = 1; s <= TOTAL_SEATS; s++) {
            if (busFleet[i].seats[s] == 1) {
                bookedSeatsCount++;
            }
        }
    }

    for (int i = 0; i < totalBookings; i++) {
        if (strcmp(bookingHistory[i].status, "Confirmed") == 0) {
            confirmedBookings++;
            totalRevenue += bookingHistory[i].fare;
        } else if (strcmp(bookingHistory[i].status, "Cancelled") == 0) {
            cancelledBookings++;
        }
    }

    printf("\n=========================================================================\n");
    printf("                   ADMINISTRATOR FLEET ANALYTICS DASHBOARD               \n");
    printf("=========================================================================\n");
    printf(" Total Active Buses in Fleet : %d coaches\n", totalBuses);
    printf(" Total Fleet Seat Capacity   : %d seats\n", totalCapacity);
    printf(" Currently Booked Seats      : %d seats\n", bookedSeatsCount);
    printf(" Available Seats for Booking : %d seats\n", totalCapacity - bookedSeatsCount);
    printf(" Overall Fleet Occupancy     : %.1f%%\n", ((double)bookedSeatsCount / totalCapacity) * 100.0);
    printf(" Total Confirmed Bookings    : %d\n", confirmedBookings);
    printf(" Total Cancelled Bookings    : %d (Seats returned to pool)\n", cancelledBookings);
    printf(" Gross Passenger Revenue     : $%.2f\n", totalRevenue);
    printf("=========================================================================\n");

    printf("\nPress Enter to return to main menu...");
    getchar();
}

/* Main Interactive Menu */
int main(void) {
    srand((unsigned int)time(NULL));

    /* Initialize data structures */
    initFleet();
    initSampleBookings();

    int choice = 0;

    while (1) {
        displayHeader();
        printf("\n                 >>> MAIN APPLICATION MENU <<<                 \n");
        printf("  [1] Search Buses by Travel Date & Time Slot (Morning/Afternoon/Evening/Night)\n");
        printf("  [2] View Interactive 40-Seat Coach Layout\n");
        printf("  [3] Reserve a Seat & Generate Boarding Ticket\n");
        printf("  [4] Search / View Booking Records\n");
        printf("  [5] Cancel a Reservation & Release Seat\n");
        printf("  [6] Administrator Analytics Dashboard\n");
        printf("  [7] Exit System\n");
        printf("----------------------------------------------------------------------------------------\n");
        printf("Enter your choice (1-7): ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[!] Invalid input! Please enter a number between 1 and 7.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                searchBusesByDateAndSlot();
                break;

            case 2: {
                printf("\nEnter Bus ID to view (e.g. 101, 102, 201, 301, 401): ");
                int bId;
                if (scanf("%d", &bId) == 1) {
                    clearInputBuffer();
                    Bus *b = findBusById(bId);
                    if (b != NULL) {
                        displaySeatLayout(b);
                    } else {
                        printf("[!] Bus ID not found.\n");
                    }
                } else {
                    clearInputBuffer();
                }
                break;
            }

            case 3:
                bookSeat();
                break;

            case 4:
                viewBookings();
                break;

            case 5:
                cancelBooking();
                break;

            case 6:
                adminDashboard();
                break;

            case 7:
                printf("\nThank you for using SkyRoute Public Transport Reservation System. Goodbye!\n\n");
                exit(0);

            default:
                printf("\n[!] Choice must be between 1 and 7.\n");
                break;
        }

        printf("\nPress Enter to continue...");
        getchar();
    }

    return 0;
}
