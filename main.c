#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct thread_arg {
    int from;
    int to;
    int id;
    pthread_t thread;
};

typedef struct node {
  int vertex;
  double value;
  int incomingEdges;
  int outgoingEdges;
  pthread_mutex_t lock;
  struct node* next;
}node;

typedef struct Graph {
  int numVertices;
  node** adjLists;
}Graph;

#define NUM_THREADS 2
int A[10000] = {1,2,3,4,5,6};
int B[6] = {1,2,3,4,5,6};
int C[6];
Graph* graph = NULL;

void *do_work(void* voidarg) {
    struct thread_arg *arg = (struct thread_arg *)voidarg;
    for(int i = arg->from; i < arg->to; i++) {
        C[i] = A[i] + B[i];
        printf("thread id: %d check graph position: %d\n" , arg->id , i);
    }
}

node* createNode(int v) {
  node* newNode = (node*)malloc(sizeof(node));
  newNode->vertex = v;
  newNode->value = 1.0;
  newNode->incomingEdges = 0;
  newNode->outgoingEdges = 0;
  pthread_mutex_init(&newNode->lock,NULL);
  newNode->next = NULL;
  return newNode;
}

struct Graph* createAGraph(int vertices) {
  struct Graph* graph = (Graph*)malloc(sizeof(Graph));
  graph->numVertices = vertices;

  graph->adjLists = (node**)malloc(vertices * sizeof(node*));

  int i;
  for (i = 0; i < vertices; i++){
    // graph->adjLists[i] = NULL;
    graph->adjLists[i] = createNode(i);
  }
  return graph;
}

void addEdge(Graph* graph, int s, int d) {
  // Add edge from s to d
  node* newNode = createNode(d);
  newNode->next = graph->adjLists[s];
  newNode->outgoingEdges = graph->adjLists[s]->outgoingEdges;
  newNode->incomingEdges = graph->adjLists[s]->incomingEdges;
  graph->adjLists[s] = newNode;
  graph->adjLists[s]->outgoingEdges++;
  graph->adjLists[d]->incomingEdges++;

  // Add edge from d to s
  // newNode = createNode(s);
  // newNode->next = graph->adjLists[d];
  // graph->adjLists[d]->next = newNode;
}

void printGraph(Graph* graph) {
  int v;
  for (v = 0; v < graph->numVertices; v++) {
    struct node* temp = graph->adjLists[v];
    printf("\n Vertex %d have incoming %d and outcoming %d\n: ", v , graph->adjLists[v]->incomingEdges , graph->adjLists[v]->outgoingEdges);
    while (temp) {
      printf("%d -> ", temp->vertex);
      temp = temp->next;
    }
    printf("\n");
  }
}

int main(int argc , char** argv) {
  char* fileName = argv[1];
  // int numberThreads = atoi(argv[2]);
  char line[200];
  FILE* file = fopen(fileName, "r");
  if(!file){
      printf("\n Unable to open : %s ", fileName);
      return -1;
  }
  long src, dst, vertices;

  while (fgets(line, sizeof(line), file)) {
    if(line[0]=='#')continue;
    sscanf(line, "%ld %ld",&src,&dst);
    if(src > vertices) vertices=src;
    if(dst > vertices) vertices=dst;
  }
  vertices++;
  printf("%ld\n",vertices );
  fclose(file);

  Graph *graph = createAGraph(vertices);

  file = fopen(fileName , "r");
  if(!file){
      printf("\n Unable to open : %s ", fileName);
      return -1;
  }

  while (fgets(line, sizeof(line), file)) {
    if(line[0]=='#')continue;
    sscanf(line, "%ld %ld",&src,&dst);
    addEdge(graph , src , dst);
  }
  fclose(file);

  printGraph(graph);




  // struct thread_arg thread_arg[NUM_THREADS];
  // int from = 0, to = vertices;
  // int step = to / NUM_THREADS;
  // int i ;
  // for(i = 0; i < NUM_THREADS; i++) {
  //     thread_arg[i].from = from;
  //     thread_arg[i].to = (i < NUM_THREADS-1) ? (from + step) : to;
  //     thread_arg[i].id = i;
  //     from = thread_arg[i].to;
  //     pthread_create(&thread_arg[i].thread, NULL, &do_work, &thread_arg[i]);
  // }

  // for(i = 0; i < NUM_THREADS; i++) {
  //   pthread_join(thread_arg[i].thread, NULL);
  // }

  // for(int i=0; i<6; i++){
  //     printf("%d" , C[i]);
  // }
    return 0;
}