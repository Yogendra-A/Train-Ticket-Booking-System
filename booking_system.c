#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define FILENAME "data.txt"
#define TRAIN_FILE "trains.txt"
#define ADMIN_PASS "admin123"

struct Ticket {
    int ticket_id;
    char passenger_name[MAX_NAME];
    int train_number;
    char date[20];
    int seats_booked;
};

struct Train {
    int train_number;
    char train_name[50];
    int seats_available;
};

void book_ticket();
void cancel_ticket();
void admin_panel();
void view_bookings();
void add_train();
void delete_train();
void view_trains();
int generate_ticket_id();
int check_train_availability(int train_number, int requested_seats);
void update_train_seats(int train_number, int change);

int main() {
    int choice;
    while (1) {
        printf("\n🚆 Train Ticket Booking System\n");
        printf("1. Book Ticket\n");
        printf("2. Cancel Ticket\n");
        printf("3. View Available Trains\n");
        printf("4. Admin Login\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: book_ticket(); break;
            case 2: cancel_ticket(); break;
            case 3: view_trains(); break;
            case 4: admin_panel(); break;
            case 5: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}

void book_ticket() {
    struct Ticket t;
    FILE *fp = fopen(FILENAME, "a");
    if (!fp) { printf("\nError opening file!\n"); return; }

    t.ticket_id = generate_ticket_id();

    printf("\nEnter Passenger Name: ");
    scanf("%s", t.passenger_name);
    printf("Enter Train Number: ");
    scanf("%d", &t.train_number);
    printf("Enter Travel Date (DD-MM-YYYY): ");
    scanf("%s", t.date);
    printf("Enter Number of Seats to Book: ");
    scanf("%d", &t.seats_booked);

    if (!check_train_availability(t.train_number, t.seats_booked)) {
        printf("\nTrain not available or not enough seats.\n");
        fclose(fp);
        return;
    }

    fprintf(fp, "%d %s %d %s %d\n", t.ticket_id, t.passenger_name, t.train_number, t.date, t.seats_booked);
    fclose(fp);

    update_train_seats(t.train_number, -t.seats_booked);
    printf("\n🎫 Ticket booked successfully! Your Ticket ID is: %d\n", t.ticket_id);
}

int check_train_availability(int train_number, int requested_seats) {
    struct Train tr;
    FILE *fp = fopen(TRAIN_FILE, "r");
    while (fp && fscanf(fp, "%d %s %d", &tr.train_number, tr.train_name, &tr.seats_available) != EOF) {
        if (tr.train_number == train_number && tr.seats_available >= requested_seats) {
            fclose(fp);
            return 1;
        }
    }
    if (fp) fclose(fp);
    return 0;
}

void update_train_seats(int train_number, int change) {
    struct Train tr[100];
    int n = 0;
    FILE *fp = fopen(TRAIN_FILE, "r");
    while (fp && fscanf(fp, "%d %s %d", &tr[n].train_number, tr[n].train_name, &tr[n].seats_available) != EOF) {
        n++;
    }
    if (fp) fclose(fp);

    for (int i = 0; i < n; i++) {
        if (tr[i].train_number == train_number) {
            tr[i].seats_available += change;
            if (tr[i].seats_available < 0)
                tr[i].seats_available = 0;
        }
    }

    fp = fopen(TRAIN_FILE, "w");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d %s %d\n", tr[i].train_number, tr[i].train_name, tr[i].seats_available);
    }
    fclose(fp);
}

int generate_ticket_id() {
    struct Ticket t;
    int max_id = 1000;
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) return max_id;

    while (fscanf(fp, "%d %s %d %s %d", &t.ticket_id, t.passenger_name, &t.train_number, t.date, &t.seats_booked) != EOF) {
        if (t.ticket_id >= max_id) max_id = t.ticket_id + 1;
    }
    fclose(fp);
    return max_id;
}

void cancel_ticket() {
    int id, found = 0;
    struct Ticket t;
    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    printf("Enter Ticket ID to cancel: ");
    scanf("%d", &id);

    while (fp && fscanf(fp, "%d %s %d %s %d", &t.ticket_id, t.passenger_name, &t.train_number, t.date, &t.seats_booked) != EOF) {
        if (t.ticket_id == id) {
            found = 1;
            update_train_seats(t.train_number, t.seats_booked);
            continue;
        }
        fprintf(temp, "%d %s %d %s %d\n", t.ticket_id, t.passenger_name, t.train_number, t.date, t.seats_booked);
    }

    if (fp) fclose(fp);
    fclose(temp);
    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("Ticket cancelled successfully.\n");
    else
        printf("Ticket ID not found.\n");
}

void view_trains() {
    struct Train tr;
    FILE *fp = fopen(TRAIN_FILE, "r");
    if (!fp) { printf("No trains found.\n"); return; }

    printf("\n🚉 Available Trains:\n");
    printf("---------------------------------------------\n");
    while (fscanf(fp, "%d %s %d", &tr.train_number, tr.train_name, &tr.seats_available) != EOF) {
        printf("Train No: %d | Name: %s | Seats: %d\n", tr.train_number, tr.train_name, tr.seats_available);
    }
    fclose(fp);
}

void view_bookings() {
    struct Ticket t;
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) { printf("No bookings found.\n"); return; }

    printf("\n📋 All Bookings:\n");
    printf("---------------------------------------------\n");
    while (fscanf(fp, "%d %s %d %s %d", &t.ticket_id, t.passenger_name, &t.train_number, t.date, &t.seats_booked) != EOF) {
        printf("ID: %d | Name: %s | Train: %d | Date: %s | Seats: %d\n",
               t.ticket_id, t.passenger_name, t.train_number, t.date, t.seats_booked);
    }
    fclose(fp);
}

void add_train() {
    struct Train tr;
    FILE *fp = fopen(TRAIN_FILE, "a");
    if (!fp) { printf("\nError opening train file!\n"); return; }

    printf("Enter Train Number: ");
    scanf("%d", &tr.train_number);
    printf("Enter Train Name: ");
    scanf("%s", tr.train_name);
    printf("Enter Total Seats Available: ");
    scanf("%d", &tr.seats_available);

    fprintf(fp, "%d %s %d\n", tr.train_number, tr.train_name, tr.seats_available);
    fclose(fp);
    printf("Train added successfully!\n");
}

void delete_train() {
    int number, found = 0;
    struct Train tr;
    FILE *fp = fopen(TRAIN_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");
    printf("Enter Train Number to delete: ");
    scanf("%d", &number);

    while (fp && fscanf(fp, "%d %s %d", &tr.train_number, tr.train_name, &tr.seats_available) != EOF) {
        if (tr.train_number == number) {
            found = 1;
            continue;
        }
        fprintf(temp, "%d %s %d\n", tr.train_number, tr.train_name, tr.seats_available);
    }

    if (fp) fclose(fp);
    fclose(temp);
    remove(TRAIN_FILE);
    rename("temp.txt", TRAIN_FILE);

    if (found)
        printf("Train deleted successfully.\n");
    else
        printf("Train not found.\n");
}

void admin_panel() {
    char pass[20];
    printf("\nEnter Admin Password: ");
    scanf("%s", pass);
    if (strcmp(pass, ADMIN_PASS) != 0) {
        printf("Incorrect password.\n");
        return;
    }

    int choice;
    do {
        printf("\nAdmin Panel\n");
        printf("1. View Bookings\n2. Add Train\n3. View Trains\n4. Delete Train\n5. Exit Admin Panel\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: view_bookings(); break;
            case 2: add_train(); break;
            case 3: view_trains(); break;
            case 4: delete_train(); break;
            case 5: printf("Exiting admin panel.\n"); break;
            default: printf("Invalid choice!\n"); break;
        }
    } while (choice != 5);
}

