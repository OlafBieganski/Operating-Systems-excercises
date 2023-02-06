#include <iostream>
#include <openssl/evp.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <csignal>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>

#define THREAD_NR 5 // number of searching threads

std::vector<std::string> dictionary, hash, emails;
std::vector<std::array<std::string, 3>> passwdPairs; // first is pswd second hash, third email
bool passwdFound = false; // flags  for threads
bool continueFlag = true;
bool isInput = false;
bool threadsFinished = false;
std::array<std::thread, THREAD_NR> threadArr;
std::array<bool, THREAD_NR> thread_state; // inform if threads finished with no result
std::string userInput;
std::mutex passwdFound_mtx, passwdPairs_mtx, userInput_mtx, dict_mtx, hashEmail_mtx, cont_mtx, threads_finished_mtx;
std::condition_variable newPasswd_CV, newInput_CV, threadsFinished_CV;

// sets thread's state as finished after work is done
// and notify suitable thread if all have finished
void threadSetState(){
	// set state as work finished
	for(auto &state : thread_state)
		if(state == false){
			state = true;
			break;
		}
	// if still false values do not notify
	for(auto &state : thread_state)
		if(state == false) return;
	// if all threads finished notify about it
	std::unique_lock<std::mutex> lock(threads_finished_mtx);
	threadsFinished = true;
	threadsFinished_CV.notify_one();
}

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
	if(passwdPairs.empty()){
		std::cout << "No password has been found so far.\n";
		return;
	}
	for(const auto &arr : passwdPairs)
		std::cout << "The deciphered password is: " << arr[0]
		 << ". Associated email: " << arr[2] << std::endl;  
}

// perform mutex protected read of var
template<typename T>
const T mtx_protrd(std::mutex &mtx, const T &var){
	T temp;
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
	while(userInput != "q"){
		// get whole line from input
		std::cout << "Provide file with passwords or \"q\" for exit. \n";
		std::getline(std::cin, tempInput);
		std::unique_lock<std::mutex> lock(userInput_mtx);
		userInput = tempInput;
		isInput = true;
		newInput_CV.notify_one();
		lock.unlock(); // may cause an error?
	}
}

void checkPswd(const std::string &currWord){
	if(hash.empty()) return;
	char md5[33]; // 32 characters + null terminator
	const char *word = currWord.c_str();
	bytes2md5(word, strlen(word), md5);
	for(auto it = hash.begin(); it != hash.end(); it++){ // nie bylo it++
		if(*it == md5){
			uint idx = std::distance(hash.begin(), it);
			std::array<std::string, 3> newPswd = {currWord, hash[idx], emails[idx]};
			//delete hash and email from list
			hashEmail_mtx.lock();
			it = hash.erase(it);
			emails.erase(emails.begin() + idx);
			hashEmail_mtx.unlock();
			// inform consumer that passwd found via condVar
			std::unique_lock<std::mutex> lock(passwdFound_mtx);
			passwdPairs.push_back(newPswd);
			passwdFound = true;
			newPasswd_CV.notify_one();
			if(hash.empty()){
				// stop threads
				mtx_protwrt(cont_mtx, continueFlag,false);
				// notify inform thread
				threadsFinished_CV.notify_one();
				std::cout << "End of the password list. Provide new file." << std::endl;
				return;
			}
			lock.unlock(); // may cause an error?
		}
	}
}

enum class producerNR {zero, one, two};
enum class digitPlace {noDigit, prefixDigit, postfixDigit, middleDigit, prePostDigit};
std::array<std::string,16> symbols = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
 "!", "@", "#", "$", "%", "&"};

//
void producer1word(digitPlace _digitPlace){
	for(const std::string &y : dictionary){
		// all possible letter size combinations for one word
		if(!mtx_protrd(cont_mtx, continueFlag)) return;
		std::string word = mtx_protrd(dict_mtx, y);
		auto wordFUP = word;
		wordFUP[0] = toupper(word[0]);
		auto wordAUP = word;
		for(auto &letter : wordAUP)
		letter = toupper(letter);
		// adding digits
		switch(_digitPlace){
			case digitPlace::noDigit: break;
			case digitPlace::prefixDigit:
				for(const auto &sym : symbols){
					auto prefix1 = sym;
					checkPswd(prefix1 + word);
					checkPswd(prefix1 + wordFUP);
					checkPswd(prefix1 + wordAUP);
					for(const auto &sym2 : symbols){
						if(!mtx_protrd(cont_mtx, continueFlag)) return;
						auto prefix2 = prefix1 + sym2;
						checkPswd(prefix2 + word);
						checkPswd(prefix2 + wordFUP);
						checkPswd(prefix2 + wordAUP);
					}
				}
			break;
			case digitPlace::postfixDigit:
				for(const auto &sym : symbols){
					auto postfix1 = sym;
					checkPswd(word + postfix1);
					checkPswd(wordFUP + postfix1);
					checkPswd(wordAUP + postfix1);
					for(const auto &sym2 : symbols){
						if(!mtx_protrd(cont_mtx, continueFlag)) return;
						auto postfix2 = postfix1 + sym2;
						checkPswd(word + postfix2);
						checkPswd(wordFUP + postfix2);
						checkPswd(wordAUP + postfix2);
					}
				}
			break;
			case digitPlace::prePostDigit:
				for(uint i = 0; i < 100; i++){
					auto prefix = std::to_string(i);
					for(uint i = 0; i < 100; i++){
						if(!mtx_protrd(cont_mtx, continueFlag)) return;
						auto postfix = std::to_string(i);
						checkPswd(prefix + word  + postfix);
						checkPswd(prefix + wordFUP + postfix);
						checkPswd(prefix + wordAUP + postfix);
					}
				}
			break;
		}
		// check all three options
		if(_digitPlace == digitPlace::noDigit){
			checkPswd(word);
			checkPswd(wordFUP);
			checkPswd(wordAUP);
		}
	}
	// setting state as finished with no result
	threadSetState();
}

// possible check for 2 word passwd with all letter size cominations
// and pre, post and middle additional numbers up to 2 symbols gigit and special signs combinations
void producer2word(producerNR _producer, digitPlace _digitPlace){
	for(const std::string &x : dictionary){

		std::string word1 = mtx_protrd(dict_mtx, x);
		// first word letter size based on thread argument
		switch(_producer){
			case producerNR::one:
				word1[0] = toupper(word1[0]);
				break;
			case producerNR::two:
				for(auto &letter : word1)
					letter = toupper(letter);
				break;
		}

		for(const std::string &y : dictionary){
			// all possible letter size combinations for second word
			if(!mtx_protrd(cont_mtx, continueFlag)) return;
			std::string word2 = mtx_protrd(dict_mtx, y);
			auto word2FUP = word2;
			word2FUP[0] = toupper(word2[0]);
			auto word2AUP = word2;
			for(char &letter : word2AUP)
				letter = toupper(letter);
			// adding digits
			switch(_digitPlace){
				case digitPlace::noDigit: break;
				case digitPlace::prefixDigit:
					for(const auto &sym : symbols){
						std::string prefix1 = sym;
						checkPswd(prefix1 + word1 + word2);
						checkPswd(prefix1 + word1 + word2FUP);
						checkPswd(prefix1 + word1 + word2AUP);
						for(const auto &sym2 : symbols){
							if(!mtx_protrd(cont_mtx, continueFlag)) return;
							std::string prefix2 = prefix1 + sym2;
							checkPswd(prefix2 + word1 + word2);
							checkPswd(prefix2 + word1 + word2FUP);
							checkPswd(prefix2 + word1 + word2AUP);
						}
					}
				break;
				case digitPlace::postfixDigit:
					for(const auto &sym : symbols){
						std::string postfix1 = sym;
						checkPswd(word1 + word2 + postfix1);
						checkPswd(word1 + word2FUP + postfix1);
						checkPswd(word1 + word2AUP + postfix1);
						for(const auto &sym2 : symbols){
							if(!mtx_protrd(cont_mtx, continueFlag)) return;
							std::string postfix2 = postfix1 + sym2;
							checkPswd(word1 + word2 + postfix2);
							checkPswd(word1 + word2FUP + postfix2);
							checkPswd(word1 + word2AUP + postfix2);
						}
					}
				break;
				case digitPlace::middleDigit:
					for(const auto &sym : symbols){
						std::string middle1 = sym;
						checkPswd(word1 + middle1 + word2);
						checkPswd(word1 + middle1 + word2FUP);
						checkPswd(word1 + middle1 + word2AUP);
						for(const auto &sym2 : symbols){
							if(!mtx_protrd(cont_mtx, continueFlag)) return;
							std::string middle2 = middle1 + sym2;
							checkPswd(word1 + middle2 + word2);
							checkPswd(word1 + middle2 + word2FUP);
							checkPswd(word1 + middle2 + word2AUP);
						}
					}
				break;
				case digitPlace::prePostDigit:
					for(uint i = 0; i < 100; i++){
						auto prefix = std::to_string(i);
						for(uint i = 0; i < 100; i++){
							if(!mtx_protrd(cont_mtx, continueFlag)) return;
							auto postfix = std::to_string(i);
							checkPswd(prefix + word1 + word2  + postfix);
							checkPswd(prefix + word1 + word2FUP + postfix);
							checkPswd(prefix + word1 + word2AUP + postfix);
						}
					}
				break;
			}
			// check all three options
			if(_digitPlace == digitPlace::noDigit){
				checkPswd(word1 + word2);
				checkPswd(word1 + word2FUP);
				checkPswd(word1 + word2AUP);
			}
		}
	}
	// setting state as finished with no result
	threadSetState();
}

template<std::size_t SIZE>
void stopThreads(std::array<std::thread, SIZE> &thArr){
	// stop all threads
	continueFlag = false;
	//wait a litlle bit
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// joining threads to terminate them
	for(int i = 0; i < SIZE; i++)
		if(threadArr[i].joinable())
			threadArr[i].join();
}

// before use of this function you have to stop all running threads
// and run utility threads
void threadStartnewSearch(){
	for(auto &state : thread_state) state = false;
	// remember to change THREAD_NR when adding new threads
	threadArr[0] = std::thread(producer1word,digitPlace::noDigit);
	threadArr[1] = std::thread(producer1word,digitPlace::prefixDigit);
	threadArr[2] = std::thread(producer1word,digitPlace::postfixDigit);
	threadArr[3] = std::thread(producer1word,digitPlace::prePostDigit);
	threadArr[4] = std::thread(producer2word, producerNR::zero, digitPlace::prefixDigit);
	//threadArr[5] = std::thread(producer2word, producerNR::two, digitPlace::postfixDigit);
	//threadArr[6] = std::thread(producer2word, producerNR::one, digitPlace::prePostDigit);
}

void consumer(){
	// setting initial state
	continueFlag = true;
	passwdFound = false;
	// while threads run check for new passwd
	while(continueFlag){
		std::unique_lock<std::mutex> lock(passwdFound_mtx);
		if(!passwdFound){
			newPasswd_CV.wait(lock);
		}
		if(!continueFlag) return;
		std::cout << "The deciphered password is: " << passwdPairs.back()[0]
			<< ". Associated email: " << passwdPairs.back()[2] << std::endl;
		passwdFound = false;
	}
}

// has to be called with working threads each time new passwds provided
void informThreadsFinish(){
	threadsFinished = false;
	std::unique_lock<std::mutex> lock(threads_finished_mtx);
	if(!threadsFinished){
		threadsFinished_CV.wait(lock);
	}
	if(continueFlag) return; // if threads finished cause of contFlag do not print
	std::cout << "All threads finished its work. Not all passwords have been found. Provide new file" << 
		" or \"q\" for exit." << std::endl;
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
	std::thread inputThread(getInput), consumerThread, informFinish;

	while(true){
		std::unique_lock<std::mutex> lock(userInput_mtx);
		if(!(isInput)){
			newInput_CV.wait(lock);
		}
		if(userInput == "q"){
			// before exit stop all threads
			stopThreads(threadArr);
			if(inputThread.joinable()) inputThread.join();
			newPasswd_CV.notify_one();
			if(consumerThread.joinable()) consumerThread.join();
			threadsFinished_CV.notify_one();
			if(informFinish.joinable()) informFinish.join();
			break;
		}
		passwdFile.open(userInput, std::ios::out);
		if(!passwdFile){
			std::cout << "No file of such name in current directory. Provide correct filename."
			 << std::endl;
			isInput = false;
		}
		if(passwdFile){
			// stop threads' execution
			stopThreads(threadArr);
			newPasswd_CV.notify_one();
			if(consumerThread.joinable()) consumerThread.join();
			threadsFinished_CV.notify_one();
			if(informFinish.joinable()) informFinish.join();
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
				std::cout << "Passwords and emails successfuly read to memory. Starting new search." << std::endl;
				passwdFile.close();
			}
			isInput = false;
			lock.unlock(); // here?
			// start threads to find passwords
			consumerThread = std::thread(consumer);
			informFinish = std::thread(informThreadsFinish);
			threadStartnewSearch();
		}
	}
    return 0;
}