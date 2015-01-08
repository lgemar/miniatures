template<typename T>
void db(T s) {
	std::cout << s << std::endl;
}

template<typename T, typename U>
void db(T s1, U s2) {
	std::cout << s1 << s2 << std::endl;
}

template<typename T, typename U, typename V>
void db(T s1, U s2, V s3) {
	std::cout << s1 << s2 << s3 << std::endl;
}

template <typename S1> // debug_ green
void dbg(const S1 s1) {
	std::cout << "\x1b[32m" << s1 << "\x1b[0m" << std::endl;
}
