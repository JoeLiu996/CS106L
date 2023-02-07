/*
* Assignment 2: HashMap template implementation (BASIC SOLUTION)
*      Notes: this file is what we call a .tpp file. It's not a .cpp file,
*      because it's not exactly a regular source code file. Instead, we are
*      defining the template definitions for our HashMap class.
*
*      TODO: write a comment here.
*
*      You'll notice that the commenting provided is absolutely stunning.
*      It was really fun to read through the starter code, right?
*      Please emulate this commenting style in your code, so your grader
*      can have an equally pleasant time reading your code. :)
*/

#include "hashmap.h"


template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap() : HashMap(kDefaultBuckets) { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(size_t bucket_count, const H& hash) :
   _size(0),
   _hash_function(hash),
   _buckets_array(bucket_count, nullptr) { }

template <typename K, typename M, typename H>
HashMap<K, M, H>::~HashMap() {
   clear();
}

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::size() const noexcept {
   return _size;
}

template <typename K, typename M, typename H>
inline bool HashMap<K, M, H>::empty() const noexcept {
   return size() == 0;
}

template <typename K, typename M, typename H>
inline float HashMap<K, M, H>::load_factor() const noexcept {
   return static_cast<float>(size())/bucket_count();
};

template <typename K, typename M, typename H>
inline size_t HashMap<K, M, H>::bucket_count() const noexcept {
   return _buckets_array.size();
};

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::contains(const K& key) const noexcept {
   return find_node(key).second != nullptr;
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::clear() noexcept {
   for (auto& curr : _buckets_array) {
       while (curr != nullptr) {
           auto trash = curr;
           curr = curr->next;
           delete trash;
       }
   }
   _size = 0;
}

template <typename K, typename M, typename H>
std::pair<typename HashMap<K, M, H>::value_type*, bool>
           HashMap<K, M, H>::insert(const value_type& value) {
   const auto& [key, mapped] = value;
   auto [prev, node_to_edit] = find_node(key);
   size_t index = _hash_function(key) % bucket_count();

   if (node_to_edit != nullptr) return {&(node_to_edit->value), false};
   _buckets_array[index] = new node(value, _buckets_array[index]);

   ++_size;
   return {&(_buckets_array[index]->value), true};
}

template <typename K, typename M, typename H>
M& HashMap<K, M, H>::at(const K& key) const {
   auto [prev, node_found] = find_node(key);
   if (node_found == nullptr) {
       throw std::out_of_range("HashMap<K, M, H>::at: key not found");
   }
   return node_found->value.second;
}

template <typename K, typename M, typename H>
typename HashMap<K, M, H>::node_pair HashMap<K, M, H>::find_node(const K& key) const {
   size_t index = _hash_function(key) % bucket_count();
   auto curr = _buckets_array[index];
   node* prev = nullptr; // if first node is the key, return {nullptr, front}
   while (curr != nullptr) {
       const auto& [found_key, found_mapped] = curr->value;
       if (found_key == key) return {prev, curr};
       prev = curr;
       curr = curr->next;
   }
   return {nullptr, nullptr}; // key not found at all.
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::debug() const {
   std::cout << std::setw(30) << std::setfill('-') << '\n' << std::setfill(' ')
             << "Printing debug information for your HashMap implementation\n"
             << "Size: " << size() << std::setw(15) << std::right
             << "Buckets: " << bucket_count() << std::setw(20) << std::right
             << "(load factor: " << std::setprecision(2) << load_factor() << ") \n\n";

   for (size_t i = 0; i < bucket_count(); ++i) {
       std::cout << "[" << std::setw(3) << i << "]:";
       auto curr = _buckets_array[i];
       while (curr != nullptr) {
           const auto& [key, mapped] = curr->value;
           // next line will not compile if << not supported for K or M
           std::cout <<  " -> " << key << ":" << mapped;
           curr = curr->next;
       }
       std::cout <<  " /" <<  '\n';
   }
   std::cout << std::setw(30) << std::setfill('-') << '\n';
}

template <typename K, typename M, typename H>
bool HashMap<K, M, H>::erase(const K& key) {
    auto [prev, node_to_erase] = find_node(key);
    if (node_to_erase == nullptr) {
        return false;
    } else {
        size_t index = _hash_function(key) % bucket_count();
        (prev ? prev->next : _buckets_array[index]) = node_to_erase->next;
        delete node_to_erase;
        --_size;
        return true;
    }
}

template <typename K, typename M, typename H>
void HashMap<K, M, H>::rehash(size_t new_bucket_count) {
    if (new_bucket_count == 0) {
        throw std::out_of_range("HashMap<K, M, H>::rehash: new_bucket_count must be positive.");
    }

    std::vector<node*> new_buckets_array(new_bucket_count);
    /* Optional Milestone 1: begin student code */

        // Hint: you should NOT call insert, and you should not call
        // new or delete in this function. You must reuse existing nodes.

    for(auto& cur : _buckets_array) {
        while(cur != nullptr) {
            const auto& [key, mapped] = cur->value;
            size_t index = _hash_function(key) % new_bucket_count;

            auto temp = cur;
            cur = cur->next;
            //insert temp into new_buckets
            temp->next = new_buckets_array[index];
            new_buckets_array[index] = temp;
        }
    }

    _buckets_array = std::move(new_buckets_array);

    /* end student code */
}

/*
    Milestone 2-3: begin student code

    Here is a list of functions you should implement:
    Milestone 2
        - operator[]
        - operator<<
        - operator== and !=
        - make existing functions const-correct

    Milestone 3
        - copy constructor
        - copy assignment
        - move constructor
        - move assignment
*/

/*Milestone 2*/
template <typename K, typename M, typename H>
M& HashMap<K, M, H>::operator[](const K& key) {
    const auto& [prev, node_found] = find_node(key);
    if (node_found == nullptr) {
        return insert({key, {}}).first->second;
    }
    return node_found->value.second;
}

template <typename K, typename M, typename H>
std::ostream& operator<<(std::ostream& os, const HashMap<K, M, H>& map) {
    bool is_first_element = true;
    os << "{";

    for(auto cur : map._buckets_array) {
        while(cur != nullptr) {
            const auto& [found_key, found_mapped] = cur->value;
            if(!is_first_element) {
                os << ", ";
            }
            os << found_key << ":" << found_mapped;
            is_first_element = false;
            cur = cur->next;
        }
    }

    os << "}";
    return os;
}

template <typename K, typename M, typename H>
bool operator==(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
    if(lhs.size() != rhs.size()) {
        return false;
    }

    for(auto cur : lhs._buckets_array) {
        while(cur != nullptr) {
            const auto& [lhs_found_key, lhs_found_mapped] = cur->value;
            const auto& [prev_node, rhs_curr_node] = rhs.find_node(lhs_found_key);
            const auto& [rhs_found_key, rhs_found_mapped] = rhs_curr_node->value;
            if(rhs_curr_node == nullptr || lhs_found_mapped != rhs_found_mapped) {
                return false;
            }
            cur = cur->next;
        }
    }
    return true;
}

template <typename K, typename M, typename H>
bool operator!=(const HashMap<K, M, H>& lhs, const HashMap<K, M, H>& rhs) {
    return !(lhs == rhs);
}


/*Milestone 3*/
//copy constructor
template <typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(const HashMap<K, M, H>& others) :
    _size(0),
    _hash_function(others._hash_function),
    _buckets_array(others.bucket_count(), nullptr) {
    for(auto cur : others._buckets_array) {
        while(cur != nullptr) {
            const auto& [key, mapped] = cur->value;
            insert({key, mapped});
            cur = cur->next;
        }
    }
}

//copy assignment
template<typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator=(const HashMap<K, M, H>& others) {
    if(this != &others) {
        clear();
        //_size = others._size;
        _hash_function = others._hash_function;

        for(auto cur : others._buckets_array) {
            while(cur != nullptr) {
                const auto& [key, mapped] = cur->value;
                insert({key, mapped});
                cur = cur->next;
            }
        }
    }

    return *this;
}

//move constructor
template<typename K, typename M, typename H>
HashMap<K, M, H>::HashMap(HashMap<K, M, H>&& others) :
    _size(std::move(others._size)),
    _hash_function(std::move(others._hash_function)),
    _buckets_array(others.bucket_count(), nullptr) {
    for(int i = 0; i < others.bucket_count(); i++) {
        _buckets_array[i] = std::move(others._buckets_array[i]);
        others._buckets_array[i] = nullptr;
    }
    others.clear();
}

//move operator
template<typename K, typename M, typename H>
HashMap<K, M, H>& HashMap<K, M, H>::operator=(HashMap<K, M, H>&& others) {
    if(this != &others) {
        clear();
        _size = std::move(others._size);
        _hash_function = std::move(others._hash_function);
        _buckets_array.resize(others.bucket_count());

        for(int i = 0; i < others.bucket_count(); i++) {
            _buckets_array[i] = std::move(others._buckets_array[i]);
            others._buckets_array[i] = nullptr;
        }
        others.clear();
    }

    return *this;
}
