#include <iostream>
#include "maptel.h"
#include <map>
#include <string>
#include <cassert>

using book_t = std::map<std::string, std::string>;//<tel_src, tel_dst>
using library_t = std::map<long, book_t>;//<id słownika, słownik>
//using id_t = unsigned long //!?

static library_t library;

/*
pytania
	-czy usunięcie nieistniejącego słownika to błąd?
	-co ze stwarzaniem istniejącego?

	-czy wstawienie (maptel_insert) do nie istniejącego słownika to błąd? jeśli tak,
	to set kluczy (id) lub przeglądanie kluczy (wsm to drugie, find() na pewno idzie binsearchem, podobno count() też)
	//w unordered map można w czasie stały???

	library[id][tel_src] = tel_dst; <-- działa nawet jeśli słownik library[id] nie został stworzony wcześniej
	-zatem może set istniejących id, np. jeśli insert(nie_istniejący, tel_src, tel_dst) jest błędem, trzeba sprawdzać czy istnieje
	-w maptel_create() pewnie i tak najlepiej tworzyć słowniki o kolejnych id (tj. 0, 1, 2,...) 
*/

unsigned long maptel_create(void) {
    static unsigned long id = 0;
    assert(id != -1);
    library[id];//teraz para (id, pusty_slownik) jest w mapie (library)
    ++id;
    return id - 1;
}

void maptel_delete(unsigned long id) {
	if (library.find(id) == library.end()) {//!Err
		std::cerr << "deleting non existing book no " << id << std::endl;
	}
	library.erase(id);
}

/**
 * Wypisuje zawartość wszystkich słowników
 */
void diag() {
	std::cerr << "diagnostyka" << std::endl;
	for (auto book : library) {
		std::cerr << "book no " << book.first << std::endl;
		for (auto tel : book.second) {//tel można nazwać change
			std::cerr << tel.first << " -> " << tel.second << std::endl;
		}
	}
	std::cerr << std::endl;
}

/**
 * Użyta z argumentem id równym -1 wywołuje diag() i nie insertuje,
 * a argumenty tel_sth nie mają znaczenia
 */
void maptel_insert(unsigned long id, char const *tel_src, char const *tel_dst) {
	//! //Diagnostyczne
	if(id == -1) {
		diag();
		return;
	}

	//!Uwaga: dodać sprawdzenie danych
	
	std::cerr << "inserting into " << id << ": " <<tel_src << " -> " << tel_dst << std::endl;
	if (library.find(id) == library.end()) {//!Err
		std::cerr << "inserting into non existing book " << id << std::endl;
	}
	library[id][tel_src] = tel_dst;	//działa nawet jeśli słownik library[id] nie istnieje
									//po prostu go tworzy i umieszcza tam tel_src jako klucz

}