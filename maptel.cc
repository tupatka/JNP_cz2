#include <iostream>
#include "maptel.h"
#include <map>
#include <set>
#include <cstring>
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

void maptel_erase(unsigned long id, char const *tel_src) {
    book_t book = library[id];

    auto tel_it = book.find(tel_src);
    if (tel_it != book.end()) {
        book.erase(tel_it);
    }
}

void maptel_transform(unsigned long id, char const *tel_src, char *tel_dst, size_t len) {
    book_t book = library[id];

    auto tel_it = book.find(tel_src);
    if (tel_it == book.end()) { // Nie było zmiany numeru.
        // mam nadzieje ze da sie to zrobic ladniej
        tel_dst = (char*)malloc(strlen(tel_src + 1));
        strcpy(tel_dst, tel_src);
        return;
    }

    std::set<char*> set; // Do szukania powtórzeń numerów.
    auto tel_dst_it = tel_it;
    while (tel_it != book.end()) {

        // TO NIE DZIALA I NIE WIEM JAK ZMIENIC
        char* tel_curr = tel_it->first; // chce zdjac napis z seta, ale moze w sumie by wystarczyl tylko wskaznik a nie gleboka kopia
        if (set.find(tel_curr) != set.end()) { // Mamy cykl.
            tel_dst = (char*)malloc(strlen(tel_src + 1));
            strcpy(tel_dst, tel_src);
            return;
        }

        // TO TAK SAMO
        char* tel_next = tel_it->second;
        set.insert(tel_next);
        tel_dst_it = tel_it;
        tel_it = book.find(tel_next);
    }
}
