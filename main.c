#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define START_SIZE_FOR_STRING 32

typedef struct LongNumber {
    int* arrayOfNumber;
    size_t size;
    int sign;
}LongNumber;

LongNumber* initLongNumber(){
    LongNumber* LN = (LongNumber*)malloc(sizeof(LongNumber));
    if(LN == NULL) {
        printf("[error]\n");
        return NULL;
    }
    LN->arrayOfNumber = NULL;
    LN->size = 0;
    LN->sign = 1;

    return LN;
}

void freeLongNumber(LongNumber* LN) {
    free(LN->arrayOfNumber);
    free(LN);
}

char toChar(int value) {
    return value + '0';
}

int toInt(char value) {
    return value - '0';
}

void deleteZero(LongNumber* targetLN) {
    while (targetLN->arrayOfNumber[targetLN->size - 1] == 0) {
        targetLN->size--;
    }
}

LongNumber* createLongNumberFromString(char* stringOfNumbers, size_t stringSize, int sign) {
    LongNumber* LN = initLongNumber();
    if(LN == NULL) {
        return NULL;
    }
    LN->arrayOfNumber = (int*)malloc(stringSize * sizeof(int));
    if(LN->arrayOfNumber == NULL) {
        printf("[error]\n");
        free(LN);
        return NULL;
    }
    LN->size = stringSize;
    LN->sign = sign;

    for (int i = (int)LN->size - 1; i >= 0; i--) {
        LN->arrayOfNumber[LN->size - 1 - i] = toInt(stringOfNumbers[i]);
    }
    return LN;
}

char* createStringFromLongNumber(LongNumber* LN) {
    char* stringOfNumbers = (char*)malloc(LN->size * sizeof(char));
    if(stringOfNumbers == NULL) {
        printf("[error]\n");
        return NULL;
    }

    for (int i = (int)LN->size - 1; i >= 0; i--) {
        stringOfNumbers[LN->size - 1 - i] = toChar(LN->arrayOfNumber[i]);
    }
    return stringOfNumbers;
}

//----------------------------------------------------------------------------------------------------------------------

LongNumber* calcAdditionWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue, size_t resultSize) {
    LongNumber* resultLN = initLongNumber();
    if(resultLN == NULL) {
        return NULL;
    }
    resultLN->arrayOfNumber = (int*)malloc(resultSize * sizeof(int));
    if(resultLN->arrayOfNumber == NULL) {
        printf("[error]\n");
        free(resultLN);
        freeLongNumber(leftValue);
        freeLongNumber(rightValue);
        return NULL;
    }
    resultLN->size = resultSize;
    resultLN->sign = 1;

    for (size_t i = 0; i < resultLN->size; i++) {
        resultLN->arrayOfNumber[i] += leftValue->arrayOfNumber[i] + rightValue->arrayOfNumber[i];
        resultLN->arrayOfNumber[i + 1] += resultLN->arrayOfNumber[i] / 10;
        resultLN->arrayOfNumber[i] %= 10;
    }

    freeLongNumber(leftValue);
    freeLongNumber(rightValue);

    return resultLN;
}

char* calcAdditionWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize) {
    LongNumber* leftLN = createLongNumberFromString(leftValue, leftSize, 1);
    if(leftLN == NULL) {
        return NULL;
    }
    LongNumber* rightLN = createLongNumberFromString(rightValue, rightSize, 1);
    if(rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }

    size_t resultSize = 0;

    if(rightSize > leftSize) {
        resultSize = rightSize + 1;
    }
    else {
        resultSize = leftSize + 1;
    }

    LongNumber* resultLN = calcAdditionWIthLongNumber(leftLN, rightLN, resultSize);

    deleteZero(resultLN);
    char* resultString = createStringFromLongNumber(resultLN);
    freeLongNumber(resultLN);

    return resultString;
}

//---------------------------------------------------------------------------------------------------------------------

LongNumber* calcMultiplicationWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue, size_t resultSize) {
    LongNumber* resultLN = initLongNumber();
    if(resultLN == NULL) {
        return NULL;
    }
    resultLN->arrayOfNumber = (int*)malloc(resultSize * sizeof(int));
    if(resultLN->arrayOfNumber == NULL) {
        printf("[error]\n");
        free(resultLN);
        freeLongNumber(leftValue);
        freeLongNumber(rightValue);
        return NULL;
    }
    resultLN->size = resultSize;
    resultLN->sign = 1;

    for (size_t i = 0; i < leftValue->size; i++) {
        for (size_t j = 0; j < rightValue->size; j++) {
            resultLN->arrayOfNumber[i + j] += leftValue->arrayOfNumber[i] * rightValue->arrayOfNumber[j];
        }
    }

    for (size_t i = 0; i < resultLN->size; i++) {
        resultLN->arrayOfNumber[i + 1] += resultLN->arrayOfNumber[i] / 10;
        resultLN->arrayOfNumber[i] %= 10;
    }

    freeLongNumber(leftValue);
    freeLongNumber(rightValue);

    return resultLN;
}

char* calcMultiplicationWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize) {
    size_t resultSize = rightSize + leftSize + 1;

    LongNumber* leftLN = createLongNumberFromString(leftValue, leftSize, 1);
    if(leftLN == NULL) {
        return NULL;
    }
    LongNumber* rightLN = createLongNumberFromString(rightValue, rightSize, 1);
    if(rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }
    LongNumber* resultLN = calcMultiplicationWIthLongNumber(leftLN, rightLN, resultSize);
    if(resultLN == NULL){
        return NULL;
    }

    deleteZero(resultLN);
    char* resultString = createStringFromLongNumber(resultLN);
    free(resultLN);

    return resultString;
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

int main(int argc, char const *argv[]) {

    char* leftValue = input_row();
    char* rightValue = input_row();

    char* result = calcMultiplicationWIthString(leftValue, strlen(leftValue), rightValue, strlen(rightValue));
    if(result == NULL) {
        free(leftValue);
        free(rightValue);
        return 0;
    }

    // mult(leftValue, strlen(leftValue), rightValue, strlen(rightValue));

    free(leftValue);
    free(rightValue);
    printf("%s\n", result);
    free(result);

    return 0;
}
