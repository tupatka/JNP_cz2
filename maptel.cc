#include "maptel.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cassert>
#include <set>
#include <cstring>
#include <sstream>

#ifdef NDEBUG
	const bool debug = false;
#else
	const bool debug = true;
#endif

namespace jnp1 {
	using book_t = std::unordered_map<std::string, std::string>; // <tel_src, tel_dst>
	using library_t = std::unordered_map<long, book_t>; // <id słownika, słownik>

	library_t& get_library() {
		static library_t library;
		return library;
	}

	namespace {
	//debug_msg_utils
		inline void debug_msg(std::string s) {
			if (debug) {
				std::cerr << s << std::endl;
			}
		}
		
		//Skróty zamieniające na std::string

		/** zamienia unsigned long (również size_t) na odpowiedni napis */
		inline std::string S(unsigned long id) {
			return std::to_string(id);
		}

		/** zamienia wskaźnik na string (jak %p, nie %s – np. "0x00ac1310") */
		inline std::string S(char const* ptr) {
			std::stringstream ss;
			ss << (void *)ptr;
			return ss.str();
		}
	}	/* namespace */
	
	unsigned long maptel_create(void) {
		static unsigned long id = -1;
		static library_t& library = get_library();
		
		debug_msg("maptel_create()");

		++id;

		//zakładamy, że do tych sytuacji nie dojdzie:
		assert(!library.count(id) && "maptel_create: new book should have a new id");
	   	assert(id != (unsigned int)-1 && "maptel_create: too many books in the library");

		library[id];	//para (id, pusty_slownik) jest teraz w mapie (library)

		debug_msg("maptel_create: created book with id = " + S(id));

		return id;
	}

	void maptel_delete(unsigned long id) {
		static library_t& library = get_library();
	  	
	  	debug_msg("maptel_delete(" + S(id) + ")");

		if (!library.count(id)) {
			debug_msg("maptel_delete: ERROR: there is no book with id = " + S(id));
			assert(library.count(id));
			return;
		}
		
		library.erase(id);

		debug_msg("maptel_delete: deleted");
	}

	namespace {
		/**
		 * Zwraca odpowiedź na pytanie:
		 * czy napis wskazywany przez tel jest poprawynm numerem telefonu?
		 * 
		 * Zakłada, że wskaźnik nie jest nullem
		 */
		bool correct_tel_num(char const* tel) {
			assert(tel != NULL);
			for (unsigned short i = 0; i < TEL_NUM_MAX_LEN; ++i) {
				if (tel[i] == '\0') {
					return i != 0;	//pusty napis nie jest poprawnym numerem telefonu
				}
				else if (!isdigit(tel[i])) {
					return false;
				}
			}
			/* tel[0]..tel[TEL_NUM_MAX_LEN - 1] to cyfry */
			if (tel[TEL_NUM_MAX_LEN] != '\0') {
				return false;
			}
			else {
				return true;
			}
		}
	}	/* namespace */

	void maptel_insert(unsigned long id, char const* tel_src, char const* tel_dst) {
		static library_t& library = get_library();

		if (tel_src == NULL || tel_dst == NULL) {
			debug_msg("maptel_insert(): ERROR: invalid argument: NULL");
			assert(tel_src != NULL && tel_dst != NULL);
			return;
		}

		if (!correct_tel_num(tel_src) || !correct_tel_num(tel_dst)){
			debug_msg("maptel_insert(): ERROR: invalid argument: not a valid tel number");
			assert(correct_tel_num(tel_src) && correct_tel_num(tel_dst));
			return;
		}

		/* argumenty poprawne, nie boimy się ich wypisywać */

		debug_msg("maptel_insert(" + S(id) + ", " + tel_src + ", " + tel_dst + ")");

		if (!library.count(id)) {
			debug_msg("maptel_insert: ERROR: no book with such id found");
			assert(library.count(id));
			return;
		}

		library[id][tel_src] = tel_dst;

		debug_msg("maptel_insert: inserted");
	}
  
  	void maptel_erase(unsigned long id, char const *tel_src) {
	  	static library_t& library = get_library();
		
	  	if (tel_src == NULL) {
	  		debug_msg("maptel_erase(): ERROR: invalid argument: NULL");
			assert(tel_src != NULL);
	  		return;
	  	}

	  	if (!correct_tel_num(tel_src)) {
	  		debug_msg("maptel_erase(): ERROR: invalid argument: not a valid tel number");
			assert(correct_tel_num(tel_src));
	  		return;
	  	}

	  	/* tel_src poprawny, nie boimy się wypisywać argumentów */

	  	debug_msg("maptel_erase(" + S(id) + ", " + tel_src + ")");

		if (!library.count(id)) {
			debug_msg("maptel_erase: ERROR: no book with such id found");
			assert(library.count(id));
			return;
		}

		library[id].erase(tel_src);

		debug_msg("maptel_erase: erased");	//"either erased or wasn't there at all"
	}


	namespace {
		//transform utils
		//patrz: maptel_transform();

		/**
		 * Zakłada, że tel_src, res - poprawne numery, dst_buf != NULL, buf_len > 0,
		 * res_len jest długością res (strlen)
		 */
		void transform_copy_util(char const *tel_src, char const *res, char *dst_buf,
														size_t res_len, size_t buf_len) {
			if (buf_len > res_len) {
				strcpy(dst_buf, res);
				debug_msg((std::string)"maptel_transform: " + tel_src + " -> " + res);
			}
			else {
				debug_msg((std::string)"maptel_transform: ERROR: "
					+ "result " + res + " won't fit in " + S(buf_len) + " bytes");
				assert(buf_len > res_len);
				
				assert(buf_len > 0);
				*dst_buf = '\0';
			}
		}

		
		/**
		 * Zakłada, że tel_src - poprawny numer, dst_buf != NULL, len > 0
		 */
		void transform_result_is_src(char const *tel_src, char *dst_buf, size_t len) {
			if (len > strlen(tel_src)) {
				memmove(dst_buf, tel_src, strlen(tel_src) + 1);
				assert(strcmp(dst_buf, tel_src) == 0);
				debug_msg((std::string)"maptel_transform: " + tel_src + " -> " + tel_src);
			}
			else {
				debug_msg((std::string)"maptel_transform: ERROR: "
					+ "result " + tel_src + " won't fit in " + S(len) + " bytes");
				assert(len > strlen(tel_src));

				assert(len > 0);
				*dst_buf = '\0';
			}
		}

		/**
		 * 	Zakłada, że tel_src - poprawny nr tel, tel_dst != NULL, len > 0
		 *	Transformuje numer na podstawie przekazanej książki.
		 * 	Wypisuje komunikaty zarówno o sukcesie, jak i o problemach
		 */
		void transform_helper(const book_t& book, char const *tel_src, char *tel_dst, size_t len) {
			assert(tel_src != NULL && tel_dst != NULL && len > 0);

			//poszukiwanie cyklu "żółwiem i zającem" - jeśli zając dogoni żółwia, to oznacza cykl
			
			auto slow_iter = book.find(tel_src);//żółw
			if (slow_iter == book.end()) { // Nie było zmiany numeru.
				transform_result_is_src(tel_src, tel_dst, len);
				return;
			}

			auto fast_iter = book.find(slow_iter->second);//zając
			auto behind_fast = slow_iter;//chodzi za zającem
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
				transform_copy_util(tel_src, behind_fast->second.c_str(), tel_dst,
												behind_fast->second.length(), len);
				return;
			}
			else {
				assert(fast_iter == slow_iter);
				/* cykl! */
				transform_result_is_src(tel_src, tel_dst, len);
				return;
			}
		}


		/**
		 * Sprawdza wszystko poza tym,
		 * co możemy sprawdzić dopiero po znalezieniu wyniku (tj. czy len bajtów wystarczy)
		 *
		 * a więc: czy wskaźniki nie są nullami, tel_src jest poprawnym numerem, len > 0
		 * (z góry wiemy, że potrzeba), wreszcie czy istnieje słownik o danym id
		 * 
		 * w debugu wypisuje komunikaty,
		 * m.in. w odpowiedniej sytuacji wypisuje "maptel_transform(argumenty)"
		 */
		bool transform_valid_args(const library_t& library, unsigned long id, char const *tel_src, char *tel_dst, size_t len) {
			if (tel_src == NULL || tel_dst == NULL) {
				//tel_dst == NULL oznacza, że wynik się nie zmieści
				debug_msg("maptel_transform(): ERROR: invalid argument: NULL");
				assert(tel_src != NULL && tel_dst != NULL);
				return false;
			}

			if (!correct_tel_num(tel_src)) {
				debug_msg("maptel_transform(): ERROR: invalid argument: not a valid tel number");
				assert(correct_tel_num(tel_src));
				return false;
			}

			/* parametry poprawne, można wypisywać (tel_dst jako wskaźnik, nie jako napis!) */

			debug_msg("maptel_transform(" + S(id) + ", " + tel_src + ", " + S(tel_dst) + ", " + S(len) + ")");

			if (!library.count(id)) {
				debug_msg("maptel_transform: ERROR: no book with such id found");
				assert(library.count(id));
				return false;
			}

			if (len == 0) {
				debug_msg("maptel_transform: ERROR: 0 bytes can't contain any possible result!");
				assert(len > 0);
				return false;
			}
			assert(len > 0);
			
			return true;
		}
	} /* namespace */

	void maptel_transform(unsigned long id, char const *tel_src, char *tel_dst, size_t len) {
		static library_t& library = get_library();

		bool valid_arguments = transform_valid_args(library, id, tel_src, tel_dst, len);
		if (valid_arguments) {
			transform_helper(library[id], tel_src, tel_dst, len);
		}
	}
}