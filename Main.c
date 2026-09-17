#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Opretter struct
struct event
{
    char title[100], catagory[100], alcohol[100], location[100];
    int price, time;
};
typedef struct event event;

// prototyper
void get_events(event all_events[]);
void read_all_events(FILE *f, event all_events[], int length);
int count_events(event all_events[]);
int sort_time(const void *p1, const void *p2);
void print_all_events(event all_events[], int length);
void print_event(event e);
void do_task(char option, event all_events[], int *length);
void scan_input(char *option);
void sort_events(event all_events[], int length);
void sort_by_alcohol(event all_events[], int length);
int sort_price_low_to_high(const void *p1, const void *p2);
int sort_price_high_to_low(const void *p1, const void *p2);
void sort_events_by_price(event all_events[], int length);
void add_new_events(event all_events[], int *length);
void search_specific_events(event all_events[], int length);

int main(void)
{
    // Denne kode søger for at vi får læst events fra text fil ind i array af structs
    event all_events[500];
    int events;
    get_events(all_events);
    // vi tæller hvor mange events vi fik læst ind fra filen
    int length = count_events(all_events);
    // vi sorterer de indlæste events baseret på time
    qsort(all_events, length, sizeof(event), sort_time);
    // vi printer den nu tidssorteret liste
    print_all_events(all_events, length);

    // Denne kode udgør vores menu, som sluttes når bruger inputter "q"
    char option;
    do
    {
        scan_input(&option);

        do_task(option, all_events, &length);

    } while (option != 'q');

    return 0;
}

// Funktionen åbner filen "Event.txt" fil og tjekker om den er åbnet korrekt
void get_events(event all_events[])
{
    FILE *f = fopen("Events_database.txt", "r");
    if (f == NULL)
    {
        printf("Failed to open file\n");
        exit(EXIT_FAILURE);
    }
    read_all_events(f, all_events, 500);
    fclose(f);
}

// Funktionen skriver alle evnets fra filen over i all_events arrayet.
void read_all_events(FILE *f, event all_events[], int length)
{
    char header[256];
    fgets(header, sizeof(header), f);
    for (int i = 0; i < length; i++)
    {
        event e;
        int success = fscanf(f, "%s %s %s %s %d %d", e.title, e.catagory, e.alcohol, e.location, &e.price, &e.time);
        if (success != 6)
        {
            break;
        }
        all_events[i] = e;
    }
}

// Funktionen tilføjer et nyt event som brugeren kan skrive ind,
// og den opdatere så automatisk databasen med de nye inputs
void add_new_events(event all_events[], int *length)
{
    event new_event;
    printf("Enter title: \n");
    scanf("%s", new_event.title);
    printf("Enter catagory: \n");
    scanf("%s", new_event.catagory);
    printf("Enter alcohol: \n");
    scanf("%s", new_event.alcohol);
    printf("Enter location: \n");
    scanf("%s", new_event.location);
    printf("Enter price: \n");
    scanf(" %d", &new_event.price);
    printf("Enter time: \n");
    scanf(" %d", &new_event.time);

    all_events[*length] = new_event;
    (*length)++;

    qsort(all_events, *length, sizeof(event), sort_time);

    FILE *f = fopen("Events_database.txt", "a");
    if (f == NULL)
    {
        printf("Failed to open file\n");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "%s %s %s %s %d %d \n",
            new_event.title,
            new_event.catagory,
            new_event.alcohol,
            new_event.location,
            new_event.price,
            new_event.time);
    fclose(f);

    printf("Event is succesfully added! \n");
}

// prints out from a different txt file different clubs that exists in AAU
void show_clubs(event all_events[])
{
    FILE *f = fopen("Social_communities.txt", "r");

    if (f == NULL)
    {
        printf("Failed to open file\n");
        exit(EXIT_FAILURE);
    }
    int line = 1;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), f) != NULL)
    {
        printf("%d %s \n", line, buffer);
        ++line;
    }
    printf("\n");
    fclose(f);
}

// Funktionen tæller hvor mange events vi har ved at checke om time er nul. time er nul, hvis der ikke er scannet noget ind.
int count_events(event all_events[])
{
    int i;
    for (i = 0; i < 500; i++)
    {
        if (all_events[i].time == 0)
        {
            break;
        }
    }
    return i;
}

// Denne funktion sammenligner 2 events time og retunerer hvilke der er først.
int sort_time(const void *p1, const void *p2)
{
    const struct event *e1 = p1;
    const struct event *e2 = p2;

    // disse tre linjer gør at vi får en int som indeholder dagens dato, på samme måde som vores events dato står på
    time_t time_today = time(NULL);

    struct tm today_date = *localtime(&time_today);

    int date_today_int = (today_date.tm_year + 1900) * 10000 + (today_date.tm_mon + 1) * 100 + today_date.tm_mday;

    // Hvis e1 er nutid og e2 datid, så skal e1 først
    if (date_today_int <= e1->time && date_today_int > e2->time)
    {
        return 1;
    }

    // Hvis e1 er datid og e2 er nutid, så skal e1 bagerest.
    if (date_today_int > e1->time && date_today_int <= e2->time)
    {
        return -1;
    }

    // Hvis begge datoer er efter i dag, så vil vi gerne have den mindste af de to først, da den et tættest på dagens dato
    if (date_today_int <= e1->time && date_today_int <= e2->time)
    {
        if (e1->time < e2->time)
        {
            return 1;
        }
        if (e1->time > e2->time)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    // Hvis begge datoer er datid, så vil vi gerne have den den største af de to først, da den er tættest på dagens dato
    if (date_today_int > e1->time && date_today_int > e2->time)
    {
        if (e1->time > e2->time)
        {
            return 1;
        }
        if (e1->time < e2->time)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

// Funktionen printer alle events fra all_events arrayet.
void print_all_events(event all_events[], int length)
{
    for (int i = 0; i < length; i++)
    {
        print_event(all_events[i]);
    }
}

// Funktionen printer et eventsf
void print_event(event e)
{
    printf("\nTitle:     %s\n", e.title);
    printf("Catagory:  %s\n", e.catagory);
    printf("Alchohol?: %s\n", e.alcohol);
    printf("Location:  %s\n", e.location);
    printf("Price:     %dkr\n", e.price);
    printf("Time:      %d\n\n", e.time);
}

// Funktionen får input fra bruger til menu
// Funktionen får input fra bruger til menu's
void scan_input(char *option)
{
    printf("Choose from menu:\n");
    printf("c: Sort events by catagory\n");
    printf("a: Sort events by alcohol\n");
    printf("p: Sort events by price\n");
    printf("t: Search events, by title\n");
    printf("s: See social clubs\n");
    printf("n: Add new event\n");
    printf("q: Exit menu\n");
    scanf(" %c", option);
}

// Funktionen udfører det som brugeren har valgt fra menu
void do_task(char option, event all_events[], int *length)
{
    switch (option)
    {
    case 'c':
        sort_events(all_events, *length);
        break;
    case 'p':
        sort_events_by_price(all_events, *length);
        break;
    case 't':
        search_specific_events(all_events, *length);
        break;
    case 'a':
        sort_by_alcohol(all_events, *length);
        break;
    case 's':
        show_clubs(all_events);
        break;
    case 'n':
        add_new_events(all_events, length);
        break;
    case 'q':
        break;
    }
}

// Funktionen printer de events, som brugere sorterer efter, både "bar" og "Bar" er gyldige input.
void sort_events(event all_events[], int length)
{
    printf("What categories would you like to see?\n");
    printf("Career, Academic, Party, Gaming, Social, Culture, Sport, Tech\n");

    char input[100];
    scanf(" %[^\n]", input);

    // Split input i flere ord
    char *token = strtok(input, " ");

    while (token != NULL)
    {
        token[0] = toupper((unsigned char)token[0]); // gør første bogstav stort

        // Søger efter alle events der matcher token
        for (int i = 0; i < length; i++)
        {
            if (strcmp(all_events[i].catagory, token) == 0)
            {
                print_event(all_events[i]);
            }
        }

        token = strtok(NULL, " ");
    }
}

void sort_events_by_price(event all_events[], int length)
{
    event copy_all_events[500];
    for (int i = 0; i < length; i++)
    {
        copy_all_events[i] = all_events[i];
    }
    int sub_choice_price;
    int valid_choice = 0;

    do
    {
        printf("\n Price Menu \n");
        printf("1: Show all (Lowest price first)\n");
        printf("2: Show all (Highest price first)\n");
        printf("3: Search for specific price range\n");
        printf("Choose option: ");
        scanf("%d", &sub_choice_price);

        if (sub_choice_price == 1)
        {
            valid_choice = 1;
            qsort(copy_all_events, length, sizeof(event), sort_price_low_to_high);
            print_all_events(copy_all_events, length);
        }
        else if (sub_choice_price == 2)
        {
            valid_choice = 1;
            qsort(copy_all_events, length, sizeof(event), sort_price_high_to_low);
            print_all_events(copy_all_events, length);
        }
        else if (sub_choice_price == 3)
        {
            int lowest_price;
            int highest_price;
            valid_choice = 1;

            do
            {
                printf("What price range do you want to see? \n");
                printf("Enter the lowest price you are willing to pay\n");
                scanf("%d", &lowest_price);
                printf(" \n");
                printf("Enter the highest price you are willing to pay\n");
                scanf("%d", &highest_price);

                if (lowest_price > highest_price)
                {
                    printf("\nError: The lowest price %d is higher than the highest price %d. The lowest price must be less or equal to the highest price.", lowest_price, highest_price);
                    printf("Please try again\n");
                }

            } while (lowest_price > highest_price);

            printf("Here are the events in the price range of %d to %d\n", lowest_price, highest_price);

            int found = 0;
            for (int i = 0; i < length; i++)
            {
                if (all_events[i].price >= lowest_price && all_events[i].price <= highest_price)
                {
                    print_event(all_events[i]);
                    found = 1;
                }
            }

            if (found == 0)
            {
                printf("No events have been found\n");
            }
        }
        else
        {
            printf("\nError:Invalid choice\n");
            printf("Please choose 1, 2 or 3");
        }
    } while (valid_choice == 0);
}
int sort_price_high_to_low(const void *p1, const void *p2)
{
    const struct event *e1 = p1;
    const struct event *e2 = p2;

    if (e1->price < e2->price)
        return -1;
    if (e1->price > e2->price)
        return 1;

    // disse tre linjer gør at vi får en int som indeholder dagens dato, på samme måde som vores events dato står på
    time_t time_today = time(NULL);

    struct tm today_date = *localtime(&time_today);

    int date_today_int = (today_date.tm_year + 1900) * 10000 + (today_date.tm_mon + 1) * 100 + today_date.tm_mday;

    // Hvis e1 er nutid og e2 datid, så skal e1 først
    if (date_today_int <= e1->time && date_today_int > e2->time)
    {
        return 1;
    }

    // Hvis e1 er datid og e2 er nutid, så skal e1 bagerest.
    if (date_today_int > e1->time && date_today_int <= e2->time)
    {
        return -1;
    }

    // Hvis begge datoer er efter i dag, så vil vi gerne have den mindste af de to først, da den et tættest på dagens dato
    if (date_today_int <= e1->time && date_today_int <= e2->time)
    {
        if (e1->time < e2->time)
        {
            return 1;
        }
        if (e1->time > e2->time)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    // Hvis begge datoer er datid, så vil vi gerne have den den største af de to først, da den er tættest på dagens dato
    if (date_today_int > e1->time && date_today_int > e2->time)
    {
        if (e1->time > e2->time)
        {
            return 1;
        }
        if (e1->time < e2->time)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}
int sort_price_low_to_high(const void *p1, const void *p2)
{
    const struct event *e1 = p1;
    const struct event *e2 = p2;

    if (e1->price > e2->price)
        return -1;
    if (e1->price < e2->price)
        return 1;

    // disse tre linjer gør at vi får en int som indeholder dagens dato, på samme måde som vores events dato står på
    time_t time_today = time(NULL);

    struct tm today_date = *localtime(&time_today);

    int date_today_int = (today_date.tm_year + 1900) * 10000 + (today_date.tm_mon + 1) * 100 + today_date.tm_mday;

    // Hvis e1 er nutid og e2 datid, så skal e1 først
    if (date_today_int <= e1->time && date_today_int > e2->time)
    {
        return 1;
    }

    // Hvis e1 er datid og e2 er nutid, så skal e1 bagerest.
    if (date_today_int > e1->time && date_today_int <= e2->time)
    {
        return -1;
    }

    // Hvis begge datoer er efter i dag, så vil vi gerne have den mindste af de to først, da den et tættest på dagens dato
    if (date_today_int <= e1->time && date_today_int <= e2->time)
    {
        if (e1->time < e2->time)
        {
            return 1;
        }
        if (e1->time > e2->time)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    // Hvis begge datoer er datid, så vil vi gerne have den den største af de to først, da den er tættest på dagens dato
    if (date_today_int > e1->time && date_today_int > e2->time)
    {
        if (e1->time > e2->time)
        {
            return 1;
        }
        if (e1->time < e2->time)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

// sorterer efter om der er alkohol til events, ved at scanne efter char alkohol
void sort_by_alcohol(event all_events[], int length)
{
    printf("Show events with alcohol? (Yes / No / Optional)\n");

    char choice[100];
    scanf("%s", choice);

    // Gør første bogstav stort for at matche filens dataformat
    choice[0] = toupper((unsigned char)choice[0]);

    printf("\nEvents with alcohol status: %s\n", choice);

    int found = 0;

    for (int i = 0; i < length; i++)
    {
        if (strcmp(all_events[i].alcohol, choice) == 0)
        {
            print_event(all_events[i]);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No events found with alcohol = %s\n", choice);
    }
}

void search_specific_events(event all_events[], int length)
{
    // får et ord fra brugeren og gemmer det som 'search'
    printf("Search word: ");
    char search[100];
    scanf("%s", search);

    // laver alt om til lowercase i sogeordet
    for (int i = 0; search[i]; i++)
    {
        search[i] = tolower(search[i]);
    }

    // laver 2 tomme arrays. 1 for alt der matcher og 1 for alt der ikke gor
    event non_matches[500];
    event matches[500];
    int nm_count = 0;
    int m_count = 0;

    // laver et for loop som undersoger alle events om de matcher (i lowercase).
    for (int i = 0; i < length; i++)
    {
        char title_low[100];

        // kopirer titlen (titel_low) og laver den om til en lowercase version (nemmere at soge på)
        for (int k = 0; all_events[i].title[k]; k++)
        {
            title_low[k] = tolower(all_events[i].title[k]);
            title_low[strlen(all_events[i].title)] = '\0';
        }

        // tjekker hvilke matches der er og laver om til en pointer hvis ordet findes i en titel
        if (strstr(title_low, search) != NULL)
        {
            // hvis der er match gemmes det i det tomme array m_count ellers gemmes i nm_count (nm_count)
            matches[m_count++] = all_events[i];
        }
        else
        {
            non_matches[nm_count++] = all_events[i];
        }
    }

    printf("\nEvents with \"%s\":\n\n", search);

    // først printer den alt det som IKKE matcher og ligger det nederst (som er overst)
    for (int i = 0; i < nm_count; i++)
    {
        print_event(non_matches[i]);
    }

    // så printer den det der matcher og ligger det overst (som så er nederst)
    for (int i = 0; i < m_count; i++)
    {
        print_event(matches[i]);
    }
}