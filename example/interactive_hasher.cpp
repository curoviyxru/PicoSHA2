#include <iostream>
#include "../picosha2.h"

int main(int argc, char* argv[])
{
	picosha2::hash256_one_by_one hasher;
	while(true){
		hasher.init();
		while(true){
			std::string line;
			std::getline(std::cin, line);
			std::cout << line.length() << std::endl;
			if(line == "hash!"){
				break;
			}
			hasher.process(line.begin(), line.end());
		}
		hasher.finish();
		std::string hex_str;
		picosha2::get_hash_hex_string(hasher, hex_str);
		std::cout << hex_str << std::endl;
	}

    return 0;
}
