#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define MAXSIZE 100
#define T 1000000

typedef struct {
    double stk[MAXSIZE];
    int top;
} stack;

typedef struct {
    double values[MAXSIZE];
    int front;
    int rear;
} queue;

stack operators;
queue postfix;
stack operands;

stack *pOperators = &operators;
stack *pOperands = &operands;


// Stack
void push(double, stack *);

double pop(stack *);

double top(stack *);

void display_s(stack *);

void reset_stack(stack *);

// Queue
void enqueue(double);

double dequeue();

int postfixIsEmpty();

void display_q();

void reset_queue();

void transfer(const char *, int);

double calculate();

double opCalculate(double, double, double);

int opToNum(char);

int main(void) {

    char expressions[T];
    int expn_length[T / 3];
    char *pExpression = expressions;

    int ch;
    int lengthCount = 0, i = 0, j = 0;

    printf("Enter the expressions (press ctrl+Z/control+D to stop)\n");
    printf("%d: ", j + 1);

    while ((ch = getchar()) != EOF) {
        if (isspace(ch)) {
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

        length = expn_length[pos];

        transfer(pExpression, length);
        printf("expression %d = %.13lf\n", ++pos, calculate());
        pExpression += length;
    }
}

void transfer(const char *expression, int length) {

    int pos = 0; // expression position
    int number = -1; // initialize with -1

    // 如果到字串結尾(\0)就停止迴圈
    for (int i = 0; i < length; i++) {
//    while (expression[pos] != '\0') {
        char now = *(expression + pos++);

        // digit -> store them in temp number
        if (isdigit(now)) {
            if (number == -1) number = 0; // 如果number為初始值-1就將number設為0(讓後面能計算總數值)
            number = number * 10 + ((int) now - 48); // 12 = 1*10 + (int) '2' - 48
            continue;
        }

        // 確保不會重複存值
        if (number >= 0) enqueue(number);
        number = -1;

        // parentheses ()
        if (now == '(') {
            push(-1, pOperators);
            continue;
        }

        if (now == ')') {
            while (top(pOperators) != -1)
                enqueue(pop(pOperators));
            pop(pOperators);
            continue;
        }

        // use a negative number to represent an operator
        int symbol_value = opToNum(now);

        // compare the value to decide precedence
        while ((int) (top(pOperators) / 10) <= (int) (symbol_value / 10))
            enqueue(pop(pOperators));

        push(symbol_value, pOperators);
    }
    // 將剩下的數字跟運算子存入queue中
    if (number >= 0)
        enqueue(number);
    while (operators.top != -1)
        enqueue(pop(pOperators));
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

double opCalculate(double operand1, double operand2, double operator) {
    if (operator == -11)
        return operand1 + operand2;
    if (operator == -12)
        return operand1 - operand2;
    if (operator == -21)
        return operand1 * operand2;
    if (operator == -22) {
        if (operand2 == 0) // return operand1 if operand2 equals 0
            return operand1;
        return (double) operand1 / operand2;
    }
    if (operator == -23)
        return (int) operand1 % (int) operand2;
    if (operator == -31)
        return pow(operand1, operand2);
    return 0;
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
    return 0;
}

/* Stack Function */

void push(double elem, stack *pS) {
    (*pS).stk[++(*pS).top] = elem;
}

double pop(stack *pS) {
    return (*pS).stk[(*pS).top--];
}

double top(stack *pS) {
    return (*pS).stk[(*pS).top];
}

void display_s(stack *pS) {
    for (int i = (*pS).top; i >= 0; i--)
        printf("%lf ", (*pS).stk[i]);
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

void display_q() {
    int begin = postfix.front + 1;
    while (begin <= postfix.rear) {
        printf("%lf ", postfix.values[begin]);
        begin += 1;
    }
    printf("\n");
}

/* Reset */

void reset_stack(stack *pS) {
    (*pS).top = -1;
}

void reset_queue() {
    postfix.rear = -1;
    postfix.front = -1;
}
