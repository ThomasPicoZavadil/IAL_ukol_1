/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void)
{
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init(DLList *list) // Inicializace seznamu
{							// Nastavení pointerů na NULL a délky seznamu na 0
	list->firstElement = NULL;
	list->lastElement = NULL;
	list->activeElement = NULL;
	list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose(DLList *list)
{
	for (int i = 0; i < list->currentLength; i++) // Pro počet prvků v seznamu se první element
	{											  // uloží do dočasné proměnné, ukazatel na první
		DLLElementPtr delElement;				  // element se nastaví na další element a
		delElement = list->firstElement;		  // dočasná proměnná se uvolní
		list->firstElement = delElement->nextElement;
		free(delElement);
	}
	DLL_Init(list); // Seznam se následně uvede do původního stavu
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst(DLList *list, long data)
{
	DLLElementPtr newElemPtr = (DLLElementPtr)malloc(sizeof(*newElemPtr)); // Vytvoření nového prvku
	if (newElemPtr == NULL)
	{
		DLL_Error(); // Volání chybové funkce v případě nedostatku paměti
		return;
	}
	newElemPtr->data = data;					  // Do nového elementu se vloží data
	newElemPtr->nextElement = list->firstElement; // Jako ukazatel na další element se vloží ukazatel na první element
	newElemPtr->previousElement = NULL;			  // Ukazatel na minulý prvek se nastaví na NULL
	if (list->firstElement != NULL)
	{
		list->firstElement->previousElement = newElemPtr; // Pokud je v seznamu víc než jeden prvek,
	} // nastaví se ukazatel starého prvního elementu
	else // na nový element
	{
		list->lastElement = newElemPtr; // Pokud je v seznamu pouze jen nový element,
	} // nastaví se i jako ukazatel na poslední element
	list->firstElement = newElemPtr; // Nastavení nového elementu jako první element
	list->currentLength += 1;		 // Zvýšení délky seznamu o 1
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast(DLList *list, long data)
{
	DLLElementPtr newElemPtr = (DLLElementPtr)malloc(sizeof(*newElemPtr)); // Vytvoření nového prvku
	if (newElemPtr == NULL)
	{
		DLL_Error(); // Volání chybové funkce v případě nedostatku paměti
		return;
	}
	newElemPtr->data = data;						 // Vložení dat do nového prvku
	newElemPtr->nextElement = NULL;					 // Nastavení ukazatele na další element na NULL
	newElemPtr->previousElement = list->lastElement; // Nastavení ukazatele na předchozí element na původní poslední element
	if (list->firstElement != NULL)					 // Pokud je v seznamu víc než jeden element,
	{												 // do ukazatele na další prvek původního posledního elemntu
		list->lastElement->nextElement = newElemPtr; // se vloží nový element
	}
	else
	{
		list->firstElement = newElemPtr; // Pokud je v seznamu jen nový prvek, nastaví se i jako ukazatel na první element
	}
	list->lastElement = newElemPtr; // Nastavení nového prvku jako ukazatel na poslední element
	list->currentLength += 1;		// Zvýšení délky seznamu o 1
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First(DLList *list)
{
	list->activeElement = list->firstElement; // Do ukazatele na aktivní element se vloží ukazatel na první element
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last(DLList *list)
{
	list->activeElement = list->lastElement; // Do ukazatele na aktivní element se vloží ukazatel na poslední element
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst(DLList *list, long *dataPtr)
{
	if (list->firstElement == NULL)
	{
		DLL_Error(); // Volání chybové funkce pokud je seznam prázdný
		return;
	}
	else
	{
		*dataPtr = list->firstElement->data; // Vložení ukazatele na první element do dataPtr
	}
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast(DLList *list, long *dataPtr)
{
	if (list->firstElement == NULL)
	{
		DLL_Error(); // Volání chybové funkce pokud je seznam prázdný
		return;
	}
	else
	{
		*dataPtr = list->lastElement->data; // Vložení ukazatele na poslední element do dataPtr
	}
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst(DLList *list)
{
	if (list->firstElement != NULL)
	{
		DLLElementPtr delElement;		 // Vytvoření dočasného elementu
		delElement = list->firstElement; // Vložení nového elementu do dočasného
		if (list->firstElement == list->activeElement)
		{
			list->activeElement = NULL; // Pokud je první element aktivní, aktivita se ztrácí
		}
		if (list->firstElement == list->lastElement)
		{
			list->firstElement = NULL; // Pokud měl seznam jeden element, nastaví se ukazatele
			list->lastElement = NULL;  // na první a poslední element na NULL
		}
		else
		{
			list->firstElement = list->firstElement->nextElement; // Do ukazatele na nový první element se vloží ukazatel na další prvek starého prvního elementu
			list->firstElement->previousElement = NULL;			  // Ukazatel na předchozí element nového prvního elementu se nastaví na NULL
		}
		free(delElement);		  // Uvlonění dočasného elementu
		list->currentLength -= 1; // Snížení délky seznamu o 1
	}
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast(DLList *list)
{
	if (list->firstElement != NULL)
	{
		DLLElementPtr delElement;		// Vytvoření dočasného elementu
		delElement = list->lastElement; // Vložení nového elementu do dočasného
		if (list->lastElement == list->activeElement)
		{
			list->activeElement = NULL; // Pokud je poslední element aktivní, aktivita se ztrácí
		}
		if (list->firstElement == list->lastElement)
		{
			list->firstElement = NULL; // Pokud měl seznam jeden element, nastaví se ukazatele
			list->lastElement = NULL;  // na první a poslední element na NULL
		}
		else
		{
			list->lastElement = list->lastElement->previousElement; // Do ukazatele na nový poslední element se vloží ukazatel na předchozí element starého posledního elementu
			list->lastElement->nextElement = NULL;					// Ukazatel na další element nového posledního elementu se nastaví na NULL
		}
		free(delElement);		  // Uvlonění dočasného elementu
		list->currentLength -= 1; // Snížení délky seznamu o 1
	}
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter(DLList *list)
{
	if (list->activeElement != NULL)
	{
		if (list->activeElement->nextElement != NULL)
		{
			DLLElementPtr delElement;
			delElement = list->activeElement->nextElement;				// Do dočasného elementu se vloží následující element aktivního elementu
			list->activeElement->nextElement = delElement->nextElement; // Jako nový následující element aktivního elementu, se nastaví původní následující element aktivního elementu
			if (delElement == list->lastElement)
			{
				list->lastElement = list->activeElement; // Pokud je rušený prvek poslední v seznamu, bude ukazatel na aktivní element zároveň i ukazatel na poslední element
			}
			else
			{
				delElement->nextElement->previousElement = list->activeElement; // Nastavení ukazatele na předchozí element následujícího elementu za rušeným elementem na aktivní element
			}
			free(delElement);		  // Uvolnění dočasného elementu
			list->currentLength -= 1; // Snížení délky seznamu o 1
		}
	}
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore(DLList *list) // Analogicky k DeleteAfter
{
	if (list->activeElement != NULL)
	{
		if (list->activeElement->previousElement != NULL)
		{
			DLLElementPtr delElement;
			delElement = list->activeElement->previousElement;
			list->activeElement->previousElement = delElement->previousElement;
			if (delElement == list->firstElement)
			{
				list->firstElement = list->activeElement;
			}
			else
			{
				delElement->previousElement->nextElement = list->activeElement;
			}
			free(delElement);
			list->currentLength -= 1;
		}
	}
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter(DLList *list, long data)
{
	if (list->activeElement != NULL)
	{
		DLLElementPtr newElemPtr = (DLLElementPtr)malloc(sizeof(*newElemPtr));
		if (newElemPtr == NULL)
		{
			DLL_Error();
			return;
		}
		newElemPtr->data = data;
		newElemPtr->nextElement = list->activeElement->nextElement; // Nastavení ukazatele na další prvek nového prvku na následující element aktivního prvku
		newElemPtr->previousElement = list->activeElement;			// Nastavení ukazatele na předchozí prvek nového elementu na aktivní prvek
		list->activeElement->nextElement = newElemPtr;				// Nastavení ukazatele na další prvek aktivného elemenetu na nový element
		if (list->activeElement == list->lastElement)
		{
			list->lastElement = newElemPtr; // Pokud je aktivní element poslední element v seznamu, nastaví se nový element jako poslední element
		}
		else
		{
			newElemPtr->nextElement->previousElement = newElemPtr; // Nastavení ukazatele následujícího elementu na předchozí element na nový element
		}
		list->currentLength += 1; // Zvýšení délky seznamu o 1
	}
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore(DLList *list, long data)
{
	if (list->activeElement != NULL)
	{
		DLLElementPtr newElemPtr = (DLLElementPtr)malloc(sizeof(*newElemPtr));
		if (newElemPtr == NULL)
		{
			DLL_Error();
			return;
		}
		newElemPtr->data = data;
		newElemPtr->nextElement = list->activeElement;						// Nastavení ukazatele na další element nového elementu na aktivní element
		newElemPtr->previousElement = list->activeElement->previousElement; // Nastavení ukazatele na předchozí element nového elementu na předchozí element aktivního elementu
		list->activeElement->previousElement = newElemPtr;					// Nastavení ukazatele na předchozí element aktivního elementu na nový element
		if (list->activeElement == list->firstElement)
		{
			list->firstElement = newElemPtr; // Pokud je aktivní element první v seznamu, nastaví se ukazatel na první element na nový element
		}
		else
		{
			newElemPtr->previousElement->nextElement = newElemPtr; // Ukazatel na následující element předchozího elementu se nastaví na nový element
		}
		list->currentLength += 1; // Zvýšení délky seznamu o 1
	}
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue(DLList *list, long *dataPtr)
{
	if (list->activeElement == NULL)
	{
		DLL_Error();
		return;
	}
	else
	{
		*dataPtr = list->activeElement->data; // Uložení ukazatele na data aktivního prvku do dataPtr
	}
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue(DLList *list, long data)
{
	if (list->activeElement != NULL)
	{
		list->activeElement->data = data; // Vložení dat do aktivního elementu
	}
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next(DLList *list)
{
	if (list->activeElement != NULL) // Kontrola zda je seznam aktivní
	{
		list->activeElement = list->activeElement->nextElement; // Nastavení dalšího elementu jako aktivní
	}
}

/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous(DLList *list)
{
	if (list->activeElement != NULL)
	{
		list->activeElement = list->activeElement->previousElement; // Nastavení předchozího elementu jako aktivní
	}
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive(DLList *list)
{
	return (list->activeElement != NULL) ? 1 : 0; // Pokud je seznam aktivní vrací 1, jinak 0
}

/* Konec c206.c */
