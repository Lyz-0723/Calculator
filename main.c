#include <stdio.h>
#include <ctype.h>
#include <math.h>

// Define
#define LT 1000000

typedef struct {
    double value;
    int errorCode;
} output;

typedef struct {
    double stk[LT];
    int top;
} stack;

typedef struct {
    double values[LT];
    int front;
    int rear;
} queue;


// Declare
output result;

stack operators;
queue postfix;
stack operands;

stack *pOperators = &operators;
stack *pOperands = &operands;


// Stack
void push(double, stack *);

double pop(stack *);

double top(stack *);

int stackIsEmpty(stack *);

void reset_stack(stack *);

// Queue
void enqueue(double);

double dequeue();

int postfixIsEmpty();

void reset_queue();

// Calculate
void transfer(const char *, int);

double calculate();

double opCalculate(double, double, double);

int opToNum(char);

// Exception
void exception_handling(int);

int main(void) {

    char expressions[LT];
    int expn_length[LT];             // Length of each expression
    char *pExpression = expressions; // Pointer of expression array


    /* ---------- Stores expressions ---------- */
    int ch;
    int lengthCount = 0, i = 0, j = 0;

    printf("Enter the expressions (press ctrl+Z/control+D to stop)\n");
    printf("%d: ", j + 1);

    // Get user input until EOF (End Of File)
    while ((ch = getchar()) != EOF) {
        if (ch == '\n') {
            printf("%d: ", j + 2);
            expn_length[j++] = lengthCount;
            lengthCount = 0;
            continue;
        }

        lengthCount++;
        expressions[i++] = (char) ch;
    }
    printf("\n---\n");


    /* ------ Gets each expression result ------ */
    int length, pos = 0;
    while (*pExpression != '\0') {

        // Reset stacks and queue
        reset_stack(pOperators);
        reset_stack(pOperands);
        reset_queue();

        length = expn_length[pos++];

        // Transfer each expression into postfix format
        // then calculate value and store in result
        transfer(pExpression, length);
        result.value = calculate();

        // Point to next expression
        pExpression += length;

        // Handle exception if it's exist
        if (result.errorCode) {
            exception_handling(pos);
            continue;
        }

        printf("Expression %d = %.13lf\n", pos, result.value);
    }
}

//
void transfer(const char *expression, int length) {

    int pos = 0;         // Current position in expression
    int number = -1;     // Stand for each number in expression, initialize with -1
    int parentheses = 0; // Count for parentheses (left: +1, right: -1)


    for (int i = 0; i < length; i++) {
        char now = *(expression + pos++);

        // Digit -> Store in temp number
        if (isdigit(now)) {
            if (number == -1) number = 0;
            number = number * 10 + ((int) now - 48);
            continue;
        }

        int symbol_value = opToNum(now);
        if (symbol_value == 0) continue;        // Ignore invalid operator

        if (number >= 0) enqueue(number);  // Store the number except init value
        number = -1;                            // Reset number value

        // Left parentheses -> (
        if (now == '(') {
            parentheses++;

            push(-1, pOperators);
            continue;
        }

        // Right parentheses -> )
        if (now == ')') {
            parentheses--;

            // When top value of operator stack is -1 (stand for '(') or
            // stack is empty, it will stop enqueue the value into postfix queue
            while (top(pOperators) != -1 && !stackIsEmpty(pOperators))
                enqueue(pop(pOperators));

            pop(pOperators);
            continue;
        }

        // Tens digit of symbol value stands for precedence,
        // so divided the value by 10 to decide precedence
        while ((int) (top(pOperators) / 10) <= (int) (symbol_value / 10))
            enqueue(pop(pOperators));

        push(symbol_value, pOperators);
    }

    // Put the rest number and operators in queue
    if (number >= 0)
        enqueue(number);
    while (!stackIsEmpty(pOperators))
        enqueue(pop(pOperators));

    // Set errorCode as 1 if numbers of left parentheses not equal to right one
    result.errorCode = parentheses ? 1 : 0;
}

// Calculates the expression stored in postfix queue
double calculate() {
    while (!postfixIsEmpty()) {
        double now = dequeue();

        // Digit -> Store in operands stack
        if (now >= 0) {
            push(now, pOperands);
            continue;
        }

        double opd2 = pop(pOperands);
        double opd1 = pop(pOperands);
        push(opCalculate(opd1, opd2, now), pOperands);
    }
    if (operands.top != 0)
        result.errorCode = 3;
    return pop(pOperands);
}

// Calculates value by operator
double opCalculate(double op1, double op2, double operator) {
    if (operator == -11)
        return op1 + op2;
    if (operator == -12)
        return op1 - op2;
    if (operator == -21)
        return op1 * op2;
    if (operator == -22) {
        // Return error if divisor equals 0
        if (op2 == 0) result.errorCode = 2;
        return op1 / op2;
    }
    if (operator == -23)
        return fmod(op1, op2);
    if (operator == -31)
        return pow(op1, op2);
    return op1;
}

// Transforms the operators into number.
// Each operator is presented by a negative,
// tens digit of the number stands for its precedence,
// and unit digit is its id.
int opToNum(char op) {
    if (op == '+')
        return -11;
    if (op == '-')
        return -12;
    if (op == '*')
        return -21;
    if (op == '/')
        return -22;
    if (op == '%')
        return -23;
    if (op == '^')
        return -31;
    if (op == '(')
        return -41;
    if (op == ')')
        return -42;
    // Invalid operator return 0
    return 0;
}

// Print out the error message
void exception_handling(int line) {
    if (result.errorCode == 1)
        printf("Error(line %d): You should enter a pair of parentheses.\n", line);
    if (result.errorCode == 2)
        printf("Error(line %d): The divisor should not be zero.\n", line);
    if (result.errorCode == 3)
        printf("Error(line %d): Illegal expression.\n", line);
    result.errorCode = 0;
}

/* Stack Function */

void push(double elem, stack *pS) {
    (*pS).stk[++(*pS).top] = elem;
}

double pop(stack *pS) {
    if (!stackIsEmpty(pS))
        return (*pS).stk[(*pS).top--];
    return 0;
}

double top(stack *pS) {
    return (*pS).stk[(*pS).top];
}

int stackIsEmpty(stack *pS) {
    return (*pS).top == -1;
}

/* Queue Function */

void enqueue(double elem) {
    postfix.values[++postfix.rear] = elem;
}

double dequeue() {
    return postfix.values[++postfix.front];
}

int postfixIsEmpty() {
    return postfix.front == postfix.rear;
}

/* Reset */

void reset_stack(stack *pS) {
    (*pS).top = -1;
}

void reset_queue() {
    postfix.rear = -1;
    postfix.front = -1;
}
