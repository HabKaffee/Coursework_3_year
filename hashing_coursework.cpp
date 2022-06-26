#include <random>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <set>
#include <map>
#include <fstream>

#include <cstdio>
#include <ctime>

const int treshold = 2000;

void init(std::vector<std::vector<int>>& T1,
          std::vector<std::vector<int>>& T2,
          int size_of_word, int size_of_alphabet) {
    for (int i = 0; i < size_of_word; ++i) {
        T1.push_back(std::vector<int>());
        T2.push_back(std::vector<int>());
        for (int j = 0; j < size_of_alphabet; ++j) {
            T1[i].push_back(0);
            T2[i].push_back(0);
        }
    }
    
}

void generate_table(std::vector<std::vector<int>>& table, int size_of_word, int size_of_alphabet, int n) {
    //std::cout << "tab_gen" << std::endl;
    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, n-1);
    for (int i = 0; i < size_of_word; ++i) {
        for (int j = 0; j < size_of_alphabet; ++j) {
            //table[i][j] = std::rand()%n;
            table[i][j] = distrib(gen);
        }
    }
}


void print_table(std::vector<std::vector<int>> table) {
    for (int i = 0; i < table.size(); ++i) {
        std::cout << i << "[";
        for (int j = 0; j < table[i].size(); ++j) {
            std::cout << table[i][j] << "\t";
        }
        std::cout << "]" << std::endl;
    }
}

int calc_f(std::vector<std::vector<int>> T, std::string word, int n) {
    long long res = 0;
    //print_table(T);
    //std::cout << std::endl;
    for (int i = 0; i < word.size(); ++i) {
        res += T[i][word[i] - 'a'];
    }
    return res % n;
}


void dfs(int vertex, std::vector<std::vector<int>> adj_list, 
        std::vector<int>& colours, bool& dont_have_backward_edge) { //0 - white, 1 - grey, 2 - black
    colours[vertex] = 1;
    for (auto vert:adj_list[vertex]) {
        if (colours[vert] == 0) {
            dfs(vert, adj_list, colours, dont_have_backward_edge);
            colours[vert] = 2;
        } else if (colours[vert] == 1) {
            //std::cout << "has cycle" << std::endl;
            dont_have_backward_edge = false;
            return;
        }
    }
    colours[vertex] = 2;
}

bool checkIfAcyclic(std::vector<std::vector<int>> adj_list) {
    std::vector<int> colours(adj_list.size(), 0);
    bool acyclic = true;
    for (int i = 0; i < adj_list.size(); ++i) {
        if (colours[i] == 0) {
            dfs(i, adj_list, colours, acyclic);
        }
    }
    // for (auto col: colours) {
    //     std::cout << col << " ";
    // }
    // std::cout << std::endl;
    return acyclic;
}

std::vector<std::vector<int>> get_adj_list(std::vector<std::pair<int, int>> E, int n) {
    std::vector<std::vector<int>> matrix(n, std::vector<int>(n,0));
    for (auto edge: E) {
        if (edge.first < 0 || edge.second < 0) {
            return std::vector<std::vector<int>>();
        }
        matrix[edge.first][edge.second] = 1;
        //matrix[edge.second][edge.first] = 1;
    }
    std::vector<std::vector<int>> adj_list;
    for (int i = 0; i < E.size(); ++i) {
        adj_list.push_back(std::vector<int>());
        for (int j = 0; j < E.size(); ++j) {
            if (matrix[i][j] && (i != j)) {
                adj_list[i].push_back(j);
            }
        }
    }
    return adj_list;
}


std::vector<std::vector<int>> mapping(std::vector<std::string> W, 
                                    int size_of_word, int size_of_alphabet,
                                    int n, std::vector<std::pair<int, int>>& E,
                                    std::vector<std::vector<int>>& T1,
                                    std::vector<std::vector<int>>& T2) {
    bool isAcyclic = false;
    std::vector<std::vector<int>> adj_list;
    while(!isAcyclic) {
        init(T1, T2, size_of_word, size_of_alphabet);
        generate_table(T1, size_of_word, size_of_alphabet, n);
        generate_table(T2, size_of_word, size_of_alphabet, n);
        
        for (auto word: W) {
            int f1 = calc_f(T1, word, n);
            int f2 = calc_f(T2, word, n);
            E.push_back(std::pair<int, int>(f1, f2));
        }
        adj_list = get_adj_list(E, n);
        if (adj_list.empty()) {
            E.clear();
            T1.clear();
            T2.clear();
            continue;
        }
        isAcyclic = checkIfAcyclic(adj_list);
        if (isAcyclic) {
            return adj_list;
        }
        E.clear();
        T1.clear();
        T2.clear();
    }
    return adj_list;
}

void print_vec(std::vector<int> a) {
    for (auto _a: a) {
        std::cout << _a << " ";
    }
    std::cout << std::endl;
}

void traverse(int u, std::vector<std::vector<int>> adj_list, 
                std::vector<std::pair<int, int>> E, 
                std::vector<bool>& visited, std::vector<int>& g,
                std::map<std::pair<int, int>, int> h) {
    visited[u] = true;
    for (auto w: adj_list[u]){
        if (!visited[w]) {
            g[w] = (h[std::pair<int, int>(u,w)] - g[u]) % E.size();
            traverse(w, adj_list, E, visited, g, h);
        }
    }

}

std::vector<int> assignment(std::vector<std::string> W, int size_of_alphabet, 
                int size_of_word, int n,
                std::vector<std::vector<int>>& T1, 
                std::vector<std::vector<int>>& T2,
                std::vector<std::pair<int, int>>& E,
                std::map<std::pair<int, int>, int>& h) {
    auto adj_list = mapping(W, size_of_word, size_of_alphabet, n, E, T1, T2);

    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, n-1);

    //preset h to h[i] = i
    for (int i = 0; i < E.size(); ++i) {
        h.insert(std::pair<std::pair<int, int>, int>(E[i], i));
    }

    std::vector<int> g(n, 0);
    std::vector<bool> visited(adj_list.size(), false);
    for (int v = 0; v < visited.size(); ++v) {
        if (!visited[v]) {
            g[v] = 0;
            traverse(v, adj_list, E, visited, g, h);
        }
    }
    return g;
}

void hash_func(std::vector<std::vector<int>> T1, std::vector<std::vector<int>> T2, std::map<std::pair<int, int>, int> h,
                std::vector<int> g, std::vector<std::pair<int, int>> E, std::string word, int size_of_word, int size_of_alphabet, int n) {
        int u = calc_f(T1, word, n);
        int v = calc_f(T2, word, n);
        std::cout << word << " (" << u << ", " << v << ")," << std::endl;
        h[std::pair<int, int>(u, v)] = (g[u] + g[v]) % E.size();
}

int find_in_table(std::vector<std::vector<int>> T1, std::vector<std::vector<int>> T2, std::map<std::pair<int, int>, int> h, std::vector<int> g,
                std::string word, int n, std::vector<std::pair<int, int>> E) {
    int u = calc_f(T1, word, n);
    int v = calc_f(T2, word, n);
    //int hash = (g[u] + g[v]) % E.size();
    return h[std::pair<int, int>(u, v)];
}

int main() {
    std::ifstream file("text.txt");
    std::vector<std::string> input;
    for (int i = 0 ; i < treshold; ++i) {
        std::string tmp;
        file >> tmp;
        input.push_back(tmp);
    }
    std::set<std::string> words_set;
    for (auto w: input) {
        words_set.insert(w);
    }
    int size_of_word = 0, size_of_alphabet = 26, n = words_set.size();
    for (auto s: words_set) {
        size_of_word = std::max(static_cast<std::size_t>(size_of_word), s.size());
    }
    //std::cout << size_of_word << std::endl;

    std::vector<std::vector<int>> T1, T2;
    std::vector<std::pair<int, int>> E;
    std::map<std::pair<int, int>, int> h;
    input.clear();
    for (auto s: words_set) {
        input.push_back(s);
    }
    std::time_t start_build = std::clock();
    auto table_g = assignment(input, size_of_alphabet, size_of_word, n, T1, T2, E, h);
    
    // std::cout << "T1" << std::endl;
    // print_table(T1);
    // std::cout << "T2" << std::endl;
    // print_table(T2);
    // std::cout << std::endl;
    
    for (auto word : input) {
        hash_func(T1, T2, h, table_g, E, word, size_of_word, size_of_alphabet, n);
    }
    std::time_t finish_build = std::clock();

    // for (auto it: h) {
    //     std::cout << "[" << it.first.first << ", " << it.first.second << "] " << it.second << std::endl; 
    // }
    std::cout << n << std::endl;
    std::vector<double> time_to_find;
    for (int i = 0; i < 50; ++i) {
        std::string word_to_find = input[(std::rand())%(input.size())];
        //std::cout << word_to_find << std::endl;
        std::time_t start_finding = std::clock();
        int word_to_find_res = find_in_table(T1, T2, h, table_g, word_to_find, n, E);
        std::time_t finish_finding = std::clock();
        std::cout << "Time for find = " << (finish_finding - start_finding) / (double) CLOCKS_PER_SEC << " seconds" << std::endl;
        time_to_find.push_back((finish_finding - start_finding) / (double) CLOCKS_PER_SEC);
    }
    for (auto a : time_to_find) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
    std::cout << "Time for build = " << (finish_build - start_build) / (double) CLOCKS_PER_SEC << " seconds" << std::endl;
    std::cout << h.size() << std::endl;
    return 0;
}