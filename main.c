#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

#define rowsBM 10
#define columnsBM 10
#define maxCellLength 10
char boardMatrix[rowsBM][columnsBM][maxCellLength];
pthread_mutex_t lock;
pthread_t timerThread;
volatile int keepPlaying = 1;

typedef struct
{
    char originalWord[10];
    char currentWord[10];
    char alternativeWords[5][10];
    char originalDefinition[200];
    char definition[200];
    char alternativeDefinitions[5][200];
    int numberOfAlternatives;
    int row;
    int column;
    char direction;
    int guessed;
    int usingAlternative;
    int length;
    int index;
    int hidden;
    int constantIndex;
} word;

word wordsInBoard[6];

void showBoard();

int allGuessed()
{
    for (int i = 0; i < sizeof(wordsInBoard) / sizeof(wordsInBoard[0]); i++)
    {
        if (!wordsInBoard[i].guessed) return 0;
    }
    return 1;
}

void showInstructions()
{
    printf("Bienvenido al juego: La Casa de Hojas.\n");
    printf("Instrucciones:\n");
    printf("1. Las palabras en el tablero pueden cambiar si aún no se han adivinado.\n");
    printf("2. Se te mostrarán definiciones de palabras. Elige la palabra a adivinar basándote en la definición proporcionada.\n");
    printf("3. Las suposiciones correctas fijarán la palabra en el tablero.\n");
    printf("4. Las palabras no fijadas pueden cambiar después de un intervalo de tiempo específico.\n");
    printf("5. Intenta resolver el crucigrama antes de que cambien todas las palabras.\n");
    printf("¡Buena suerte y diviértete!\n\n");
}

void clearMatrix()
{
    for (int i = 0; i < rowsBM; i++)
    {
        for (int j = 0; j < columnsBM; j++)
        {
            strcpy(boardMatrix[i][j], "X");
        }
    }
}

void initWords()
{
    printf("Inicializando las palabras... \n");
    strcpy(wordsInBoard[0].currentWord, "oso");
    strcpy(wordsInBoard[0].alternativeWords[0], "asar");
    strcpy(wordsInBoard[0].definition, "Son animales de gran tamaño, generalmente omnívoro.");
    strcpy(wordsInBoard[0].alternativeDefinitions[0], "Cocinar un alimento en el horno o a la parrilla.");
    strcpy(wordsInBoard[0].originalWord, "oso");
    strcpy(wordsInBoard[0].originalDefinition, "Son animales de gran tamaño, generalmente omnívoro.");
    wordsInBoard[0].length = strlen(wordsInBoard[0].currentWord);
    wordsInBoard[0].index = 0;
    wordsInBoard[0].row = 0;
    wordsInBoard[0].column = 1;
    wordsInBoard[0].direction = 'h';
    wordsInBoard[0].numberOfAlternatives = 1;
    wordsInBoard[0].hidden = 1;
    wordsInBoard[0].usingAlternative = 0;


    strcpy(wordsInBoard[1].currentWord, "silla");
    strcpy(wordsInBoard[1].alternativeWords[0], "sillon");
    strcpy(wordsInBoard[1].definition, "Asiento con respaldo, por lo general con cuatro patas, y en que solo cabe una persona.");
    strcpy(wordsInBoard[1].alternativeDefinitions[0], "Silla de brazos, mayor y más cómoda que la ordinaria.");
    strcpy(wordsInBoard[1].originalWord, "silla");
    strcpy(wordsInBoard[1].originalDefinition, "Asiento con respaldo, por lo general con cuatro patas, y en que solo cabe una persona.");
    wordsInBoard[1].length = strlen(wordsInBoard[1].currentWord);
    wordsInBoard[1].index = 1;
    wordsInBoard[1].row = 0;
    wordsInBoard[1].column = 2;
    wordsInBoard[1].direction = 'v';
    wordsInBoard[1].numberOfAlternatives = 1;
    wordsInBoard[1].hidden = 1;
    wordsInBoard[1].usingAlternative = 0;

    strcpy(wordsInBoard[2].currentWord, "lampara");
    strcpy(wordsInBoard[2].alternativeWords[0], "leopardo");
    strcpy(wordsInBoard[2].definition, "Utensilio o aparato que, colgado o sostenido sobre un pie, sirve de soporte a una o varias luces artificiales.");
    strcpy(wordsInBoard[2].alternativeDefinitions[0], "Mamífero carnívoro félido, que generalmente tiene el pelaje amarillo rojizo con manchas negras.");
    strcpy(wordsInBoard[2].originalWord, "lampara");
    strcpy(wordsInBoard[2].originalDefinition, "Utensilio o aparato que, colgado o sostenido sobre un pie, sirve de soporte a una o varias luces artificiales.");
    wordsInBoard[2].length = strlen(wordsInBoard[2].currentWord);
    wordsInBoard[2].index = 2;
    wordsInBoard[2].row = 3;
    wordsInBoard[2].column = 2;
    wordsInBoard[2].direction = 'h';
    wordsInBoard[2].numberOfAlternatives = 1;
    wordsInBoard[2].hidden = 1;
    wordsInBoard[2].usingAlternative = 0;

    strcpy(wordsInBoard[3].currentWord, "copa");
    strcpy(wordsInBoard[3].alternativeWords[0], "cupo");
    strcpy(wordsInBoard[3].definition, "Vaso con pie para beber.");
    strcpy(wordsInBoard[3].alternativeDefinitions[0], "Lugar disponible en un vehículo, local o institución. ");
    strcpy(wordsInBoard[3].originalWord, "copa");
    strcpy(wordsInBoard[3].originalDefinition, "Vaso con pie para beber.");
    wordsInBoard[3].length = strlen(wordsInBoard[3].currentWord);
    wordsInBoard[3].index = 3;
    wordsInBoard[3].row = 1;
    wordsInBoard[3].column = 5;
    wordsInBoard[3].direction = 'v';
    wordsInBoard[3].numberOfAlternatives = 1;
    wordsInBoard[3].hidden = 1;
    wordsInBoard[3].usingAlternative = 0;

    strcpy(wordsInBoard[4].currentWord, "foca");
    strcpy(wordsInBoard[4].alternativeWords[0], "fugas");
    strcpy(wordsInBoard[4].definition, "Nombre genérico para diversos mamíferos pinnípedos marinos, seal en inglés");
    strcpy(wordsInBoard[4].alternativeDefinitions[0], " Salida accidental de gas o de líquido por un orificio o una abertura producidos en su contenedor, en plural");
    strcpy(wordsInBoard[4].originalWord, "foca");
    strcpy(wordsInBoard[4].originalDefinition, "Nombre genérico para diversos mamíferos pinnípedos marinos, seal en inglés");
    wordsInBoard[4].length = strlen(wordsInBoard[4].currentWord);
    wordsInBoard[4].index = 4;
    wordsInBoard[4].row = 6;
    wordsInBoard[4].column = 4;
    wordsInBoard[4].direction = 'h';
    wordsInBoard[4].numberOfAlternatives = 1;
    wordsInBoard[4].hidden = 1;
    wordsInBoard[4].usingAlternative = 0;

    strcpy(wordsInBoard[5].currentWord, "rata");
    strcpy(wordsInBoard[5].alternativeWords[0], "ramas");
    strcpy(wordsInBoard[5].definition, "Hembra del ratón.");
    strcpy(wordsInBoard[5].alternativeDefinitions[0], "Cada una de las partes que nacen del tronco de la planta y en las cuales brotan por lo común las hojas.");
    strcpy(wordsInBoard[5].originalWord, "rata");
    strcpy(wordsInBoard[5].originalDefinition, "Hembra del ratón.");
    wordsInBoard[5].length = strlen(wordsInBoard[5].currentWord);
    wordsInBoard[5].index = 5;
    wordsInBoard[5].row = 3;
    wordsInBoard[5].column = 7;
    wordsInBoard[5].direction = 'v';
    wordsInBoard[5].numberOfAlternatives = 1;
    wordsInBoard[5].hidden = 1;
    wordsInBoard[5].usingAlternative = 0;


    wordsInBoard[0].constantIndex = 1;
    wordsInBoard[1].constantIndex = 2;
    wordsInBoard[2].constantIndex = 3;
    wordsInBoard[3].constantIndex = 4;
    wordsInBoard[4].constantIndex = 5;
    wordsInBoard[5].constantIndex = 6;
}


void updateBoardForWord(word *w)
{
    if (w->guessed)
    {
        for (int i = 0; i < w->length; i++)
        {
            int targetRow = w->row + (w->direction == 'v' ? i : 0);
            int targetCol = w->column + (w->direction == 'h' ? i : 0);
            pthread_mutex_lock(&lock);
            snprintf(boardMatrix[targetRow][targetCol], maxCellLength, "%c", w->currentWord[i]);
            pthread_mutex_unlock(&lock);
        }
    } else
    {
        if (w->hidden)
        {
            for (int i = 0; i < w->length; i++)
            {
                int targetRow = w->row + (w->direction == 'v' ? i : 0);
                int targetCol = w->column + (w->direction == 'h' ? i : 0);
                pthread_mutex_lock(&lock);
                snprintf(boardMatrix[targetRow][targetCol], maxCellLength, "%d", w->index);
                pthread_mutex_unlock(&lock);
            }
        } else
        {
            for (int i = 0; i < w->length; i++)
            {
                int targetRow = w->row + (w->direction == 'v' ? i : 0);
                int targetCol = w->column + (w->direction == 'h' ? i : 0);
                pthread_mutex_lock(&lock);
                snprintf(boardMatrix[targetRow][targetCol], maxCellLength, "X");
                pthread_mutex_unlock(&lock);
            }
        }
    }
}

void changeWord(int word_index)
{
    word *w = &wordsInBoard[word_index];
    if (!w->guessed && w->numberOfAlternatives > 0)
    {
        if (w->usingAlternative)
        {
            strncpy(w->currentWord, w->originalWord, sizeof(w->currentWord) - 1);
            strncpy(w->definition, w->originalDefinition, sizeof(w->definition) - 1);
            w->usingAlternative = 0;
        }
        else
        {
            int randomIndex = rand() % w->numberOfAlternatives;
            strncpy(w->currentWord, w->alternativeWords[randomIndex], sizeof(w->currentWord) - 1);
            strncpy(w->definition, w->alternativeDefinitions[randomIndex], sizeof(w->definition) - 1);
            w->usingAlternative = 1;
        }
        w -> length = strlen(w->currentWord);
        printf("La palabra ha cambiado...\n");
        printf("Definición: %s\n", w->definition);
        if (!w->hidden)
        {
            for (int i = 0; i < w->length; i++)
            {
                int targetRow = w->row + (w->direction == 'v' ? i : 0);
                int targetCol = w->column + (w->direction == 'h' ? i : 0);
                pthread_mutex_lock(&lock);
                snprintf(boardMatrix[targetRow][targetCol], maxCellLength, "%c", w->currentWord[i]);
                pthread_mutex_unlock(&lock);
            }
        }
    }
}

void alarmHandler(int signum)
{
    if (signum == SIGALRM)
    {
        pthread_mutex_lock(&lock);
        for (int i = 0; i < 6; i++)
        {
            if (!wordsInBoard[i].guessed)
            {
                changeWord(i);
            }
        }
        showBoard();
        pthread_mutex_unlock(&lock);
        alarm(50);
    }
}

void *timerFunction(void *arg)
{
    alarm(50);
    pause();
    while (keepPlaying)
    {
        pause();
    }
    pthread_exit(NULL);
}

void signalHandlerInterruptedGame(int signum)
{
    if (signum == SIGINT)
    {
        printf("\nEl juego ha sido interrumpido, ¿realmente quieres terminarlo? (s/n) ");
        char answer = getchar();
        if (answer == 'S' || answer == 's')
        {
            keepPlaying = 0;
        }
    }
}

void setupSignals()
{
    signal(SIGINT, signalHandlerInterruptedGame);
    signal(SIGALRM, alarmHandler);
}

void showBoard()
{
    printf("+-----");
    for (int i = 1; i < columnsBM; i++)
    {
        printf("+-----");
    }
    printf("+\n");
    for (int i = 0; i < rowsBM; i++)
    {
        for (int j = 0; j < columnsBM; j++)
        {
            char cellContent[maxCellLength] = "";
            int found = 0;
            int guessedFound = 0;
            for (int k = 0; k < sizeof(wordsInBoard) / sizeof(wordsInBoard[0]); k++)
            {
                word *w = &wordsInBoard[k];
                int startRow = w->row;
                int startCol = w->column;
                int endRow = startRow + (w->direction == 'v' ? w->length - 1 : 0);
                int endCol = startCol + (w->direction == 'h' ? w->length - 1 : 0);
                if (i >= startRow && i <= endRow && j >= startCol && j <= endCol)
                {
                    if (w->guessed)
                    {
                        int offset = (w->direction == 'h' ? j - startCol : i - startRow);
                        snprintf(cellContent, sizeof(cellContent), "%c", w->currentWord[offset]);
                        guessedFound = 1;
                        break;
                    }
                }
            }

            if (!guessedFound)
            {
                for (int k = 0; k < sizeof(wordsInBoard) / sizeof(wordsInBoard[0]); k++)
                {
                    word *w = &wordsInBoard[k];
                    int startRow = w->row;
                    int startCol = w->column;
                    int endRow = startRow + (w->direction == 'v' ? w->length - 1 : 0);
                    int endCol = startCol + (w->direction == 'h' ? w->length - 1 : 0);

                    if (i >= startRow && i <= endRow && j >= startCol && j <= endCol && !w->guessed)
                    {
                        if (found)
                        {
                            strncat(cellContent, "/", sizeof(cellContent) - strlen(cellContent) - 1);
                        }
                        char indexStr[4];
                        snprintf(indexStr, sizeof(indexStr), "%d", w->constantIndex);
                        strncat(cellContent, indexStr, sizeof(cellContent) - strlen(cellContent) - 1);
                        found = 1;
                    }
                }
            }
            if (!found && !guessedFound)
            {
                strcpy(cellContent, " ");
            }
            printf("|%4s ", cellContent);
        }
        printf("|\n");

        for (int j = 0; j < columnsBM; j++)
        {
            printf("+-----");
        }
        printf("+\n");
    }
}

void userInput()
{
    int wordIndex;
    char guess[10];
    printf("Buscando las palabras posibles para adivinar: \n");
    for (int i = 0; i < sizeof(wordsInBoard) / sizeof(wordsInBoard[0]); i++)
    {
        if (!wordsInBoard[i].guessed)
        {
            printf("%d: %s\n", i + 1, wordsInBoard[i].definition);
        }
    }
    printf("Seleccione el número de la palabra que desea adivinar: ");
    scanf("%d", &wordIndex);
    getchar();
    if (wordIndex > 0 && wordIndex <= sizeof(wordsInBoard) / sizeof(wordsInBoard[0]) && !wordsInBoard[wordIndex - 1].guessed)
    {
        printf("Ingresa la palabra que piensas que es para la siguiente definición '%s': ", wordsInBoard[wordIndex - 1].definition);
        fgets(guess, sizeof(guess), stdin);
        guess[strcspn(guess, "\n")] = 0;
        if (strcasecmp(wordsInBoard[wordIndex - 1].currentWord, guess) == 0)
        {
            wordsInBoard[wordIndex - 1].guessed = 1;
            updateBoardForWord(&wordsInBoard[wordIndex - 1]);
            printf("¡Correcto! La palabra ha sido añadida al tablero.\n");
        }
        else
        {
            printf("Incorrecto, vuelve a intentar\n");
        }
    }
    else
    {
        printf("Número inválido o correspondiente a una palabra ya adivinada\n");
    }
}

void initWordsAndBoard()
{
    initWords();
    clearMatrix();
    for (int i = 0; i < sizeof(wordsInBoard) / sizeof(wordsInBoard[0]); i++)
    {
        updateBoardForWord(&wordsInBoard[i]);
    }
}

int askToContinue()
{
    char response;
    printf("¿Quieres jugar de nuevo? (s/n): ");
    scanf(" %c", &response);
    getchar();
    return (response == 's' || response == 'S');
}

void resetGame()
{
    clearMatrix();
    for (int i = 0; i < sizeof(wordsInBoard) / sizeof(wordsInBoard[0]); i++)
    {
        wordsInBoard[i].guessed = 0;
        updateBoardForWord(&wordsInBoard[i]);
    }
}

int main()
{
    pthread_mutex_init(&lock, NULL);
    setupSignals();
    pthread_create(&timerThread, NULL, timerFunction, NULL);

    pid_t pid = fork();

    if (pid == 0)
    {
        char answer;
        do
        {
            showInstructions();
            printf("¿Has leído las instrucciones? (s/n): ");
            scanf(" %c", &answer);
        } while (answer != 'S' && answer != 's');
        exit(0);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
    initWordsAndBoard();
    while (keepPlaying)
    {
        showBoard();
        userInput();
        if (allGuessed())
        {
            printf("¡Has adivinado todas las palabras!\n");
            if (!askToContinue())
            {
                keepPlaying = 0;
            }
            else
            {
                resetGame();
            }
        }
    }
    
    showBoard();
    pthread_cancel(timerThread);
    pthread_join(timerThread, NULL);
    pthread_mutex_destroy(&lock);

    printf("Gracias por jugar. ¡Hasta la próxima!\n");
    return 0;
}

