#include <iostream>
#include <openssl/evp.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <csignal>
#include <array>
#include <thread>
#include <mutex>

#define THREAD_NR 1

std::vector<std::string> dictionary, hash, emails;
std::vector<std::array<std::string, 3>> passwdPairs; // first is pswd second hash, third email
bool passwdFound = false; // flags  for threads
bool continueFlag = false;
bool isInput = false;
std::array<std::thread, THREAD_NR> threadArr;
std::array<bool, THREAD_NR> thread_state;
std::string userInput;
std::mutex passwdFound_mtx, passwdPairs_mtx, userInput_mtx; 

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
void signalHandler(int sigcode ) {
	std::cout << "Interrupt signal (" << sigcode << ") received.\n";
	// signal reaction code
	for(const auto &arr : passwdPairs)
		std::cout << "The deciphered password is: " << arr[0]
		 << ". Associated email: " << arr[2] << std::endl;  
}

// perform mutex protected read of var
template<typename T>
const T mtx_protrd(std::mutex &mtx, const T &var){
	const T temp;
	mtx.lock();
	temp = var;
	mtx.unlock();
	return temp;
}

// perform mutex protecetd write to var
template<typename T>
void mtx_protwrt(std::mutex &mtx, T &var, const T &newVal){
	mtx.lock();
	var = newVal;
	mtx.unlock();
}

const std::string readUserInput(){
	userInput_mtx.lock();
	const std::string temp = userInput;
	userInput_mtx.unlock();
	return temp;
}

void getInput(){
	std::string tempInput;
	std::cout << "Provide file with passwords or \"q\" for exit. \n";
	while(readUserInput() != "q"){
		// get whole line from input
		std::getline(std::cin, tempInput);
		userInput_mtx.lock();
		userInput = tempInput;
		isInput = true;
		userInput_mtx.unlock();
	}
}

enum class producerNR {zero, one, two};

void producer1word(const std::string &_crrhash, producerNR _producer){
	uint i = 0;
	char md5[33]; // 32 characters + null terminator
	while(continueFlag && !passwdFound &&  i < dictionary.size()){
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
			std::array<std::string, 3> newPswd = {currWord, _crrhash, emails.back()};
			// protect with mutex
			passwdPairs_mtx.lock();
			passwdPairs.push_back(newPswd);
			passwdPairs_mtx.unlock();
			passwdFound_mtx.lock();
			passwdFound = true;
			passwdFound_mtx.unlock();
			return;
		}
		i++;
	}
	// setting state as finished
	for(auto &state : thread_state)
		if(state == false){
			state = true;
			break;
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
	// setting state as unfinisehd
	for(auto &state: thread_state)
		state = false;
	continueFlag = true;
}

bool threadNoResult(){
	for(auto &state: thread_state)
		if(!state) return false;
	// if previously no found, we have no result
	return !passwdFound;
}

void threadStartnewSearch(){
	if(!hash.empty()) hash.pop_back();
	if(!emails.empty()) emails.pop_back();
	// start new search
	passwdFound_mtx.lock();
	passwdFound = false;
	passwdFound_mtx.unlock();
	//setting state as unfinished
	for(auto &state : thread_state)
		state = false;
	// all threads' initialization
	threadArr[0] = std::thread(producer1word, hash.back(), producerNR::zero);
}

int main(int argc, char* argv[]){
	// enable signal handling
	signal(SIGHUP, signalHandler);

	std::ifstream passwdFile;
	char *filename;

	// Check if dictionary file is given
    if (argc < 2){
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
		// we assume each line is new word
		std::string line;
		while(dictFile){
			std::getline(dictFile, line);
			dictionary.push_back(line);
		}
		std::cout << "Dictionary " << dictName << " successfuly read to memory."
		 << std::endl;
	}
	
	// start user input handling thread
	std::thread inputThread(getInput);

	while(true){
		// input is handled in separate thread
		while(!(isInput || passwdFound)){
			if(threadNoResult() && !hash.empty()){
				if(!emails.empty()){
					std::cout << "No password was found for: "
				 	 << emails.back() << std::endl;
				}
				stopThreads(threadArr);
				threadStartnewSearch();
			}
			if(hash.empty() && threadNoResult()){
				std::cout << "Passwords list's end reached."
				 << " Enter new file name or \"q\" for exit." << std::endl;
				while(!isInput);
			}
		}
		// stop if q is input
		if(readUserInput() == "q"){
			// before exit stop all threads
			inputThread.join();
			stopThreads(threadArr);
			break;
		}
		if(passwdFound){
			// stop all threads
			stopThreads(threadArr);
			// show found password
			std::cout << "The deciphered password is: " << passwdPairs.back()[0]
			 << ". Associated email: " << passwdPairs.back()[2] << std::endl;
			// start new search
			threadStartnewSearch();
		}
		if(isInput) passwdFile.open(readUserInput(), std::ios::out);
		if(!passwdFile && isInput){
			std::cout << "No file of such name in current directory."
			 << std::endl;
			userInput_mtx.lock();
			isInput = false;
			userInput_mtx.unlock();
		}
		if(isInput){
			userInput_mtx.lock();
			isInput = false;
			userInput_mtx.unlock();
			// stop threads' execution
			stopThreads(threadArr);
			// clear data structures
			hash.clear();
			emails.clear();
			passwdPairs.clear(); 
			// read new hashed passwords to memory
			std::string lineHash, lineEmail, trash;
			if(passwdFile.is_open()) {
 				do{
					passwdFile >> trash >> lineHash >> lineEmail;
					std::getline(passwdFile, trash);
      				hash.push_back(lineHash);
					emails.push_back(lineEmail);
				}while(passwdFile);
				std::cout << "Passwords and emails successfuly read to memory." << std::endl;
				passwdFile.close();
				//
			}
			// start thread to find passwords
			threadStartnewSearch();
		}
	}
    return 0;
}