#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

struct thread_arg {
    int from;
    int to;
    int id;
    pthread_t thread;
};

typedef struct node {
  int vertex;
  long double value;
  int incomingEdges;
  int outgoingEdges;
  struct node* next;
}node;

typedef struct Graph {
  int numVertices;
  node** adjLists;
}Graph;

int NUM_THREADS;
pthread_barrier_t barrier; 
Graph* graph = NULL;


void *do_work(void* voidarg) {
    struct thread_arg *arg = (struct thread_arg *)voidarg;
    for(int k=0; k<50; k++){
      for(int i = arg->from; i <= arg->to; i++) {
          // printf("thread id: %d check graph position: %d\n" , arg->id , i);
          long double sum = 0.0;
          node *neigh = graph->adjLists[i];
          // if(!isdigit(neigh->value)) neigh->value = 1.0;
          for(int j=0; j<graph->adjLists[i]->incomingEdges; j++){
            // printf("id = %d neig = %d\n" , i , j);
            // printf("neight value = %Lf \n" , neigh->value);
            sum += neigh->value / (double)neigh->outgoingEdges;
            neigh = neigh->next;
            // printf("sum = %Lf\n" , sum);
          }
          graph->adjLists[i]->value = 0.15 + 0.85*sum;
          // printf("new id = %d\n" , i);
      }
      pthread_barrier_wait(&barrier);
    }
    // printf("hello");
}

node* createNode(int v) {
  node* newNode = (node*)malloc(sizeof(node));
  newNode->vertex = v;
  newNode->value = 1.0;
  newNode->incomingEdges = 0;
  newNode->outgoingEdges = 0;
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
  // node* newNode = createNode(d);
  // newNode->next = graph->adjLists[s];
  // newNode->outgoingEdges = graph->adjLists[s]->outgoingEdges;
  // newNode->incomingEdges = graph->adjLists[s]->incomingEdges;
  // graph->adjLists[s] = newNode;
  // graph->adjLists[s]->outgoingEdges++;
  // graph->adjLists[d]->incomingEdges++;

  // Add edge from d to s
  // newNode = createNode(s);
  // newNode->next = graph->adjLists[d];
  // graph->adjLists[d]->next = newNode;
  node* newNode = createNode(s);
  newNode->next = graph->adjLists[d];
  newNode->outgoingEdges = graph->adjLists[d]->outgoingEdges;
  newNode->incomingEdges = graph->adjLists[d]->incomingEdges;
  graph->adjLists[d] = newNode;

  graph->adjLists[s]->outgoingEdges++;
  graph->adjLists[d]->incomingEdges++;
}

void printGraph(Graph* graph) {
  int v;
  for (v = 0; v < graph->numVertices; v++) {
    struct node* temp = graph->adjLists[v];
    printf("\n Vertex %d have incoming %d and outcoming %d\n and value %Lf: ", v , graph->adjLists[v]->incomingEdges , graph->adjLists[v]->outgoingEdges , graph->adjLists[v]->value);
    while (temp) {
      printf("%d -> ", temp->vertex);
      temp = temp->next;
    }
    printf("\n");
  }
}

void createCSV(Graph *graph){
  FILE *f;
  f = fopen("results.csv" , "w+");
  fprintf(f , "node value\n");
  for(int i=0; i<graph->numVertices; i++){
    fprintf(f , "%d %Lf\n", i , graph->adjLists[i]->value);
  }
  fclose(f);
}

int main(int argc , char** argv) {
  char* fileName = argv[1];
  int NUM_THREADS = atoi(argv[2]);
  pthread_barrier_init (&barrier, NULL, NUM_THREADS);
  char line[200];
  FILE* file = fopen(fileName, "r");
  if(!file){
      printf("\n Unable to open : %s ", fileName);
      return -1;
  }
  long src, dst, vertices=0;

  while (fgets(line, sizeof(line), file)) {
    if(line[0]=='#')continue;
    sscanf(line, "%ld %ld",&src,&dst);
    if(src > vertices) vertices=src;
    if(dst > vertices) vertices=dst;
  }
  vertices++;
  printf("%ld\n",vertices );
  fclose(file);

  graph = createAGraph(vertices);

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




  struct thread_arg thread_arg[NUM_THREADS];
  int from = 0, to = vertices-1;
  int step = to / NUM_THREADS;
  int i ;
  for(i = 0; i < NUM_THREADS; i++) {
      thread_arg[i].from = from;
      thread_arg[i].to = (i < NUM_THREADS-1) ? (from + step) : to;
      thread_arg[i].id = i;
      // printf("thread %d from=%d , to=%d \n" , thread_arg[i].id , thread_arg[i].from , thread_arg[i].to);
      from = thread_arg[i].to+1;
      pthread_create(&thread_arg[i].thread, NULL, &do_work, &thread_arg[i]);
  }

  for(i = 0; i < NUM_THREADS; i++) {
    pthread_join(thread_arg[i].thread, NULL);
  }

  // printGraph(graph);
  createCSV(graph);
  return 0;
}