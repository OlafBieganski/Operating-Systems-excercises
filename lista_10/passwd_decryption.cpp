#include <iostream>
#include <openssl/evp.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <csignal>
#include <array>
#include <thread>
#include <mutex>

std::vector<std::string> dictionary, hash, emails;
std::vector<std::array<std::string, 3>> passwdPairs; // first is pswd second hash, third email
bool passwdFound = false; // flags  for threads
bool continueFlag = false;
std::array<std::thread, 6> threadArr;
std::mutex passwdFound_mtx, passwdPairs_mtx; 


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

// signal handling for SIGHUP
void signalHandler( int sigcode ) {
	std::cout << "Interrupt signal (" << sigcode << ") received.\n";
	// signal reaction code
	for(const auto &arr : passwdPairs)
		std::cout << "The deciphered password is: " << arr[0]
		 << ". Associated email: " << arr[2] << std::endl;  
}

enum class producerNR {zero, one, two};

void producer1word(const std::string &_crrhash, producerNR _producer){
	uint i = 0;
	char md5[33]; // 32 characters + null terminator
	while(continueFlag && !passwdFound){
		std::string currWord = dictionary[i];
		switch(_producer){
			case producerNR::one:
				currWord[0] = toupper(currWord[0]);
				break;
			case producerNR::two:
				for(auto &letter : currWord)
					letter = toupper(letter);
				break;
		}
		const char *word = currWord.c_str();
		bytes2md5(word, strlen(word), md5);
		if(md5 == _crrhash){
			// protect with mutex
			passwdFound_mtx.lock();
			passwdFound = true;
			passwdFound_mtx.unlock();
			std::array<std::string, 3> newPswd = {currWord, _crrhash, emails.back()};
			passwdPairs_mtx.lock();
			passwdPairs.push_back(newPswd);
			passwdPairs_mtx.unlock();
			return;
		}
		i++;
	}
}

void producer1(const std::string &_crrhash){

}

template<std::size_t SIZE>
void stopThreads(std::array<std::thread, SIZE> &thArr){
	// stop all threads
	continueFlag = false;
	// joining threads to terminate them
	for(int i = 0; i < SIZE; i++)
		if(threadArr[i].joinable())
			threadArr[i].join();
}

int main(int argc, char* argv[]){
	// enable signal handling
	signal(SIGHUP, signalHandler);

	std::string userInput;
	std::ifstream passwdFile;
	char *filename;

	// Check if dictionary file is given
    if (argc < 2)
    {
        std::cout << "Argument 1 not specified. Filename required!\n";
        exit(-1);
    }
	// read dictionary to memory
	std::string dictName = argv[1];
	std::ifstream dictFile(dictName, std::ios::out);
	if(!dictFile){
		std::cout << "Invalid dictionary file argument." << std::endl;
		exit(-1);
	}
	if(dictFile.is_open()){
		std::string line;
		while(dictFile){
			std::getline(dictFile, line);
			dictionary.push_back(line);
		}
		std::cout << "Dictionary " << dictName << " successfuly read to memory."
		 << std::endl;
	}

	while(true){
		// get whole line from input
		while(!std::getline(std::cin, userInput) || passwdFound);
		// stop if q is input
		if(userInput == "q"){
			// before exit stop all threads
			stopThreads(threadArr);
			break;
		}
		if(passwdFound){
			// stop all threads
			stopThreads(threadArr);
			// show found password
			std::cout << "The deciphered password is: " << passwdPairs.back()[0]
			 << ". Associated email: " << passwdPairs.back()[2] << std::endl;
			// erase password from the list 
			hash.pop_back();
			emails.pop_back();
		}
		if(!passwdFound) passwdFile.open(userInput, std::ios::out);
		if(!passwdFile){
			std::cout << "No file of such name in current directory."
			 << std::endl;
		}
		else if(!passwdFound){
			// stop threads execution
			stopThreads(threadArr);
			// read new hashed passwords to memory
			std::string lineHash, lineEmail, trash;
			if(passwdFile.is_open()) {
				while(passwdFile){
					passwdFile >> trash >> lineHash >> lineEmail;
					std::getline(passwdFile, trash);
      				hash.push_back(lineHash);
					emails.push_back(lineEmail);
				}
				std::cout << "Passwords and emails successfuly read to memory." << std::endl;
				passwdFile.close();
			}
			// start thread to find passwords
			threadArr[0] = std::thread(producer1word, hash.back(), producerNR::one);
		}
	}
    return 0;
}