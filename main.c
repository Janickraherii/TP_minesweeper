#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 6

int score = 0; // Initialisation du score
int life = 2; // Déclaration de la variable life

// nombre de mines
int numMines = 20;
bool gameJustStarted = true;
bool playerLost = false;

// Déclaration de la structure pour la liste chaînée
typedef struct Node {
    int row;
    int col;
    struct Node* next;
} Node;

Node* head = NULL;

void markAsVisited(int row, int col);
int isVisited(int row, int col); // Déclaration du prototype de la fonction

void playGame(char realBoard[][SIZE], char playerBoard[][SIZE], int *life);
char countAdjacentMines(char realBoard[][SIZE], int row, int col);
char playerName[50]; // nom du joueur
void printBoard(char board[][SIZE]);
bool checkWin(char realBoard[][SIZE], char playerBoard[][SIZE]);
void displayRecords();

void showRules() {
    printf("🕹️ La règle du jeu est simple :\n");
    printf("🧩 Le joueur sélectionne une case.\n");
    printf("💣 Évite les cases piégées.\n");
    printf("🚩 Marque les cases dangereuses.\n");
    printf("───────────────────────\n");
}

void showMenu() {
    printf("1/Jouer \n2/ Charger une partie\n3/ Records\n");
}

void chooseMenu(int choice, char realBoard[][SIZE], char playerBoard[][SIZE], int *life) {
    switch (choice) {
        case 1:
            printf("----Start 🚩🚩----\n");
            char playerName[50];
            printf("Entrez votre pseudo: ");
            scanf("%s", playerName);
            printf("C'est parti %s ! \n", playerName);
            playGame(realBoard, playerBoard, life);
            break;
        case 2:
            printf("Tu as choisi de CHARGER UNE PARTIE\n");
            break;
        case 3:
            printf("Tu as choisi RECORD : Je vais t'afficher les records\n");
            FILE *file = fopen("savegame.txt", "r");
            if (file == NULL) {
                printf("Impossible d'ouvrir le fichier de sauvegarde\n");
                break;
            }

            printf("Records:\n");
            int score;

            // Lire chaque ligne du fichier
            while (fscanf(file, "%s %d", playerName, &score) == 2) {
                printf("Joueur: %s - Score: %d\n", playerName, score);
            }

            fclose(file);

            printf("\nAppuyez sur '0' pour retourner au menu principal: ");
            int choice;
            scanf("%d", &choice);
            if (choice == 0) {
                printf("\n");
                chooseMenu(0, realBoard, playerBoard, life); // Appel récursif avec le choix 0
                return; // Sortir de la fonction et retourner au menu principal
            }
            break;
        default:
            printf("Choisis un des options pour continuer: 1/Jouer \n2/ Charger une partie \n");
            break;
    }
}

void saveGame(int life, char playerBoard[SIZE][SIZE]) {
    FILE *file = fopen("savegame.txt", "w");
    if (file == NULL) {
        printf("Erreur d'ouverture du fichier de sauvegarde!\n");
        return;
    }

    // Sauvegarder vie
    fprintf(file, "Life: %d\n", life);

    // Sauvegarder l'état du tableau de jeu
    fprintf(file, "Game Board:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fprintf(file, "%c ", playerBoard[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Partie sauvegardée avec succès!\n");
}

void displayRecords() {
    FILE *file = fopen("records.txt", "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier de records\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

// Fonction pour vérifier si '-' existe dans playerBoard
bool containsDash(char playerBoard[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (playerBoard[i][j] == '-') {
                return true;
            }
        }
    }
    return false;
}

int main() {
    char realBoard[SIZE][SIZE];
    char playerBoard[SIZE][SIZE];

    // initialisation du tableau avec mines (realBoard) et du tableau vu par le joueur (playerBoard)

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            realBoard[i][j] = '0';
            playerBoard[i][j] = '-';
        }
    }
    //générateur de nombres aléatoires
    srand(time(0));
    // Générer des positions aléatoires pour 'M'
    for (int i = 0; i < numMines; i++) {
        int randomRow, randomCol;
        do {
            randomRow = rand() % SIZE;
            randomCol = rand() % SIZE;
        } while (realBoard[randomRow][randomCol] == 'M');
        realBoard[randomRow][randomCol] = 'M';
    }

    showRules();
    showMenu();
    int choice;
    do {
        scanf("%d", &choice);
        chooseMenu(choice, realBoard, playerBoard, &life);
    } while (choice < 1 || choice > 2);
    playGame(realBoard, playerBoard, &life);

    return 0;
}

void markAsVisited(int row, int col) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->row = row;
    newNode->col = col;
    newNode->next = head;
    head = newNode;
}

int isVisited(int row, int col) {
    Node* temp = head;
    while (temp != NULL) {
        if (temp->row == row && temp->col == col) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

void playGame(char realBoard[][SIZE], char playerBoard[][SIZE], int *life) {
    int row, col;
    bool gameOver = false;

    // Afficher "GRILLE" avant le playerBoard
    printf("---- GRILLE ----\n");
    printBoard(playerBoard); // Afficher playerBoard avant de demander les coordonnées

    while (*life > 0 && !gameOver && containsDash(playerBoard)) {
        do {
            printf("Entrez les coordonnées sous la forme 'ligne colonne' avec espace : ");
            scanf("%d %d", &row, &col);
            if (isVisited(row, col)) {
                printf("Vous avez déjà utilisé cette combinaison de chiffres. Veuillez en entrer une nouvelle.\n");
            }
        } while (row < 0 || row >= SIZE || col < 0 || col >= SIZE || isVisited(row, col));

        markAsVisited(row, col);

        if (realBoard[row][col] == 'M') {
            printf("BOOM! Vous avez frappé une mine, vous perdez une vie.\n");
            printf("Démineur:\n");
            playerBoard[row][col] = 'M'; // Remplacer '-' par 'M' dans le playerBoard
            printBoard(playerBoard);
            (*life)--;
            printf("Il vous reste %d vies.\n", *life);
            if (*life == 0) { // Si le joueur n'a plus de vie, définir gameOver à true
                gameOver = true;
            }
        } else {
            playerBoard[row][col] = countAdjacentMines(realBoard, row, col);
            printBoard(playerBoard);

            // Incrément du score si le joueur ne tombe pas sur une mine
            score++;
            printf("Votre score est maintenant de : %d\n", score);

            if (checkWin(realBoard, playerBoard)) {
                printf("Félicitations, vous avez gagné!\n");
                printf("Votre score final est : %d\n", score);

                printf("Voulez-vous sauvegarder votre partie ? (O/N)\n");
                char choice;
                scanf(" %c", &choice); // Lire un seul caractère et ignorer les espaces
                if (choice == 'O' || choice == 'o') {
                    saveGame(score, playerBoard);
                    printf("Votre partie a été sauvegardée.\n");
                } else {
                    printf("Votre partie n'a pas été sauvegardée.\n");
                }
                gameOver = true; // Définir gameOver à true si le joueur gagne
            }
        }
    }

    if (*life == 0 && !gameOver) {
        printf("Désolé, vous avez perdu.\n");
        printf("Voulez-vous sauvegarder votre score ? (O/N)\n");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'O' || choice == 'o') {
            saveGame(score, playerBoard);
            printf("Votre score a été sauvegardé.\n");
        }
        
        gameOver = true;
    }
}


char countAdjacentMines(char realBoard[][SIZE], int row, int col) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (row + i >= 0 && row + i < SIZE && col + j >= 0 && col + j < SIZE && realBoard[row + i][col + j] == 'M') {
                count++;
            }
        }
    }
    return count;
}

void printBoard(char board[][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

bool checkWin(char realBoard[][SIZE], char playerBoard[][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (realBoard[i][j] != 'M' && playerBoard[i][j] == '-') {
                return false;
            }
        }
    }
    return true;
}
