#include <iostream>
#include "maptel.h"
#include <unordered_map>
#include <string>
#include <cassert>
#include <set>
#include <cstring>
//#include <cstdlib>
constexpr bool debug = true;

namespace jnp1 {
	using book_t = std::unordered_map<std::string, std::string>;//<tel_src, tel_dst>
    using library_t = std::unordered_map<long, book_t>;//<id słownika, słownik>
	//using id_t = unsigned long //!?

    library_t& get_library() {
    	static library_t library;
    	return library;
    }
    

/*
pytania
	-czy usunięcie nieistniejącego słownika to błąd?taktaktak
	-co ze stwarzaniem istniejącego? ????

    -przy debug==true komunikaty wszelkie

	library[id][tel_src] = tel_dst; <-- działa nawet jeśli słownik library[id] nie został stworzony wcześniej
*/

    unsigned long maptel_create(void) {
        static unsigned long id = 0;
        static library_t& library = get_library();
        assert(id != (unsigned long)-1);
        //!spr czy książka już nie istniała
        if (debug) {
        	std::cerr << "creating book no " << id << std::endl;
        }
        library[id];//teraz para (id, pusty_slownik) jest w mapie (library)
        ++id;
        return id - 1;
    }

    void maptel_delete(unsigned long id) {
    	static library_t& library = get_library();
        //!komunikat normalnie
        if (library.find(id) == library.end()) {//!Err
            std::cerr << "deleting non existing book no " << id << std::endl;
        }
        library.erase(id);
    }

/**
 * Wypisuje zawartość wszystkich słowników
 */
    void diag() {
    	static library_t& library = get_library();
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
        static library_t& library = get_library();
        //! //Diagnostyczne
        if (id == (unsigned long)-1) {
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
  	static library_t& library = get_library();
    std::cerr << "Erasing tel_src " << tel_src << " from book " << id << std::endl;
    library[id].erase(tel_src);//!spr.czy na pewno istnieje lib[id]
	}

	void maptel_transform(unsigned long id, char const *tel_src, char *tel_dst, size_t len) {
		static library_t& library = get_library();
		
		if (debug) {
			std::cerr << "Trying to transform " << tel_src << " using book " << id << std::endl;
		}

		if (!library.count(id)) {
			//nie ma takiego słownika
			if (debug) {
				std::cerr << "no such book found" << std::endl;
			}
			return;
		}
	    
	    book_t book = library[id];

	    auto slow_iter = book.find(tel_src);
	    if (slow_iter == book.end()) { // Nie było zmiany numeru.
	        strcpy(tel_dst, tel_src);//!być może tel_dst nie ma dosyć pamięci
		    if (debug) {
		    	std::cerr << "nie było zmiany numeru" << std::endl;
	        	std::cerr << "result: " << tel_dst << std::endl;
	        }
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
	    	if (debug) {//! bez klamer
	    		std::cerr << "no cycle found" << std::endl;
	    		std::cerr << "result: " << tel_dst << std::endl;
			}
	    }
	    else {
	    	assert(fast_iter == slow_iter);
	    	/* cykl! */
	    	strcpy(tel_dst, tel_src);//!być może tel_dst nie ma dosyć pamięci
	    	if (debug) {
	    		std::cerr << "cycle found" << std::endl;
	    		std::cerr << "result: " << tel_dst << std::endl;
	    	}
	    }
	}   
}