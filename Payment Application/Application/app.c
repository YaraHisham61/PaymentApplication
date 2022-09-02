#include "app.h"
#include <stdio.h>
#include "../Card/card.h"
#include "../Terminal/terminal.h"
#include "../Server/server.h"

void appStart(void)
{
	uint8_t c = 'y',z;
	while (c == 'y')
	{
		ST_cardData_t card;
		ST_terminalData_t term;

		while (setMaxAmount(&term) == INVALID_MAX_AMOUNT)
		{
			printf("Invalid max amount, please enter a float number more than zero\n");

		}

		/////////////////////////Card/////////////////////////

		while (getCardHolderName(&card) == WRONG_NAME) //Checking name format
		{
			printf("Wrong name format,Try again\n");
		}
		while (getCardExpiryDate(&card) == WRONG_EXP_DATE) //Checking expiry date format
		{
			printf("Wrong date format,Try again\n");
		}
		while (getCardPAN(&card) == WRONG_PAN) //Checking PAN format
		{
			printf("Wrong PAN format,Try again\n");
		}

		/////////////////////////Terminal/////////////////////////

		while (getTransactionDate(&term) == WRONG_DATE) //Checking expiry date format
		{
			printf("Wrong date format,Try again\n");
		}
		if (isCardExpired(&card, &term) == EXPIRED_CARD)
		{
			printf("Expired card");
			return;
		}
		while (getTransactionAmount(&term) == INVALID_AMOUNT) //Checking expiry date format
		{
			printf("Invalid amount,Try again\n");
		}
		if (isBelowMaxAmount(&term) == EXCEED_MAX_AMOUNT)
		{
			printf("Amount exceeding limit");
			return;
		}

		/////////////////////////Server/////////////////////////

		ST_transaction transaction = { card,term,0,0 };
		transaction.transState = recieveTransactionData(&transaction);
		if (transaction.transState == FRAUD_CARD)
			printf("This account doesn't exist\n");
		else if (transaction.transState == DECLINED_INSUFFECIENT_FUND)
			printf("Insuffecient funds\n");
		else if (transaction.transState == DECLINED_STOLEN_CARD)
			printf("This account is blocked\n");
		else if (transaction.transState == APPROVED)
			printf("Approved transaction ... saving \n\n");
		printf("Do you want to search for a transaction? (y/n)");
		fseek(stdin, 0, SEEK_END); //Ignoring any past input
		z = getchar();
		if (z == 'y')
		{
			uint32_t x;
			ST_transaction transatemp;
			printf("Enter the transaction sequence number you want to seaarch for : ");
			fseek(stdin, 0, SEEK_END); //Ignoring any past input
			scanf_s("%d", &x);
			if (getTransaction(x, &transatemp) == TRANSACTION_NOT_FOUND)
				printf("Transaction not found \n");
			else
			{
				printf("Transaction found .. transaction details: \n");
				printf("Card holder name : %s\n", transatemp.cardHolderData.cardHolderName);
				printf("Primary account number : %s\n", transatemp.cardHolderData.primaryAccountNumber);
				printf("Card expiration date : %s\n", transatemp.cardHolderData.cardExpirationDate);
				printf("Max transaction amount : %f\n", transatemp.terminalData.maxTransAmount);
				printf("Transaction amount : %f\n", transatemp.terminalData.transAmount);
				printf("Transaction date : %s\n", transatemp.terminalData.transactionDate);
				printf("Transaction sequence number : %ld\n", transatemp.transactionSequenceNumber);
				printf("Transaction state is : ");
				switch (transaction.transState)
				{
				case FRAUD_CARD:
				{
					printf("DECLINED :: FRAUD CARD");
					break;
				}
				case DECLINED_INSUFFECIENT_FUND:
				{
					printf("DECLINED :: INSUFFECIENT FUND");
					break;
				}
				case DECLINED_STOLEN_CARD:
				{
					printf("DECLINED :: STOLEN_CARD");
					break;
				}
				case APPROVED:
				{
					printf("APPROVED");
					break;
				}
				default:break;
				}

			}
		}
		printf("Do you want to continue? (y/n)");
		fseek(stdin, 0, SEEK_END); //Ignoring any past input
		c = getchar();
	}
	return;
}
