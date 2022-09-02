#include "card.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
    if(!cardData) //Making sure the pointer is not NULL
        return WRONG_NAME;

    fseek(stdin, 0, SEEK_END); //Ignoring any past input
    uint8_t temp[30];
    printf("Please enter cardholder's name : ");
    fgets(temp,30,stdin);
    if (strlen(temp)-1 <20 || strlen(temp)-1 > 24)  //Checking length
        return WRONG_NAME;
    for (size_t i = 0; i < strlen(temp)-1; i++)
    {
        if ((temp[i] < 'A' || temp[i] >'z') && temp[i] != ' ') //Checking there is only alphabetic characters
            return WRONG_NAME;
    }
    strncpy_s(cardData->cardHolderName,25,temp, _TRUNCATE);
    return CARD_OK;
}

EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
    if (!cardData) //Making sure the pointer is not NULL
        return WRONG_EXP_DATE;

    fseek(stdin, 0, SEEK_END); //Ignoring any past input
    uint8_t expDate[8]={0};
    uint8_t* temp;
    uint64_t month, year;
    printf("Please enter card's expiry date in in the format (MM/YY) : ");
    fgets(expDate, 8, stdin);
    if (strlen(expDate)-1 != 5 || expDate[2] != '/') //Checking length & format
        return WRONG_EXP_DATE;
    for (uint16_t i = 0; i < 5; i++) //Checking there is no extra '/'
    {
        if (i == 2)
            continue;
        if (expDate == '/')
            return WRONG_EXP_DATE;
    }
    month = strtol(expDate, &temp, 10);
    if(*temp != '/' || month>12 || month <1) //Checking the month range from 1-12
        return WRONG_EXP_DATE;
    year = strtol(++temp, &temp, 10);
    if (year == 0 && expDate[3] != 0 && expDate[4] != 0) //Checking if the function returned zero or not
        return WRONG_EXP_DATE;
    strncpy_s(cardData->cardExpirationDate,6, expDate, _TRUNCATE);
    return CARD_OK;
}

EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
    if (!cardData) //Making sure the pointer is not NULL
        return WRONG_PAN;

    fseek(stdin, 0, SEEK_END); //Ignoring any past input
    uint8_t pan[25] = { 0 };
    printf("Please enter card's Primary Account Number: ");
    fgets(pan,25,stdin);
    if (strlen(pan) - 1 < 16 || strlen(pan) - 1 > 19) //Checking length
        return WRONG_PAN;
    for (size_t i = 0; i < strlen(pan)-1; i++)
    {
        if (pan[i] < '0' || pan[i] >'9') //checking there is only numeric characters
            return WRONG_PAN;
    }
    strncpy_s(cardData->primaryAccountNumber, 20, pan, _TRUNCATE);
    return CARD_OK;
}