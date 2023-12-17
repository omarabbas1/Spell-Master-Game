#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

// Function to check if a word is in the list of spells
bool spellProvided(char *spells[], int spellsSize, char word[])
{
    for (int i = 0; i < spellsSize; i++)
    {
        int comp = strcmp(word, spells[i]);
        if (comp == 0)
            return true;
    }
    return false;
}

// Function to check if a word has already been used
bool spellAlreadyUsed(char *usedWords[], int spellsSize, char usedWord[])
{
    for (int i = 0; i < spellsSize; i++)
    {
        int comp = strcmp(usedWord, usedWords[i]);
        if (comp == 0)
            return true;
    }
    return false;
}

// Function to check if the last character of the previous word matches the first character of the current word
bool charMatching(char previousWord[], char currentWord[])
{
    size_t len = strlen(previousWord);
    char lastChar = tolower(previousWord[len - 1]);
    char firstChar = tolower(currentWord[0]);
    return lastChar == firstChar;
}

// Function to check if a spell has run out of uses
bool runOut(char currentWord[], int Tally[])
{
    size_t len = strlen(currentWord);
    char lastLetter = tolower(currentWord[len - 1]);
    int Index = lastLetter - 97;
    if (Tally[Index] <= 0)
        return false;
    else
        return true;
}

// Return a word that will finish the game.
char *finishGame(char *spells[], int spellsSize, char *usedWords[], char userWord[], bool BotTurn, int Tally[])
{
    char *availableWords[100];
    for (int i = 0; i < 100; ++i)
    {
        availableWords[i] = NULL;
    }
    int index = 0;
    if (BotTurn == false)
    {
        for (int i = 0; i < spellsSize; i++)
        {
            if (charMatching(userWord, spells[i]) == true && spellAlreadyUsed(usedWords, spellsSize, spells[i]) == false)
            {
                availableWords[index] = spells[i];
                index++;
            }
        }
        for (int i = 0; i < index; i++)
        {
            size_t len = strlen(availableWords[i]);
            char lastCharacter = availableWords[i][len - 1];
            int tallyIndex = lastCharacter - 97;
            if (Tally[tallyIndex] == 0)
                return availableWords[i];
        }
    }
    else
    {
        for (int i = 0; i < spellsSize; i++)
        {
            size_t len = strlen(spells[i]);
            char lastLetter = spells[i][len - 1];
            if (Tally[lastLetter - 97] == 0)
                return spells[i];
        }
    }
    return NULL;
}

// Check if the goodAvailableWord is also the in the badAvailableWordArray
bool wordAsess(char *badAivalableBotWords[], char goodAvailableWord[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(badAivalableBotWords[i], goodAvailableWord) == 0)
            return true;
    }
    return false;
}

char *botMove(char *spells[], int spellsSize, char *usedWords[], char userWord[], int Tally[])
{
    char *finishingWord = finishGame(spells, spellsSize, usedWords, userWord, false, Tally);
    if (finishingWord != NULL)
    {
        return finishingWord;
    }

    char *goodAvailableBotWords[100];
    for (int i = 0; i < 100; ++i)
    {
        goodAvailableBotWords[i] = NULL;
    }

    char *badAvailableBotWords[100];
    for (int i = 0; i < 100; i++)
    {
        badAvailableBotWords[i] = NULL;
    }

    int indexOfbadAvailableBotWords = 0;
    int index = 0; // index of goodAvailableBotWords

    for (int i = 0; i < spellsSize; i++)
    {

        if (charMatching(userWord, spells[i]) && !spellAlreadyUsed(usedWords, spellsSize, spells[i]))
        {
            goodAvailableBotWords[index++] = spells[i];
        }
    }

    double proba[78];
    for (int i = 0; i < 78; i++)
    {
        proba[i] = 0;
    }
    double probaCounter = 0;

    for (int i = 0; i < index; i++)
    { 
        size_t len = strlen(goodAvailableBotWords[i]);
        char lastCharacter = goodAvailableBotWords[i][len - 1];
        char *userPossibleWords[78];
        Tally[lastCharacter - 'a']--;
        usedWords[spellsSize - 1] = strdup(goodAvailableBotWords[i]);
        int indexOfUser = 0;
        bool badWord = true;
        for (int j = 0; j < 78; j++)
        {

            if (charMatching(goodAvailableBotWords[i], spells[j]) && !spellAlreadyUsed(usedWords, spellsSize, spells[j]))
            {
                userPossibleWords[indexOfUser] = spells[j];
                size_t len = strlen(userPossibleWords[indexOfUser]);
                char lastCharacter = userPossibleWords[indexOfUser][len - 1];
                if (Tally[lastCharacter - 'a'] == 0 && badWord == true)
                {
                    badAvailableBotWords[indexOfbadAvailableBotWords++] = goodAvailableBotWords[i];
                    badWord = false;
                }
                indexOfUser++;
            }
        }
        bool Asess = wordAsess(badAvailableBotWords, goodAvailableBotWords[i], indexOfbadAvailableBotWords);
        probaCounter = 0;
        if (Asess == false)
        {
            for (int k = 0; k < indexOfUser; k++)
            {
                usedWords[spellsSize - 2] = strdup(userPossibleWords[k]);
                size_t len = strlen(userPossibleWords[k]);
                char lastCharacter = userPossibleWords[k][len - 1];
                Tally[lastCharacter - 'a']--;

                char *PossibleFinishingWord = finishGame(spells, spellsSize, usedWords, userPossibleWords[k], false, Tally);

                if (PossibleFinishingWord != NULL)
                {
                    probaCounter++;
                }

                Tally[lastCharacter - 'a']++;
                free(usedWords[spellsSize - 2]);
            }
        }
        proba[i] = (indexOfUser) == 0 ? 0 : probaCounter / (indexOfUser);
        Tally[lastCharacter - 'a']++;
        free(usedWords[spellsSize - 1]);
    }

    double max = 0;
    int winningWordIndex = -1;

    for (int i = 0; i < 78; i++)
    {
        if (proba[i] > max)
        {
            max = proba[i];
            winningWordIndex = i;
        }
    }
    if (winningWordIndex >= 0)
    {
        return goodAvailableBotWords[winningWordIndex];
    }

    for (int i = 0; i < indexOfbadAvailableBotWords; i++)
    {
        size_t len = strlen(badAvailableBotWords[i]);
        char lastLetter = badAvailableBotWords[i][len - 1];
        Tally[lastLetter - 'a']--;
        usedWords[spellsSize - 1] = strdup(badAvailableBotWords[i]);
        double wordCounter = 0;
        probaCounter = 0;
        for (int j = 0; j < 78; j++)
        {
            if (charMatching(badAvailableBotWords[i], spells[j]) && !spellAlreadyUsed(usedWords, spellsSize, spells[j]))
            {
                size_t len = strlen(spells[j]);
                char lastLetter = spells[j][len - 1];
                if (Tally[lastLetter - 'a'] == 0)
                {
                    probaCounter++;
                }
                wordCounter++;
            }
        }
        proba[i] = (wordCounter) == 0 ? 0 : probaCounter / (wordCounter);
        Tally[lastLetter - 'a']++;
        free(usedWords[spellsSize - 1]);
    }
    double min = 1;
    int bestMoveIndex = -1;
    for (int i = 0; i < indexOfbadAvailableBotWords; i++)
    {
        if (proba[i] < min)
        {
            min = proba[i];
            bestMoveIndex = i;
        }
    }
    return badAvailableBotWords[bestMoveIndex];
}

void ReadingFileIntoString(FILE *filePointer, char *spells[], size_t spellsLength, char size[], size_t sizeArray)
{

    if (filePointer == NULL)
    {
        printf("File did not open \n");
        return;
    }

    size_t i = 0;

    // Read spell names into the spells array
    while (i < spellsLength && fgets(size, sizeArray, filePointer))
    {
        size_t len = strlen(size); // Check for lines that are too long
        if (len > 0 && size[len - 1] == '\n')
        {
            size[len - 1] = '\0'; // Remove the newline character
        }
        spells[i] = strdup(size);
        i++;
    }
    printf("Done copying file into List.\n");
}

void PrintingList(char *spells[], size_t spellsSize)
{

    for (size_t i = 0; i < spellsSize; i++)
    {
        if (i % 5 != 0)
        {
            printf("%s\t", spells[i]);
        }
        else
        {
            printf("\n");
            printf("%s\t", spells[i]);
        }
    }
    printf("\n\n");
}

bool Player1Turn(char player1[], char player2[])
{

    srand(time(NULL));
    int startingPlayer = rand() % 2;

    if (startingPlayer == 0)
    {
        printf("%s will start the game\n\n", player1);
        return true;
    }
    printf("%s will start the game\n\n", player2);
    return false;
}

int GamePlay(int * indexPtr, char *usedWords[], int Tally[], bool player1Turn, char player1[], char player2[], char *spells[])
{

    bool gameInProgress = true;

    while (gameInProgress)
    {
        if (*indexPtr != 0)
        { // After the 1st Input we should always check if there is sufficient number of words to continue the game.
            if (runOut(usedWords[*indexPtr - 1], Tally) == false)
                break;
        }
        if (player1Turn)
        {
            printf("%s pick a spell from the following List: ", player1);
            scanf("%s", usedWords[*indexPtr]);
            player1Turn = false;
        }
        else
        {
            if (*indexPtr == 0)
            {
                usedWords[*indexPtr] = finishGame(spells, 78, usedWords, NULL, true, Tally);
                printf("%s will pick a spell from the following List: %s\n", player2, usedWords[*indexPtr]);
                player1Turn = true;
            }
            else
            {
                usedWords[*indexPtr] = botMove(spells, 78, usedWords, usedWords[*indexPtr - 1], Tally);
                printf("%s will pick a spell from the following List: %s\n", player2, usedWords[*indexPtr]);
                player1Turn = true;
                usedWords[77] = "";
                usedWords[76] = "";
            }
        }

        char firstLetter = usedWords[*indexPtr][0];
        int value = firstLetter - 97;
        Tally[value]--;
        (*indexPtr)++;

        if (*indexPtr == 1) // At the first iteration we have Only 1 word to avoid calling the function charNotMatching
            gameInProgress = spellProvided(spells, 78, usedWords[*indexPtr - 1]) && !spellAlreadyUsed(usedWords, *indexPtr - 1, usedWords[*indexPtr - 1]);
        else
            gameInProgress = spellProvided(spells, 78, usedWords[*indexPtr - 1]) && !spellAlreadyUsed(usedWords, *indexPtr - 1, usedWords[*indexPtr - 1]) && charMatching(usedWords[*indexPtr - 2], usedWords[*indexPtr - 1]);
    }

    return player1Turn;
}

void AnnouncingWinner(bool player1Turn, char *usedWords[], int index, char *spells[], char player1[], char player2[], int Tally[])
{

    char winner[100];
    char loser[100];

    if (player1Turn && (spellAlreadyUsed(usedWords, 78, usedWords[index - 1]) || !spellProvided(spells, index - 1, usedWords[index - 1]) || !charMatching(usedWords[index - 2], usedWords[index - 1])))
    {
        strcpy(winner, player1);
        strcpy(loser, player2);
    }
    else
    {
        strcpy(winner, player2);
        strcpy(loser, player1);
    }
    if (!spellProvided(spells, 78, usedWords[index - 1]))
    {
        printf("%s is the Winner! Congratulations! %s did not provide a Spell that is found in the list.\n ", winner, loser);
    }
    else if (spellAlreadyUsed(usedWords, index - 1, usedWords[index - 1]))
    {
        printf("%s is the Winner! Congratulations! You won since %s casted a Spell that is already used.\n ", winner, loser);
    }
    else if (index != 1 && !charMatching(usedWords[index - 2], usedWords[index - 1]))
    {
        printf("%s is the Winner! Congratulations! You won since the first character of the word provided by %s is not matching the last character of the previous word.\n ", winner, loser);
    }
    else if (runOut(usedWords[index - 1], Tally) == false)
    {
        printf("%s is the Winner! Congratulations! You won due to %s running out of words.\n ", loser, winner); // It is the only opposite case.
    }
}

int main()
{
    // Step 1
    FILE *filePointer = fopen("spells.txt", "r");
    char size[100];
    char *spells[78];

    ReadingFileIntoString(filePointer, spells, 78, size, 100);
    fclose(filePointer);

    // Using The Tally Method we are creating an array to keep track of the remaining words.
    int Tally[26];
    for (int i = 0; i < 26; i++)
    {
        Tally[i] = 0;
    }
    for (int i = 0; i < 78; i++)
    {
        char firstLetter = spells[i][0];
        int value = firstLetter - 97;
        Tally[value] = Tally[value] + 1;
    }

    // Step 2
    char player1[100];
    printf("Player 1 please enter your first name: ");
    scanf(" %s", player1);
    printf("Welcome %s!\n", player1);
    char player2[100] = "Bot"; // Player 2 is the Bot

    // Step 3

    PrintingList(spells, 78);

    // Step 4

    bool player1Turn = Player1Turn(player1, player2);

    // Step 5
    int numberOfPlays = 78;
    char *usedWords[78];
    // Allocate memory for each char pointer
    for (int i = 0; i < numberOfPlays; i++)
    {
        usedWords[i] = (char *)malloc(78 * sizeof(char));
    }
    int index = 0;

    player1Turn = GamePlay(&index, usedWords, Tally, player1Turn, player1, player2, spells);

    // Step 6 announcing the Winner

    AnnouncingWinner(player1Turn, usedWords, index, spells, player1, player2, Tally);

    // free the allocated memory when you're done with it
    for (int i = 0; i < numberOfPlays; i++)
    {
        free(usedWords[i]);
    }
    for (int i = 0; i < numberOfPlays; i++)
    {
        free(spells[i]);
    }
    return 0;
}