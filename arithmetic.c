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

typedef struct Long_number {
  int *array_of_number;
  size_t size;
  int sign;
} Long_number;

Long_number *init_long_number(size_t size) {
  Long_number *LN = (Long_number *)malloc(sizeof(Long_number));
  if (LN == NULL)
    return NULL;

  LN->size = size;
  LN->sign = 1;
  LN->array_of_number = NULL;

  if (size != 0) {
    LN->array_of_number = (int *)calloc(LN->size, sizeof(int));
    if (LN->array_of_number == NULL) {
      free(LN);
      return NULL;
    }
  }

  return LN;
}

void free_long_number(Long_number *LN) {
  free(LN->array_of_number);
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

char *input_row();                      // Ввод сторки
char *polish_notation(const char *str); // Вычиление выражения
int parse_expression(Stack *operators, Stack *operands, const char *str);
int is_operator(char symbol);
int add_operator_with_calc_operations(
    Stack *operators, char op,
    Stack *operands); // Добавление операторов в стек
int create_and_add_operand(Stack *operands, const char *str,
                           size_t *start_index, int sign);
char *create_operand(const char *str, size_t *start_index, int sign);
int add_operand(Stack *operands, char *new_operand);
int check_minus_and_add_operator_or_create_operation(Stack *operands,
                                                     Stack *operators,
                                                     const char *str,
                                                     size_t *start_index,
                                                     int *number_is_last);
int get_priority(char symbol); // Приоритеты операций
char *find_result_of_expression(Stack *operators, Stack *operands);
int create_operation(Stack *operands,
                     char op); // Вычиление операндов в зависимости от оператора
char *copy_string(const char *source, size_t size);
char *calculate_operation(char *left_value, int left_sign, char *right_value,
                          int right_sign, char op);
//----------------------------------------------------------------------------------------------------------------------
Long_number *create_long_number_from_string(const char *string_of_numbers,
                                            size_t string_size, int sign);
int to_digit(char value);
//----------------------------------------------------------------------------------------------------------------------
Long_number *calc_addition(Long_number *left_LN, Long_number *right_LN);
Long_number *calc_addition_algo(Long_number *left_LN, Long_number *right_LN);
//----------------------------------------------------------------------------------------------------------------------
Long_number *calc_multiplication(Long_number *left_LN, Long_number *right_LN);
//----------------------------------------------------------------------------------------------------------------------
Long_number *calc_subtraction(Long_number *left_LN, Long_number *rightLN);
Long_number *calc_subtraction_algo(Long_number *left_LN, Long_number *right_LN);
//----------------------------------------------------------------------------------------------------------------------
Long_number *calc_division(Long_number **left_LN, Long_number **right_LN);
int number_is_not_zero(Long_number *value);
Long_number *shift_for_long_number(Long_number *value, int difference);
Long_number *calc_division_algo(Long_number **left_LN, Long_number **right_LN,
                                Long_number *result_LN, int index);
//----------------------------------------------------------------------------------------------------------------------
int calc_compare_with_long_number(Long_number *left_LN, Long_number *right_LN);
//----------------------------------------------------------------------------------------------------------------------
void delete_zero(Long_number *target_LN);
int revers_long_number(Long_number *LN);
char *create_string_from_long_number(Long_number *LN);
char to_char(int value);
char *check_result(Stack *operands, Stack *operators); // Вывод результата
void output_result(const char *result);

int main() {
  char *str = input_row();
  if (str == NULL) {
    printf("[error]\n");
    return 0;
  }

  char *result = polish_notation(str);
  if (result == NULL) {
    free(str);
    printf("[error]\n");
    return 0;
  }

  output_result(result);

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

char *polish_notation(const char *str) {
  Stack *operators = create_stack();
  if (operators == NULL)
    return NULL;

  Stack *operands = create_stack();
  if (operands == NULL) {
    free_stack(operators);
    return NULL;
  }

  int do_one_iteration = 1;
  char *result = NULL;

  while (do_one_iteration--) {
    if (parse_expression(operators, operands, str) == EXIT_FAILURE)
      break;

    result = find_result_of_expression(operators, operands);
    if (result == NULL)
      break;
  }

  free_stack(operands);
  free_stack(operators);
  return result;
}

int parse_expression(Stack *operators, Stack *operands, const char *str) {
  size_t i = 0;
  int number_is_last = 0;
  size_t size_of_str = strlen(str);
  char *top_operator_in_stack = NULL;
  for (; i < size_of_str; i++) {

    while (str[i] && isspace(str[i]))
      i++;

    if (is_operator(str[i]) && str[i] != __sub) {
      number_is_last = 0;
      if (add_operator_with_calc_operations(operators, str[i], operands) ==
          EXIT_FAILURE)
        return EXIT_FAILURE;
      continue;
    }

    if (isdigit(str[i])) {
      number_is_last = 1;
      if (create_and_add_operand(operands, str, &i, 1) == EXIT_FAILURE)
        return EXIT_FAILURE;
      continue;
    }

    if (str[i] == __sub) {
      if (check_minus_and_add_operator_or_create_operation(
              operands, operators, str, &i, &number_is_last) == EXIT_FAILURE)
        return EXIT_FAILURE;
      continue;
    }

    if (str[i] == __close_br) {
      top_operator_in_stack = top(operators);
      while (top_operator_in_stack == NULL ||
             *top_operator_in_stack != __open_br) {
        if (top_operator_in_stack == NULL)
          return EXIT_FAILURE;
        if (create_operation(operands, *top_operator_in_stack) == EXIT_FAILURE)
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

int is_operator(const char symbol) {
  return symbol == __mult || symbol == __div || symbol == __add ||
         symbol == __sub || symbol == __open_br;
}

int add_operator_with_calc_operations(Stack *operators, const char op,
                                      Stack *operands) {
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
      if (create_operation(operands, *top(operators)) == EXIT_FAILURE) {
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

int create_and_add_operand(Stack *operands, const char *str,
                           size_t *start_index, const int sign) {
  char *new_operand = create_operand(str, start_index, sign);
  if (add_operand(operands, new_operand) == EXIT_FAILURE) {
    free(new_operand);
    return EXIT_FAILURE;
  }

  free(new_operand);
  return EXIT_SUCCESS;
}

char *create_operand(const char *str, size_t *start_index, const int sign) {
  size_t j = *start_index;
  while (str[j] && isdigit(str[j]))
    j++;

  char *new_operand = (char *)calloc((j - *start_index + 2), sizeof(char));
  if (new_operand == NULL)
    return NULL;
  new_operand[0] = (char)(sign < 0 ? __sub : __add);
  new_operand[j - *start_index + 1] = '\0';

  memcpy(new_operand + 1, str + *start_index, j - *start_index);
  *start_index = --j;

  return new_operand;
}

int add_operand(Stack *operands, char *new_operand) {
  if (push(operands, new_operand) == EXIT_FAILURE)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

int check_minus_and_add_operator_or_create_operation(Stack *operands,
                                                     Stack *operators,
                                                     const char *str,
                                                     size_t *start_index,
                                                     int *number_is_last) {
  size_t j = *start_index + 1;
  while (str[j] && isspace(str[j]))
    j++;

  if (is_operator(str[j]) || (isdigit(str[j]) && *number_is_last)) {
    *number_is_last = 0;
    return add_operator_with_calc_operations(operators, __sub, operands);
  }

  if (isdigit(str[j]) && !*number_is_last) {
    *start_index = j;
    *number_is_last = 1;
    return create_and_add_operand(operands, str, start_index, -1);
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

char *find_result_of_expression(Stack *operators, Stack *operands) {
  while (operators->real_size) {
    if (create_operation(operands, *top(operators)) == EXIT_FAILURE)
      return NULL;

    pop(operators);
  }

  char *result = check_result(operands, operators);
  if (result == NULL)
    return NULL;

  return result;
}

int create_operation(Stack *operands, const char op) {
  if (op == __open_br)
    return EXIT_FAILURE;

  if (top(operands) == NULL)
    return EXIT_FAILURE;
  char *right_value = copy_string(top(operands), strlen(top(operands)));
  if (right_value == NULL)
    return EXIT_FAILURE;
  int right_sign = right_value[0] == __sub ? -1 : 1;
  pop(operands);

  if (top(operands) == NULL) {
    free(right_value);
    return EXIT_FAILURE;
  }
  char *left_value = copy_string(top(operands), strlen(top(operands)));
  if (left_value == NULL) {
    free(right_value);
    return EXIT_FAILURE;
  }
  int left_sign = left_value[0] == __sub ? -1 : 1;
  pop(operands);

  int do_one_iteration = 1;
  int verdict = EXIT_FAILURE;

  while (do_one_iteration--) {
    char *result =
        calculate_operation(left_value, left_sign, right_value, right_sign, op);
    if (result == NULL)
      break;

    if (push(operands, result) == EXIT_FAILURE) {
      free(result);
      break;
    }

    free(result);
    verdict = EXIT_SUCCESS;
  }
  free(left_value);
  free(right_value);
  return verdict;
}
char *copy_string(const char *source, size_t size) {
  char *result = (char *)malloc((size + 1) * sizeof(char));
  if (result == NULL)
    return NULL;
  memcpy(result, source, size + 1);

  return result;
}

char *calculate_operation(char *left_value, int left_sign, char *right_value,
                          int right_sign, char op) {
  Long_number *left_LN = create_long_number_from_string(
      left_value + 1, strlen(left_value) - 1, left_sign);
  if (left_LN == NULL) {
    return NULL;
  }
  Long_number *right_LN = create_long_number_from_string(
      right_value + 1, strlen(right_value) - 1, right_sign);
  if (right_LN == NULL) {
    free_long_number(left_LN);
    return NULL;
  }

  Long_number *result_LN = NULL;

  switch (op) {
  case __add:
    result_LN = calc_addition(left_LN, right_LN);
    break;
  case __sub:
    result_LN = calc_subtraction(left_LN, right_LN);
    break;
  case __mult:
    result_LN = calc_multiplication(left_LN, right_LN);
    break;
  case __div:
    result_LN = calc_division(&left_LN, &right_LN);
    break;
  default:
    break;
  }

  int do_one_iteration = 1;
  char *string_result = NULL;
  while (do_one_iteration--) {
    if (result_LN == NULL)
      break;

    string_result = create_string_from_long_number(result_LN);
    if (string_result == NULL) {
      free_long_number(result_LN);
      break;
    }

    free_long_number(result_LN);
  }

  free_long_number(left_LN);
  free_long_number(right_LN);
  return string_result;
}

//----------------------------------------------------------------------------------------------------------------------

Long_number *create_long_number_from_string(const char *string_of_numbers,
                                            size_t string_size, int sign) {
  Long_number *LN = init_long_number(string_size);
  if (LN == NULL)
    return NULL;

  LN->sign = sign;

  for (int i = (int)LN->size - 1; i >= 0; i--) {
    LN->array_of_number[LN->size - 1 - i] = to_digit(string_of_numbers[i]);
  }
  return LN;
}

int to_digit(char value) { return value - '0'; }

//----------------------------------------------------------------------------------------------------------------------

Long_number *calc_addition(Long_number *left_LN, Long_number *right_LN) {
  Long_number *result_LN = NULL;

  if (left_LN->sign == right_LN->sign) {
    result_LN = calc_addition_algo(left_LN, right_LN);
  } else {
    if (left_LN->sign < 0) {
      left_LN->sign *= -1;
      result_LN = calc_subtraction_algo(right_LN, left_LN);
    }
    if (right_LN->sign < 0) {
      right_LN->sign *= -1;
      result_LN = calc_subtraction_algo(left_LN, right_LN);
    }
  }

  if (result_LN == NULL)
    return NULL;

  if (revers_long_number(result_LN)) {
    free_long_number(result_LN);
    return NULL;
  }

  return result_LN;
}

Long_number *calc_addition_algo(Long_number *left_LN, Long_number *right_LN) {
  size_t result_size =
      right_LN->size > left_LN->size ? right_LN->size + 1 : left_LN->size + 1;

  Long_number *result_LN = init_long_number(result_size);
  if (result_LN == NULL) {
    return NULL;
  }
  result_LN->sign = left_LN->sign;

  int left_buffer = 0;
  int right_buffer = 0;

  for (size_t i = 0; i < result_LN->size - 1; i++) {
    left_buffer = left_LN->size > i ? left_LN->array_of_number[i] : 0;
    right_buffer = right_LN->size > i ? right_LN->array_of_number[i] : 0;

    result_LN->array_of_number[i] += left_buffer + right_buffer;
    result_LN->array_of_number[i + 1] += result_LN->array_of_number[i] / 10;
    result_LN->array_of_number[i] %= 10;
  }

  delete_zero(result_LN);

  return result_LN;
}

//---------------------------------------------------------------------------------------------------------------------

Long_number *calc_multiplication(Long_number *left_LN, Long_number *right_LN) {
  size_t result_size = right_LN->size + left_LN->size;

  Long_number *result_LN = init_long_number(result_size);
  if (result_LN == NULL) {
    return NULL;
  }
  result_LN->sign = left_LN->sign * right_LN->sign;

  for (size_t i = 0; i < left_LN->size; i++) {
    for (size_t j = 0; j < right_LN->size; j++) {
      result_LN->array_of_number[i + j] +=
          left_LN->array_of_number[i] * right_LN->array_of_number[j];
    }
  }

  for (size_t i = 0; i < result_LN->size - 1; i++) {
    result_LN->array_of_number[i + 1] += result_LN->array_of_number[i] / 10;
    result_LN->array_of_number[i] %= 10;
  }

  delete_zero(result_LN);
  if (revers_long_number(result_LN)) {
    free_long_number(result_LN);
    return NULL;
  }

  return result_LN;
}

//----------------------------------------------------------------------------------------------------------------------

Long_number *calc_subtraction(Long_number *left_LN, Long_number *rightLN) {
  Long_number *result_LN = NULL;

  if (left_LN->sign == rightLN->sign) {
    if (left_LN->sign == 1) {
      result_LN = calc_subtraction_algo(left_LN, rightLN);
    } else {
      rightLN->sign *= -1;
      left_LN->sign *= -1;
      result_LN = calc_subtraction_algo(rightLN, left_LN);
    }
  } else {
    result_LN = calc_addition_algo(left_LN, rightLN);
  }

  if (result_LN == NULL)
    return NULL;

  if (revers_long_number(result_LN)) {
    free_long_number(result_LN);
    return NULL;
  }

  return result_LN;
}

Long_number *calc_subtraction_algo(Long_number *left_LN,
                                   Long_number *right_LN) {
  if (calc_compare_with_long_number(left_LN, right_LN) == -1) {
    Long_number *temp_LN = left_LN;
    left_LN = right_LN;
    right_LN = temp_LN;
    left_LN->sign *= -1;
  }

  Long_number *result_LN = init_long_number(left_LN->size);
  if (result_LN == NULL) {
    return NULL;
  }
  result_LN->sign = left_LN->sign;

  int buffer = 0;
  int right_buffer = 0;

  for (size_t i = 0; i < result_LN->size; i++) {
    right_buffer = right_LN->size > i ? right_LN->array_of_number[i] : 0;
    buffer = left_LN->array_of_number[i] - right_buffer - buffer;
    result_LN->array_of_number[i] = (buffer + 10) % 10;
    buffer = buffer < 0 ? 1 : 0;
  }

  delete_zero(result_LN);

  return result_LN;
}

//----------------------------------------------------------------------------------------------------------------------

Long_number *calc_division(Long_number **left_LN, Long_number **right_LN) {
  Long_number *result_LN = init_long_number((*left_LN)->size);
  if (result_LN == NULL) {
    return NULL;
  }
  if (calc_compare_with_long_number(*left_LN, *right_LN) == -1) {
    delete_zero(result_LN);
    return result_LN;
  }

  if (number_is_not_zero(*right_LN) == EXIT_FAILURE) {
    free_long_number(result_LN);
    return NULL;
  }

  result_LN->sign = (*left_LN)->sign * (*right_LN)->sign;
  (*left_LN)->sign = (*right_LN)->sign = 1;

  int difference = (int)(*left_LN)->size - (int)(*right_LN)->size;

  if (revers_long_number(*right_LN)) {
    free_long_number(result_LN);
    return NULL;
  }

  Long_number *temp_LN = shift_for_long_number(*right_LN, difference);
  free_long_number(*right_LN);
  *right_LN = temp_LN;

  if (revers_long_number(*right_LN)) {
    free_long_number(result_LN);
    return NULL;
  }

  temp_LN = calc_division_algo(left_LN, right_LN, result_LN, difference);
  if (temp_LN == NULL) {
    free_long_number(result_LN);
    return NULL;
  }
  result_LN = temp_LN;

  delete_zero(result_LN);
  if (revers_long_number(result_LN)) {
    free_long_number(result_LN);
    return NULL;
  }

  return result_LN;
}

int number_is_not_zero(Long_number *value) {
  Long_number *zero = init_long_number(1);
  if (zero == NULL)
    return EXIT_FAILURE;
  if (calc_compare_with_long_number(value, zero) == 0) {
    free_long_number(zero);
    return EXIT_FAILURE;
  }
  free_long_number(zero);
  return EXIT_SUCCESS;
}

Long_number *shift_for_long_number(Long_number *value, int difference) {
  Long_number *result_LN = init_long_number(0);
  if (result_LN == NULL) {
    return NULL;
  }
  result_LN->size = value->size + difference;
  result_LN->array_of_number = (int *)calloc(result_LN->size, sizeof(int));
  if (result_LN->array_of_number == NULL) {
    free(result_LN);
    return NULL;
  }

  for (size_t i = 0; i < value->size; i++) {
    result_LN->array_of_number[i] = value->array_of_number[i];
  }

  return result_LN;
}

Long_number *calc_division_algo(Long_number **left_LN, Long_number **right_LN,
                                Long_number *result_LN, int index) {
  Long_number *buffer_LN = init_long_number((*left_LN)->size + 1);
  if (buffer_LN == NULL) {
    return NULL;
  }
  int digit = 0;
  Long_number *temp_LN = NULL;
  for (; index >= 0; index--) {
    delete_zero(buffer_LN);
    digit = 0;

    do {
      temp_LN = calc_addition_algo(buffer_LN, *right_LN);
      free_long_number(buffer_LN);
      buffer_LN = temp_LN;
      digit++;
    } while (calc_compare_with_long_number(buffer_LN, *left_LN) <= 0);

    temp_LN = calc_subtraction_algo(buffer_LN, *right_LN);
    free_long_number(buffer_LN);
    buffer_LN = temp_LN;
    digit--;

    temp_LN = calc_subtraction_algo(*left_LN, buffer_LN);
    free_long_number(*left_LN);
    *left_LN = temp_LN;
    result_LN->array_of_number[index] = digit;

    if (revers_long_number(*right_LN)) {
      free_long_number(buffer_LN);
      return NULL;
    }
    (*right_LN)->size--;
    if (revers_long_number(*right_LN)) {
      free_long_number(buffer_LN);
      return NULL;
    }
  }

  free_long_number(buffer_LN);
  return result_LN;
}

//----------------------------------------------------------------------------------------------------------------------

int calc_compare_with_long_number(Long_number *left_LN, Long_number *right_LN) {
  if (left_LN->size > right_LN->size) {
    return 1;
  } else if (left_LN->size < right_LN->size) {
    return -1;
  } else {

    for (int i = (int)left_LN->size - 1; i >= 0; i--) {
      if (left_LN->array_of_number[i] > right_LN->array_of_number[i])
        return 1;
      if (left_LN->array_of_number[i] < right_LN->array_of_number[i])
        return -1;
    }
    return 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------

int revers_long_number(Long_number *LN) {
  int *array_of_number = (int *)calloc(LN->size, sizeof(int));
  if (array_of_number == NULL)
    return EXIT_FAILURE;

  for (int i = (int)LN->size - 1; i >= 0; i--) {
    array_of_number[LN->size - 1 - i] = LN->array_of_number[i];
  }

  free(LN->array_of_number);
  LN->array_of_number = array_of_number;

  return EXIT_SUCCESS;
}

void delete_zero(Long_number *target_LN) {
  while (target_LN->array_of_number[target_LN->size - 1] == 0 &&
         target_LN->size > 1) {
    target_LN->size--;
  }
}

char *create_string_from_long_number(Long_number *LN) {
  char *string = (char *)malloc((LN->size + 2) * sizeof(char));
  if (string == NULL)
    return NULL;

  for (size_t i = 1; i <= LN->size; i++) {
    string[i] = to_char(LN->array_of_number[i - 1]);
  }
  string[0] = (char)(LN->sign < 0 ? __sub : __add);
  string[LN->size + 1] = '\0';
  return string;
}

char to_char(int value) { return value + '0'; }

char *check_result(Stack *operands, Stack *operators) {
  if (operands->real_size == 1 && operators->real_size == 0) {
    char *result = copy_string(top(operands), strlen(top(operands)));
    if (result == NULL)
      return NULL;

    return result;
  } else {
    return NULL;
  }
}

void output_result(const char *result) {
  if (result[0] == __sub)
    printf("%c", result[0]);
  printf("%s\n", result + 1);
}

