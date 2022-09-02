#include "server.h"
#include <stdio.h>
#include<string.h>

#define MAXDATA 255
#define accNumber 8

ST_accountsDB_t accountsDB[MAXDATA] = {
	{1300.000000,RUNNING,"3684908260086636140"},
	{2000.520000,RUNNING,"3475125161574720448"},
	{23000.520000,RUNNING,"3836731390623544140"},
	{90.000000,BLOCKED,"5807007076043875323"},
	{34567.900500,RUNNING,"2927426788796883161"},
	{23404.320000,RUNNING,"3967600852956214683"},
	{10000.000000,BLOCKED,"1234567890123456789"},
	{2.000000,RUNNING,"5975279185167329490"}

};
ST_transaction transactionsDB[MAXDATA] ={0};
static uint32_t transCounter=1;

EN_transStat_t recieveTransactionData(ST_transaction* transData)
{
	ST_accountsDB_t acc;

	if (isValidAccount(&transData->cardHolderData, &acc) == ACCOUNT_NOT_FOUND)
		return FRAUD_CARD;
	if (isBlockedAccount(&acc) == BLOCKED_ACCOUNT)
		return DECLINED_STOLEN_CARD;
	if (isAmountAvailable(&transData->terminalData, &acc) == LOW_BALANCE)
		return DECLINED_INSUFFECIENT_FUND;
	if (saveTransaction(transData) == SAVING_FAILED)
		return INTERNAL_SERVER_ERROR;

	return APPROVED;
}

EN_serverError_t isValidAccount(ST_cardData_t *cardData, ST_accountsDB_t *accountRefrence)
{
	if(!cardData || !accountRefrence ||!accountsDB)
		return ACCOUNT_NOT_FOUND;

	for (uint32_t i = 0; i < accNumber; i++)
	{
		if (strncmp(accountsDB[i].primaryAccountNumber, cardData->primaryAccountNumber,19) == 0)
		{
			accountRefrence->balance = accountsDB[i].balance;
			strncpy_s(accountRefrence->primaryAccountNumber, 20, accountsDB[i].primaryAccountNumber, _TRUNCATE);
			accountRefrence->state = accountsDB[i].state;
			return SERVER_OK;
		}
	}

	return ACCOUNT_NOT_FOUND;
}

EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
	return (accountRefrence->state == BLOCKED)? BLOCKED_ACCOUNT: SERVER_OK;
}

EN_serverError_t isAmountAvailable(ST_terminalData_t* termData,ST_accountsDB_t* accountRefrence)
{
	if (termData->transAmount > accountRefrence->balance)
		return LOW_BALANCE;
	return SERVER_OK;
}

EN_serverError_t saveTransaction(ST_transaction* transData)
{
	if (!transactionsDB ||!transData)
		return SAVING_FAILED;
	uint32_t i = transCounter - 1;

	/////////////////////////card data/////////////////////////

	strncpy_s(transactionsDB[i].cardHolderData.cardHolderName, 25, transData->cardHolderData.cardHolderName, _TRUNCATE);
	strncpy_s(transactionsDB[i].cardHolderData.primaryAccountNumber, 20, transData->cardHolderData.primaryAccountNumber, _TRUNCATE);
	strncpy_s(transactionsDB[i].cardHolderData.cardExpirationDate, 5, transData->cardHolderData.cardExpirationDate, _TRUNCATE);

	/////////////////////////terminal data/////////////////////////

	strncpy_s(transactionsDB[i].terminalData.transactionDate, 11, transData->terminalData.transactionDate, _TRUNCATE);
	transactionsDB[i].terminalData.transAmount = transData->terminalData.transAmount;
	transactionsDB[i].terminalData.maxTransAmount = transData->terminalData.maxTransAmount;

	/////////////////////////server data/////////////////////////

	transactionsDB[i].transactionSequenceNumber = transCounter++;
	transactionsDB[i].transState = transData->transState;

	/////////////////////////changing balance/////////////////////////
	if (updateBalance(transData) == SAVING_FAILED)
		return SAVING_FAILED;
	return SERVER_OK;
}

EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction* transData)
{
	if(!transData || transactionSequenceNumber>transCounter || !transactionsDB)
		return TRANSACTION_NOT_FOUND;

	for (uint32_t i = 0; i < transCounter-1; i++)
	{
		if (transactionsDB[i].transactionSequenceNumber == transactionSequenceNumber)
		{
			strncpy_s(transData->cardHolderData.cardHolderName, 25, transactionsDB[i].cardHolderData.cardHolderName, _TRUNCATE);
			strncpy_s(transData->cardHolderData.primaryAccountNumber, 20, transactionsDB[i].cardHolderData.primaryAccountNumber, _TRUNCATE);
			strncpy_s(transData->cardHolderData.cardExpirationDate, 5, transactionsDB[i].cardHolderData.cardExpirationDate, _TRUNCATE);
			strncpy_s(transData->terminalData.transactionDate,11, transactionsDB[i].terminalData.transactionDate, _TRUNCATE);
			transData->terminalData.maxTransAmount = transactionsDB[i].terminalData.maxTransAmount;
			transData->terminalData.transAmount = transactionsDB[i].terminalData.transAmount;
			transData->transactionSequenceNumber = transactionsDB[i].transactionSequenceNumber;
			transData->transState = transactionsDB[i].transState;
			return SERVER_OK;
		}
	}
	return TRANSACTION_NOT_FOUND;
}

EN_serverError_t updateBalance(ST_transaction* transData)
{
	if (!transData || !accountsDB)
		return SAVING_FAILED;

	for (size_t i = 0; i < accNumber; i++)
	{
		if (strcmp(accountsDB[i].primaryAccountNumber, transData->cardHolderData.primaryAccountNumber) == 0)
		{
			accountsDB[i].balance -= transData->terminalData.transAmount;
			printf("Balance after transaction : %f\n", accountsDB[i].balance);
			return SERVER_OK;
		}
	}
	return SAVING_FAILED;
}

