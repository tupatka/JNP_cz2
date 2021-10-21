#include <iostream>
#include "maptel.h"
#include <unordered_map>
#include <string>
#include <cassert>
#include <set>
#include <cstring>
//#include <cstdlib>
bool debug = true;
namespace jnp1 {
    using book_t = std::unordered_map<std::string, std::string>;//<tel_src, tel_dst>
    using library_t = std::unordered_map<long, book_t>;//<id słownika, słownik>
//using id_t = unsigned long //!?

    static library_t library;

/*
pytania
	-czy usunięcie nieistniejącego słownika to błąd?taktaktak
	-co ze stwarzaniem istniejącego? ????

    -przy debug==true komunikaty wszelkie

	library[id][tel_src] = tel_dst; <-- działa nawet jeśli słownik library[id] nie został stworzony wcześniej
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
    void maptel_insert(unsigned long id, char const* tel_src, char const* tel_dst) {
        //! //Diagnostyczne
        if (id == -1) {
            diag();
            return;
        }

        //!Uwaga: dodać sprawdzenie danych

        std::cerr << "inserting into " << id << ": " << tel_src << " -> " << tel_dst << std::endl;
        if (library.find(id) == library.end()) {//!Err
            std::cerr << "inserting into non existing book " << id << std::endl;
        }
        library[id][tel_src] = tel_dst;    //działa nawet jeśli słownik library[id] nie istnieje
        //po prostu go tworzy i umieszcza tam tel_src jako klucz
    }
  
  void maptel_erase(unsigned long id, char const *tel_src) {
    std::cerr << "Erasing tel_src " << tel_src << " from book " << id << std::endl;
    library[id].erase(tel_src);//!spr.czy na pewno istnieje lib[id]
	}

	void maptel_transform(unsigned long id, char const *tel_src, char *tel_dst, size_t len) {
		//! dodać sprawdzenie, czy istnieje słownik
	    
		std::cerr << "Trying to transform " << tel_src << " using book " << id << std::endl;

	    book_t book = library[id];//tworzy słownik o nummerze id, potrzebne sprawdzenie czy istnieje

	    auto slow_iter = book.find(tel_src);
	    if (slow_iter == book.end()) { // Nie było zmiany numeru.
	        strcpy(tel_dst, tel_src);//!być może tel_dst nie ma dosyć pamięci
		    std::cerr << "nie było zmiany numeru" << std::endl;
	        return;
	    }
	    auto fast_iter = book.find(slow_iter->second);
	    auto behind_fast = slow_iter;
	    bool slow_moves = false;

	    while (fast_iter != book.end() && fast_iter != slow_iter) {
	    	behind_fast = fast_iter;
	    	fast_iter = book.find(fast_iter->second);
	    	if (slow_moves) {
	    		slow_iter = book.find(slow_iter->second);
	    	}
	    	slow_moves = !slow_moves;
	    }

	    if (fast_iter == book.end()) {
	    	/* nie ma cyklu */
	    	strcpy(tel_dst, behind_fast->second.c_str());//!być może tel_dst nie ma dosyć pamięci
	    	//!return;
	    }
	    else {
	    	assert(fast_iter == slow_iter);
	    	/* cykl! */
	    	strcpy(tel_dst, tel_src);//!być może tel_dst nie ma dosyć pamięci
	    	//!return;
	    }

	    std::cerr << "Result: " << tel_dst << std::endl;
	}   
}

unsigned long maptel_create(void) {
    return jnp1::maptel_create();
}
void maptel_delete(unsigned long id) {
    jnp1::maptel_delete(id);
}
void maptel_insert(unsigned long id, char const* tel_src, char const* tel_dst) {
    jnp1::maptel_insert(id, tel_src, tel_dst);
}
void maptel_erase(unsigned long id, char const* tel_src) {
    jnp1::maptel_erase(id, tel_src);
}
void maptel_transform(unsigned long id, char const* tel_src, char* tel_dst, size_t len)  {
    jnp1::maptel_transform(id, tel_src, tel_dst, len);
}



