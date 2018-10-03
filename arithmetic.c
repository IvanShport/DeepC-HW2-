#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#define START_MAX_SIZE_FOR_VECTOR 4
#define START_MAX_SIZE_FOR_STACK 4
#define START_SIZE_FOR_STRING 32

const char* __true = "True";
const char* __false = "False";
const char* __mult = "*";
const char* __add = "+";
const char* __sub = "-";
const char* __div = "/";

typedef struct Stack{  //  Стек для операндов и операторов
    void* values;
    size_t real_size;
    size_t max_size;
}Stack;

Stack* create_stack(size_t item_size){
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if(stack == NULL) return NULL;
    stack->max_size = START_MAX_SIZE_FOR_STACK;
    stack->real_size = 0;
    stack->values = malloc(item_size * stack->max_size);
    if(stack->values == NULL){
        free(stack);
        return NULL;
    }
    return stack;
}

void free_stack(Stack* stack){
    free(stack->values);
    free(stack);
}

void* realloc_for_stack(Stack* stack, size_t item_size){
    stack->max_size *= 2;
    void* temp = realloc(stack->values, stack->max_size * item_size);
    if(temp == NULL) return NULL;
    stack->values = temp;
    return stack->values;
}

void pop(Stack* stack){
    if(!stack->real_size)
        return;

    stack->real_size--;
}

int push(Stack* stack, void* new_value, size_t item_size){
    if(stack->real_size == stack->max_size){
        if(realloc_for_stack(stack, item_size) == NULL) return 1;
    }
    memcpy(stack->values + stack->real_size, new_value, item_size);
    stack->real_size++;
    return 0;
}

void* top(Stack* stack){
    return stack->real_size != 0? stack->values + stack->real_size - 1: NULL;
}

//-------------------------------------------------------------------

char* input_row();  // Ввод сторки
int calc(char* str);  // Вычиление выражения
int search_const_name(char* str, const char* const_str);
int search_true(char* str, size_t i);
int set_operator(Stack* operators, char op, Stack* operands); // Добавление операторов в стек
int get_priority(char symbol);  // Приоритеты операций
int calculate(Stack* operands, char op);  // Вычиление операндов в зависимости от оператора
int output_result(Stack* operands, Stack* operators); // Вывод результата

int main(){
    char* str = input_row();
    if(str == NULL){
        printf("[error]\n");
        return 0;
    }

    if(calc(str) != 0){
        printf("[error]\n");
        return 0;
    }

    return 0;
}

char* input_row(){
    size_t size = START_SIZE_FOR_STRING;
    char* row_pointer = (char*)malloc(size * sizeof(char));
    if(row_pointer == NULL) return NULL;

    ptrdiff_t cur_size = 1;
    size_t empty_size = size;

    char* end_row = row_pointer;

    row_pointer[0] = '\0';
    while(fgets(end_row, empty_size, stdin)){
        if(end_row[strlen(end_row) - 1] == '\n'){
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
    if(temp_row == NULL) return NULL;
    row_pointer = temp_row;

    return row_pointer;
}

int calc(char* str){
    Stack* operators = create_stack(sizeof(char));
    if(operators == NULL){
        free(str);
        return 1;
    }
    Stack* operands = create_stack(sizeof(int));
    if(operands == NULL){
        free_stack(operators);
        free(str);
        return 1;
    }
    size_t i = 0;
    size_t j = 0;
    for(; i < strlen(str);){

        for(; str[i] && isspace(str[i]); i++){}

        j = i;
        for(; str[j] &&
              !isspace(str[j]) &&
              str[j] != '(' &&
              str[j] != ')';
              j++){}

        char* temp = (char*)calloc((j - i + 1), sizeof(char));
        if(temp == NULL){
            free(str);
            free_stack(operators);
            free_stack(operands);
            return 1;
        }

        temp = memcpy(temp, str + i, j - i);
        i = j;

        if(search_const_name(temp, __mult)){
            if(set_operator(operators, '*', operands) != 0){
                free(temp);
                free(str);
                free_stack(operators);
                free_stack(operands);
                return 1;
            }
        }
        else if(search_const_name(temp, __div)){
            if(set_operator(operators, '/', operands) != 0){
                free(temp);
                free(str);
                free_stack(operators);
                free_stack(operands);
                return 1;
            }
        }
        else if(search_const_name(temp, __sub)){
            if(set_operator(operators, '-', operands) != 0){
                free(temp);
                free(str);
                free_stack(operators);
                free_stack(operands);
                return 1;
            }
        }
        else if(search_const_name(temp, __add)){
            if(set_operator(operators, '+', operands) != 0){
                free(temp);
                free(str);
                free_stack(operators);
                free_stack(operands);
                return 1;
            }
        }
        else if(search_const_name(temp, __true)){
            int true_value = 1;
            if(operators->real_size != 0 && *(char*)(top(operators)) == '~'){
                true_value = !true_value;
                pop(operators);
            }
            if(push(operands, &true_value, sizeof(int)) != 0){
                free(temp);
                free(str);
                free_stack(operators);
                free_stack(operands);
                return 1;
            }
        }
        else if(search_const_name(temp, __false)){
            int false_value = 0;
            if(operators->real_size != 0 && *(char*)(top(operators)) == '~'){
                false_value = !false_value;
                pop(operators);
            }
            if(push(operands, &false_value, sizeof(int)) != 0){
                free(temp);
                free(str);
                free_stack(operators);
                free_stack(operands);
                return 1;
            }
        }
        else if(str[i] == '('){           // Поиск оператора (
            i++;
            if(set_operator(operators, '(', operands) != 0){
                free(temp);
                free(str);
                free_stack(operators);
                free_stack(operands);
                return 1;
            }
        }
        else if(str[i] == ')'){          // Поиск оператора )
            i++;
            while(*(char*)(top(operators)) != '('){          // Вычисляем содержимое скобок
                if(calculate(operands, *(char*)(top(operators))) != 0){
                    free(temp);
                    free(str);
                    free_stack(operators);
                    free_stack(operands);
                    return 1;
                }
                pop(operators);
            }
            pop(operators);
            if(operators->real_size != 0 && *(char*)(top(operators)) == '~'){  // Проверка на оператор not
                int new_value = !(*(int*)top(operands) % 256);
                pop(operands);
                if(push(operands, &new_value, sizeof(int)) != 0){
                    free(temp);
                    free(str);
                    free_stack(operators);
                    free_stack(operands);
                    return 1;
                }

                pop(operators);
            }
        }
        free(temp);

    }

    free(str);
    while(operators->real_size){         //  Вычисление выражения
        if(calculate(operands, *(char*)(top(operators))) != 0){
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

int search_const_name(char* str, const char* const_str){
    if(strcmp(str, const_str) == 0){
        return 1;
    }
    return 0;
}

int set_operator(Stack* operators, char op, Stack* operands){
    if(op == '(' ||
       operators->real_size == 0 ||
       get_priority(op) >= get_priority(*(char*)(top(operators))))
    {
        if(push(operators, &op, sizeof(op)) != 0) return 1;
    }
    else{
        while(operators->real_size != 0 &&
              get_priority(*(char*)(top(operators))) > get_priority(op))
        {
            if(calculate(operands, *(char*)(top(operators))) != 0) return 1;
            pop(operators);
        }
        if(push(operators, &op, sizeof(op)) != 0) return 1;
    }
    return 0;
}

int get_priority(char symbol){
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

int calculate(Stack* operands, char op){
    int value_right = *(int*)top(operands) % 256;
    pop(operands);
    int value_left = *(int*)top(operands) % 256;
    pop(operands);
    int result = 1;
    switch (op) {
        case '|':
            result = value_left | value_right;
            break;
        case '^':
            result = value_left ^ value_right;
            break;
        case '&':
            result = value_left & value_right;
            break;
    }
    if(push(operands, &result, sizeof(int)) != 0) return 1;
    return 0;
}

int output_result(Stack* operands, Stack* operators){
    if(operands->real_size == 1 && operators->real_size == 0){
        if(*(int*)top(operands) % 256 == 0)
            printf("False");
        else
            printf("True");
        return 0;
    }
    else{
        return 1;
    }
}