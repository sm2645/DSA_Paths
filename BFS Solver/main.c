#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Node representing a board state during BFS traversal
typedef struct BFSNode {
    int *currBoard;
    struct LLNode *prevMove;
    char Color;
    int Distance;
    int movedTileNumb;
} BFSNode;

// Node to represent a board and its move history
typedef struct LLNode {
    int *currBoard;
    struct LLNode *prevMove;
    int movedTileNumb;
} LLNode;

// Queue node wrapping an LLNode
typedef struct ListNode {
    LLNode *llNode;
    struct ListNode *next;
} ListNode;

// Linked list based queue structure
typedef struct LLQueue {
    ListNode *head;
    ListNode *tail;
} LLQueue;

// Initialize a linked list queue
LLQueue* initializeLLQueue() {
    LLQueue *queue = (LLQueue *)malloc(sizeof(LLQueue));
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

// Enqueue a new node into the queue
void enqueueLLNode(LLQueue *queue, LLNode *newNode) {
    ListNode *newListNode = (ListNode *)malloc(sizeof(ListNode));
    newListNode->llNode = newNode;
    newListNode->next = NULL;

    if (queue->tail == NULL) {
        queue->head = newListNode;
        queue->tail = newListNode;
    } else {
        queue->tail->next = newListNode;
        queue->tail = newListNode;
    }
}

// Dequeue a node from the queue
LLNode* dequeueLLNode(LLQueue *queue) {
    if (queue->head == NULL) {
        printf("Failed to dequeue from an empty queue!\n");
        exit(-1);
    }
    ListNode *tempNode = queue->head;
    LLNode *dequeuedNode = tempNode->llNode;
    queue->head = queue->head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    free(tempNode);
    return dequeuedNode;
}

// Check if queue is empty
int isLLQueueEmpty(LLQueue *queue) {
    return (queue->head == NULL);
}

// Check if puzzle board is solvable
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

#define HASH_TABLE_SIZE 100003

// Hash node for storing board states
typedef struct HashNode {
    int *board;
    struct HashNode *next;
} HashNode;

typedef struct HashTable {
    HashNode *table[HASH_TABLE_SIZE];
} HashTable;

// Simple hash function for board state
unsigned int hashFunction(int *board, int k) {
    unsigned int hash = 0;
    for (int i = 0; i < k * k; i++) {
        hash = hash * 31 + board[i];
    }
    return hash % HASH_TABLE_SIZE;
}

// Insert a board into the hash table
void insertHashTable(HashTable *ht, int *board, int k) {
    unsigned int hash = hashFunction(board, k);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    newNode->board = (int *)malloc(k * k * sizeof(int));
    memcpy(newNode->board, board, k * k * sizeof(int));
    newNode->next = ht->table[hash];
    ht->table[hash] = newNode;
}

// Check if a board already exists in hash table
int containsHashTable(HashTable *ht, int *board, int k) {
    unsigned int hash = hashFunction(board, k);
    HashNode *current = ht->table[hash];
    while (current != NULL) {
        if (memcmp(current->board, board, k * k * sizeof(int)) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Reconstruct path from goal to start and output the move sequence
void reconstructPath(LLNode *goalNode, FILE *fp_out) {
    int moves[1000], moveCount = 0;
    LLNode *currentNode = goalNode;

    while (currentNode->prevMove != NULL) {
        moves[moveCount++] = currentNode->movedTileNumb;
        currentNode = currentNode->prevMove;
    }

    fprintf(fp_out, "#moves\n");
    for (int i = moveCount - 1; i >= 0; i--) {
        fprintf(fp_out, "%d ", moves[i]);
    }
    fprintf(fp_out, "\n");
}

// Find the index of the empty tile (0)
int findEmptyTile(int *board, int k) {
    for (int i = 0; i < k * k; i++) {
        if (board[i] == 0) {
            return i;
        }
    }
    return -1;
}

// Helper to check move boundaries
int isValidMove(int row, int col, int k) {
    return (row >= 0 && row < k && col >= 0 && col < k);
}

// Swap two tiles in board array
void swapTiles(int *board, int i, int j) {
    int temp = board[i];
    board[i] = board[j];
    board[j] = temp;
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

    if (!isSolvable(initial_board, k)) {
        fprintf(fp_out, "#moves\nno solution\n");
        fclose(fp_out);
        return 0;
    }

    LLQueue *queue = initializeLLQueue();
    LLNode *initialNode = (LLNode *)malloc(sizeof(LLNode));
    initialNode->currBoard = (int *)malloc(k * k * sizeof(int));
    memcpy(initialNode->currBoard, initial_board, k * k * sizeof(int));
    initialNode->prevMove = NULL;
    initialNode->movedTileNumb = -1;
    enqueueLLNode(queue, initialNode);

    HashTable ht;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        ht.table[i] = NULL;
    }
    insertHashTable(&ht, initial_board, k);

    int goal_board[k * k];
    for (int i = 0; i < k * k - 1; i++) {
        goal_board[i] = i + 1;
    }
    goal_board[k * k - 1] = 0;

    while (!isLLQueueEmpty(queue)) {
        LLNode *currentNode = dequeueLLNode(queue);

        if (memcmp(currentNode->currBoard, goal_board, k * k * sizeof(int)) == 0) {
            reconstructPath(currentNode, fp_out);
            fclose(fp_out);
            return 0;
        }

        int emptyIndex = findEmptyTile(currentNode->currBoard, k);
        int directions[4] = {-k, k, -1, 1};

        for (int i = 0; i < 4; i++) {
            int newIndex = emptyIndex + directions[i];
            if (newIndex >= 0 && newIndex < k * k) {
                if ((emptyIndex % k == 0 && directions[i] == -1) || 
                    (emptyIndex % k == k - 1 && directions[i] == 1)) {
                    continue;
                }

                int *newBoard = (int *)malloc(k * k * sizeof(int));
                memcpy(newBoard, currentNode->currBoard, k * k * sizeof(int));
                swapTiles(newBoard, emptyIndex, newIndex);

                if (!containsHashTable(&ht, newBoard, k)) {
                    LLNode *newNode = (LLNode *)malloc(sizeof(LLNode));
                    newNode->currBoard = newBoard;
                    newNode->prevMove = currentNode;
                    newNode->movedTileNumb = currentNode->currBoard[newIndex];

                    enqueueLLNode(queue, newNode);
                    insertHashTable(&ht, newBoard, k);
                } else {
                    free(newBoard);
                }
            }
        }
    }

    fprintf(fp_out, "#moves\nno solution\n");
    fclose(fp_out);
    return 0;
}
