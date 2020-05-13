#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int next_id = 1;
int ticket_amount = 240;
sem_t lock;
sem_t lines;
sem_t operators;
sem_t purchase;
int caller_amount;
int cnt = 0;
void* phonecall(void* vargp);

int main(int argc, char const *argv[]) {

  if(sem_init(&lock, 0, 1)==-1){ //initialize binary semaphore to protect next_id
    printf("error sem_init\n");
  }
  if(sem_init(&lines, 0, 5)==-1){ //initialize quinary semaphore to protect 5 phone lines
    printf("error sem_init\n");
  }
  if(sem_init(&operators, 0, 3)==-1){ //initialize trinary semaphore to protect 3 operators
    printf("error sem_init\n");
  }
  if(sem_init(&purchase, 0, 1)==-1){ //initialize binary semaphore to protect purchase counter
    printf("error sem_init\n");
  }
  if(argc>1){ //checks for addtional arguments
    caller_amount = atoi(argv[1]); // converts arg to number of int callers
  }else{
    printf("you didnt enter an amount of callers\n"); // error if nothing is entered
    return 0;
  }
  pthread_t tid[caller_amount]; //pthread ID array
  for (int i = 0; i < caller_amount; i++) {
    if(pthread_create(&tid[i], NULL, phonecall, NULL)>0){ // creates pthreads per #of callers
      printf("error pthread_create\n"); // or prints error if fault
    }
  }
  for (int i = 0; i < caller_amount; i++) {
    if(pthread_join(tid[i], NULL) != 0) { // collects all the terminated threads
        printf("error pthread_join\n"); // or prints error if thread had fault
    }
  }
  if(sem_destroy(&lock)==-1){ // destroy semaphore for clean up
    printf("error sem_destroy\n");
  }
  if(sem_destroy(&lines)==-1){ // destroy semaphore for clean up
    printf("error sem_destroy\n");
  }
  if(sem_destroy(&operators)==-1){ // destroy semaphore for clean up
    printf("error sem_destroy\n");
  }
  if(sem_destroy(&purchase)==-1){ // destroy semaphore for clean up
    printf("error sem_destroy\n");
  }
  //tests
  printf("callers %d\n",cnt); //shows amount of threads to exicute
  printf("tickets left %d\n",ticket_amount); // shows ticket left over
  printf("tickets sold %d\n",240-ticket_amount ); // shows tickets sold
  return 0;
}

void* phonecall(void* vargp) {
  const static int NUM_OPERATORS = 3;
  const static int NUM_LINES = 5;
  int id; // phone call id
  int sval; // holds number of available lines
  sem_wait(&lock); //unlock next_id
  id = next_id++; //phone call ID = to next_id + 1
  sem_post(&lock); //lock next_id
  while(sem_getvalue(&lines,&sval)==0 && (0 == abs(sval))){ //while no lines are available...
    printf("Thread [%d] is calling line, busy signal\n",id); //display busy
    sleep(1);//pause 3 sec
  }
  sem_wait(&lines); // unlock available line
  printf("Thread [%d] has available line, call ringing\n",id); // display connection
  sem_wait(&operators); // wait for operator
  printf("Thread [%d] is speaking to operator\n", id ); //display operator interaction
  //sleep(3);// pause 3 sec
  printf("Thread [%d] has bought a ticket!\n", id );  // ticket bought
  sem_wait(&purchase); //unlock purchase
  if(ticket_amount>0){ // if ticket available
    ticket_amount--; // substact from total tickets
  }else{
    printf("sorry caller [%d] we are out of tickets\n",id); //else say we out
  }
  sem_post(&purchase); //lock purchase
  sem_post(&operators); // return one operator to available
  printf("Thread [%d] has hung up!\n", id ); //hang up
  cnt++; // add to # of calls recieved and processed
  sem_post(&lines); //open line for waiting callers
}
