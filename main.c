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

LongNumber* initLongNumber(size_t size){
    LongNumber* LN = (LongNumber*)malloc(sizeof(LongNumber));
    if(LN == NULL) {
        printf("[error]\n");
        return NULL;
    }

    LN->size = size;
    LN->sign = 1;
    LN->arrayOfNumber = NULL;

    if (size != 0) {
        LN->arrayOfNumber = (int *) malloc(LN->size * sizeof(int));
        if (LN->arrayOfNumber == NULL) {
            printf("[error]\n");
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
LongNumber* shift(LongNumber* value, int difference);
void inputZero(LongNumber* value);
LongNumber* calcDivisionWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue);
LongNumber* calcDivisionWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize);
//----------------------------------------------------------------------------------------------------------------------
char* input_row();

int main(int argc, char const *argv[]) {

    char* leftValue = input_row();
    char* rightValue = input_row();

    LongNumber* result = calcDivisionWIthString(leftValue, strlen(leftValue), rightValue, strlen(rightValue));
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

    LongNumber* resultLN = initLongNumber(resultSize);
    if(resultLN == NULL) {
        return NULL;
    }
    resultLN->sign = leftValue->sign;

    int leftBuffer = 0;
    int rightBuffer = 0;

    for (size_t i = 0; i < resultLN->size - 1; i++) {
        leftBuffer = leftValue->size > i ? leftValue->arrayOfNumber[i] : 0;
        rightBuffer = rightValue->size > i ? rightValue->arrayOfNumber[i] : 0;

        resultLN->arrayOfNumber[i] += leftBuffer + rightBuffer;
        resultLN->arrayOfNumber[i + 1] += resultLN->arrayOfNumber[i] / 10;
        resultLN->arrayOfNumber[i] %= 10;
    }

    deleteZero(resultLN);

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

    if(resultLN == NULL){
        freeLongNumber(leftLN);
        freeLongNumber(rightLN);
        return NULL;
    }

    reversLongNumber(resultLN);
    freeLongNumber(leftLN);
    freeLongNumber(rightLN);
    return resultLN;
}

//---------------------------------------------------------------------------------------------------------------------

LongNumber* calcMultiplicationWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue) {
    size_t resultSize = rightValue->size + leftValue->size + 1;

    LongNumber* resultLN = initLongNumber(resultSize);
    if(resultLN == NULL) {
        return NULL;
    }
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

    deleteZero(resultLN);
    reversLongNumber(resultLN);

    return resultLN;
}

LongNumber* calcMultiplicationWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize) {
    LongNumber* leftLN = createLongNumberFromString(leftValue, leftSize, -1);
    if(leftLN == NULL) {
        return NULL;
    }
    LongNumber* rightLN = createLongNumberFromString(rightValue, rightSize, -1);
    if(rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }
    LongNumber* resultLN = calcMultiplicationWIthLongNumber(leftLN, rightLN);
    if(resultLN == NULL){
        freeLongNumber(leftLN);
        freeLongNumber(rightLN);
        return NULL;
    }

    freeLongNumber(leftLN);
    freeLongNumber(rightLN);
    return resultLN;
}

//----------------------------------------------------------------------------------------------------------------------

LongNumber* calcSubtractionWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue) {
    if (calcCompareWIthLongNumber(leftValue, rightValue) == -1) {
        LongNumber* tempLN = leftValue;
        leftValue = rightValue;
        rightValue = tempLN;
        leftValue->sign *= -1;
    }

    LongNumber* resultLN = initLongNumber(leftValue->size);
    if(resultLN == NULL) {
        return NULL;
    }
    resultLN->sign = leftValue->sign;

    int buffer = 0;
    int rightBuffer = 0;

    for (size_t i = 0; i < resultLN->size; i++) {
        rightBuffer = rightValue->size > i ? rightValue->arrayOfNumber[i] : 0;
        buffer = leftValue->arrayOfNumber[i] - rightBuffer - buffer;
        resultLN->arrayOfNumber[i] = (buffer + 10) % 10;
        buffer = buffer < 0 ? 1 : 0;
    }

    deleteZero(resultLN);

    return resultLN;
}

LongNumber* calcSubtractionWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize) {
    LongNumber *leftLN = createLongNumberFromString(leftValue, leftSize, -1);
    if (leftLN == NULL) {
        return NULL;
    }
    LongNumber *rightLN = createLongNumberFromString(rightValue, rightSize, -1);
    if (rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }

    LongNumber *resultLN = NULL;

    if (leftLN->sign == rightLN->sign) {
        if (leftLN->sign == 1) {
            resultLN = calcSubtractionWIthLongNumber(leftLN, rightLN);
        } else {
            rightLN->sign *= -1;
            resultLN = calcSubtractionWIthLongNumber(rightLN, leftLN);
        }
    } else {
        resultLN = calcAdditionWIthLongNumber(leftLN, rightLN);

    }

    if(resultLN == NULL){
        freeLongNumber(leftLN);
        freeLongNumber(rightLN);
        return NULL;
    }

    reversLongNumber(resultLN);
    freeLongNumber(rightLN);
    freeLongNumber(leftLN);
    return resultLN;
}

//----------------------------------------------------------------------------------------------------------------------

LongNumber* shift(LongNumber* value, int difference) {
    LongNumber* resultLN = initLongNumber(0);
    if (resultLN == NULL) {
        return NULL;
    }
    resultLN->size = value->size + difference;
    resultLN->arrayOfNumber = (int*)calloc(resultLN->size, sizeof(int));
    if(resultLN->arrayOfNumber == NULL) {
        printf("[error]\n");
        free(resultLN);
        return NULL;
    }
//    memcpy(resultLN->arrayOfNumber, value->arrayOfNumber, value->size);

    for (size_t i = 0; i < value->size; i++) {
        resultLN->arrayOfNumber[i] = value->arrayOfNumber[i];
    }
    freeLongNumber(value);
    return resultLN;
}

void inputZero(LongNumber* value) {
    for (size_t i = 0; i < value->size; i++) {
        value->arrayOfNumber[i] = 0;
    }
    deleteZero(value);
}

void output(LongNumber* value) {
    for (size_t i = 0; i < value->size; i++) {
        printf("%d ", value->arrayOfNumber[i]);
    }
    printf("\n");
}

LongNumber* calcDivisionWIthLongNumber(LongNumber* leftValue, LongNumber* rightValue) {
    LongNumber* resultLN = initLongNumber(leftValue->size);
    if(resultLN == NULL) {
        return NULL;
    }

    if (calcCompareWIthLongNumber(leftValue, rightValue) == -1) {
        inputZero(resultLN);
        return resultLN;
    }

    resultLN->sign = leftValue->sign * rightValue->sign;

    leftValue->sign = rightValue->sign = 1;
    reversLongNumber(rightValue);

    int difference = (int)leftValue->size - (int)rightValue->size;
    rightValue = shift(rightValue, difference);

    LongNumber* tempLN = initLongNumber(leftValue->size + 1);
    if(tempLN == NULL) {
        freeLongNumber(resultLN);
        return NULL;
    }

    reversLongNumber(rightValue);
    int digit = 0;

    for (; difference >= 0; difference--) {
        inputZero(tempLN);
        digit = 0;

        do {
            tempLN = calcAdditionWIthLongNumber(tempLN, rightValue);
            digit++;
        } while(calcCompareWIthLongNumber(tempLN, leftValue) <= 0);

        tempLN = calcSubtractionWIthLongNumber(tempLN, rightValue);
        digit--;

        leftValue = calcSubtractionWIthLongNumber(leftValue, tempLN);
        resultLN->arrayOfNumber[difference] = digit;

        reversLongNumber(rightValue);
        rightValue->size--;
        reversLongNumber(rightValue);
    }

    deleteZero(resultLN);
    reversLongNumber(resultLN);
    freeLongNumber(tempLN);

    return resultLN;
}

LongNumber* calcDivisionWIthString(char* leftValue, size_t leftSize, char* rightValue, size_t rightSize) {
    LongNumber *leftLN = createLongNumberFromString(leftValue, leftSize, 1);
    if (leftLN == NULL) {
        return NULL;
    }
    LongNumber *rightLN = createLongNumberFromString(rightValue, rightSize, 1);
    if (rightLN == NULL) {
        freeLongNumber(leftLN);
        return NULL;
    }

    LongNumber* resultLN = calcDivisionWIthLongNumber(leftLN, rightLN);
    if(resultLN == NULL){
        freeLongNumber(leftLN);
        freeLongNumber(rightLN);
        return NULL;
    }

    freeLongNumber(leftLN);
    freeLongNumber(rightLN);
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
    if(LN == NULL) {
        return NULL;
    }
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
