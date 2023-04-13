#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define T 1000000
#define L (T/3)

// Define
typedef struct {
    double value;
    int errorCode;
} output;

typedef struct {
    double stk[L];
    int top;
} stack;

typedef struct {
    double values[L];
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

void transfer(const char *, int);

double calculate();

double opCalculate(double, double, double);

int opToNum(char);

void exception_handling(int);

int main(void) {

    char expressions[T]; // 一維陣列存放所有expression
    int expn_length[L]; // 存放每個expression長度
    char *pExpression = expressions;

    int ch;
    int lengthCount = 0, i = 0, j = 0;

    printf("Enter the expressions (press ctrl+Z/control+D to stop)\n"); // 修改
    printf("%d: ", j + 1);

    // get the user input
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

    int length, pos = 0;
    while (*pExpression != '\0') {
        reset_stack(pOperators);
        reset_stack(pOperands);
        reset_queue();

        length = expn_length[pos++];

        transfer(pExpression, length);
        result.value = calculate();
        pExpression += length;

        // exception handling
        if (result.errorCode) {
            exception_handling(pos);
            continue;
        }

        printf("Expression %d = %.13lf\n", pos, result.value);
    }
}

void transfer(const char *expression, int length) {

    int pos = 0; // expression position
    int number = -1; // initialize with -1

    int parentheses = 0;

    for (int i = 0; i < length; i++) {
        char now = *(expression + pos++); // 當前位置的值

        // digit -> store them in temp number
        if (isdigit(now)) {
            if (number == -1) number = 0; // 如果number為初始值-1就將number設為0(讓後面能計算總數值)
            number = number * 10 + ((int) now - 48); // 12 = 1 * 10 + (int) '2' - 48
            continue;
        }

        // use a negative number to represent an operator
        // ignore invalid operator
        int symbol_value = opToNum(now);
        if (symbol_value == 0) continue;

        // 確保不會重複存值
        if (number >= 0) enqueue(number);
        number = -1;

        // left parentheses -> (
        if (symbol_value == -41) {
            parentheses++;

            push(-1, pOperators);
            continue;
        }

        // right parentheses -> )
        if (symbol_value == -42) {
            parentheses--;

            while (top(pOperators) != -1 && !stackIsEmpty(pOperators))
                enqueue(pop(pOperators));

            pop(pOperators);
            continue;
        }

        // compare the value to decide precedence
        while ((int) (top(pOperators) / 10) <= (int) (symbol_value / 10))
            enqueue(pop(pOperators));

        push(symbol_value, pOperators);
    }

    // 將剩下的數字跟運算子存入queue中
    if (number >= 0)
        enqueue(number);
    while (!stackIsEmpty(pOperators))
        enqueue(pop(pOperators));

    result.errorCode = parentheses ? 1 : 0;
}

double calculate() {
    while (!postfixIsEmpty()) {
        double now = dequeue();
        if (now >= 0) {
            push(now, pOperands);
            continue;
        }
        double opd2 = pop(pOperands);
        double opd1 = pop(pOperands);
        push(opCalculate(opd1, opd2, now), pOperands);
    }
    return pop(pOperands);
}

double opCalculate(double op1, double op2, double operator) {
    if (operator == -11)
        return op1 + op2;
    if (operator == -12)
        return op1 - op2;
    if (operator == -21)
        return op1 * op2;
    if (operator == -22) {
        // return error if divisor equals 0
        if (op2 == 0) result.errorCode = 2;
        return op1 / op2;
    }
    if (operator == -23)
        return fmod(op1, op2);
    if (operator == -31)
        return pow(op1, op2);
    return op1;
}

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
    return 0;
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

void exception_handling(int line) {
    if (result.errorCode == 1)
        printf("Error(line %d): You should enter a pair of parentheses.\n", line);
    if (result.errorCode == 2)
        printf("Error(line %d): The divisor should not be zero.\n", line);
    result.errorCode = 0;
}

