#include <iostream>
#include <openssl/evp.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <csignal>

std::vector<std::string> dictionary;
std::vector<std::string> hash;

// function to peform conversion from char array to hash code array
void bytes2md5(const char *data, int len, char *md5buf) {
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	const EVP_MD *md = EVP_md5();
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len, i;
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, data, len);
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_free(mdctx);
	for (i = 0; i < md_len; i++) {
		snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
	}
}

int main(int argc, char* argv[]){
	std::string userInput;
	std::ifstream file;
	char *filename;

	while(true){
		std::getline(std::cin, userInput);
		if(userInput == "q") break;
		file.open(userInput, std::ios::out);
		if(!file){
			std::cout << "No file of such name in current directory."
			 << std::endl;
		}
		else{

		}

	}
	
	std::string line;
    if(file.is_open()) {
		while(file) {
			getline(file, line);
			dictionary.push_back(line);
		}
	}

    char md5[33]; // 32 characters + null terminator
    for(int i = 0; i < dictionary.size(); i++){
      const char *word = dictionary[i].c_str();
      bytes2md5(word, strlen(word), md5);
      hash.push_back(md5);
    }

    

    return 0;
}

/*
// Check if dictionary file is given
    if (argc < 2)
    {
        cout << "Argument 1 not specified. Filename required!\n";
        exit(-1);
    }

*/