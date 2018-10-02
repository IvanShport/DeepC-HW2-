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

//----------------------------------------------------------------------------------------------------------------------

char toChar(int value);
int toInt(char value);
void deleteZero(LongNumber* targetLN);
LongNumber* createLongNumberFromString(char* stringOfNumbers, size_t stringSize, int sign);
int reversLongNumber(LongNumber* LN);
//----------------------------------------------------------------------------------------------------------------------
int calcCompareWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue);
//----------------------------------------------------------------------------------------------------------------------
LongNumber* calcAdditionWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue);
LongNumber* calcAdditionWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize);
//----------------------------------------------------------------------------------------------------------------------
LongNumber* calcMultiplicationWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue);
LongNumber* calcMultiplicationWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize);
//----------------------------------------------------------------------------------------------------------------------
LongNumber* calcSubtractionWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize);
//----------------------------------------------------------------------------------------------------------------------
char* input_row();

int main(int argc, char const *argv[]) {

    char* leftValue = input_row();
    char* rightValue = input_row();

    LongNumber* result = calcMultiplicationWIthString(leftValue, strlen(leftValue), rightValue, strlen(rightValue));
    if(result == NULL) {
        free(leftValue);
        free(rightValue);
        return 0;
    }

    free(leftValue);
    free(rightValue);
    printf("%d\n", result->sign);
    for (size_t i = 0; i < result->size; i++) {
        printf("%d", result->arrayOfNumber[i]);
    }
    printf("\n");

    freeLongNumber(result);

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

//----------------------------------------------------------------------------------------------------------------------

LongNumber* calcAdditionWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue) {
    size_t resultSize = rightValue->size > leftValue->size ? rightValue->size + 1 : leftValue->size + 1;

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
    resultLN->sign = leftValue->sign;

    for (size_t i = 0; i < resultLN->size; i++) {
        resultLN->arrayOfNumber[i] += leftValue->arrayOfNumber[i] + rightValue->arrayOfNumber[i];
        resultLN->arrayOfNumber[i + 1] += resultLN->arrayOfNumber[i] / 10;
        resultLN->arrayOfNumber[i] %= 10;
    }

    freeLongNumber(leftValue);
    freeLongNumber(rightValue);

    return resultLN;
}

LongNumber* calcSubtractionWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue);

LongNumber* calcAdditionWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize) {
    LongNumber* leftLN = createLongNumberFromString(leftValue, leftSize, 1);
    if(leftLN == NULL) {
        return NULL;
    }
    LongNumber* rightLN = createLongNumberFromString(rightValue, rightSize, 1);
    if(rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }

    LongNumber* resultLN = NULL;

    if (leftLN->sign == rightLN->sign) {
        resultLN = calcAdditionWIthLongNumber(leftLN, rightLN);
    } else {
        if (leftLN->sign < 0) {
            resultLN = calcSubtractionWIthLongNumber(rightLN, leftLN);
        }
        if (rightLN->sign < 0) {
            resultLN = calcSubtractionWIthLongNumber(leftLN, rightLN);
        }

    }

    deleteZero(resultLN);
    reversLongNumber(resultLN);

    return resultLN;
}

//---------------------------------------------------------------------------------------------------------------------

LongNumber* calcMultiplicationWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue) {
    size_t resultSize = rightValue->size + leftValue->size + 1;

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
    resultLN->sign = leftValue->sign * rightValue->sign;

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

LongNumber* calcMultiplicationWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize) {
    LongNumber* leftLN = createLongNumberFromString(leftValue, leftSize, -1);
    if(leftLN == NULL) {
        return NULL;
    }
    LongNumber* rightLN = createLongNumberFromString(rightValue, rightSize, 1);
    if(rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }
    LongNumber* resultLN = calcMultiplicationWIthLongNumber(leftLN, rightLN);
    if(resultLN == NULL){
        return NULL;
    }

    deleteZero(resultLN);
    reversLongNumber(resultLN);

    return resultLN;
}

//----------------------------------------------------------------------------------------------------------------------

LongNumber* calcSubtractionWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue) {
    if (calcCompareWIthLongNumber(leftValue, rightValue) == -1) {
        LongNumber* tempLN = leftValue;
        leftValue = rightValue;
        rightValue = tempLN;
        leftValue->sign = -1;
    }

    int buffer = 0;
    int rightBuffer = 0;

    for (size_t i = 0; i < leftValue->size; i++) {
        rightBuffer = rightValue->size > i ? rightValue->arrayOfNumber[i] : 0;
        buffer = leftValue->arrayOfNumber[i] - rightBuffer - buffer;
        leftValue->arrayOfNumber[i] = (buffer + 10) % 10;
        buffer = buffer < 0 ? 1 : 0;
    }

    freeLongNumber(rightValue);

    return leftValue;
}

LongNumber* calcSubtractionWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize) {
    LongNumber *leftLN = createLongNumberFromString(leftValue, leftSize, -1);
    if (leftLN == NULL) {
        return NULL;
    }
    LongNumber *rightLN = createLongNumberFromString(rightValue, rightSize, 1);
    if (rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }

    LongNumber *resultLN = NULL;

    if (leftLN->sign == rightLN->sign) {
        if (leftLN->sign == 1) {
            resultLN = calcSubtractionWIthLongNumber(leftLN, rightLN);
        } else {
            resultLN = calcSubtractionWIthLongNumber(rightLN, leftLN);
        }
    } else {
        resultLN = calcAdditionWIthLongNumber(leftLN, rightLN);
    }

    deleteZero(resultLN);
    reversLongNumber(resultLN);

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

int reversLongNumber(LongNumber* LN) {
    int* arrayOfNumber = (int*)malloc(LN->size * sizeof(int));
    if(arrayOfNumber == NULL) {
        printf("[error]\n");
        return 1;
    }

    for (int i = (int)LN->size - 1; i >= 0; i--) {
        arrayOfNumber[LN->size - 1 - i] = LN->arrayOfNumber[i];
    }

    free(LN->arrayOfNumber);
    LN->arrayOfNumber = arrayOfNumber;

    return 0;
}

char toChar(int value) {
    return value + '0';
}

int toInt(char value) {
    return value - '0';
}

void deleteZero(LongNumber* targetLN) {
    while (targetLN->arrayOfNumber[targetLN->size - 1] == 0 && targetLN->size > 1) {
        targetLN->size--;
    }
}
