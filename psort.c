#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>

typedef struct {
  char *records;
  size_t startIndex;
  size_t endIndex;
} BlockInfo;

typedef struct {
  char *record;
  size_t blockID;
} Node;

typedef struct {
  Node *nodes;
  size_t size;
  size_t capacity;
} MinHeap;

int compare(const void *a, const void *b) {
  return memcmp(a, b, 4); // Compares by the 4 byte key
}

void *sort(void *theData) {
  BlockInfo *data = (BlockInfo *)theData;
  char *start = data->records + data->startIndex * 100;
  size_t blockSize = (data->endIndex - data->startIndex + 1) * 100;

  qsort(start, blockSize / 100, 100, compare);
  return NULL;
}

// Heapify down and up make sure that the properties of the min heap are followed
void heapifyDown(MinHeap *heap, size_t index) {
  size_t smallest = index;
  size_t left = 2 * index + 1;
  size_t right = 2 * index + 2;

  if (left < heap->size && compare(heap->nodes[left].record, heap->nodes[smallest].record) < 0) {
    smallest = left;
  }
  if (right < heap->size && compare(heap->nodes[right].record, heap->nodes[smallest].record) < 0) {
    smallest = right;
  }
  if (smallest != index) {
    Node temp = heap->nodes[index];
    heap->nodes[index] = heap->nodes[smallest];
    heap->nodes[smallest] = temp;
    heapifyDown(heap, smallest);
  }
}

void heapifyUp(MinHeap *heap, size_t index) {
  if (index == 0) return;

  size_t parent = (index - 1) / 2;
  if (compare(heap->nodes[index].record, heap->nodes[parent].record) < 0) {
    Node temp = heap->nodes[index];
    heap->nodes[index] = heap->nodes[parent];
    heap->nodes[parent] = temp;
    heapifyUp(heap, parent);
  }
}

// inserts node into heap
void insert(MinHeap *heap, Node node) {
  if (heap->size == heap->capacity) {
    printf("Heap overflow\n");
    return;
  }
  heap->nodes[heap->size] = node;
  heapifyUp(heap, heap->size);
  heap->size++;
}

// returns the smallest current record
Node extractMin(MinHeap *heap) {
  Node root = heap->nodes[0];
  size_t lastIndex = heap->size - 1;
  heap->nodes[0] = heap->nodes[lastIndex];
  heap->size--;
  heapifyDown(heap, 0);
  return root;
}

int main(int argc, char *argv[]) {
  clock_t start, end;
  double cpu_time_used;
  start = clock();
  if (argc != 3) {
    fprintf(stderr, "Incorrect usage");
    return 0;
  }

  char *inputFile = argv[1];
  char *outputFile = argv[2];

  int fd = open(inputFile, O_RDONLY);
  if (fd == -1) {
    perror("Failed to open input file");
    return 0;
  }

  struct stat sb;
  fstat(fd, &sb);
  size_t fileSize = sb.st_size;

  // Memory map the file
  char *mapped = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  close(fd);

  // Creating & executing threads
  size_t numberOfRecords = fileSize / 100;
  int numberOfThreads = get_nprocs();
  //int numberOfThreads = 1;
  size_t recordsPerThread = numberOfRecords / numberOfThreads;
  size_t remainingRecords = numberOfRecords % numberOfThreads;

  pthread_t threads[numberOfThreads];
  BlockInfo *blockData = malloc(numberOfThreads * sizeof(BlockInfo));

  for (int i = 0; i < numberOfThreads; ++i) {
    blockData[i].records = mapped;
    blockData[i].startIndex = i * recordsPerThread;
    if (i < remainingRecords) {
      blockData[i].startIndex += i;
    } else {
      blockData[i].startIndex += remainingRecords; // also have to account for the remainder records
    }
    blockData[i].endIndex = blockData[i].startIndex + recordsPerThread - 1;
    if (i < remainingRecords) {
      blockData[i].endIndex += 1;
    }
    pthread_create(&threads[i], NULL, sort, &blockData[i]);
  }

  for (int i = 0; i < numberOfThreads; ++i) {
    pthread_join(threads[i], NULL);
  }

  // Creating the heap
  Node *heapNodes = malloc(numberOfThreads * sizeof(Node));
  MinHeap heap;
  heap.nodes = heapNodes;
  heap.size = 0;
  heap.capacity = numberOfThreads;
  size_t *currentPositions = calloc(numberOfThreads, sizeof(size_t));

  for (int i = 0; i < numberOfThreads; ++i) {
    char *startOfRecord = mapped + blockData[i].startIndex * 100;
    Node node;
    node.record = startOfRecord;
    node.blockID = i;
    insert(&heap, node);
  }

  int openedOutputFile = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

  // Merging the sections of sorted records together
  while (heap.size > 0) {
    Node smallest = extractMin(&heap);
    if (write(openedOutputFile, smallest.record, 100) != 100) {
      close(openedOutputFile);
      return 0;
    }

    size_t blockID = smallest.blockID;
    currentPositions[blockID] = currentPositions[blockID] + 1;
    size_t nextIndex = currentPositions[blockID] + blockData[blockID].startIndex;

    if (nextIndex <= blockData[blockID].endIndex) {
      Node theNode;
      theNode.record = mapped + nextIndex * 100;
      theNode.blockID = blockID;
      insert(&heap, theNode);
    }
  }

  fsync(openedOutputFile);
  close(openedOutputFile);

  // Free memory
  free(heap.nodes);
  free(currentPositions);
  free(blockData);
  munmap(mapped, fileSize);

  // Calculate the time it took to execute the program
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Time taken: %f seconds\n", cpu_time_used);

  return 0;
}
