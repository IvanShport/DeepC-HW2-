#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_MAX_SIZE_FOR_STACK 4
#define BUFFER_FOR_INPUT 64
#define START_MEM_FOR_INPUT 1024

#define __add '+'
#define __sub '-'
#define __mult '*'
#define __div '/'
#define __open_br '('
#define __close_br ')'

typedef struct LongNumber {
  int *arrayOfNumber;
  size_t size;
  int sign;
} LongNumber;

LongNumber *initLongNumber(size_t size) {
  LongNumber *LN = (LongNumber *)malloc(sizeof(LongNumber));
  if (LN == NULL)
    return NULL;

  LN->size = size;
  LN->sign = 1;
  LN->arrayOfNumber = NULL;

  if (size != 0) {
    LN->arrayOfNumber = (int *)calloc(LN->size, sizeof(int));
    if (LN->arrayOfNumber == NULL) {
      free(LN);
      return NULL;
    }
  }

  return LN;
}

void freeLongNumber(LongNumber *LN) {
  free(LN->arrayOfNumber);
  free(LN);
}

//----------------------------------------------------------------------------------------------------------------------

typedef struct Stack { //  Стек для операндов и операторов
  char **strings;
  size_t real_size;
  size_t max_size;
} Stack;

Stack *create_stack() {
  Stack *stack = (Stack *)malloc(sizeof(Stack));
  if (stack == NULL)
    return NULL;
  stack->max_size = START_MAX_SIZE_FOR_STACK;
  stack->real_size = 0;
  stack->strings = (char **)malloc(sizeof(char *) * stack->max_size);
  if (stack->strings == NULL) {
    free(stack);
    return NULL;
  }
  return stack;
}

void free_stack(Stack *stack) {
  for (size_t i = 0; i < stack->real_size; i++) {
    free(stack->strings[i]);
  }
  free(stack->strings);
  free(stack);
}

char **realloc_for_stack(Stack *stack) {
  stack->max_size *= 2;
  char **temp = realloc(stack->strings, stack->max_size * sizeof(char *));
  if (temp == NULL)
    return NULL;
  stack->strings = temp;
  return stack->strings;
}

void pop(Stack *stack) {
  if (!stack->real_size)
    return;

  free(stack->strings[stack->real_size - 1]);
  stack->real_size--;
}

int push(Stack *stack, char *new_value) {
  if (stack->real_size == stack->max_size) {
    if (realloc_for_stack(stack) == NULL)
      return EXIT_FAILURE;
  }

  char *copy_new_value = (char *)malloc((strlen(new_value) + 1) * sizeof(char));
  if (copy_new_value == NULL)
    return EXIT_FAILURE;
  memcpy(copy_new_value, new_value, strlen(new_value));
  copy_new_value[strlen(new_value)] = '\0';

  stack->strings[stack->real_size] = copy_new_value;
  stack->real_size++;
  return EXIT_SUCCESS;
}

char *top(Stack *stack) {
  return stack->real_size != 0 ? stack->strings[stack->real_size - 1] : NULL;
}

//----------------------------------------------------------------------------------------------------------------------

char *input_row();                          // Ввод сторки
char *calculateExpression(const char *str); // Вычиление выражения
int parseExpression(Stack *operators, Stack *operands, const char *str);
int isOperator(char symbol);
int addOperator(Stack *operators, char op,
                Stack *operands); // Добавление операторов в стек
int addOperand(Stack *operands, const char *str, size_t *startIndex, int sign);
int checkOnUnoOrBinMinus(Stack *operands, Stack *operators, const char *str,
                         size_t *startIndex, int *numberIsLast);
int get_priority(char symbol); // Приоритеты операций
char *findResultOfExpression(Stack *operators, Stack *operands);
int createOperation(Stack *operands,
                    char op); // Вычиление операндов в зависимости от оператора
char *copyString(const char *source, size_t size);
char *calculateOperation(char *leftValue, int leftSign, char *rightValue,
                         int rightSign, char op);
//----------------------------------------------------------------------------------------------------------------------
LongNumber *createLongNumberFromString(const char *stringOfNumbers,
                                       size_t stringSize, int sign);
int toDigit(char value);
//----------------------------------------------------------------------------------------------------------------------
LongNumber *calcAddition(LongNumber *leftLN, LongNumber *rightLN);
LongNumber *calcAdditionAlgo(LongNumber *leftLN, LongNumber *rightLN);
//----------------------------------------------------------------------------------------------------------------------
LongNumber *calcMultiplication(LongNumber *leftLN, LongNumber *rightLN);
//----------------------------------------------------------------------------------------------------------------------
LongNumber *calcSubtraction(LongNumber *leftLN, LongNumber *rightLN);
LongNumber *calcSubtractionAlgo(LongNumber *leftLN, LongNumber *rightLN);
//----------------------------------------------------------------------------------------------------------------------
LongNumber *calcDivision(LongNumber **leftLN, LongNumber **rightLN);
int numberIsNotZero(LongNumber *value);
LongNumber *shiftForLongNumber(LongNumber *value, int difference);
LongNumber *calcDivisionAlgo(LongNumber **leftLN, LongNumber **rightLN,
                             LongNumber *resultLN, int index);
//----------------------------------------------------------------------------------------------------------------------
int calcCompareWIthLongNumber(LongNumber *leftValue, LongNumber *rightValue);
//----------------------------------------------------------------------------------------------------------------------
void deleteZero(LongNumber *targetLN);
int reversLongNumber(LongNumber *LN);
char *createStringFromLongNumber(LongNumber *LN);
char toChar(int value);
char *checkResult(Stack *operands, Stack *operators); // Вывод результата
void outputResult(const char *result);

int main() {
  char *str = input_row();
  if (str == NULL) {
    printf("[error]\n");
    return 0;
  }

  char *result = calculateExpression(str);
  if (result == NULL) {
    free(str);
    printf("[error]\n");
    return 0;
  }

  outputResult(result);

  free(result);
  free(str);
  return 0;
}

char *input_row() {
  ptrdiff_t cur_size = 0;
  size_t empty_size = BUFFER_FOR_INPUT;
  size_t filled_size = 0;
  size_t row_size = START_MEM_FOR_INPUT;

  char *end_row = (char *)malloc(empty_size * sizeof(char));
  if (end_row == NULL)
    return NULL;
  char *row_pointer = (char *)malloc(row_size * sizeof(char));
  if (row_pointer == NULL) {
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
      char *temp_row = (char *)realloc(row_pointer, row_size * sizeof(char));
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

  char *temp_row =
      (char *)realloc(row_pointer, (strlen(row_pointer) + 1) * sizeof(char));
  if (temp_row == NULL) {
    free(row_pointer);
    return NULL;
  }
  row_pointer = temp_row;

  return row_pointer;
}

char *calculateExpression(const char *str) {
  Stack *operators = create_stack();
  if (operators == NULL)
    return NULL;

  Stack *operands = create_stack();
  if (operands == NULL) {
    free_stack(operators);
    return NULL;
  }

  if (parseExpression(operators, operands, str) == EXIT_FAILURE) {
    free_stack(operands);
    free_stack(operators);
    return NULL;
  }

  char *result = findResultOfExpression(operators, operands);
  if (result == NULL) {
    free_stack(operators);
    free_stack(operands);
    return NULL;
  }

  free_stack(operators);
  free_stack(operands);

  return result;
}

int parseExpression(Stack *operators, Stack *operands, const char *str) {
  size_t i = 0;
  int numberIsLast = 0;
  size_t size_of_str = strlen(str);
  char *top_operator_in_stack = NULL;
  for (; i < size_of_str; i++) {

    while (str[i] && isspace(str[i])) i++;

    if (isOperator(str[i]) && str[i] != __sub) {
      numberIsLast = 0;
      if (addOperator(operators, str[i], operands) == EXIT_FAILURE)
        return EXIT_FAILURE;
      continue;
    }

    if (isdigit(str[i])) {
      numberIsLast = 1;
      if (addOperand(operands, str, &i, 1) == EXIT_FAILURE)
        return EXIT_FAILURE;
      continue;
    }

    if (str[i] == __sub) {
      if (checkOnUnoOrBinMinus(operands, operators, str, &i, &numberIsLast) ==
        EXIT_FAILURE)
        return EXIT_FAILURE;
      continue;
    }

    if (str[i] == __close_br) {
      top_operator_in_stack = top(operators);
      while (top_operator_in_stack == NULL ||
             *top_operator_in_stack != __open_br) {
        if (top_operator_in_stack == NULL)
          return EXIT_FAILURE;
        if (createOperation(operands, *top_operator_in_stack) == EXIT_FAILURE)
          return EXIT_FAILURE;
        pop(operators);
        top_operator_in_stack = top(operators);
      }
      pop(operators);
      continue;
    }

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int isOperator(const char symbol) {
  return symbol == __mult || symbol == __div || symbol == __add || symbol == __sub ||
         symbol == __open_br;
}

int addOperator(Stack *operators, const char op, Stack *operands) {
  char *temp = (char *)malloc(2 * sizeof(char));
  if (temp == NULL)
    return EXIT_FAILURE;
  temp[0] = op;
  temp[1] = '\0';

  if (*temp == __open_br || operators->real_size == 0 ||
      get_priority(*temp) > get_priority(*top(operators))) {
    if (push(operators, temp) == EXIT_FAILURE) {
      free(temp);
      return EXIT_FAILURE;
    }
  }

  else {
    while (operators->real_size != 0 &&
           get_priority(*top(operators)) >= get_priority(*temp)) {
      if (createOperation(operands, *top(operators)) == EXIT_FAILURE) {
        free(temp);
        return EXIT_FAILURE;
      }
      pop(operators);
    }
    if (push(operators, temp) == EXIT_FAILURE) {
      free(temp);
      return EXIT_FAILURE;
    }
  }
  free(temp);
  return EXIT_SUCCESS;
}

int addOperand(Stack *operands, const char *str, size_t *startIndex,
               const int sign) {
  size_t j = *startIndex;
  for (; str[j] && isdigit(str[j]); j++) {
  }

  char *temp = (char *)calloc((j - *startIndex + 2), sizeof(char));
  if (temp == NULL)
    return EXIT_FAILURE;
  temp[0] = (char)(sign < 0 ? __sub : __add);
  temp[j - *startIndex + 1] = '\0';

  memcpy(temp + 1, str + *startIndex, j - *startIndex);

  if (push(operands, temp) == EXIT_FAILURE) {
    free(temp);
    return EXIT_FAILURE;
  }
  *startIndex = --j;
  free(temp);

  return EXIT_SUCCESS;
}

int checkOnUnoOrBinMinus(Stack *operands, Stack *operators, const char *str,
                         size_t *startIndex, int *numberIsLast) {
  size_t j = *startIndex + 1;
  for (; str[j] && isspace(str[j]); j++) {
  }

  if (isOperator(str[j]) || (isdigit(str[j]) && *numberIsLast)) {
    *numberIsLast = 0;
    return addOperator(operators, __sub, operands);
  }

  if (isdigit(str[j]) && !*numberIsLast) {
    *startIndex = j;
    *numberIsLast = 1;
    return addOperand(operands, str, startIndex, -1);
  }

  return EXIT_FAILURE;
}

int get_priority(const char symbol) {
  switch (symbol) {
  case __open_br:
    return 1;
  case __add:
    return 2;
  case __sub:
    return 2;
  case __mult:
    return 3;
  case __div:
    return 3;
  default:
    return 0;
  }
}

char *findResultOfExpression(Stack *operators, Stack *operands) {
  while (operators->real_size) {
    if (createOperation(operands, *top(operators)) == EXIT_FAILURE)
      return NULL;

    pop(operators);
  }

  char *result = checkResult(operands, operators);
  if (result == NULL)
    return NULL;

  return result;
}

int createOperation(Stack *operands, const char op) {
  if (op == __open_br)
    return EXIT_FAILURE;

  if (top(operands) == NULL)
    return EXIT_FAILURE;
  char *rightValue = copyString(top(operands), strlen(top(operands)));
  if (rightValue == NULL)
    return EXIT_FAILURE;
  int rightSign = rightValue[0] == __sub ? -1 : 1;
  pop(operands);

  if (top(operands) == NULL) {
    free(rightValue);
    return EXIT_FAILURE;
  }
  char *leftValue = copyString(top(operands), strlen(top(operands)));
  if (leftValue == NULL) {
    free(rightValue);
    return EXIT_FAILURE;
  }
  int leftSign = leftValue[0] == __sub ? -1 : 1;
  pop(operands);

  char *result =
      calculateOperation(leftValue, leftSign, rightValue, rightSign, op);
  if (result == NULL) {
    free(leftValue);
    free(rightValue);
    return EXIT_FAILURE;
  }

  if (push(operands, result) == EXIT_FAILURE) {
    free(result);
    free(leftValue);
    free(rightValue);
    return EXIT_FAILURE;
  }

  free(result);
  free(leftValue);
  free(rightValue);
  return EXIT_SUCCESS;
}
char *copyString(const char *source, const size_t size) {
  char *result = (char *)malloc((size + 1) * sizeof(char));
  if (result == NULL)
    return NULL;
  memcpy(result, source, size + 1);

  return result;
}

char *calculateOperation(char *leftValue, int leftSign, char *rightValue,
                         int rightSign, const char op) {
  LongNumber *leftLN = createLongNumberFromString(
      leftValue + 1, strlen(leftValue) - 1, leftSign);
  if (leftLN == NULL) {
    return NULL;
  }
  LongNumber *rightLN = createLongNumberFromString(
      rightValue + 1, strlen(rightValue) - 1, rightSign);
  if (rightLN == NULL) {
    freeLongNumber(leftLN);
    return NULL;
  }

  LongNumber *resultLN = NULL;

  switch (op) {
  case __add:
    resultLN = calcAddition(leftLN, rightLN);
    break;
  case __sub:
    resultLN = calcSubtraction(leftLN, rightLN);
    break;
  case __mult:
    resultLN = calcMultiplication(leftLN, rightLN);
    break;
  case __div:
    resultLN = calcDivision(&leftLN, &rightLN);
    break;
  default:
    break;
  }

  if (resultLN == NULL) {
    freeLongNumber(leftLN);
    freeLongNumber(rightLN);
    return NULL;
  }

  char *stringResult = createStringFromLongNumber(resultLN);
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

//----------------------------------------------------------------------------------------------------------------------

LongNumber *createLongNumberFromString(const char *stringOfNumbers,
                                       const size_t stringSize,
                                       const int sign) {
  LongNumber *LN = initLongNumber(stringSize);
  if (LN == NULL)
    return NULL;

  LN->sign = sign;

  for (int i = (int)LN->size - 1; i >= 0; i--) {
    LN->arrayOfNumber[LN->size - 1 - i] = toDigit(stringOfNumbers[i]);
  }
  return LN;
}

int toDigit(const char value) { return value - '0'; }

//----------------------------------------------------------------------------------------------------------------------

LongNumber *calcAddition(LongNumber *leftLN, LongNumber *rightLN) {
  LongNumber *resultLN = NULL;

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

  if (resultLN == NULL)
    return NULL;

  if (reversLongNumber(resultLN)) {
    freeLongNumber(resultLN);
    return NULL;
  }

  return resultLN;
}

LongNumber *calcAdditionAlgo(LongNumber *leftLN, LongNumber *rightLN) {
  size_t resultSize =
      rightLN->size > leftLN->size ? rightLN->size + 1 : leftLN->size + 1;

  LongNumber *resultLN = initLongNumber(resultSize);
  if (resultLN == NULL) {
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

//---------------------------------------------------------------------------------------------------------------------

LongNumber *calcMultiplication(LongNumber *leftLN, LongNumber *rightLN) {
  size_t resultSize = rightLN->size + leftLN->size;

  LongNumber *resultLN = initLongNumber(resultSize);
  if (resultLN == NULL) {
    return NULL;
  }
  resultLN->sign = leftLN->sign * rightLN->sign;

  for (size_t i = 0; i < leftLN->size; i++) {
    for (size_t j = 0; j < rightLN->size; j++) {
      resultLN->arrayOfNumber[i + j] +=
          leftLN->arrayOfNumber[i] * rightLN->arrayOfNumber[j];
    }
  }

  for (size_t i = 0; i < resultLN->size - 1; i++) {
    resultLN->arrayOfNumber[i + 1] += resultLN->arrayOfNumber[i] / 10;
    resultLN->arrayOfNumber[i] %= 10;
  }

  deleteZero(resultLN);
  if (reversLongNumber(resultLN)) {
    freeLongNumber(resultLN);
    return NULL;
  }

  return resultLN;
}

//----------------------------------------------------------------------------------------------------------------------

LongNumber *calcSubtraction(LongNumber *leftLN, LongNumber *rightLN) {
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

  if (resultLN == NULL)
    return NULL;

  if (reversLongNumber(resultLN)) {
    freeLongNumber(resultLN);
    return NULL;
  }

  return resultLN;
}

LongNumber *calcSubtractionAlgo(LongNumber *leftLN, LongNumber *rightLN) {
  if (calcCompareWIthLongNumber(leftLN, rightLN) == -1) {
    LongNumber *tempLN = leftLN;
    leftLN = rightLN;
    rightLN = tempLN;
    leftLN->sign *= -1;
  }

  LongNumber *resultLN = initLongNumber(leftLN->size);
  if (resultLN == NULL) {
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

//----------------------------------------------------------------------------------------------------------------------

LongNumber *calcDivision(LongNumber **leftLN, LongNumber **rightLN) {
  LongNumber *resultLN = initLongNumber((*leftLN)->size);
  if (resultLN == NULL) {
    return NULL;
  }
  if (calcCompareWIthLongNumber(*leftLN, *rightLN) == -1) {
    deleteZero(resultLN);
    return resultLN;
  }

  if (numberIsNotZero(*rightLN) == EXIT_FAILURE) {
    freeLongNumber(resultLN);
    return NULL;
  }

  resultLN->sign = (*leftLN)->sign * (*rightLN)->sign;
  (*leftLN)->sign = (*rightLN)->sign = 1;

  int difference = (int)(*leftLN)->size - (int)(*rightLN)->size;

  if (reversLongNumber(*rightLN)) {
    freeLongNumber(resultLN);
    return NULL;
  }

  LongNumber *tempLN = shiftForLongNumber(*rightLN, difference);
  freeLongNumber(*rightLN);
  *rightLN = tempLN;

  if (reversLongNumber(*rightLN)) {
    freeLongNumber(resultLN);
    return NULL;
  }

  tempLN = calcDivisionAlgo(leftLN, rightLN, resultLN, difference);
  if (tempLN == NULL) {
    freeLongNumber(resultLN);
    return NULL;
  }
  resultLN = tempLN;

  deleteZero(resultLN);
  if (reversLongNumber(resultLN)) {
    freeLongNumber(resultLN);
    return NULL;
  }

  return resultLN;
}

int numberIsNotZero(LongNumber *value) {
  LongNumber *zero = initLongNumber(1);
  if (zero == NULL)
    return EXIT_FAILURE;
  if (calcCompareWIthLongNumber(value, zero) == 0) {
    freeLongNumber(zero);
    return EXIT_FAILURE;
  }
  freeLongNumber(zero);
  return EXIT_SUCCESS;
}

LongNumber *shiftForLongNumber(LongNumber *value, const int difference) {
  LongNumber *resultLN = initLongNumber(0);
  if (resultLN == NULL) {
    return NULL;
  }
  resultLN->size = value->size + difference;
  resultLN->arrayOfNumber = (int *)calloc(resultLN->size, sizeof(int));
  if (resultLN->arrayOfNumber == NULL) {
    free(resultLN);
    return NULL;
  }

  for (size_t i = 0; i < value->size; i++) {
    resultLN->arrayOfNumber[i] = value->arrayOfNumber[i];
  }

  return resultLN;
}

LongNumber *calcDivisionAlgo(LongNumber **leftLN, LongNumber **rightLN,
                             LongNumber *resultLN, int index) {
  LongNumber *bufferLN = initLongNumber((*leftLN)->size + 1);
  if (bufferLN == NULL) {
    return NULL;
  }
  int digit = 0;
  LongNumber *tempLN = NULL;
  for (; index >= 0; index--) {
    deleteZero(bufferLN);
    digit = 0;

    do {
      tempLN = calcAdditionAlgo(bufferLN, *rightLN);
      freeLongNumber(bufferLN);
      bufferLN = tempLN;
      digit++;
    } while (calcCompareWIthLongNumber(bufferLN, *leftLN) <= 0);

    tempLN = calcSubtractionAlgo(bufferLN, *rightLN);
    freeLongNumber(bufferLN);
    bufferLN = tempLN;
    digit--;

    tempLN = calcSubtractionAlgo(*leftLN, bufferLN);
    freeLongNumber(*leftLN);
    *leftLN = tempLN;
    resultLN->arrayOfNumber[index] = digit;

    if (reversLongNumber(*rightLN)) {
      freeLongNumber(bufferLN);
      return NULL;
    }
    (*rightLN)->size--;
    if (reversLongNumber(*rightLN)) {
      freeLongNumber(bufferLN);
      return NULL;
    }
  }

  freeLongNumber(bufferLN);
  return resultLN;
}

//----------------------------------------------------------------------------------------------------------------------

int calcCompareWIthLongNumber(LongNumber *leftValue, LongNumber *rightValue) {
  if (leftValue->size > rightValue->size) {
    return 1;
  } else if (leftValue->size < rightValue->size) {
    return -1;
  } else {

    for (int i = (int)leftValue->size - 1; i >= 0; i--) {
      if (leftValue->arrayOfNumber[i] > rightValue->arrayOfNumber[i])
        return 1;
      if (leftValue->arrayOfNumber[i] < rightValue->arrayOfNumber[i])
        return -1;
    }
    return 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------

int reversLongNumber(LongNumber *LN) {
  int *arrayOfNumber = (int *)calloc(LN->size, sizeof(int));
  if (arrayOfNumber == NULL)
    return EXIT_FAILURE;

  for (int i = (int)LN->size - 1; i >= 0; i--) {
    arrayOfNumber[LN->size - 1 - i] = LN->arrayOfNumber[i];
  }

  free(LN->arrayOfNumber);
  LN->arrayOfNumber = arrayOfNumber;

  return EXIT_SUCCESS;
}

void deleteZero(LongNumber *targetLN) {
  while (targetLN->arrayOfNumber[targetLN->size - 1] == 0 &&
         targetLN->size > 1) {
    targetLN->size--;
  }
}

char *createStringFromLongNumber(LongNumber *LN) {
  char *string = (char *)malloc((LN->size + 2) * sizeof(char));
  if (string == NULL)
    return NULL;

  for (size_t i = 1; i <= LN->size; i++) {
    string[i] = toChar(LN->arrayOfNumber[i - 1]);
  }
  string[0] = (char)(LN->sign < 0 ? __sub : __add);
  string[LN->size + 1] = '\0';
  return string;
}

char toChar(const int value) { return value + '0'; }

char *checkResult(Stack *operands, Stack *operators) {
  if (operands->real_size == 1 && operators->real_size == 0) {
    char *result = copyString(top(operands), strlen(top(operands)));
    if (result == NULL)
      return NULL;

    return result;
  } else {
    return NULL;
  }
}

void outputResult(const char *result) {
  if (result[0] == __sub)
    printf("%c", result[0]);
  printf("%s\n", result + 1);
}

