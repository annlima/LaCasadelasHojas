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
#define MAX_CELL_LENGTH 10
char boardMatrix[rowsBM][columnsBM][MAX_CELL_LENGTH];

pthread_mutex_t lock;
pthread_t timerThread;
volatile int keepPlaying = 1;

typedef struct {
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


word my_words[6];

void showBoard();

int allGuessed()
{
    for (int i = 0; i < sizeof(my_words) / sizeof(my_words[0]); i++)
    {
        if (!my_words[i].guessed) return 0;
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
    strcpy(my_words[0].currentWord, "oso");
    strcpy(my_words[0].alternativeWords[0], "asar");
    strcpy(my_words[0].definition, "Son animales de gran tamaño, generalmente omnívoro.");
    strcpy(my_words[0].alternativeDefinitions[0], "Cocinar un alimento en el horno o a la parrilla.");
    strcpy(my_words[0].originalWord, "oso");
    strcpy(my_words[0].originalDefinition, "Son animales de gran tamaño, generalmente omnívoro.");
    my_words[0].length = strlen(my_words[0].currentWord);
    my_words[0].index = 0;
    my_words[0].row = 0;
    my_words[0].column = 1;
    my_words[0].direction = 'h';
    my_words[0].numberOfAlternatives = 1;
    my_words[0].hidden = 1;
    my_words[0].usingAlternative = 0;


    strcpy(my_words[1].currentWord, "silla");
    strcpy(my_words[1].alternativeWords[0], "sillon");
    strcpy(my_words[1].definition, "Asiento con respaldo, por lo general con cuatro patas, y en que solo cabe una persona.");
    strcpy(my_words[1].alternativeDefinitions[0], "Silla de brazos, mayor y más cómoda que la ordinaria.");
    strcpy(my_words[1].originalWord, "silla");
    strcpy(my_words[1].originalDefinition, "Asiento con respaldo, por lo general con cuatro patas, y en que solo cabe una persona.");
    my_words[1].length = strlen(my_words[1].currentWord);
    my_words[1].index = 1;
    my_words[1].row = 0;
    my_words[1].column = 2;
    my_words[1].direction = 'v';
    my_words[1].numberOfAlternatives = 1;
    my_words[1].hidden = 1;
    my_words[1].usingAlternative = 0;

    strcpy(my_words[2].currentWord, "lampara");
    strcpy(my_words[2].alternativeWords[0], "leopardo");
    strcpy(my_words[2].definition, "Utensilio o aparato que, colgado o sostenido sobre un pie, sirve de soporte a una o varias luces artificiales.");
    strcpy(my_words[2].alternativeDefinitions[0], "Mamífero carnívoro félido, que generalmente tiene el pelaje amarillo rojizo con manchas negras.");
    strcpy(my_words[2].originalWord, "lampara");
    strcpy(my_words[2].originalDefinition, "Utensilio o aparato que, colgado o sostenido sobre un pie, sirve de soporte a una o varias luces artificiales.");
    my_words[2].length = strlen(my_words[2].currentWord);
    my_words[2].index = 2;
    my_words[2].row = 3;
    my_words[2].column = 2;
    my_words[2].direction = 'h';
    my_words[2].numberOfAlternatives = 1;
    my_words[2].hidden = 1;
    my_words[2].usingAlternative = 0;

    strcpy(my_words[3].currentWord, "copa");
    strcpy(my_words[3].alternativeWords[0], "copia");
    strcpy(my_words[3].definition, "Vaso con pie para beber.");
    strcpy(my_words[3].alternativeDefinitions[0], "Imitación de una obra ajena, con la pretensión de que parezca original.");
    strcpy(my_words[3].originalWord, "copa");
    strcpy(my_words[3].originalDefinition, "Vaso con pie para beber.");
    my_words[3].length = strlen(my_words[3].currentWord);
    my_words[3].index = 3;
    my_words[3].row = 1;
    my_words[3].column = 5;
    my_words[3].direction = 'v';
    my_words[3].numberOfAlternatives = 1;
    my_words[3].hidden = 1;
    my_words[3].usingAlternative = 0;

    strcpy(my_words[4].currentWord, "foca");
    strcpy(my_words[4].alternativeWords[0], "fugas");
    strcpy(my_words[4].definition, "Nombre genérico para diversos mamíferos pinnípedos marinos");
    strcpy(my_words[4].alternativeDefinitions[0], " Salida accidental de gas o de líquido por un orificio o una abertura producidos en su contenedor, en plural");
    strcpy(my_words[4].originalWord, "foca");
    strcpy(my_words[4].originalDefinition, "Nombre genérico para diversos mamíferos pinnípedos marinos");
    my_words[4].length = strlen(my_words[4].currentWord);
    my_words[4].index = 4;
    my_words[4].row = 6;
    my_words[4].column = 4;
    my_words[4].direction = 'h';
    my_words[4].numberOfAlternatives = 1;
    my_words[4].hidden = 1;
    my_words[4].usingAlternative = 0;

    strcpy(my_words[5].currentWord, "rata");
    strcpy(my_words[5].alternativeWords[0], "ramas");
    strcpy(my_words[5].definition, "Hembra del ratón.");
    strcpy(my_words[5].alternativeDefinitions[0], "Cada una de las partes que nacen del tronco de la planta y en las cuales brotan por lo común las hojas.");
    strcpy(my_words[5].originalWord, "rata");
    strcpy(my_words[5].originalDefinition, "Hembra del ratón.");
    my_words[5].length = strlen(my_words[5].currentWord);
    my_words[5].index = 5;
    my_words[5].row = 3;
    my_words[5].column = 7;
    my_words[5].direction = 'v';
    my_words[5].numberOfAlternatives = 1;
    my_words[5].hidden = 1;
    my_words[5].usingAlternative = 0;


    my_words[0].constantIndex = 1;
    my_words[1].constantIndex = 2;
    my_words[2].constantIndex = 3;
    my_words[3].constantIndex = 4;
    my_words[4].constantIndex = 5;
    my_words[5].constantIndex = 6;
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
            snprintf(boardMatrix[targetRow][targetCol], MAX_CELL_LENGTH, "%c", w->currentWord[i]);
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
                snprintf(boardMatrix[targetRow][targetCol], MAX_CELL_LENGTH, "%d", w->index);
                pthread_mutex_unlock(&lock);
            }
        } else
        {
            for (int i = 0; i < w->length; i++)
            {
                int targetRow = w->row + (w->direction == 'v' ? i : 0);
                int targetCol = w->column + (w->direction == 'h' ? i : 0);
                pthread_mutex_lock(&lock);
                snprintf(boardMatrix[targetRow][targetCol], MAX_CELL_LENGTH, "X");
                pthread_mutex_unlock(&lock);
            }
        }
    }
}

void changeWord(int word_index) {
    word *w = &my_words[word_index];
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
            int random_index = rand() % w->numberOfAlternatives;
            strncpy(w->currentWord, w->alternativeWords[random_index], sizeof(w->currentWord) - 1);
            strncpy(w->definition, w->alternativeDefinitions[random_index], sizeof(w->definition) - 1);
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
                snprintf(boardMatrix[targetRow][targetCol], MAX_CELL_LENGTH, "%c", w->currentWord[i]);
                pthread_mutex_unlock(&lock);
            }
        }
    }
}

void alarm_handler(int signum)
{
    if (signum == SIGALRM) {
        pthread_mutex_lock(&lock);
        for (int i = 0; i < 6; i++)
        {
            if (!my_words[i].guessed)
            {
                changeWord(i);
            }
        }
        showBoard();
        pthread_mutex_unlock(&lock);
        alarm(30);
    }
}

void *timer_function(void *arg)
{
    alarm(20);
    pause();
    while (keepPlaying)
    {
        pause();
    }
    pthread_exit(NULL);
}


void sig_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nEl juego ha sido interrumpido, ¿realmente quieres terminarlo? ");
        char answer = getchar();
        if (answer == 'y' || answer == 'Y') {
            keepPlaying = 0;
        }
    }
}

void setup_signals()
{
    signal(SIGINT, sig_handler);
    signal(SIGALRM, alarm_handler);
}

void showBoard() {
    printf("+-----");
    for (int i = 1; i < columnsBM; i++) {
        printf("+-----");
    }
    printf("+\n");

    for (int i = 0; i < rowsBM; i++) {
        for (int j = 0; j < columnsBM; j++) {
            char cellContent[MAX_CELL_LENGTH] = "";
            int found = 0;
            int guessed_found = 0;

            for (int k = 0; k < sizeof(my_words) / sizeof(my_words[0]); k++) {
                word *w = &my_words[k];
                int startRow = w->row;
                int startCol = w->column;
                int endRow = startRow + (w->direction == 'v' ? w->length - 1 : 0);
                int endCol = startCol + (w->direction == 'h' ? w->length - 1 : 0);

                if (i >= startRow && i <= endRow && j >= startCol && j <= endCol) {
                    if (w->guessed) {
                        int offset = (w->direction == 'h' ? j - startCol : i - startRow);
                        snprintf(cellContent, sizeof(cellContent), "%c", w->currentWord[offset]);
                        guessed_found = 1;
                        break;
                    }
                }
            }

            if (!guessed_found) {
                for (int k = 0; k < sizeof(my_words) / sizeof(my_words[0]); k++) {
                    word *w = &my_words[k];
                    int startRow = w->row;
                    int startCol = w->column;
                    int endRow = startRow + (w->direction == 'v' ? w->length - 1 : 0);
                    int endCol = startCol + (w->direction == 'h' ? w->length - 1 : 0);

                    if (i >= startRow && i <= endRow && j >= startCol && j <= endCol && !w->guessed) {
                        if (found) {
                            strncat(cellContent, "/", sizeof(cellContent) - strlen(cellContent) - 1);
                        }
                        char indexStr[4];
                        snprintf(indexStr, sizeof(indexStr), "%d", w->constantIndex);
                        strncat(cellContent, indexStr, sizeof(cellContent) - strlen(cellContent) - 1);
                        found = 1;
                    }
                }
            }

            if (!found && !guessed_found) {
                strcpy(cellContent, " ");
            }
            printf("|%4s ", cellContent);
        }
        printf("|\n");

        for (int j = 0; j < columnsBM; j++) {
            printf("+-----");
        }
        printf("+\n");
    }
}



void userInput() {
    int wordIndex;
    char guess[10];

    printf("Buscando las palabras posibles para adivinar: \n");
    for (int i = 0; i < sizeof(my_words) / sizeof(my_words[0]); i++)
    {
        if (!my_words[i].guessed) {
            printf("%d: %s\n", i + 1, my_words[i].definition);
        }
    }
    printf("Seleccione el número de la palabra que desea adivinar según la definición proporcionada: ");
    scanf("%d", &wordIndex);
    getchar();

    if (wordIndex > 0 && wordIndex <= sizeof(my_words) / sizeof(my_words[0]) && !my_words[wordIndex - 1].guessed)
    {
        printf("Ingresa la palabra que piensas que es para la siguiente definición '%s': ", my_words[wordIndex - 1].definition);
        fgets(guess, sizeof(guess), stdin);
        guess[strcspn(guess, "\n")] = 0;
        if (strcasecmp(my_words[wordIndex - 1].currentWord, guess) == 0)
        {
            my_words[wordIndex - 1].guessed = 1;
            updateBoardForWord(&my_words[wordIndex - 1]);
            printf("¡Correcto! La palabra ha sido añadida al tablero.\n");
        } else {
            printf("Incorrecto, vuelve a intentar\n");
        }
    } else {
        printf("Número inválido o correspondiente a una palabra ya adivinada\n");
    }
}

void initWordsAndBoard()
{
    initWords();
    clearMatrix();

    for (int i = 0; i < sizeof(my_words) / sizeof(my_words[0]); i++)
    {
        updateBoardForWord(&my_words[i]);
    }
}

int askToContinue() {
    char response;
    printf("¿Quieres jugar de nuevo? (s/n): ");
    scanf(" %c", &response);
    getchar();

    return (response == 's' || response == 'S');
}

void resetGame() {
    clearMatrix();
    for (int i = 0; i < sizeof(my_words) / sizeof(my_words[0]); i++) {
        my_words[i].guessed = 0;
        updateBoardForWord(&my_words[i]);
    }
}

int main() {
    pthread_mutex_init(&lock, NULL);
    setup_signals();
    pthread_create(&timerThread, NULL, timer_function, NULL);

    pid_t pid = fork();

    if (pid == 0) {
        char answer;
        do {
            showInstructions();
            printf("¿Has leído las instrucciones? (sí: y / no: n): ");
            scanf(" %c", &answer);
        } while (answer != 'Y' && answer != 'y');
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }

    initWordsAndBoard();
    while (keepPlaying) {
        showBoard();
        userInput();
        if (allGuessed()) {
            printf("¡Has adivinado todas las palabras!\n");
            if (!askToContinue()) {
                keepPlaying = 0;
            } else {
                resetGame();
            }
        }
    }

    pthread_cancel(timerThread);
    pthread_join(timerThread, NULL);
    pthread_mutex_destroy(&lock);

    printf("Gracias por jugar. ¡Hasta la próxima!\n");
    return 0;
}

