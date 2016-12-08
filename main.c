// ****************************** ****************************** ******************************
//
//	Group 8
//	Due Date: Sunday, June 12 2016 11:00 AM
//	
// 	I hereby acknowledge that this program is of OUR own hard work
//	
//
// ****************************** ****************************** ******************************

//ALL OLD CODE GOT SCRAPPED BECAUSE REASONS. BUT DON'T WORRY, THIS IS WORKING THREAD CODE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>
#include <time.h>	//Program Execution Timer
#include <ctype.h>

#include <math.h>


#define NUM_THREADS 8	//This will need to be changed based off # of equations


/*-------------------------------------------------
RPN METHODS/VARS - BEGIN
-------------------------------------------------*/
#define MAXCHR 100

typedef int bool;
enum boolean {
	false,
	true
};


enum errors {
	noError,
	outOfMemory,
	divisionByZero,
	missingOperator,
	missingOperand,
	wrongSymbol,
	leftParensMissing
};

struct node {
	int value;
	struct node *next;
};

struct operator {
	char characterRep;
	int priority;
	char associativity[1];
};

// stack function prototypes
bool push(struct  node *head, int val);
struct node *pop( struct node *head);
int peek( struct node *head);
struct node *initStack (void);
void printStack(struct node *head);
void printOStack(struct node *head);
bool stackIsEmpty(struct node *head);
void emptyStack(struct node *head);

// helper function prototypes
bool validateError(int);
void initOperators(void);
void freeOperators(void);
int getPriorityOf(char s);
char getAssociativity(char operator);
bool isOperator(char s);
char* addNullTerminator(char *s);
bool isOperand(char s);
int convertOperand(int *expIndex, char* expression);
void evaluate(struct node *operandHead, struct node *operatorHead, int*);
int binaryOperate(char operator, int val_1, int val_2, int* errorStat);
int power(int b, int x);
void expressionParser(char* xpn, char *opnA, char *opnB,  char* opr);

/* 
	Global operator structs, used to store/retrieve the user inputted
	priorities and associativities
*/
struct operator *plus;
struct operator *minus;
struct operator *multiply;
struct operator *division;
struct operator *exponent;
struct operator *leftParens;
//struct operator *rigthParens;

/*-------------------------------------------------
RPN METHODS/VARS - END
-------------------------------------------------*/


/*-------------------------------------------------
THREAD CODE BEGIN
-------------------------------------------------*/
//Thread 0 = Master
//All other threads > 0 = Slaves

//Struct THREAD_DATA handles all thread data.
typedef struct 
 {
//   double *A;
//   double *B;
   
   int t_weight[NUM_THREADS];		//Thread equation weight
   int t_solutions[NUM_THREADS];	//Thread Solutions
   
   long long int sum; //Total
	
 } THREAD_DATA;
 
 THREAD_DATA tdata;	//Global Data accessed from different threads

char *messages[NUM_THREADS];

void *grabString(void *threadid)
{
	printf("grabString Working\n");
	long taskid;
	
	sleep(1);
	taskid = (long)threadid;
	printf("Slave %d: %s\n", taskid, messages[taskid]);
	
	tdata.t_solutions[taskid] = solveExpression(messages[taskid]);	

	pthread_exit((void*) threadid);	//Keep running until finished?
}

//Master thread, which spawns slave threads
void *masterThread( void *threadid )
{
	clock_t begin = clock();
	
	char * opr;
	
	/*-----------------------------------
	  char *opA = malloc(sizeof(char) *101);
	  char *opr = malloc(sizeof(char) * 2);
	  char *opB = malloc(sizeof(char) * 101);
	  memset(opA, '\0', sizeof(opA));
	  memset(opB, '\0', sizeof(opB));
	  memset(opr, '\0', sizeof(opr));
		
	  char *expression = "7*8+3*4-3*4-3*6-4+7*8+3*4-3*4-3*6-4*7*8+3*4-3*4-3*6-4";
	  expressionParser(expression, opA, opB, opr);
	  fprintf("original expression %s, and the first operand is %s, second operand is %s, and the operator is %s", expression, opA, opB, opr);
	-----------------------------------*/
	
	//We can use an array holding data. (TEMP)
	//char *t_equations[] = { "(4+(3*7))", "(8/(2^2))" };
	//char *t_equations[] = { "(4+(3*7))", "(8/(2^2))","(4+(3*7))", "(8/(2^2))" };
	char *t_equations[] = { "((((7+2)+4)^6)*7)+(9*(3+8)*((4-2)^(3))*(5-4)*(2+3))" , 
							"((((7+2)+4)^6)*7)+(9*(3+8)*((4-2)^(3))*(5-4)*(2+3))" ,
							"((((7+2)+4)^6)*7)+(9*(3+8)*((4-2)^(3))*(5-4)*(2+3))" , 
							"((((7+2)+4)^6)*7)+(9*(3+8)*((4-2)^(3))*(5-4)*(2+3))" ,
							"((((7+2)+4)^6)*7)+(9*(3+8)*((4-2)^(3))*(5-4)*(2+3))" , 
							"((((7+2)+4)^6)*7)+(9*(3+8)*((4-2)^(3))*(5-4)*(2+3))" ,
							"((((7+2)+4)^6)*7)+(9*(3+8)*((4-2)^(3))*(5-4)*(2+3))" , 
							"((((7+2)+4)^6)*7)+(9*(3+8)*((4-2)^(3))*(5-4)*(2+3))" 							
							};

    int *p;
    int thread_count=0;
	int is_master = 0;
	if ( is_master == 0 )
	{
		for( p = t_equations; p < &t_equations+1; p++ )
			thread_count++;
		
		thread_count /= 2;
		is_master = 1;
	}
	printf("Thread Count : %d\n", thread_count);

	//Weight test
//	char *t_equations[NUM_THREADS] = { "((4+(3*7))", "((8/(2^2))-5+(3*8)))" };

	//Operand
	opr = "-";		
	
	int i;
	for( i = 0; i < NUM_THREADS; i++ )
	{
		messages[i] = t_equations[i];
	}	
	
	/**/
	pthread_t threads[NUM_THREADS];

	int rc;
	long t;
	long taskids[NUM_THREADS];
	void *status;

	//	Creating Slave threads
	for(t=0; t < thread_count/*NUM_THREADS*/; t++){
		taskids[t] = t;
		printf("In main: creating SLAVE thread %ld\n", t);
		rc = pthread_create(&threads[t], NULL, grabString, (void *) taskids[t]);
		if (rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
		//	exit(-1);
		}
	}
	
	//Keep looping until all values are retrieved from threads
	while( 1 )
	{
		if ( 	tdata.t_solutions[0] && 
				tdata.t_solutions[1] && 
				tdata.t_solutions[2] && 
				tdata.t_solutions[3] &&
				tdata.t_solutions[4] && 
				tdata.t_solutions[5] && 
				tdata.t_solutions[6] && 
				tdata.t_solutions[7] 				
		)
			break;
					
	}
	
	
	int j;

	for( j = 0; j < thread_count; j++ )	//# of Solutions
	{
		//TODO: Perform math on all solutions. Take into account operators
		tdata.sum += tdata.t_solutions[j];
	}
		
	printf("Sum: %d\n", tdata.sum);
			
	//Timer
	clock_t end = clock();			
	double time_spent = (double)(end-begin) / CLOCKS_PER_SEC;		
	printf("Execution Time: %.6f\n", time_spent);			
			
	//Only exit thread once sum has been obtained
	pthread_exit(NULL);		
}
/*-------------------------------------------------
THREAD CODE END
-------------------------------------------------*/

/*-------------------------------------------------
PROGRAM MAIN BEGIN
-------------------------------------------------*/

int main( int argc, char *argv[] )
{
	/**/
	pthread_t threads[NUM_THREADS];
		
	int rc;
	long t;
	long taskids[NUM_THREADS];

	//Creating Master
	printf("In main: creating MASTER thread %ld\n", t);
	rc = pthread_create(&threads[t], NULL, masterThread, (void *) t);
	if (rc)
	{
		printf("ERROR; return code from pthread_create() is %d\n", rc);
	}
		
	pthread_exit(NULL);
	/**/
	
	return 0;
}

/*-------------------------------------------------
PROGRAM MAIN END
-------------------------------------------------*/

void expressionParser(char* xpn, char *opnA, char *opnB,  char* opr)
{
  int len = strlen(xpn);
  int midpoint = len/2;
  int i;
  int j = -1; // index of operator to split at

  // find index of first operator
  i = 0;
  while (j == -1)
  {
    if (isOperator(xpn[i]))
    {
      j = i;
    }
    else
      ++i;
  }

  // j should now have index of first operator
  // iterate through the user expression with i as the index
  for (i = 0; i < len; ++i)
  {
    // if the char at index i is an operator, and the distance of i to the midpoint
		// is less than the distance of the operator at index j to the midpoint
		// and if the priority of the operator at i has smaller predence than operator
		// at index j then make j the new index of the operator where we will
		// split the expression at
    if (isOperator(xpn[i]) && ( fabs( i - midpoint) < fabs( j - midpoint)) && getPriorityOf(xpn[i]) < getPriorityOf(xpn[j]))
    {
      j = i;
    }
  }

  // index j contains the operator 
  *opr = xpn[j];
  memmove(opnA, xpn, j);
  strncpy(opnB, xpn + j,strlen(xpn) - j - 1);


}

/*-------------------------------------------------
RPN BEGIN
-------------------------------------------------*/
int solveExpression(char * expression)
{
	struct node *operandStk = NULL;
	struct node *operatorStk = NULL;

	operandStk = initStack();
	operatorStk = initStack();

	// accessory variables and pointers

	int (*peeK)(struct node*);	// create a function pointer for peek
	struct node* (*Pop)(struct node*);
	peeK = &peek;				// assign the address of function peek to its pointer
	Pop = &pop;

//	char expression[100];	//Taking out expression from a string
	int operandVal = 0;
	int error = noError;
	int solution = 0;

	initOperators();

//	printf("Enter the expression. Press enter when done. Press Ctrl + D to exit program\n");

	while (/*fgets(expression, sizeof(expression), stdin)*/expression != NULL)
	{
		error = noError; // set the error flat to its default at the start of each expression
		//strcpy(expression, addNullTerminator(expression));	//REMOVED
		//printStack(operandStk);
		//printOStack(operatorStk);
		
		int i;
		for (i = 0; i < strlen(expression); i++)
		{
			if (isspace(expression[i]))
			{
				continue;
			}
			if (isOperand(expression[i]))
			{
				operandVal = convertOperand(&i, expression);	// convert the operand string into int
				push(operandStk, operandVal); 		// insert the operand into the operand stack
				//printStack(operandStk);				// print the stack	
			} 
			else { 
					if (isOperator(expression[i]))
					{
						if (stackIsEmpty(operatorStk))	// if stack is empty
						{
							push(operatorStk, expression[i]); // push current operator into stack
							//printOStack(operatorStk);
						}
						else {
							if (expression[i] == '(')
							{
								push(operatorStk, expression[i]);
								//printOStack(operatorStk);
							}
							else {
								if (expression[i] == ')')
								{
									while (operatorStk->next != NULL && peek(operatorStk) != '(')
									{
										evaluate(operandStk, operatorStk, &error);
										if (validateError(error))	// if there's an error
											break;					// stop reading expression

									}
									//peek(operatorStk) is '('' then pop the ' ('
									if (operatorStk->next != NULL && peek(operatorStk) == '(')
									{
										free((*Pop)(operatorStk));
									}
									else {
										if (operatorStk->next == NULL)
										{
											//left parens is missing
											error = leftParensMissing;
											validateError(error);
											break;
										}
									}

								}
								else {
									if (getPriorityOf(expression[i]) > getPriorityOf((*peeK)(operatorStk)))
									{
										push(operatorStk, expression[i]);
										//printOStack(operatorStk);
									}
									else {
										if (getPriorityOf(expression[i]) < getPriorityOf((*peeK)(operatorStk)))
										{
											evaluate(operandStk, operatorStk, &error);
											if (validateError(error))	// if there's an error
												break;					// stop reading expression
											push(operatorStk, expression[i]);
											//printOStack(operatorStk);
										}
										else {
											if (getAssociativity(expression[i]) == 'L')
											{
												evaluate(operandStk, operatorStk, &error);
												if (validateError(error))	// if there's an error
													break;					// stop reading expressionevaluate(operandStk, operatorStk, &error);
												push(operatorStk, expression[i]);
												//printOStack(operatorStk);
											}
											else {
												push(operatorStk, expression[i]);
												//printOStack(operatorStk);
											}
										}
									}
								}
							}
						}
					}
				else
				{
					error = wrongSymbol;
					printf("Wrong Symbol: Your expression uses an unsupported character\n\n");
					break; 	// exit reading the expression
				}
			}

		}

		if ( i == strlen(expression)) {		// we have exited previous for loop
			while (!stackIsEmpty(operatorStk))
			{
				evaluate(operandStk, operatorStk, &error);
				if (validateError(error))	// if there's an error
					break;					// stop reading expression											
				
			}
			if (!stackIsEmpty(operandStk))
			{
				//EDWARD -- Returns answer
				solution = peek(operandStk);
				return solution;	//Returns answer
				
				printf("The solution for the expression: %s is\n%d\n\n", expression, solution);
			}

			if (!stackIsEmpty(operandStk)){
				//printStack(operandStk);
				emptyStack(operandStk);
				//printStack(operandStk);
			}

			if (!stackIsEmpty(operatorStk)){
				//printOStack(operatorStk);
				emptyStack(operatorStk);
				//printOStack(operatorStk);
			}
			
		}

		if (error != noError)
		{
			if (!stackIsEmpty(operandStk)){
				//printStack(operandStk);
				emptyStack(operandStk);
				//printStack(operandStk);
			}

			if (!stackIsEmpty(operatorStk)){
				//printOStack(operatorStk);
				emptyStack(operatorStk);
				//printOStack(operatorStk);
			}
		}

//	printf("Enter the expression. Press enter when done. Press Ctrl + D to exit program\n");
	}//while loop ends here

	free(operandStk);
	free(operatorStk);

	freeOperators();

//	return 0;
}


// Helper functions

/* 
	Checks what error code has been signed
	to the param and executes behavior. If it is a critical error
	then exit() is called else just an error statement is printed.
	If one of the error codes is set then true is returned, else
	there is no error and false is returned.
	Preconditions: param error has been assigned zero
	Postconditions: If not enough memory can be allocated for a push
					or there is division by zero, the program exits().
					If it is one of the error code, a statement is printed
					and true is returned.
*/
bool validateError(int error)
{
	if (error == noError)
		return false;
	
	if (error == outOfMemory)
	{
		printf("The system is unable to allocate for memory for this program.\n");
		exit(outOfMemory);
	}

	if (error == divisionByZero)
	{
		printf("Division by zero, result is undefined.\n");
		exit(divisionByZero);
	}

	if (error == missingOperator)
	{
		printf("An operator is missing, exiting program.\n");
		return true;
	}

	if (error == missingOperand)
	{
		printf("An operand is missing, ending reading of expression. \n");
		return true;
	}

	if (error == wrongSymbol)
	{
		printf("Wrong symbol\n");
		return true;
	}

	if (error == leftParensMissing)
	{
		printf("Left parenthis is missing in the expression, ending reading of expression\n");
		return true;
	}

}
/* 
	Initialize each operator node with its fields:
	character representation = 'X'
	priority = Y
	associativity = Z
	where X is an ascii value hard-coded into the design
	Y is an int as user defined
	and Z is a char

	Assumptions: user will not put a char for priority and user will enter a capital L
	or R for associativity

	TODO: write a helper function to validate user input

*/
void initOperators(void)
{
	plus = (struct operator *)(malloc(sizeof(struct operator)));
	minus = (struct operator *)(malloc(sizeof(struct operator)));
	multiply = (struct operator *)(malloc(sizeof(struct operator)));
	division = (struct operator *)(malloc(sizeof(struct operator)));
	exponent = (struct operator *)(malloc(sizeof(struct operator)));
	leftParens = (struct operator *)(malloc(sizeof(struct operator)));

	int priorityForLeftParens = -1;
	leftParens->priority = priorityForLeftParens;


	char inputBuffer[100];
	int prt;		// user defined priority
	char asc[1]; 	// buffer for single char associativity


//	printf("For the following input please format as follows:\n");
//	printf("X Y where X is an integer value representing operator priority and Y is either L or R representing associativity\n");
//	printf("Please enter the priority and associativity for the minus operator\n");

//	scanf("%d %c", &prt, asc);
	prt = 1;
	minus->characterRep= '-';
	minus->priority = prt;
	minus->associativity[0] = toupper(asc[0]);

	if (prt <= priorityForLeftParens)		// sets leftParens priority
	{										// to always be one less then
		leftParens->priority = prt - 1;		// the current lowest priority
	}

//	printf("Please enter the priority and associativity for the plus operator\n");

//	scanf("%d %c", &prt, asc);
	prt = 2;
	plus->characterRep= '+';
	plus->priority = prt;
	plus->associativity[0] = toupper(asc[0]);

	if (prt <= priorityForLeftParens)
	{
		leftParens->priority = prt - 1;
	}

//	printf("Please enter the priority and associativity for the multiply operator\n");

//	scanf("%d %c", &prt, asc);
	prt = 4;
	multiply->characterRep= '*';
	multiply->priority = prt;
	multiply->associativity[0] = toupper(asc[0]);

	if (prt <= priorityForLeftParens)
	{
		leftParens->priority = prt - 1;
	}

//	printf("Please enter the priority and associativity for the division operator\n");

//	scanf("%d %c", &prt, asc);
	prt = 3;
	division->characterRep= '/';
	division->priority = prt;
	division->associativity[0] = toupper(asc[0]);

	if (prt <= priorityForLeftParens)
	{
		leftParens->priority = prt - 1;
	}
//	printf("Please enter the priority and associativity for the exponent operator\n");

//	scanf("%d %c", &prt, asc);
	prt = 5;
	exponent->characterRep= '^';
	exponent->priority = prt;
	exponent->associativity[0] = toupper(asc[0]);

	if (prt <= priorityForLeftParens)
	{
		leftParens->priority = prt - 1;
	}
}

void freeOperators(void)
{
	free(plus);
	free(minus);
	free(multiply);
	free(division);
	free(exponent);
}

/*
	Remove newline character
	Preconditions: the paramater string must have been allocated memory
	Postconditions: the parameter string will have a '\0' character where there used to be a '\n' before
*/
char* addNullTerminator(char *s)
{
	int len = strlen(s);
	if (len > 0) 	
		s[len-1] = '\0';

	return s;
}

/* Determines if parameter is a valid operator
	Preconditions: None
	Postconditions: boolean(int) is returned*/
bool isOperator(char s)
{
	switch(s) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '^':
		case '(':
		case ')':
			return true;
		break;
			default:
			return false;
	}
}

/*
	Determines if parapemter is a valid operator
	Preconditions: none
	Postconditions: boolean(int) is returned
*/
bool isOperand(char s)
{
	if (s >= '0' && s <= '9' )
	{
		return true;
	}

	return false;
}

/*
	Converts a string operand to its integer representation
	Precondition: 	expIndex is the current index of the start of the operand
					expression is at least length 1 
	PostConditions: expIndex is set to index of last valid operand index
					in order to resume outer loop correctly
					number is the integer representation of the ascii operand
*/
int convertOperand(int *expIndex, char* expression)
{

	int i;
	int number = 0;
	char operand[30];
	char charBuff[2];
	memset(operand, '\0', sizeof(operand));

	// tranverse the expression while index is a valid operand
	while(isOperand(expression[*expIndex]))
	{
		charBuff[0] = expression[*expIndex]; // and concatenate the char to the operand string
		charBuff[1] = '\0';
		strncat(operand,charBuff, sizeof(charBuff));
		*expIndex = *expIndex + 1;
	}



	for ( i = 0; i < strlen(operand); i++)	// tranverse the operand string and conver to an int value
	{
		if (operand[i] >= '0' && operand[i] <= '9')
		{
			number = number * 10;
			number = number + (operand[i] - '0');
		}
	}

	*expIndex = *expIndex - 1; 	// set back expression's index by one so next iteration of loop controlling the index points to 
								// a char other than a valid operand value
	return number; 
}




int getPriorityOf(char s)
{
	switch(s) {
		case '+':
			return plus->priority;
		case '-':
			return minus->priority;
		case '*':
			return multiply->priority;
		case '/':
			return division->priority;
		case '^':
			return exponent->priority;
		case '(':
			return leftParens->priority;
		break;
	}
}

char getAssociativity(char operator)
{
	switch(operator) {
		case '+':
			return plus->associativity[0];
		case '-':
			return minus->associativity[0];
		case '*':
			return multiply->associativity[0];
		case '/':
			return division->associativity[0];
		case '^':
			return exponent->associativity[0];
		break;
	}
}

/*
	pops two operands from the operand stack, pops an operator from the operator stack,
applys the operator to the two operands and pushes the result back onto the operand stack.
You need to make sure that the operand and operator stacks have what you are looking for. Otherwise the expression is bad.

	Preconditions: both param pointers .mext field must not point to null
	PostConditions: two operands will have been popped and freed, one operator popped and freed, and one operand inserted into 
	the operand stack.
*/
void evaluate(struct node *operandHead, struct node *operatorHead, int* errorStat)
{
	struct node *tempOpn1;	// temporary nodes to hold the value
	struct node *tempOpn2;  // of the popped off nodes
	struct node *tempOpr;
	int result = 0;
	int operand1 = 0;
	int operand2 = 0;	

	//pop two operands from the operand stack
	// and one operator from the operator stack
	if (!stackIsEmpty(operandHead))
		tempOpn2 = pop(operandHead);		// pop an operand
	else {
		*errorStat = missingOperand;
		return;
	}

	if (!stackIsEmpty(operandHead))
		tempOpn1 = pop(operandHead);		// pop an operand
	else {
		*errorStat = missingOperand;
		return;
	}

	if (!stackIsEmpty(operatorHead))		
		tempOpr = pop(operatorHead);		// pop an operator
	else {
		*errorStat = missingOperator;
		return;
	}

	operand1 = tempOpn1->value;
	operand2 = tempOpn2->value;

	result = binaryOperate(tempOpr->value, operand1, operand2, errorStat);
	push(operandHead, result);	// push the result back into the operand stack

	free(tempOpn1);
	free(tempOpn2);
	free(tempOpr);
}

int binaryOperate(char operator, int val_1, int val_2, int* errorStat)
{
	int result = 0;

	switch(operator) {
		case '+':
			result = val_1 + val_2;
			break;
		case '-':
			result = val_1 - val_2;
			break;
		case '*':
			result = val_1 * val_2;
			break;
		case '/':
			if (val_2 == 0)
			{
				*errorStat = divisionByZero;
				break;
			}
			result = val_1 / val_2;
			break;
		case '^':
			result = power(val_1, val_2);
			break;

		return result;
	}
}

int power(int b, int x)
{
	int result = 1;
	while(x--)
	{
		result = result * b;
	}

	return result;
}

/* STACK ADT 
 	Push 
 	Insert a new node at the top of the list 
	Preconditions:  
	 Postconditions: 	*head points to the new node,
					true is returned if insertion was successful,
					false is returned if memory could not be allocated
					for */

bool push(struct  node *head, int val)
{

	struct node *newNode = (struct node *)(malloc(sizeof(struct node)));
	
	if (newNode == NULL)
	{
		fprintf(stderr, "Out of memory\n");
		return false; 
	}

	newNode->value = val;		// assign val param to struct field

	if (head->next == NULL)		// if stack is empty
	{
		head->next = newNode;
		newNode->next = NULL;
			
	}
	else {
		newNode->next = head->next;	// have new node point to current top node
		head->next = newNode;		// reassign head's next reference to the new node
				
	}

	return true; 		// insertion was a success
}



/*POP 
 	Remove the top element of the stack 
 	PreConditions: the Stack must not be empty 
	PostCondition: 	Either the stack is empty or it has at least one node left.
					if the POP was successful then the popped node is returned.
					if stack was empty then NULL is returned
*/

struct node *pop( struct node *head)
{
	struct node *ret_Node;


	if (head->next == NULL)		// if list is empty
	{
		return NULL;			// return NULL
	}
	else {
		ret_Node = head->next;
		head->next = ret_Node->next;
		ret_Node->next = NULL;
	}

	return ret_Node;
}


/* PEEK V2 */
/*
	Peeks at the top node
	Preconditions: the stacks are not empty
	Postconditions: the top node of the stack remains intact
					int representing the node's value is returned,
					either an integer for an operand or the ascii value for
					an operator.
*/
int peek( struct node *head)
{
	struct node *ret_Node = NULL;
	int topVal = 0;
	ret_Node = head->next;
	topVal = ret_Node->value;


	return topVal;
}

/* initStack*/
/* Initialize the parameterized stack
	Preconditions: NONE
   Postconditions: the refernece to a struct is returned */
struct node *initStack (void)
{
  struct node *head = (struct node *)(malloc(sizeof(struct node)));
  if (head == NULL)  // malloc failed to allocate memory
     exit(0);
  
  // malloc must to have succeeded
  // initialize struct fields
  head->value = -1; // maybe it is not necessary
  head->next = NULL;

  return head;
}


/* printStack */
/* Print a 2d representation of the stack
/* Parse out information */

void printStack(struct node *head)
{

  struct node *iterator = head;
  printf("OPERAND STACK\n");
  printf("------|     |------ TOP\n");
  
  if ( iterator->next != NULL) {
    iterator = iterator->next; // move the iterator from the head to the first node in the stack

    while (iterator != NULL){
      printf("      |  %d  |      \n", iterator -> value);
      iterator = iterator -> next;
    }

  }
  
  printf("      -------        BOTTOM\n");
}

void printOStack(struct node *head)
{

  struct node *iterator = head;

  printf("OPERATOR STACK\n");
  printf("------|     |------ TOP\n");
  
  if ( iterator->next != NULL) {
    iterator = iterator->next; // move the iterator from the head to the first node in the stack

    while (iterator != NULL){
      printf("      |  %c  |      \n", iterator -> value);
      iterator = iterator -> next;
    }

  }
  
  printf("      -------        BOTTOM\n");

}

/*
	stackIsEmpty
	preconditions: param struct has been allocated memory
	postconditions: method returns true if head points to an empty stack
					else it returns false
*/
bool stackIsEmpty(struct node *head)
{
	if (head->next == NULL)	// if the top of the stack is NULL
		return true;		// then stack is empty
	else
		return false;			// else stack is not empty
}

/*
	emptyStack
	preconditions: param struct should be pointing to another node
	postconditions: all node should have been freed
*/
void emptyStack(struct node *head)
{
	if (head->next == NULL)		// if stack is empty
	{
		return; 				// do nothing
	}

	/*struct node *iterator = head->next;		// let iterator point to the top node of the stack
	struct node *temp;

	while (iterator != NULL)
	{
		temp = iterator;
		iterator = iterator->next;
		free(temp);
	}*/

	struct node* current = head->next;
	struct node* next;

	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}

	head->next = NULL;
}
/*-------------------------------------------------
RPN END
-------------------------------------------------*/
