#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct Relationship
{
    char target[32];
    int weight;
};

struct Person
{
    char subject[32];
    int relationshipCount;
    struct Relationship relationships[64];
};

struct Person persons[128];

int intParse(char str[], int arrayLength)
{
    // Parse strings to integers using ascii codes
    int parsedInt = 0;
    for (int i = 0; i < arrayLength; i++)
    {
        parsedInt = ((int)str[i] - 48) + (parsedInt * 10);
    }
    return parsedInt;
}

int main()
{
    // Open thrones.csv
    FILE *sourceFile;
    sourceFile = fopen("./thrones.csv", "r");

    // Read the first line and forget about it
    while (1)
    {
        char letter;
        fscanf(sourceFile, "%c", &letter);
        if (letter == '\n')
            break;
    }

    // Amount of persons
    int personCount = 0;
    // Debug variable making the while loop a for loop
    int trial = 0;
    // Breaks the while loop if not 0
    int endOfFileReached = 0;
    // Index of the current relationship within the current person
    int relationshipIndex = 0;
    // Loops once per row in thrones.csv to note sources as persons and targets as relationships
    while (endOfFileReached == 0)
    {
        // The most recently read letter from thrones.csv
        char letter;
        // A string of everything read before the designated break character
        char name[32];
        // Index of the character within name
        int nameIndex = 0;

        // ---------------
        // SOURCE / PERSON
        // ---------------
        // Read until a comma and store it as name
        while (1)
        {
            letter = fgetc(sourceFile);
            if (letter == ',')
            {
                name[nameIndex] = '\0';
                break;
            }
            name[nameIndex] = letter;
            nameIndex++;
        }

        // Index of the current person in persons
        int personIndex = 0;
        // Find where name goes
        while (1)
        {
            // If an empty slot in persons is found write name there
            if (persons[personIndex].subject[0] == '\0')
            {
                for (int i = 0; i < nameIndex; i++)
                {
                    persons[personIndex].subject[i] = name[i];
                }
                // Set the index of relationship within the current person to 0 and raise the amount of persons by 1
                relationshipIndex = 0;
                personCount++;
                break;
            }
            // If name already exists in persons raise the relationship index by 1 in preparation for filling in the next slot with a new relation target
            else if (strcmp(persons[personIndex].subject, name) == 0)
            {
                relationshipIndex++;
                break;
            }
            else
            {
                // Check the next slot of persons
                personIndex++;
            }
        }

        // ------
        // TARGET
        // ------
        // Increase the amount of relationships the current person has
        persons[personIndex].relationshipCount++;
        // Reset the nameIndex to start writing over old data
        nameIndex = 0;
        // Read until a comma and store it as a name
        while (1)
        {
            letter = fgetc(sourceFile);
            if (letter == ',')
            {
                name[nameIndex] = '\0';
                break;
            }
            name[nameIndex] = letter;
            nameIndex++;
        }

        // Write name in the current relationship slot
        for (int i = 0; i < nameIndex; i++)
        {
            persons[personIndex].relationships[relationshipIndex].target[i] = name[i];
        }

        // ------
        // WEIGHT
        // ------
        // Reset the nameIndex to start writing over old data
        nameIndex = 0;
        // Read until a new line and store it as a name
        while (1)
        {
            // If the letter is end of file break the big while loop
            letter = fgetc(sourceFile);
            if (letter == EOF)
            {
                endOfFileReached = 1;
                break;
            }
            if (letter == '\n')
            {
                break;
            }
            name[nameIndex] = letter;
            nameIndex++;
        }

        // Parse name to weight of the current relationship of the current person
        persons[personIndex].relationships[relationshipIndex].weight = intParse(name, nameIndex);
    }

    // -----------------------------------------------------------
    // Add people that only appear in relationships as persons too
    // -----------------------------------------------------------
    // Index of the person in persons
    int personIndex = 0;
    // Which index to stop at
    int oldPersonCount = personCount;
    // Loops through persons
    while (personIndex < oldPersonCount)
    {
        // Loops through relationships of a person
        for (int i = 0; i < persons[personIndex].relationshipCount; i++)
        {
            // Check if the relationship target is also a person
            int personFound = -1;
            for (int j = 0; j < personCount; j++)
            {
                if (strcmp(persons[personIndex].relationships[i].target, persons[j].subject) == 0)
                {
                    personFound = j;
                    break;
                }
            }
            // If not make them a person
            int relationshipFound = 0;
            if (personFound == -1)
            {
                strcpy(persons[personCount].subject, persons[personIndex].relationships[i].target);
                personFound = personCount;
                personCount++;
            }
            else
            {
                // Check for duplicate relationships
                for (int j = 0; j < persons[personFound].relationshipCount; j++)
                {
                    if (strcmp(persons[personIndex].relationships[i].target, persons[personFound].relationships[j].target) == 0)
                    {
                        relationshipFound = 1;
                    }
                }
            }

            if (relationshipFound == 0)
            {
                // Add the current person from the surrounding while loop as a relationship to the new or found person
                strcpy(persons[personFound].relationships[persons[personFound].relationshipCount].target, persons[personIndex].subject);
                persons[personFound].relationships[persons[personFound].relationshipCount].weight = persons[personIndex].relationships[i].weight;
                persons[personFound].relationshipCount++;
            }

            // Debug
            // printf("Write: %s-%s\n", persons[personIndex].subject, persons[personFound].subject);
        }
        personIndex++;
    }

    // -------
    // SORTING
    // -------
    // Find the person who should be last
    char lastPerson[32];
    strcpy(lastPerson, persons[personCount].subject);
    for (int i = personCount - 2; i > 0; i--)
    {
        if (strcmp(lastPerson, persons[i].subject) < 0)
        {
            strcpy(lastPerson, persons[i].subject);
        }
    }

    while (1)
    {
        // Find the index of the first person who comes after the last person in alphabetical order
        int sortedIndex = 0;
        for (int i = 0; i < personCount; i++)
        {
            if (strcmp(persons[(personCount - 1)].subject, persons[i].subject) < 0)
            {
                sortedIndex = i;
                break;
            }
        }

        // Save the last person in a temporary struct
        struct Person movePerson;
        // Copy the name
        strcpy(movePerson.subject, persons[(personCount - 1)].subject);
        // Copy the relationship count
        movePerson.relationshipCount = persons[(personCount - 1)].relationshipCount;
        // Copy all relationships
        for (int i = 0; i < movePerson.relationshipCount; i++)
        {
            strcpy(movePerson.relationships[i].target, persons[(personCount - 1)].relationships[i].target);
            movePerson.relationships[i].weight = persons[(personCount - 1)].relationships[i].weight;
        }

        // From the bottom move all persons down one step until the spot movePerson is going is cleared
        for (int i = (personCount - 1); i > sortedIndex; i--)
        {
            strcpy(persons[i].subject, persons[i - 1].subject);
            persons[i].relationshipCount = persons[i - 1].relationshipCount;
            for (int j = 0; j < persons[i - 1].relationshipCount; j++)
            {
                strcpy(persons[i].relationships[j].target, persons[i - 1].relationships[j].target);
                persons[i].relationships[j].weight = persons[i - 1].relationships[j].weight;
            }
        }

        // Insert move person into their spot
        strcpy(persons[sortedIndex].subject, movePerson.subject);
        persons[sortedIndex].relationshipCount = movePerson.relationshipCount;
        for (int i = 0; i < movePerson.relationshipCount; i++)
        {
            strcpy(persons[sortedIndex].relationships[i].target, movePerson.relationships[i].target);
            persons[sortedIndex].relationships[i].weight = movePerson.relationships[i].weight;
        }

        // Check if the alphabetically last person is last
        if (strcmp(persons[personCount - 1].subject, lastPerson) == 0)
        {
            break;
        }
    }

    // Selection sorts relationships after weight
    // Loops through persons
    for (int i = 0; i < personCount; i++)
    {
        // Loops throught the relationship array slots
        for (int j = 0; j < persons[i].relationshipCount; j++)
        {
            // Find the index of the highest weight
            int indexHighest = j;
            for (int k = j; k < persons[i].relationshipCount; k++)
            {
                if (persons[i].relationships[k].weight > persons[i].relationships[indexHighest].weight)
                    indexHighest = k;
            }
            // Move said relationship to the right index assuming it isn't already there
            if (indexHighest > j)
            {
                // Save the relationship to be moved to a temporary struct
                struct Relationship moveRelationship;
                strcpy(moveRelationship.target, persons[i].relationships[indexHighest].target);
                moveRelationship.weight = persons[i].relationships[indexHighest].weight;
                // Move all relationships down to make way
                for (int k = indexHighest; k > j; k--)
                {
                    strcpy(persons[i].relationships[k].target, persons[i].relationships[k - 1].target);
                    persons[i].relationships[k].weight = persons[i].relationships[k - 1].weight;
                }
                // Reinsert the moved relationship
                strcpy(persons[i].relationships[j].target, moveRelationship.target);
                persons[i].relationships[j].weight = moveRelationship.weight;
            }
        }
    }

    // --------------
    // USER INTERFACE
    // --------------
    char input = ' ';
    personIndex = 0;
    bool relationshipMode = false;
    // Loop until the user wants to quit
    while (input != 'q' && input != 'Q')
    {
        // Display welcome screen
        if (relationshipMode == false)
        {
            puts("\nWelcome to the thrones.csv parser\nOptions:\n(L)ist: show a list of all characters.\n(R)elationships: Toggle relationship mode.\n(Q)uit: Quits the program.");
        }
        else
        {
            // Display 5 people with a cursor on the middle one
            for (int i = -2; i < 3; i++)
            {
                printf
                (
                    "%c%s\n",
                       i == 0 ? '>' : ' ',
                       personIndex + i >= 0 ? (personIndex + i < personCount ? persons[personIndex + i].subject : "") : ""
                );
            }
        }
        //Scan for inputs
        scanf("%c", &input);
        fflush(stdin);

        if (relationshipMode)
        {
            // List relationships for the given character
            if (input == 'l' || input == 'L')
            {
                printf("Relationships for %s\n", persons[personIndex].subject);

                for (int i = 0; i < persons[personIndex].relationshipCount; i++)
                {
                    printf("%d %s\n", persons[personIndex].relationships[i].weight, persons[personIndex].relationships[i].target);
                }
                puts("\n");
            }
            else
            {
                // Move up the list
                if ((input == 'b' || input == 'B') && personIndex > 0)
                    personIndex--;
                else
                {
                    // Move down the list
                    if (personIndex < personCount - 1)
                        personIndex++;
                }
            }
        }
        else
        {
            // List all people in alphabetical order
            if (input == 'l' || input == 'L')
            {
                for (int i = 0; i < personCount; i++)
                {
                    printf("%s\n", persons[i].subject);
                }
            }
        }

        // Toggle relationship mode and display help screen if relationship mode was turned on
        if (input == 'r' || input == 'R')
        {
            relationshipMode = !relationshipMode;
            if (relationshipMode)
            {
                puts("\nRelationship mode toggled.\nOptions:\n(R)elationships: Toggle relationship mode.\n(Enter): Move down the list.\n(B)ack: go back upwards in the list.\n(L)ist: List the relationships for the current character.\n(Q)uit: Quit the program.\n");
            }
        }
    }

    // Debug
    // Using the amount of perople and amount of relationships print everything
    /* for (int a = 0; a < personCount; a++)
    {
        printf("%s---------------\n", persons[a].subject);
        for (int b = 0; b < persons[a].relationshipCount; b++)
        {
            printf("%s %d\n", persons[a].relationships[b].target, persons[a].relationships[b].weight);
        }
    } */
    return 0;
}