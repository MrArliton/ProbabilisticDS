// Only Header Library 
#pragma once

#include <cstdint>
#include <functional>
#include <cstring>
#include <list>
// License -- GNU General Public License v3.0 
// MrArliton - Developer

// Bloom filter
//----------------------------------
// Max size of struct_bloom = 2^64
// Max amount of hash_functions = 2^16 
template<typename Type, uint64_t size, uint16_t amount_hash_func = 1>
class bloom{
private:
    uint8_t* data = nullptr;

    inline void set_bit(uint64_t index)
    {
        *(data + index / 8) |= (1 << static_cast<uint8_t>(7 - index % 8)); 
    }
    /* Bloom filter does not contain a delete function.
    void clear_bit(uint64_t index)
    {
        *(data + index / 8) &= ~(1 << static_cast<uint8_t>(7 - index % 8)); 
    }
    */
    inline bool get_bit(uint64_t index)
    {
        return *(data + index / 8) & (1 << static_cast<uint8_t>(7 - index % 8)); 
    }  

    inline uint64_t* hash(const Type& obj)
    {
        uint64_t* hash_values = new uint64_t[amount_hash_func];
        for(uint16_t i = 0;i < amount_hash_func;i++)
        {
            hash_values[i] = (std::hash<Type>{}(obj) ^ std::hash<uint16_t>{}(i)) % size; 
        }
        return hash_values; 
    }
  

public:
    // Block copying and moving
    bloom(const bloom &) = delete;
    bloom(bloom &&) = delete;
    bloom& operator=(const bloom &) = delete;
    bloom& operator=(bloom &&) = delete;
    //    
    bloom()
    {
        const uint64_t amount_blocks = (size+7)/8;
        data = new uint8_t[amount_blocks];
        for(uint64_t i = 0;i < amount_blocks;i++)
        {
            data[i] = 0b00000000;
        }
    }
    ~bloom()
    {
        delete[] data;
        data = nullptr;    
    }

    void insert(const Type& obj)
    {
        uint64_t* hash_values = hash(obj);
        for(uint16_t i = 0;i < amount_hash_func;i++)
        {
            set_bit(hash_values[i]);
        }
        delete[] hash_values;
    }
    /* Bloom filter does not contain a delete function.
    void remove_object(const Type& obj)
    {
       uint64_t* hash_values = hash(obj);
       for(uint16_t i = 0;i < amount_hash_func;i++)
       {
           clear_bit(hash_values[i]);
       }
       delete[] hash_values;
    }
    */
    bool contains_p(const Type& obj) // Probabilistic [0,1]
    {
       uint64_t* hash_values = hash(obj);
       uint64_t amount = 0;
       for(uint16_t i = 0;i < amount_hash_func;i++)
       {
           amount += static_cast<uint64_t>(get_bit(hash_values[i]));
       }
       delete[] hash_values;
       return static_cast<double>(amount) /
        static_cast<double>(amount_hash_func);
    }

    double contains(const Type& obj) // true - Maybe exists --- false - Not exists exactly
    {
       uint64_t* hash_values = hash(obj);
       uint64_t amount = 0;
       for(uint16_t i = 0;i < amount_hash_func;i++)
       {
           amount += static_cast<uint64_t>(get_bit(hash_values[i]));
       }
       delete[] hash_values;
       if(amount == amount_hash_func){
            return true;
       }
       return false; 
    }    
    void clear()
    {
      const uint64_t amount_blocks = (size+7)/8;
      for(uint64_t i = 0;i < amount_blocks;i++)
        {
            data[i] = 0b00000000;
        }
    }

};
// End -- Bloom filter
//----------------------------------

// Count-min Sketch
//----------------------------------
template<typename Type, uint64_t size, uint16_t amount_hash_func = 1> 
class cmsketch
{
private:
    uint64_t** data;

    inline uint64_t* hash(const Type& obj)
    {
        uint64_t* hash_values = new uint64_t[amount_hash_func];
        for(uint16_t i = 0;i < amount_hash_func;i++)
        {
            hash_values[i] = (std::hash<Type>{}(obj) ^ std::hash<uint16_t>{}(i)) % size; 
        }
        return hash_values; 
    } 

public:
    // Block copying and moving
    cmsketch(const cmsketch &) = delete;
    cmsketch(cmsketch &&) = delete;
    cmsketch& operator=(const cmsketch &) = delete;
    cmsketch& operator=(cmsketch &&) = delete;
    //
    cmsketch()
    {
        data = new uint64_t*[size];
        for(uint64_t i = 0;i < size;i++)
        {
            data[i] = new uint64_t[amount_hash_func];           
            for(uint16_t j = 0;j < amount_hash_func;j++) data[i][j] = 0;
        }
    }

    ~cmsketch()
    {
        for(uint64_t i = 0;i < size;i++)
        {
            delete[] (data[i]);     
        }
        delete[] data;
    }

    uint64_t count(const Type& obj)
    {
        uint64_t* index_hash = hash(obj);
        uint64_t min_count = INT64_MAX;
        for(uint16_t i = 0;i < amount_hash_func;i++)
        {
            min_count = std::min(data[index_hash[i]][i], min_count); 
        }
        delete[] index_hash;
        return min_count;
    }

    void insert(const Type& obj)
    {
        uint64_t* index_hash = hash(obj);
        for(uint16_t i = 0;i < amount_hash_func;i++)
        {
            data[index_hash[i]][i]++; 
        }
        delete[] index_hash;
    }
    
};
// End -- Count-min Sketch
//----------------------------------

// Skip list
//----------------------------------
template<typename Type>
class skiplist
{
private:
    uint64_t size = 0;
    uint32_t amount_levels = 1; 
    struct node // Singly linked list
    {
        Type* value;
        node* next = nullptr;
        node* down = nullptr;
    };

    node* first = nullptr;

   
    inline node* make_upper_level(node* nd)
    {
        node* start = new node{nd->value, nullptr, nd};  
        node* buff = start;  
        nd = nd->next;
        for(uint64_t i = 1;nd != nullptr;i++)
        {   
            if(i%2==0)
            {
                buff->next = new node{nd->value, nullptr, nd};
                buff = buff->next;
            }
            nd = nd->next;
        }
        return start;
    } 

    inline node* find_pr(const Type& obj)
    {
        node* buff = first;
        node* last = nullptr;
        node* down = nullptr;  

        while(buff != nullptr)
        {
            down = buff->down;

            while(buff && *buff->value <= obj)
            {
                if(*buff->value == obj) return last;
                down = buff->down;
                last = buff;
                buff = buff->next;
            }
            buff = down;
        }

        return last;
    }

    inline node* find(const Type& obj)
    {
        node* buff = first;
        node* last = nullptr;
        node* down = nullptr;  

        while(buff != nullptr)
        {
            down = buff->down;

            while(buff && *buff->value <= obj)
            {
                if(*buff->value == obj) return buff;
                down = buff->down;
                last = buff;
                buff = buff->next;
            }
            buff = down;
        }

        return last;
    }

    inline std::pair<node*, node**> find_sp(const Type& obj)
    {
        uint32_t level = 0;
        node** levels = new node*[amount_levels+1];
 
        node* buff = first;
        node* last = nullptr;
        node* down = nullptr;  

        while(buff != nullptr)
        {
            down = buff->down;

            while(buff && *buff->value <= obj)
            {
                if(*buff->value == obj) return {buff, levels};
                down = buff->down;
                last = buff;
                buff = buff->next;
            }
            levels[level] = last;
            level++;
            buff = down;
        }

        return {last, levels};
    }

    inline bool random() // Standart Random function 1/2 - 1/2 probability
    // You can change this function for another distribution
    {   
        std::srand(time(0));
        if(std::rand() % 3 == 1) return true;
        return false;
    }

public:
    skiplist(const std::list<Type>& lst) // Input -- sorted list
    {
        if(lst.size() == 0) return;

        first = new node{new Type(lst.front())};
        node* buff = first;
        for(auto iter = std::next(lst.begin());iter != lst.end();iter++)
        {
            size++;
            buff->next = new node{new Type(*iter)};
            buff = buff->next;
        }
        uint64_t pw = size/2;
        while(pw > 0)
        {
            first = make_upper_level(first);
            pw/=2;
            amount_levels++;
        }
    }

    ~skiplist()
    {
        if(size == 0) return;
        node* down = first->down;
        node* buff = first;
        while(down != nullptr)
        {
            while(first != nullptr)
            {
                buff = buff->next;
                delete first;
                first = buff;
            }
            first = down;
            buff = first;
            down = first->down;
        }
        buff = first;
        while(first != nullptr)
        {   
            buff = buff->next;  
            delete first->value;
            delete first;
            first = buff;
 
        }
    }
    
    bool contains(const Type& obj)
    {
        node* elem;
        if(!(elem = find(obj)) || *elem->value != obj)    
            return false;
        return true;    
    }

    void insert(const Type& obj)
    {
        if(size == 0)
        {
            first = new node{obj};
            size++;
            return;
        }

        node** levels;
        node* nd;
        std::tie(nd,levels) = find_sp(obj); 
        if(nd == nullptr) // Element is first
        {
            Type* value = new Type(obj);
            node* buff = first;
            first = new node{value, buff, nullptr};
            node* nd = first;
            while(buff->down != nullptr)
            {
                buff = buff->down;
                nd->down = new node{value, buff, nullptr};
                nd = nd->down;
            }    

        }else
        {
            if(nd->down != nullptr) //  The same element was found - will not be pushed to high levels
            {
                while(nd->down != nullptr)
                {
                    nd = nd->down;
                }
                node* buff = nd->next;
                nd->next = new node{new Type(obj), buff, nullptr};
            }else //  The same element wasn't found - will be pushed to high levels
            {
                node* buff = nd->next;
                nd->next = new node{new Type(obj), buff, nullptr};
                int64_t level = amount_levels-2;
                while(random() && level >= 0)
                {
                    buff = levels[level]->next;
                    levels[level]->next = new node{nd->next->value, buff, nd->next};
                    nd = levels[level];
                    level--;
                }
            }
        }    
        size++;
        delete[]levels;
    }
    void insert(Type&& obj)
    {
        if(size == 0)
        {
            first = new node{new Type(std::move(obj))};
            size++;
            return;
        }

        node** levels;
        node* nd;
        std::tie(nd,levels) = find_sp(obj); 

        if(nd == nullptr) // Element is first
        {
            Type* value = new Type(std::move(obj));
            node* buff = first;
            first = new node{value, buff, nullptr};
            node* nd = first;
            while(buff->down != nullptr)
            {
                buff = buff->down;
                nd->down = new node{value, buff, nullptr};
                nd = nd->down;
            }    

        }else
        {
            if(nd->down != nullptr) //  The same element was found - will not be pushed to high levels
            {
                while(nd->down != nullptr)
                {
                    nd = nd->down;
                }
                node* buff = nd->next;
                nd->next = new node{new Type(std::move(obj)), buff, nullptr};
            }else //  The same element wasn't found - will be pushed to high levels
            {
                node* buff = nd->next;
                nd->next = new node{new Type(std::move(obj)), buff, nullptr};
                int64_t level = static_cast<int64_t>(amount_levels)-2;
                while(random() && level >= 0)
                {
                    buff = levels[level]->next;
                    levels[level]->next = new node{nd->next->value, buff, nd->next};
                    nd = levels[level];
                    level--;
                }
            }
        }
        uint64_t sz_f_level = 0;
        for(node* buff = first; buff!=nullptr;buff=buff->next) { sz_f_level++; }
        if(sz_f_level >= 4)
        {
            first = make_upper_level(first);
            amount_levels++;
        }
        size++;
        delete[]levels;
    }
    void erase(const Type& obj)
    {
        node* nd;
        if(nd = find_pr(obj))
        {
            if(*nd->next->value == obj)
            {
                node* buff = nd->next->next;
                Type* buff_v = nd->next->value;
                delete nd->next;
                nd->next = buff;
                nd = nd->down;

                while(nd!=nullptr)
                {
                    while(*nd->next->value != obj)
                    {
                        nd = nd->next;
                    }
                    buff = nd->next->next;
                    nd->next = buff;
                    nd = nd->down;
                }

                delete buff_v;
            }
        }else
        {
            node** levels = new node*[amount_levels];
            int64_t level = 0;
            node* nd = first;
            delete nd->value;
            while(nd!=nullptr)
            {
                levels[level] = nd->next;
                node* buff = nd->down;
                delete nd;
                nd = buff;
                level++;
            }
            level = amount_levels-1;
            nd = levels[level];
            while(level > 0)
            {
                level--;
                if(levels[level] == nullptr || levels[level]->down != levels[level + 1])
                {
                    levels[level] = new node{nd->value, levels[level], levels[level+1]};
                }
            }
            first = levels[0];
            size--;
            delete levels;

        }
    }

// Operators
friend std::ostream& operator<<(std::ostream& os, skiplist<Type>& skl)
{   
    node* nd = skl.first;
    //for(;nd->down != nullptr;nd = nd->down);
    for(;nd != nullptr;nd = nd->down)
    {
        for(node* buff = nd;buff != nullptr;buff = buff->next)
        {
            os << *buff->value << " ";
        }
        os << "\n"; 
    }
    return os;
}

};
// End -- Skip list
//----------------------------------
