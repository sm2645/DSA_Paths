#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define HASH_TABLE_SIZE 100003
#define MAX_HEAP_SIZE 100000

typedef struct Node {
    int* board;
    int g;
    int h;
    int priority;
    struct Node* prevMove;
    int movedTileNumb;
} Node;

typedef struct HashNode {
    int* board;
    int g;
    struct HashNode* next;
} HashNode;

HashNode* ht[HASH_TABLE_SIZE];
int* target_row;
int* target_col;

// Hash function to compute index for a board configuration
unsigned long hashFunction(int* board, int k) {
    unsigned int hash = 0;
    for (int i = 0; i < k * k; i++) {
        hash = hash * 31 + board[i];
    }
    return hash % HASH_TABLE_SIZE;
}

// Insert a board configuration into the hash table
void insertHashTable(int* board, int g, int k) {
    unsigned long hash = hashFunction(board, k);
    HashNode* newNode = malloc(sizeof(HashNode));
    newNode->board = malloc(k * k * sizeof(int));
    memcpy(newNode->board, board, k * k * sizeof(int));
    newNode->g = g;
    newNode->next = ht[hash];
    ht[hash] = newNode;
}

// Check if a board configuration exists in the hash table
int containsHashTable(int* board, int k) {
    unsigned long hash = hashFunction(board, k);
    HashNode* current = ht[hash];
    while (current != NULL) {
        if (memcmp(current->board, board, k * k * sizeof(int)) == 0) {
            return current->g;
        }
        current = current->next;
    }
    return -1;
}

// Free all entries in the hash table
void hashFree() {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* hashEntry = ht[i];
        while (hashEntry != NULL) {
            HashNode* next = hashEntry->next;
            free(hashEntry->board);
            free(hashEntry);
            hashEntry = next;
        }
        ht[i] = NULL;
    }
}

// Compute the Manhattan distance heuristic for a board
int manhattanDistance(int* board, int k) {
    int sum = 0;
    for (int i = 0; i < k*k; i++) {
        if (board[i] == 0) continue;
        int current_row = i / k;
        int current_col = i % k;
        sum += abs(target_row[board[i] - 1] - current_row) + abs(target_col[board[i] - 1] - current_col);
    }
    return sum;
}

// Generate a new Node
Node* GenerateNewBoard(int* board, int g, int h, Node* prevMove, int movedTileNumb, int k) {
    Node* node = malloc(sizeof(Node));
    node->board = malloc(k * k * sizeof(int));
    memcpy(node->board, board, k * k * sizeof(int));
    node->g = g;
    node->h = h;
    node->priority = g + h;
    node->prevMove = prevMove;
    node->movedTileNumb = movedTileNumb;
    return node;
}

// Swap two nodes (helper for heap operations)
void swap(Node** a, Node** b) {
    Node* temp = *a;
    *a = *b;
    *b = temp;
}

// Insert a node into the min-heap
void heapInsert(Node** heap, int* heap_size, Node* node) {
    heap[++(*heap_size)] = node;
    int i = *heap_size;
    while (i > 1 && heap[i/2]->priority > heap[i]->priority) {
        swap(&heap[i], &heap[i/2]);
        i /= 2;
    }
}

// Extract the minimum node from the min-heap
Node* extractMin(Node** heap, int* heap_size) {
    Node* min = heap[1];
    heap[1] = heap[(*heap_size)--];
    int i = 1;
    while (i * 2 <= *heap_size) {
        int child = i * 2;
        if (child + 1 <= *heap_size && heap[child + 1]->priority < heap[child]->priority) {
            child++;
        }
        if (heap[child]->priority < heap[i]->priority) {
            swap(&heap[i], &heap[child]);
            i = child;
        } else {
            break;
        }
    }
    return min;
}

// Check if the puzzle is solvable
int isSolvable(int *board, int k) {
    int inversions = 0;
    int blankRow = 0;
    for (int i = 0; i < k * k; i++) {
        if (board[i] == 0) {
            blankRow = i / k;
            continue;
        }
        for (int j = i + 1; j < k * k; j++) {
            if (board[j] != 0 && board[i] > board[j]) {
                inversions++;
            }
        }
    }
    if (k % 2 == 1) {
        return inversions % 2 == 0;
    } else {
        return (inversions + blankRow) % 2 == 1;
    }
}

// Reconstruct the path from start to goal
void reconstructPath(Node* node, FILE* fp_out) {
    int moves[1000];
    int count = 0;
    while (node->prevMove != NULL) {
        moves[count++] = node->movedTileNumb;
        node = node->prevMove;
    }
    fprintf(fp_out, "#moves\n");
    for (int i = count - 1; i >= 0; i--) {
        fprintf(fp_out, "%d ", moves[i]);
    }
    fprintf(fp_out, "\n");
}

// Find the position of the empty tile
int findEmptyTile(int *board, int k) {
    for (int i = 0; i < k * k; i++) {
        if (board[i] == 0) {
            return i;
        }
    }
    return -1;
}

// Swap two integers
void swapInt(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main(int argc, char **argv) {
    FILE *fp_in, *fp_out;
    fp_in = fopen(argv[1], "r");
    if (fp_in == NULL) {
        printf("Could not open input file.\n");
        return -1;
    }
    fp_out = fopen(argv[2], "w");
    if (fp_out == NULL) {
        printf("Could not open output file.\n");
        return -1;
    }

    char *line = NULL;
    size_t lineBuffSize = 0;
    int k;
    getline(&line, &lineBuffSize, fp_in);
    fscanf(fp_in, "%d\n", &k);
    getline(&line, &lineBuffSize, fp_in);

    int initial_board[k*k];
    for (int i = 0; i < k*k; i++)
        fscanf(fp_in, "%d ", &initial_board[i]);

    fclose(fp_in);

    target_row = malloc(k * k * sizeof(int));
    target_col = malloc(k * k * sizeof(int));
    for (int i = 0; i < k*k; i++) {
        if (i == k*k - 1) {
            target_row[i] = k - 1;
            target_col[i] = k - 1;
        } else {
            target_row[i] = i / k;
            target_col[i] = i % k;
        }
    }

    if (!isSolvable(initial_board, k)) {
        fprintf(fp_out, "#moves\nno solution\n");
        fclose(fp_out);
        return 0;
    }

    Node* heap[MAX_HEAP_SIZE];
    int heap_size = 0;

    Node* start = GenerateNewBoard(initial_board, 0, manhattanDistance(initial_board, k), NULL, -1, k);
    heapInsert(heap, &heap_size, start);
    insertHashTable(initial_board, start->g, k);

    while (heap_size > 0) {
        Node* current = extractMin(heap, &heap_size);

        int goalReached = 1;
        for (int i = 0; i < k*k - 1; i++) {
            if (current->board[i] != i + 1) {
                goalReached = 0;
                break;
            }
        }

        if (goalReached && current->board[k*k - 1] == 0) {
            reconstructPath(current, fp_out);
            hashFree();
            fclose(fp_out);
            return 0;
        }

        if (current->g > containsHashTable(current->board, k)) continue;

        int zeroIndx = findEmptyTile(current->board, k);
        int emptyPosRow = zeroIndx / k;
        int emptyPosCol = zeroIndx % k;

        int moves[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (int i = 0; i < 4; i++) {
            int new_row = emptyPosRow + moves[i][0];
            int new_col = emptyPosCol + moves[i][1];

            if (new_row >= 0 && new_row < k && new_col >= 0 && new_col < k) {
                int newZeroIndx = new_row * k + new_col;
                int new_board[k*k];
                memcpy(new_board, current->board, k * k * sizeof(int));
                swapInt(&new_board[zeroIndx], &new_board[newZeroIndx]);

                int gNew = current->g + 1;
                int h = manhattanDistance(new_board, k);
                int stored_g = containsHashTable(new_board, k);

                if (stored_g == -1 || gNew < stored_g) {
                    Node* neighbor = GenerateNewBoard(new_board, gNew, h, current, new_board[zeroIndx], k);
                    heapInsert(heap, &heap_size, neighbor);
                    insertHashTable(new_board, gNew, k);
                }
            }
        }
    }

    fprintf(fp_out, "#moves\nno solution\n");
    hashFree();
    fclose(fp_out);
    return 0;
}
