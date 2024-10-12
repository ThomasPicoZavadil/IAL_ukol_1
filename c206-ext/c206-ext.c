/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 *
 * Parametr `packetLists` obsahuje jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Pokud fronta s odpovídající prioritou neexistuje, tato metoda ji alokuje
 * a inicializuje. Za jejich korektní uvolnení odpovídá volající.
 *
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek seznamu.
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 *
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet(DLList *packetLists, PacketPtr packet)
{
	QosPacketListPtr qosList = NULL;
	DLLElementPtr temp = packetLists->firstElement;

	// Hledání fronty odpovídající priority
	while (temp != NULL)
	{
		QosPacketListPtr currentList = (QosPacketListPtr)temp->data;
		if (packet->priority == currentList->priority) // Correct comparison
		{
			qosList = currentList;
			break;
		}
		temp = temp->nextElement; // Traverse correctly to the next element
	}

	if (qosList == NULL)
	{
		qosList = (QosPacketListPtr)malloc(sizeof(QosPacketList));
		qosList->priority = packet->priority;
		qosList->list = (DLList *)malloc(sizeof(DLList));
		DLL_Init(qosList->list);
		DLL_InsertLast(packetLists, (long)qosList);
	}

	DLL_InsertLast(qosList->list, (long)packet);

	if (qosList->list->currentLength > MAX_PACKET_COUNT)
	{
		DLL_First(qosList->list);
		for (int i = 0; i < (MAX_PACKET_COUNT / 2) + 1; i++)
		{
			DLL_DeleteAfter(qosList->list);
			DLL_Next(qosList->list);
		}
	}
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 *
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 *
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Parametr `outputPacketList` obsahuje ukazatele na odeslané pakety (`PacketPtr`).
 *
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets(DLList *packetLists, DLList *outputPacketList, int maxPacketCount)
{
	int sentCount = 0;

	// Cyklus přes všechny priority (prioritní fronty jsou uložené v packetLists)
	while (sentCount < maxPacketCount)
	{
		QosPacketListPtr highestPriorityList = NULL;
		DLLElementPtr temp = packetLists->firstElement;

		// Hledání fronty s nejvyšší prioritou, která není prázdná
		while (temp != NULL)
		{
			QosPacketListPtr currentList = (QosPacketListPtr)temp->data;
			if (currentList->list->firstElement != NULL)
			{
				if (highestPriorityList == NULL || currentList->priority > highestPriorityList->priority)
				{
					highestPriorityList = currentList;
				}
			}
			temp = temp->nextElement;
		}

		// Pokud žádná fronta není k dispozici, ukončujeme
		if (highestPriorityList == NULL)
		{
			break;
		}

		// Odeslání paketu z fronty s nejvyšší prioritou
		PacketPtr packetToSend = (PacketPtr)highestPriorityList->list->firstElement->data;
		DLL_InsertLast(outputPacketList, (long)packetToSend);
		DLL_DeleteFirst(highestPriorityList->list);

		sentCount++;
	}
}
