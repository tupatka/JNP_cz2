#include <iostream>
#include "maptel.h"
#include <map>
#include <string>

using book_t = std::map<std::string, std::string>;//<tel_src, tel_dst>
using library_t = std::map<long, book_t>;//<id słownika, słownik>
//id_t = unsigned long //!???

static library_t library;

unsigned long maptel_create(void) {
    book_t b;
    library[3] = b;
    return 3;
}

void maptel_delete(unsigned long id) {
	library.erase(id);
}

void maptel_insert(unsigned long id, char const *tel_src, char const *tel_dst) {
	if(id == -1) {
		std::cout << "diagnostyka" << std::endl;
		for(auto tel : library[3]) {
			std::cout << tel.first << " " << tel.second << std::endl;
		}
		return;
	}
	//!spr wartości
	if (!library.count(id)) {
		book_t b;
		library[id] = b;
	}
	std::cout << "inserting: " << tel_src << " " << tel_dst << std::endl;
	library[id][tel_src] = tel_dst;
}

