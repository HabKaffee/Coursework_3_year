#include <vector>
#include <random>
#include <string>
#include <set>
#include <iostream>
#include <fstream>

#include <chrono>
#include <ctime>

const int treshold = 2500;
const int prime = 2347;
const int len_of_alphabet = 26;
const int multiply_const = 3;

struct Num {
    bool isOccupied = false;
    long long num = 0;
    std::string word;
    long long attempt;
};

long long hash_func(int a, int b, int m, int x) {
    return ((static_cast<unsigned long long>(a * x + b)) % prime) % m;
}

int word_to_int(std::vector<std::vector<int>> Table, std::string word) {
    int result = 0;
    for (int i = 0; i < word.size(); ++i) {
        //std::cout << Table[i][word[i]] << std::endl;
        result += Table[i][word[i]];
    }
    //std::cout << "Result = " << result << " word = " << word << std::endl;
    return result;
}

void generate_table(std::vector<std::vector<int>>& Table, int len_of_alphabet, int max_word_len, int n) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distrib(0, n - 1);
    for (int i = 0; i < max_word_len; ++i) {
        for (int j = 0; j < len_of_alphabet; ++j) {
            Table[i][j] = distrib(generator);
        }
    }
}

void print_table(std::vector<std::vector<int>> table) {
    for (int i = 0; i < table.size(); ++i) {
        std::cout << i << " [";
        for (int j = 0; j < table[i].size(); ++j) {
            std::cout << table[i][j] << "\t";
        }
        std::cout << "]" << std::endl;
    }
}

std::vector<Num> initialize(std::vector<std::string> words_to_hash,
                std::vector<std::vector<int>>& Table, 
                int max_len_of_word,
                int a_1, int a_2, int b_1, int b_2) {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<int> dist_for_a(1, prime/2 - 1);
    std::uniform_int_distribution<int> dist_for_b(0, prime/2 - 1);
    std::vector<Num> hash_table;
    for (int i = 0; i < words_to_hash.size() * multiply_const ; ++i) {
        hash_table.push_back({false, 0, std::string(), -1});
    }
    a_1 = dist_for_a(generator);
    a_2 = dist_for_a(generator);
    b_1 = dist_for_b(generator);
    b_2 = dist_for_b(generator);
    generate_table(Table, len_of_alphabet, max_len_of_word, prime/2);
    for (auto word : words_to_hash) {
        //std::cout << word << std::endl;
        int key = word_to_int(Table, word);
        long long h1 = hash_func(a_1, b_1, words_to_hash.size() * multiply_const, key);
        long long h2 = hash_func(a_2, b_2, words_to_hash.size() * multiply_const, key);
        for (long long i = 0; ; ++i) {
            int h_key_i = (h1 + i*h2) % (hash_table.size());
            if (!(hash_table[h_key_i].isOccupied)) {
                //std::cout << h_key_i << std::endl;
                hash_table[h_key_i].num = key;
                hash_table[h_key_i].isOccupied = true;
                hash_table[h_key_i].word = word;
                hash_table[h_key_i].attempt = i;
                break;
            }
        }
    }
    return hash_table;
}

int find_in_table(std::vector<std::vector<int>>& Table,
                std::vector<Num> hash_table, int max_len_of_word,
                int a_1, int a_2, int b_1, int b_2, std::string word) {
    int key = word_to_int(Table, word);
    long long h1 = hash_func(a_1, b_1, hash_table.size(), key);
    long long h2 = hash_func(a_2, b_2, hash_table.size(), key);
    for (int i = 0; i < 1000000; ++i) {
        int h_key_i = (h1 + i*h2) % (hash_table.size());
        if (hash_table[h_key_i].word == word) {
            return hash_table[h_key_i].num;
        }
    }
    //return -1;
}

int main() {
    std::vector<std::string> words_to_hash;
    std::ifstream input("text.txt");
    for (int i = 0; i < treshold; ++i) {
        std::string tmp;
        input >> tmp;
        words_to_hash.push_back(tmp);
    }
    std::set<std::string> set_input;
    for (auto el : words_to_hash) {
        set_input.insert(el);
    }
    words_to_hash.clear();
    for (auto el : set_input) {
        words_to_hash.push_back(el);
    }
    int max_len_of_word = 0;
    for (auto word : words_to_hash) {
        max_len_of_word = std::max(max_len_of_word, static_cast<int>(word.size()));
    }
    std::vector<std::vector<int>> Table;
    for (int i = 0; i < max_len_of_word; ++i) {
        Table.push_back(std::vector<int>());
        for (int j = 0; j < len_of_alphabet; ++j) {
            Table[i].push_back(0);
        }
    }
    int a_1 = 0, a_2 = 0, b_1 = 0, b_2 = 0;
    std::time_t start_build = std::clock();
    auto hash_table = initialize(words_to_hash, Table, max_len_of_word, a_1, a_2, b_1, b_2);
    std::time_t finish_build = std::clock();
    //std::cout << words_to_hash.size() << std::endl;
    //std::cout << "C = " << multiply_const << std::endl << std::endl;
    // print_table(Table);
    // for (auto el : hash_table) {
    //     std::cout << el.word << " -> " << el.num << " attempt = " << el.attempt << std::endl;
    // }
    std::vector<double> time_to_find;
    for (int i = 0; i < 50; ++i) {
        std::string word_to_find = words_to_hash[(std::rand())%(words_to_hash.size())];
        //std::cout << word_to_find << std::endl;
        std::time_t start_finding = std::clock();
        int word_to_find_res = find_in_table(Table, hash_table, max_len_of_word, a_1, a_2, b_1, b_2, word_to_find);
        std::time_t finish_finding = std::clock();
        std::cout << "Time for find = " << (finish_finding - start_finding) / (double) CLOCKS_PER_SEC << " seconds" << std::endl;
        time_to_find.push_back((finish_finding - start_finding) / (double) CLOCKS_PER_SEC);
    }
    for (auto a:time_to_find) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
    std::cout << "Time for build = " << (finish_build - start_build) / (double) CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << hash_table.size()/multiply_const << std::endl;
    return 0;
}
