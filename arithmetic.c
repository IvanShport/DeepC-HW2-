#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#define START_MAX_SIZE_FOR_STACK 4
#define START_SIZE_FOR_STRING 32

typedef struct Stack {  //  Стек для операндов и операторов
    char** strings;
    size_t real_size;
    size_t max_size;
} Stack;

Stack* create_stack(){
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if(stack == NULL) return NULL;
    stack->max_size = START_MAX_SIZE_FOR_STACK;
    stack->real_size = 0;
    stack->strings = (char**)malloc(sizeof(char*) * stack->max_size);
    if(stack->strings == NULL){
        free(stack);
        return NULL;
    }
    return stack;
}

void free_stack(Stack* stack){
    for (size_t i = 0; i < stack->real_size; i++) {
        free(stack->strings[i]);
    }
    free(stack->strings);
    free(stack);
}

char** realloc_for_stack(Stack* stack){
    stack->max_size *= 2;
    char** temp = realloc(stack->strings, stack->max_size * sizeof(char*));
    if(temp == NULL) return NULL;
    stack->strings = temp;
    return stack->strings;
}

void pop(Stack* stack){
    if(!stack->real_size)
        return;

    free(stack->strings[stack->real_size - 1]);
    stack->real_size--;
}

int push(Stack* stack, char* new_value){
    if(stack->real_size == stack->max_size){
        if(realloc_for_stack(stack) == NULL) return 1;
    }

    char* copy_new_value = (char*)malloc((strlen(new_value) + 1) * sizeof(char));
    if (copy_new_value == NULL)
        return 1;
    memcpy(copy_new_value, new_value, strlen(new_value));
    copy_new_value[strlen(new_value)] = '\0';

    stack->strings[stack->real_size] = copy_new_value;
    stack->real_size++;
    return 0;
}

char* top(Stack* stack){
    return stack->real_size != 0? stack->strings[stack->real_size - 1]: NULL;
}

//-------------------------------------------------------------------

char* input_row();  // Ввод сторки
int parseExpression(char *str);  // Вычиление выражения
int addOperatorsAndOperandsInStack(Stack* operators, Stack* operands, char* str);
int isOperator(char symbol);
int addOperator(Stack *operators, char op, Stack *operands); // Добавление операторов в стек
int addOperand(Stack *operands, char* str, size_t* startIndex, int sign);
int toInt(const char* value, size_t size);
char* toString(int value, int sign);
char toChar(int value);
int checkOnUnoOrBinMinus(Stack* operands, Stack* operators, char* str, size_t* startIndex, int* numberIsLast);
int get_priority(char symbol);  // Приоритеты операций
int createExpression(Stack *operands, char op);  // Вычиление операндов в зависимости от оператора
char* copyString(char* source, size_t size);
char* calculateExpression(char* leftValue, int leftSign, char* rightValue, int rightSign, char op);
int output_result(Stack* operands, Stack* operators); // Вывод результата

int main(){
    char* str = input_row();
    if(str == NULL){
        printf("[error]\n");
        return 0;
    }

    if(parseExpression(str) != 0){
        free(str);
        printf("[error]\n");
        return 0;
    }

    free(str);
    return 0;
}

char* input_row(){
    size_t size = START_SIZE_FOR_STRING;
    char* row_pointer = (char*)malloc(size * sizeof(char));
    if (row_pointer == NULL)
        return NULL;

    ptrdiff_t cur_size = 1;
    size_t empty_size = size;

    char* end_row = row_pointer;

    row_pointer[0] = '\0';
    while (fgets(end_row, empty_size, stdin)) {
        if (end_row[strlen(end_row) - 1] == '\n') {
            end_row[strlen(end_row) - 1] = '\0';
            break;
        }

        empty_size = size;
        size *= 2;
        char* temp_row = (char*)realloc(row_pointer, size * sizeof(char));
        if(temp_row == NULL) return NULL;
        row_pointer = temp_row;
        end_row = row_pointer + (empty_size - cur_size);
        cur_size++;
    }

    char* temp_row = (char*)realloc(row_pointer, (strlen(row_pointer) + 1) * sizeof(char));
    if (temp_row == NULL)
        return NULL;
    row_pointer = temp_row;

    return row_pointer;
}

int parseExpression(char *str) {
    Stack* operators = create_stack();
    if (operators == NULL)
        return 1;

    Stack* operands = create_stack();
    if (operands == NULL) {
        free_stack(operators);
        return 1;
    }

    if (addOperatorsAndOperandsInStack(operators, operands, str) != 0) {
        free_stack(operands);
        free_stack(operators);
        return 1;
    }

    while (operators->real_size) {         //  Вычисление выражения
        if(createExpression(operands, *top(operators)) != 0){
            free_stack(operators);
            free_stack(operands);
            return 1;
        }
        pop(operators);
    }

    if(output_result(operands, operators) != 0){    // Вывод результата
        free_stack(operators);
        free_stack(operands);
        return 1;
    }

    free_stack(operators);
    free_stack(operands);

    return 0;
}

int addOperatorsAndOperandsInStack(Stack* operators, Stack* operands, char* str) {
    size_t i = 0;
    int numberIsLast = 0;
    for (; i < strlen(str); i++) {

        for (; str[i] && isspace(str[i]); i++) {}


        if (isOperator(str[i]) && str[i] != '-') {
            numberIsLast = 0;
            if (addOperator(operators, str[i], operands) != 0)
                return 1;
        }

        if (isdigit(str[i])) {
            numberIsLast = 1;
            if (addOperand(operands, str, &i, 1) != 0)
                return 1;
        }

        if (str[i] == '-') {
            if(checkOnUnoOrBinMinus(operands, operators, str, &i, &numberIsLast) != 0)
                return 1;
        }

        if (str[i] == ')') {
            while (*top(operators) != '(') {          // Вычисляем содержимое скобок
                if (createExpression(operands, *top(operators)) != 0)
                    return 1;
                pop(operators);
            }
            pop(operators);
        }
    }

    return 0;
}

int isOperator(char symbol) {
    return symbol == '*' || symbol == '/' ||
           symbol == '+' || symbol == '-' ||
           symbol == '(';
}

int addOperator(Stack *operators, char op, Stack *operands) {
    char* temp = (char*)malloc(2 * sizeof(char));
    if (temp == NULL)
        return 1;
    temp[0] = op;
    temp[1] = '\0';

    if (*temp == '(' ||
        operators->real_size == 0 ||
        get_priority(*temp) > get_priority(*top(operators)))
    {
        if(push(operators, temp) != 0) {
            free(temp);
            return 1;
        }
    }

    else {
        while (operators->real_size != 0 &&
               get_priority(*top(operators)) >= get_priority(*temp))
        {
            if (createExpression(operands, *top(operators)) != 0) {
                free(temp);
                return 1;
            }
            pop(operators);
        }
        if (push(operators, temp) != 0) {
            free(temp);
            return 1;
        }
    }
    free(temp);
    return 0;
}

int addOperand(Stack *operands, char* str, size_t* startIndex, int sign) {
    size_t j = *startIndex;
    for (;str[j] && isdigit(str[j]); j++) {}

    char* temp = (char*)calloc((j - *startIndex + 2), sizeof(char));
    if (temp == NULL)
        return 1;
    temp[j - *startIndex] = (char)(sign < 0 ? '-' : '+');
    temp[j - *startIndex + 1] = '\0';

    temp = memcpy(temp, str + *startIndex, j - *startIndex);

    if (push(operands, temp) != 0) {
        free(temp);
        return 1;
    }
    *startIndex = --j;
    free(temp);

    return 0;
}

int toInt(const char* value, size_t size) {
    int result = 0;

    for (size_t i = 0; i < size; i++) {
        result = result * 10 + (value[i] - '0');
    }

    return result;
}

char* toString(const int value, int sign) {
    size_t i = 1;
    int div = 10;
    while (value / div != 0) {
        div *= 10;
        i++;
    }
    char* result = (char*)malloc((i + 2) * sizeof(char));
    if (result == NULL)
        return NULL;

    result[i] = (char)(sign < 0 ? '-' : '+');
    result[i + 1] = '\0';

    int mod = value;
    for (int j = (int)i - 1; j >= 0; j--) {
        result[j] = toChar(mod % 10);
        mod /= 10;
    }

    return result;
}

char toChar(int value) {
    return value + '0';
}

int checkOnUnoOrBinMinus(Stack* operands, Stack* operators, char* str, size_t* startIndex, int* numberIsLast) {
    size_t j = *startIndex + 1;
    for (; str[j] && isspace(str[j]); j++) {}

    if (isOperator(str[j]) || (isdigit(str[j]) && *numberIsLast)) {
        *numberIsLast = 0;
        return addOperator(operators, '-', operands);
    }

    if (isdigit(str[j]) && !*numberIsLast) {
        *startIndex = j;
        *numberIsLast = 1;
        return addOperand(operands, str, startIndex, -1);
    }

    return 1;
}

int get_priority(char symbol) {
    switch (symbol) {
        case '(':
            return 1;
        case '+':
            return 2;
        case '-':
            return 2;
        case '*':
            return 3;
        case '/':
            return 3;
        default:
            return 0;
    }
}

int createExpression(Stack *operands, char op){
    char* rightValue = copyString(top(operands), strlen(top(operands)));
    if (rightValue == NULL)
        return 1;

    int rightSign = rightValue[strlen(rightValue) - 1] == '-' ? -1 : 1;
    pop(operands);
    printf("%s ", rightValue);

    char* leftValue = copyString(top(operands), strlen(top(operands)));
    if (leftValue == NULL) {
        free(rightValue);
        return 1;
    }
    int leftSign = leftValue[strlen(leftValue) - 1] == '-' ? -1 : 1;
    pop(operands);
    printf("%s ", leftValue);

    printf("%c\n", op);

    char* result = calculateExpression(leftValue, leftSign, rightValue, rightSign, op);
    if (result == NULL) {
        free(leftValue);
        free(rightValue);
        return 1;
    }

    printf("%s\n", result);
    if(push(operands, result) != 0) {
        free(result);
        free(leftValue);
        free(rightValue);
        return 1;
    }

    free(result);
    free(leftValue);
    free(rightValue);
    return 0;
}
char* copyString(char* source, size_t size) {
    char* result = (char*)malloc((size + 1) * sizeof(char));
    if (result == NULL)
        return NULL;
    memcpy(result, source, size + 1);

    return result;
}

char* calculateExpression(char* leftValue, int leftSign, char* rightValue, int rightSign, char op) {
    int result = 0;

    switch (op) {
        case '+':
            result = (toInt(leftValue, strlen(leftValue) - 1) * leftSign)
                     + (toInt(rightValue, strlen(rightValue) - 1) * rightSign);
            break;
        case '-':
            result = (toInt(leftValue, strlen(leftValue) - 1) * leftSign)
                     - (toInt(rightValue, strlen(rightValue) - 1) * rightSign);
            break;
        case '*':
            result = (toInt(leftValue, strlen(leftValue) - 1) * leftSign)
                     * (toInt(rightValue, strlen(rightValue) - 1) * rightSign);
            break;
        case '/':
            result = (toInt(leftValue, strlen(leftValue) - 1) * leftSign)
                     / (toInt(rightValue, strlen(rightValue) - 1) * rightSign);
            break;
        default:
            break;
    }

    int sign = result < 0 ? -1 : 1;
    char* stringResult = toString(abs(result), sign);
    if (stringResult == NULL)
        return NULL;

    return stringResult;
}

int output_result(Stack* operands, Stack* operators) {
    if (operands->real_size == 1 && operators->real_size == 0) {
        //TODO
        printf("%s\n", top(operands));
        return 0;
    }
    else {
        return 1;
    }
}
