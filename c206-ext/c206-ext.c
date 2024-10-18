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

	while (temp != NULL) // Procházení seznamu front a hledání fronty odpovídající prioritě přijatého paketu
	{
		QosPacketListPtr currentList = (QosPacketListPtr)temp->data;
		if (packet->priority == currentList->priority) // Pokud nalezena fronta s odpovídající prioritou, ukončení hledání
		{
			qosList = currentList;
			break;
		}
		temp = temp->nextElement;
	}

	if (qosList == NULL) // Pokud fronta s danou prioritou neexistuje, je alokována a inicializována
	{
		qosList = (QosPacketListPtr)malloc(sizeof(QosPacketList));
		qosList->priority = packet->priority;
		qosList->list = (DLList *)malloc(sizeof(DLList));
		DLL_Init(qosList->list);					// Inicializace seznamu paketů pro danou prioritu
		DLL_InsertLast(packetLists, (long)qosList); // Vložení nové fronty do seznamu front
	}

	DLL_InsertLast(qosList->list, (long)packet); // Vložení nového paketu na konec fronty

	if (qosList->list->currentLength > MAX_PACKET_COUNT) // Pokud počet paketů ve frontě překročí MAX_PACKET_COUNT, odstraň přebytečné
	{
		DLL_First(qosList->list);							 // Nastavení ukazatele na první prvek seznamu
		for (int i = 0; i < (MAX_PACKET_COUNT / 2) + 1; i++) // Mazání každého druhého paketu, dokud se nezmenší počet paketů pod limit
		{
			DLL_DeleteAfter(qosList->list); // Mazání dalšího prvku
			DLL_Next(qosList->list);		// Posun na další prvek seznamu
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
	int sentCount = 0; // Počet odeslaných paketů

	while (sentCount < maxPacketCount) // Cyklus pokračuje, dokud nebyl odeslán maximální počet paketů
	{
		QosPacketListPtr highestPriorityList = NULL;
		DLLElementPtr temp = packetLists->firstElement;

		while (temp != NULL) // Procházení seznamu front a hledání fronty s nejvyšší prioritou obsahující pakety
		{
			QosPacketListPtr currentList = (QosPacketListPtr)temp->data;
			if (currentList->list->firstElement != NULL) // Pokud fronta obsahuje pakety a má vyšší prioritu než dosud nalezené, ulož ji
			{
				if (highestPriorityList == NULL || currentList->priority > highestPriorityList->priority)
				{
					highestPriorityList = currentList;
				}
			}
			temp = temp->nextElement;
		}

		if (highestPriorityList == NULL) // Pokud nejsou žádné fronty s pakety, ukonči cyklus
		{
			break;
		}

		PacketPtr packetToSend = (PacketPtr)highestPriorityList->list->firstElement->data; // Získání paketu s nejvyšší prioritou k odeslání
		DLL_InsertLast(outputPacketList, (long)packetToSend);							   // Vložení paketu do výstupního seznamu
		DLL_DeleteFirst(highestPriorityList->list);										   // Odstranění paketu z fronty

		sentCount++; // Zvýšení počtu odeslaných paketů
	}
}
