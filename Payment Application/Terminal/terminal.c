#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
    if (!termData) //Making sure the pointer is not NULL
        return WRONG_DATE;

    fseek(stdin, 0, SEEK_END); //Ignoring any past input
    uint8_t transDate[15];
    printf("Please enter the transaction date in the format (DD/MM/YYYY) : ");
    fgets(transDate, 15, stdin);
    if (strlen(transDate)-1 != 10 || transDate[2] != '/' || transDate[5] != '/') //Checking length&format
        return WRONG_DATE;
    uint16_t i;
    for (i = 0; i < 10; i++) //Checking there is no extra '/'
    {
        if (i == 2 || i == 5)
            continue;
        if (transDate[i] == '/')
            return WRONG_DATE;
    }
    uint8_t* temp = transDate;
    uint64_t d, m, y;
    d = strtol(transDate, &temp, 10);
    m = strtol(++temp, &temp, 10);
    y = strtol(++temp, &temp, 10);
    if (d <= 0 || m <= 0 || y <= 0 || d > 31 || m > 12) //Checking range of day,month and year
        return WRONG_DATE;
    if (m == 2) //handling of Feb. month
    {
        if ((transDate[8] == transDate[9] == '0')) //End of century years that ends with 00
            i=0;
        if ((i==0 && y % 400 == 0) ||(i !=0 && y % 4 == 0)) //Leap year
            if (d > 29)
                return WRONG_DATE;
        if (y % 4 != 0 && d > 28)
            return WRONG_DATE;
    }
    if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30) //Months with 30 days only
        return WRONG_DATE;
    strncpy_s(termData->transactionDate, 11, transDate,_TRUNCATE);
    return TERMINAL_OK;
}
EN_terminalError_t isCardExpired(ST_cardData_t *cardData, ST_terminalData_t *termData)
{
    if (!cardData || !termData)
        return EXPIRED_CARD;

    uint8_t* tempT= termData->transactionDate,*tempC= cardData->cardExpirationDate;
    uint64_t expMonth, transacMonth,expYear,transacYear;
    strtol(termData->transactionDate, &tempT, 10); //Ignoring days in transactionDate
    expMonth = strtol(cardData->cardExpirationDate, &tempC, 10);
    expYear= strtol(++tempC, &tempC, 10);
    transacMonth = strtol(++tempT, &tempT, 10);
    transacYear = strtol(++tempT, &tempT, 10);
    expYear += 2000; //Bec expiry date format is mm/yy
    if (transacYear > expYear)
        return EXPIRED_CARD;
    if (transacYear == expYear && transacMonth >= expMonth)
        return EXPIRED_CARD;
    return TERMINAL_OK;
}

EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{
    if (!cardData)
        return INVALID_CARD;
    uint8_t* pan = cardData->primaryAccountNumber;
    uint16_t sum = 0;
    uint16_t digit = 0;
    for (size_t i = 0; i < strlen(pan)-2; i++) 
    {
        digit = (pan[i] - '0');
        if (i % 2 == 0)
            digit *= 2;
        if (digit > 9)
            digit -= 9;
        sum += digit;
    }
    if (10 - (sum % 10) == pan[strlen(pan)-1]) //Validation of luhn number
        return TERMINAL_OK;
    else
        return INVALID_CARD;
}

EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
    if (!termData) //Making sure the pointer is not NULL
        return INVALID_AMOUNT;

    fseek(stdin, 0, SEEK_END); //Ignoring any past input
    printf("Please enter the transaction amount : ");
    scanf_s("%f", &termData->transAmount);
    if (termData->transAmount <= 0)
        return INVALID_AMOUNT;
    return TERMINAL_OK;
}

EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
    if (!termData || termData->transAmount > termData->maxTransAmount)
        return EXCEED_MAX_AMOUNT;
    return TERMINAL_OK;
}

EN_terminalError_t setMaxAmount(ST_terminalData_t* termData)
{
    if (!termData)
        return INVALID_MAX_AMOUNT;
    fseek(stdin, 0, SEEK_END); //Ignoring any past input
    printf("Enter the max amount of transaction : ");
    scanf_s("%f", &termData->maxTransAmount);
    if (termData->maxTransAmount <= 0)
        return INVALID_MAX_AMOUNT;
    return TERMINAL_OK;
}
