#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#define START_MAX_SIZE_FOR_STACK 4
#define BUFFER_FOR_INPUT 64
#define START_MEM_FOR_INPUT 1024

typedef struct LongNumber {
    int* arrayOfNumber;
    size_t size;
    int sign;
}LongNumber;

LongNumber* initLongNumber(size_t size){
    LongNumber* LN = (LongNumber*)malloc(sizeof(LongNumber));
    if(LN == NULL)
        return NULL;

    LN->size = size;
    LN->sign = 1;
    LN->arrayOfNumber = NULL;

    if (size != 0) {
        LN->arrayOfNumber = (int*)calloc(LN->size, sizeof(int));
        if (LN->arrayOfNumber == NULL) {
            free(LN);
            return NULL;
        }
    }

    return LN;
}

void freeLongNumber(LongNumber* LN) {
    free(LN->arrayOfNumber);
    free(LN);
}

//----------------------------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------

char* input_row();  // Ввод сторки
int parseExpression(char *str);  // Вычиление выражения
int addOperatorsAndOperandsInStack(Stack* operators, Stack* operands, char* str);
int isOperator(char symbol);
int addOperator(Stack *operators, char op, Stack *operands); // Добавление операторов в стек
int addOperand(Stack *operands, char* str, size_t* startIndex, int sign);
char toChar(int value);
int checkOnUnoOrBinMinus(Stack* operands, Stack* operators, char* str, size_t* startIndex, int* numberIsLast);
int get_priority(char symbol);  // Приоритеты операций
int createExpression(Stack *operands, char op);  // Вычиление операндов в зависимости от оператора
char* copyString(char* source, size_t size);
char* calculateExpression(char* leftValue, int leftSign, char* rightValue, int rightSign, char op);
//----------------------------------------------------------------------------------------------------------------------
int toDigit(char value);
void deleteZero(LongNumber* targetLN);
void assignmentZero(LongNumber *value);
LongNumber* createLongNumberFromString(char* stringOfNumbers, size_t stringSize, int sign);
char* createStringFromLongNumber(LongNumber* LN);
int reversLongNumber(LongNumber* LN);
//----------------------------------------------------------------------------------------------------------------------
int calcCompareWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue);
//----------------------------------------------------------------------------------------------------------------------
LongNumber* calcAdditionAlgo(LongNumber *leftLN, LongNumber *rightLN);
LongNumber* calcAddition(LongNumber *leftLN, LongNumber *rightLN);
//----------------------------------------------------------------------------------------------------------------------
LongNumber* calcMultiplication(LongNumber *leftLN, LongNumber *rightLN);
//----------------------------------------------------------------------------------------------------------------------
LongNumber* calcSubtractionAlgo(LongNumber *leftLN, LongNumber *rightLN);
LongNumber* calcSubtraction(LongNumber *leftLN, LongNumber *rightLN);
//----------------------------------------------------------------------------------------------------------------------
LongNumber* shiftForLongNumber(LongNumber *value, int difference);
LongNumber* calcDivision(LongNumber* leftLN, LongNumber* rightLN);
//----------------------------------------------------------------------------------------------------------------------
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

char *input_row() {
    ptrdiff_t cur_size = 0;
    size_t empty_size = BUFFER_FOR_INPUT;
    size_t filled_size = 0;
    size_t row_size = START_MEM_FOR_INPUT;

    char *end_row = (char *) malloc(empty_size * sizeof(char));
    if (end_row == NULL)
        return NULL;
    char *row_pointer = (char *) malloc(row_size * sizeof(char));
    if (row_pointer == NULL){
        free(end_row);
        return NULL;
    }

    row_pointer[0] = '\0';
    while (fgets(end_row, empty_size, stdin)) {
        if (end_row[strlen(end_row) - 1] == '\n') {
            end_row[strlen(end_row) - 1] = '\0';
            memcpy(row_pointer + filled_size - cur_size, end_row,
                   strlen(end_row) + 1);
            break;
        }

        if (filled_size >= row_size) {
            row_size *= 2;
            char* temp_row = (char *)realloc(row_pointer, row_size * sizeof(char));
            if (temp_row == NULL) {
                free(end_row);
                return NULL;
            }
            row_pointer = temp_row;
        }

        memcpy(row_pointer + filled_size - cur_size, end_row, empty_size);
        filled_size += empty_size;
        cur_size++;
    }
    free(end_row);

    char* temp_row =
            (char *)realloc(row_pointer, (strlen(row_pointer) + 1) * sizeof(char));
    if (temp_row == NULL) {
        free(row_pointer);
        return NULL;
    }
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

        else if (isdigit(str[i])) {
            numberIsLast = 1;
            if (addOperand(operands, str, &i, 1) != 0)
                return 1;
        }

        else if (str[i] == '-') {
            if(checkOnUnoOrBinMinus(operands, operators, str, &i, &numberIsLast) != 0)
                return 1;
        }

        else if (str[i] == ')') {
            while (*top(operators) != '(') {          // Вычисляем содержимое скобок
                if (createExpression(operands, *top(operators)) != 0)
                    return 1;
                pop(operators);
            }
            pop(operators);
        }
        else
            return 1;
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

int createExpression(Stack *operands, char op) {
    if (op == '(')
        return 1;
    char* rightValue = copyString(top(operands), strlen(top(operands)));
    if (rightValue == NULL)
        return 1;

    int rightSign = rightValue[strlen(rightValue) - 1] == '-' ? -1 : 1;
    pop(operands);

    char* leftValue = copyString(top(operands), strlen(top(operands)));
    if (leftValue == NULL) {
        free(rightValue);
        return 1;
    }
    int leftSign = leftValue[strlen(leftValue) - 1] == '-' ? -1 : 1;
    pop(operands);

    char* result = calculateExpression(leftValue, leftSign, rightValue, rightSign, op);
    if (result == NULL) {
        free(leftValue);
        free(rightValue);
        return 1;
    }

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

//void outputLongNumber(LongNumber* value) {
//    printf("%dsign\n", value->sign);
//    printf("%zusize\n", value->size);
//    for (size_t i = 0; i < value->size; i++) {
//        printf("%d", value->arrayOfNumber[i]);
//    }
//    printf("\n");
//}

char* calculateExpression(char* leftValue, int leftSign, char* rightValue, int rightSign, char op) {
    LongNumber* leftLN = createLongNumberFromString(leftValue, strlen(leftValue) - 1, leftSign);
    if(leftLN == NULL) {
        return NULL;
    }
    LongNumber* rightLN = createLongNumberFromString(rightValue, strlen(rightValue) - 1, rightSign);
    if(rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }
    
    LongNumber* resultLN = NULL;

    switch (op) {
        case '+':
            resultLN = calcAddition(leftLN, rightLN);
            break;
        case '-':
            resultLN = calcSubtraction(leftLN, rightLN);
            break;
        case '*':
            resultLN = calcMultiplication(leftLN, rightLN);
            break;
        case '/':
            resultLN = calcDivision(leftLN, rightLN);
            break;
        default:
            break;
    }

    char* stringResult = createStringFromLongNumber(resultLN);
    if (stringResult == NULL) {
        freeLongNumber(resultLN);
        freeLongNumber(leftLN);
        freeLongNumber(rightLN);
        return NULL;
    }

    freeLongNumber(resultLN);
    freeLongNumber(leftLN);
    freeLongNumber(rightLN);
    return stringResult;
}

LongNumber* calcAdditionAlgo(LongNumber *leftLN, LongNumber *rightLN) {
    size_t resultSize = rightLN->size > leftLN->size ? rightLN->size + 1 : leftLN->size + 1;

    LongNumber* resultLN = initLongNumber(resultSize);
    if(resultLN == NULL) {
        return NULL;
    }
    resultLN->sign = leftLN->sign;

    int leftBuffer = 0;
    int rightBuffer = 0;

    for (size_t i = 0; i < resultLN->size - 1; i++) {
        leftBuffer = leftLN->size > i ? leftLN->arrayOfNumber[i] : 0;
        rightBuffer = rightLN->size > i ? rightLN->arrayOfNumber[i] : 0;

        resultLN->arrayOfNumber[i] += leftBuffer + rightBuffer;
        resultLN->arrayOfNumber[i + 1] += resultLN->arrayOfNumber[i] / 10;
        resultLN->arrayOfNumber[i] %= 10;
    }

    deleteZero(resultLN);

    return resultLN;
}

LongNumber* calcAddition(LongNumber *leftLN, LongNumber *rightLN) {
    LongNumber* resultLN = NULL;

    if (leftLN->sign == rightLN->sign) {
        resultLN = calcAdditionAlgo(leftLN, rightLN);
    } else {
        if (leftLN->sign < 0) {
            leftLN->sign *= -1;
            resultLN = calcSubtractionAlgo(rightLN, leftLN);
        }
        if (rightLN->sign < 0) {
            rightLN->sign *= -1;
            resultLN = calcSubtractionAlgo(leftLN, rightLN);
        }
    }

    if(resultLN == NULL)
        return NULL;

    if (reversLongNumber(resultLN))
        return NULL;

    return resultLN;
}

//---------------------------------------------------------------------------------------------------------------------

LongNumber* calcMultiplication(LongNumber *leftLN, LongNumber *rightLN) {
    size_t resultSize = rightLN->size + leftLN->size;

    LongNumber* resultLN = initLongNumber(resultSize);
    if(resultLN == NULL) {
        return NULL;
    }
    resultLN->sign = leftLN->sign * rightLN->sign;

    for (size_t i = 0; i < leftLN->size; i++) {
        for (size_t j = 0; j < rightLN->size; j++) {
            resultLN->arrayOfNumber[i + j] += leftLN->arrayOfNumber[i] * rightLN->arrayOfNumber[j];
        }
    }

    for (size_t i = 0; i < resultLN->size-1; i++) {
        resultLN->arrayOfNumber[i + 1] += resultLN->arrayOfNumber[i] / 10;
        resultLN->arrayOfNumber[i] %= 10;
    }

    deleteZero(resultLN);
    if (reversLongNumber(resultLN)) {
        return NULL;
    }

    return resultLN;
}

//----------------------------------------------------------------------------------------------------------------------

LongNumber* calcSubtractionAlgo(LongNumber *leftLN, LongNumber *rightLN) {
    if (calcCompareWIthLongNumber(leftLN, rightLN) == -1) {
        LongNumber* tempLN = leftLN;
        leftLN = rightLN;
        rightLN = tempLN;
        leftLN->sign *= -1;
    }

    LongNumber* resultLN = initLongNumber(leftLN->size);
    if(resultLN == NULL) {
        return NULL;
    }
    resultLN->sign = leftLN->sign;

    int buffer = 0;
    int rightBuffer = 0;

    for (size_t i = 0; i < resultLN->size; i++) {
        rightBuffer = rightLN->size > i ? rightLN->arrayOfNumber[i] : 0;
        buffer = leftLN->arrayOfNumber[i] - rightBuffer - buffer;
        resultLN->arrayOfNumber[i] = (buffer + 10) % 10;
        buffer = buffer < 0 ? 1 : 0;
    }

    deleteZero(resultLN);

    return resultLN;
}

LongNumber* calcSubtraction(LongNumber *leftLN, LongNumber *rightLN) {
    LongNumber *resultLN = NULL;

    if (leftLN->sign == rightLN->sign) {
        if (leftLN->sign == 1) {
            resultLN = calcSubtractionAlgo(leftLN, rightLN);
        } else {
            rightLN->sign *= -1;
            leftLN->sign *= -1;
            resultLN = calcSubtractionAlgo(rightLN, leftLN);
        }
    } else {
        resultLN = calcAdditionAlgo(leftLN, rightLN);

    }

    if(resultLN == NULL)
        return NULL;

    if (reversLongNumber(resultLN))
        return NULL;

    return resultLN;
}

//----------------------------------------------------------------------------------------------------------------------

LongNumber* shiftForLongNumber(LongNumber *value, int difference) {
    LongNumber* resultLN = initLongNumber(0);
    if (resultLN == NULL) {
        return NULL;
    }
    resultLN->size = value->size + difference;
    resultLN->arrayOfNumber = (int*)calloc(resultLN->size, sizeof(int));
    if(resultLN->arrayOfNumber == NULL) {
        free(resultLN);
        return NULL;
    }

    for (size_t i = 0; i < value->size; i++) {
        resultLN->arrayOfNumber[i] = value->arrayOfNumber[i];
    }

    return resultLN;
}

LongNumber* calcDivision(LongNumber* leftLN, LongNumber* rightLN) {
    LongNumber* resultLN = initLongNumber(leftLN->size);
    if(resultLN == NULL) {
        return NULL;
    }

    if (calcCompareWIthLongNumber(leftLN, rightLN) == -1) {
        assignmentZero(resultLN);
        return resultLN;
    }

    resultLN->sign = leftLN->sign * rightLN->sign;

    leftLN->sign = rightLN->sign = 1;
    if (reversLongNumber(rightLN)) {
        return NULL;
    }

    int difference = (int)leftLN->size - (int)rightLN->size;

    LongNumber* littleLN = shiftForLongNumber(rightLN, difference);
    LongNumber* biggerLN = initLongNumber(leftLN->size);
    if(biggerLN == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < leftLN->size; i++) {
        biggerLN->arrayOfNumber[i] = leftLN->arrayOfNumber[i];
    }


    LongNumber* tempLN = initLongNumber(leftLN->size + 1);
    if(tempLN == NULL) {
        freeLongNumber(resultLN);
        return NULL;
    }

    if (reversLongNumber(littleLN)) {
        return NULL;
    }
    int digit = 0;

    for (; difference >= 0; difference--) {
        assignmentZero(tempLN);
        digit = 0;

        do {
            LongNumber* temp = calcAdditionAlgo(tempLN, littleLN);
            freeLongNumber(tempLN);
            tempLN = temp;
            digit++;
        } while(calcCompareWIthLongNumber(tempLN, biggerLN) <= 0);

        LongNumber* temp = calcSubtractionAlgo(tempLN, littleLN);
        freeLongNumber(tempLN);
        tempLN = temp;
        digit--;

        temp = calcSubtractionAlgo(biggerLN, tempLN);
        freeLongNumber(biggerLN);
        biggerLN = temp;
        resultLN->arrayOfNumber[difference] = digit;

        if (reversLongNumber(littleLN)) {
            return NULL;
        }
        littleLN->size--;
        if (reversLongNumber(littleLN)) {
            return NULL;
        }
    }

    deleteZero(resultLN);
    if (reversLongNumber(resultLN)) {
        return NULL;
    }
    freeLongNumber(biggerLN);
    freeLongNumber(littleLN);
    freeLongNumber(tempLN);

    return resultLN;
}

//----------------------------------------------------------------------------------------------------------------------

int calcCompareWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue) {
    if (leftValue->size > rightValue->size) {
        return 1;
    } else if (leftValue->size < rightValue->size) {
        return -1;
    }
    else {

        for (int i = (int)leftValue->size - 1; i >= 0; i--) {
            if (leftValue->arrayOfNumber[i] > rightValue->arrayOfNumber[i]) return 1;
            if (leftValue->arrayOfNumber[i] < rightValue->arrayOfNumber[i]) return -1;
        }
        return 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------

LongNumber* createLongNumberFromString(char* stringOfNumbers, size_t stringSize, int sign) {
    LongNumber* LN = initLongNumber(stringSize);
    if(LN == NULL)
        return NULL;

    LN->sign = sign;

    for (int i = (int)LN->size - 1; i >= 0; i--) {
        LN->arrayOfNumber[LN->size - 1 - i] = toDigit(stringOfNumbers[i]);
    }
    return LN;
}

char* createStringFromLongNumber(LongNumber* LN) {
    char* string = (char*)malloc((LN->size + 2) * sizeof(char));
    if (string == NULL)
        return NULL;

    for (size_t i = 0; i < LN->size; i++) {
        string[i] = toChar(LN->arrayOfNumber[i]);
    }
    string[LN->size] = (char)(LN->sign < 0 ? '-' : '+');
    string[LN->size + 1] = '\0';
    return string;
}

int reversLongNumber(LongNumber* LN) {
    int* arrayOfNumber = (int*)calloc(LN->size, sizeof(int));
    if(arrayOfNumber == NULL)
        return 1;

    for (int i = (int)LN->size - 1; i >= 0; i--) {
        arrayOfNumber[LN->size - 1 - i] = LN->arrayOfNumber[i];
    }

    free(LN->arrayOfNumber);
    LN->arrayOfNumber = arrayOfNumber;

    return 0;
}

int toDigit(char value) {
    return value - '0';
}

void deleteZero(LongNumber* targetLN) {
    while (targetLN->arrayOfNumber[targetLN->size - 1] == 0 && targetLN->size > 1) {
        targetLN->size--;
    }
}

void assignmentZero(LongNumber *value) {
    for (size_t i = 0; i < value->size; i++) {
        value->arrayOfNumber[i] = 0;
    }
    deleteZero(value);
}

int output_result(Stack* operands, Stack* operators) {
    if (operands->real_size == 1 && operators->real_size == 0) {
        //TODO
        char* result = top(operands);
        if (result[strlen(result) - 1] == '-')
            printf("%c", result[strlen(result) - 1]);
        result[strlen(result) - 1] = '\0';
        printf("%s\n", top(operands));
        return 0;
    }
    else {
        return 1;
    }
}
