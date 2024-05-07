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
pthread_t timer_thread;
volatile int keep_playing = 1;

typedef struct {
    char original_word[10];
    char current_word[10];
    char alternative_words[5][10];
    char original_definition[200];
    char definition[200];
    char alternative_definitions[5][200];
    int num_alternatives;
    int row;
    int col;
    char direction;
    int guessed;
    int using_alternative;
    int length;
    int index;
    int hidden;
} word;


word my_words[6];

int allGuessed() {
    for (int i = 0; i < sizeof(my_words) / sizeof(my_words[0]); i++) {
        if (!my_words[i].guessed) return 0;
    }
    return 1;
}

void showInstructions() {
    printf("Bienvenido al juego de crucigramas 'La Casa de Hojas'.\n");
    printf("Instrucciones:\n");
    printf("1. Las palabras en el tablero pueden cambiar si aún no se han adivinado.\n");
    printf("2. Se te mostrarán definiciones de palabras. Elige la palabra a adivinar basándote en la definición proporcionada.\n");
    printf("3. Las suposiciones correctas fijarán la palabra en el tablero.\n");
    printf("4. Las palabras no fijadas pueden cambiar después de un intervalo de tiempo específico.\n");
    printf("5. Intenta resolver el crucigrama antes de que cambien todas las palabras.\n");
    printf("¡Buena suerte y diviértete!\n\n");
}

void clearMatrix() {
    for (int i = 0; i < rowsBM; i++) {
        for (int j = 0; j < columnsBM; j++) {
            strcpy(boardMatrix[i][j], "X");
        }
    }
}

void init_words() {
    printf("Inicializando las palabras... \n");
    strcpy(my_words[0].current_word, "oso");
    strcpy(my_words[0].alternative_words[0], "asar");
    strcpy(my_words[0].definition, "Son animales de gran tamaño, generalmente omnívoro.");
    strcpy(my_words[0].alternative_definitions[0], "Cocinar un alimento en el horno o a la parrilla.");
    strcpy(my_words[0].original_word, "oso");
    strcpy(my_words[0].original_definition, "Son animales de gran tamaño, generalmente omnívoro.");
    my_words[0].length = strlen(my_words[0].current_word);
    my_words[0].index = 0;
    my_words[0].row = 0;
    my_words[0].col = 1;
    my_words[0].direction = 'h';
    my_words[0].num_alternatives = 1;
    my_words[0].hidden = 1;
    my_words[0].using_alternative = 0;


    strcpy(my_words[1].current_word, "silla");
    strcpy(my_words[1].alternative_words[0], "sillon");
    strcpy(my_words[1].definition, "Asiento con respaldo, por lo general con cuatro patas, y en que solo cabe una persona.");
    strcpy(my_words[1].alternative_definitions[0], "Silla de brazos, mayor y más cómoda que la ordinaria.");
    strcpy(my_words[1].original_word, "silla");
    strcpy(my_words[1].original_definition, "Asiento con respaldo, por lo general con cuatro patas, y en que solo cabe una persona.");
    my_words[1].length = strlen(my_words[1].current_word);
    my_words[1].index = 1;
    my_words[1].row = 0;
    my_words[1].col = 2;
    my_words[1].direction = 'v';
    my_words[1].num_alternatives = 1;
    my_words[1].hidden = 1;
    my_words[1].using_alternative = 0;

    strcpy(my_words[2].current_word, "lampara");
    strcpy(my_words[2].alternative_words[0], "leopardo");
    strcpy(my_words[2].definition, "Utensilio o aparato que, colgado o sostenido sobre un pie, sirve de soporte a una o varias luces artificiales.");
    strcpy(my_words[2].alternative_definitions[0], "Mamífero carnívoro félido, que generalmente tiene el pelaje amarillo rojizo con manchas negras.");
    strcpy(my_words[2].original_word, "lampara");
    strcpy(my_words[2].original_definition, "Utensilio o aparato que, colgado o sostenido sobre un pie, sirve de soporte a una o varias luces artificiales.");
    my_words[2].length = strlen(my_words[2].current_word);
    my_words[2].index = 2;
    my_words[2].row = 3;
    my_words[2].col = 2;
    my_words[2].direction = 'h';
    my_words[2].num_alternatives = 1;
    my_words[2].hidden = 1;
    my_words[2].using_alternative = 0;

    strcpy(my_words[3].current_word, "copa");
    strcpy(my_words[3].alternative_words[0], "copia");
    strcpy(my_words[3].definition, "Vaso con pie para beber.");
    strcpy(my_words[3].alternative_definitions[0], "Imitación de una obra ajena, con la pretensión de que parezca original.");
    strcpy(my_words[3].original_word, "copa");
    strcpy(my_words[3].original_definition, "Vaso con pie para beber.");
    my_words[3].length = strlen(my_words[3].current_word);
    my_words[3].index = 3;
    my_words[3].row = 1;
    my_words[3].col = 5;
    my_words[3].direction = 'v';
    my_words[3].num_alternatives = 1;
    my_words[3].hidden = 1;
    my_words[3].using_alternative = 0;

    strcpy(my_words[4].current_word, "foca");
    strcpy(my_words[4].alternative_words[0], "fugas");
    strcpy(my_words[4].definition, "Nombre genérico para diversos mamíferos pinnípedos marinos");
    strcpy(my_words[4].alternative_definitions[0], " Salida accidental de gas o de líquido por un orificio o una abertura producidos en su contenedor, en plural");
    strcpy(my_words[4].original_word, "foca");
    strcpy(my_words[4].original_definition, "Nombre genérico para diversos mamíferos pinnípedos marinos");
    my_words[4].length = strlen(my_words[4].current_word);
    my_words[4].index = 4;
    my_words[4].row = 6;
    my_words[4].col = 4;
    my_words[4].direction = 'h';
    my_words[4].num_alternatives = 1;
    my_words[4].hidden = 1;
    my_words[4].using_alternative = 0;

    strcpy(my_words[5].current_word, "rata");
    strcpy(my_words[5].alternative_words[0], "ramas");
    strcpy(my_words[5].definition, "Hembra del ratón.");
    strcpy(my_words[5].alternative_definitions[0], "Cada una de las partes que nacen del tronco de la planta y en las cuales brotan por lo común las hojas.");
    strcpy(my_words[5].original_word, "rata");
    strcpy(my_words[5].original_definition, "Hembra del ratón.");
    my_words[5].length = strlen(my_words[5].current_word);
    my_words[5].index = 5;
    my_words[5].row = 3;
    my_words[5].col = 7;
    my_words[5].direction = 'v';
    my_words[5].num_alternatives = 1;
    my_words[5].hidden = 1;
    my_words[5].using_alternative = 0;
}


void update_board_for_word(word *w) {
    if (w->guessed) {
        for (int i = 0; i < w->length; i++) {
            int targetRow = w->row + (w->direction == 'v' ? i : 0);
            int targetCol = w->col + (w->direction == 'h' ? i : 0);
            pthread_mutex_lock(&lock);
            snprintf(boardMatrix[targetRow][targetCol], MAX_CELL_LENGTH, "%c", w->current_word[i]);
            pthread_mutex_unlock(&lock);
        }
    } else {
        if (w->hidden) { // If word is hidden, show its index
            for (int i = 0; i < w->length; i++) {
                int targetRow = w->row + (w->direction == 'v' ? i : 0);
                int targetCol = w->col + (w->direction == 'h' ? i : 0);
                pthread_mutex_lock(&lock);
                snprintf(boardMatrix[targetRow][targetCol], MAX_CELL_LENGTH, "%d", w->index);
                pthread_mutex_unlock(&lock);
            }
        } else { // Otherwise, show 'X'
            for (int i = 0; i < w->length; i++) {
                int targetRow = w->row + (w->direction == 'v' ? i : 0);
                int targetCol = w->col + (w->direction == 'h' ? i : 0);
                pthread_mutex_lock(&lock);
                snprintf(boardMatrix[targetRow][targetCol], MAX_CELL_LENGTH, "X");
                pthread_mutex_unlock(&lock);
            }
        }
    }
}

void change_word(int word_index) {
    word *w = &my_words[word_index];
    if (!w->guessed && w->num_alternatives > 0) {
        if (w->using_alternative) {
            // Revertir a la palabra y definición original
            strncpy(w->current_word, w->original_word, sizeof(w->current_word) - 1);
            strncpy(w->definition, w->original_definition, sizeof(w->definition) - 1);
            w->using_alternative = 0;
        } else {
            // Cambiar a la palabra alternativa
            int random_index = rand() % w->num_alternatives;
            strncpy(w->current_word, w->alternative_words[random_index], sizeof(w->current_word) - 1);
            strncpy(w->definition, w->alternative_definitions[random_index], sizeof(w->definition) - 1);
            w->using_alternative = 1;
        }

        w->length = strlen(w->current_word);
        printf("La palabra ha cambiado...\n");
        printf("Definición: %s\n", w->definition);

        // Actualización visual en el tablero
        if (!w->hidden) {
            for (int i = 0; i < w->length; i++) {
                int targetRow = w->row + (w->direction == 'v' ? i : 0);
                int targetCol = w->col + (w->direction == 'h' ? i : 0);
                pthread_mutex_lock(&lock);
                snprintf(boardMatrix[targetRow][targetCol], MAX_CELL_LENGTH, "%c", w->current_word[i]);
                pthread_mutex_unlock(&lock);
            }
        }
    }
}



void *timer_function(void *arg) {
    struct timespec delay;
    delay.tv_sec = 20; // 20 seconds
    delay.tv_nsec = 0;

    while (keep_playing) {
        nanosleep(&delay, NULL); // Timer for changing words
        pthread_mutex_lock(&lock);
        for (int i = 0; i < 6; i++) {
            if (!my_words[i].guessed) {
                change_word(i);
            }
        }
        pthread_mutex_unlock(&lock);
    }
    pthread_exit(NULL);
}

void sig_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nEl juego ha sido interrumpido, ¿realmente quieres terminarlo?: ");
        char answer = getchar();
        if (answer == 'y' || answer == 'Y') {
            keep_playing = 0;
        }
    }
}

void setup_signals() {
    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
}

void showBoard() {
    for (int i = 0; i < rowsBM; i++) {
        for (int j = 0; j < columnsBM; j++) {
            int intersection = 0;
            char cellContent[MAX_CELL_LENGTH] = "";
            for (int k = 0; k < sizeof(my_words) / sizeof(my_words[0]); k++) {
                if (!my_words[k].guessed) {
                    int startRow = my_words[k].row;
                    int startCol = my_words[k].col;
                    int endRow = startRow + (my_words[k].direction == 'v' ? my_words[k].length : 0);
                    int endCol = startCol + (my_words[k].direction == 'h' ? my_words[k].length : 0);
                    if (i >= startRow && i < endRow && j >= startCol && j < endCol) {
                        if (intersection) {
                            strncat(cellContent, "/", sizeof(cellContent) - strlen(cellContent) - 1);
                        }
                        char wordIndex[3];
                        snprintf(wordIndex, sizeof(wordIndex), "%d", k);
                        strncat(cellContent, wordIndex, sizeof(cellContent) - strlen(cellContent) - 1);
                        intersection = 1;
                    }
                }
            }
            if (!intersection) {
                strncpy(cellContent, boardMatrix[i][j], sizeof(cellContent) - 1);
            }
            printf("%s ", cellContent);
        }
        printf("\n");
    }
}

void userInput() {
    int wordIndex;
    char guess[10];

    printf("Checking available words to guess:\n");
    for (int i = 0; i < sizeof(my_words) / sizeof(my_words[0]); i++) {
        if (!my_words[i].guessed) {
            printf("%d: %s\n", i + 1, my_words[i].definition);
        }
    }

    printf("Select the number of the word you want to guess based on the definition provided: ");
    scanf("%d", &wordIndex);
    getchar();

    if (wordIndex > 0 && wordIndex <= sizeof(my_words) / sizeof(my_words[0]) && !my_words[wordIndex - 1].guessed) {
        printf("Enter your guess for the definition '%s': ", my_words[wordIndex - 1].definition);
        fgets(guess, sizeof(guess), stdin);
        guess[strcspn(guess, "\n")] = 0; // Remove newline

        if (strcasecmp(my_words[wordIndex - 1].current_word, guess) == 0) {
            my_words[wordIndex - 1].guessed = 1;
            update_board_for_word(&my_words[wordIndex - 1]);
            printf("Correct! The word has been added to the board.\n");
        } else {
            printf("Incorrect, please try again.\n");
        }
    } else {
        printf("Invalid word number or already guessed.\n");
    }
}

int main() {
    pthread_mutex_init(&lock, NULL);
    setup_signals();
    pthread_create(&timer_thread, NULL, timer_function, NULL);

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

    init_words();
    clearMatrix();

    while (keep_playing) {
        showBoard();
        userInput();
        if (allGuessed()) break;
    }

    pthread_join(timer_thread, NULL);
    pthread_mutex_destroy(&lock);
    return 0;
}