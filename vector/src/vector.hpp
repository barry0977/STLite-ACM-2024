#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <memory>

namespace sjtu
{
    /**
     * a data container like std::vector
     * store data in a successive memory and support random access.
     */
    template<typename T>
    class vector
    {
    public:
        T* data;
        int maxsize;
        int length;
        std::allocator<T> alloc;

        void doublespace()
        {
            maxsize *= 2;
            T* tmp = data;
            data = alloc.allocate(maxsize);
            for(int i= 0;i<length;i++)//不能直接memmove,否则可能析构函数会有问题
            {
                new(data+i)T(std::move(*(tmp+i)));
            }
            alloc.deallocate(tmp, maxsize / 2);
        }

        void narrowspace()
        {
            T* tmp = data;
            data = alloc.allocate(maxsize / 2);
            for(int i= 0;i<length;i++)
            {
                new(data+i)T(std::move(*(tmp+i)));
            }
            alloc.deallocate(tmp, maxsize);
            maxsize /= 2;
        }
        /**
         * TODO
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        class const_iterator;
        class iterator
        {
            //	 The following code is written for the C++ type_traits library.
            //	 Type traits is a C++ feature for describing certain properties of a type.
            //	 For instance, for an iterator, iterator::value_type is the type that the
            //	 iterator points to.
            //	 STL algorithms and containers may use these type_traits (e.g. the following
            //	 typedef) to work properly. In particular, without the following code,
            //	 @code{std::sort(iter, iter1);} would not compile.
            //	 See these websites for more information:
            //	 https://en.cppreference.com/w/cpp/header/type_traits
            //	 About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            //	 About iterator_category: https://en.cppreference.com/w/cpp/iterator
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::output_iterator_tag;

        private:
            int order;
            T* begin;
            /**
             * TODO add data members
             *   just add whatever you want.
             */
        public:
            friend class vector<T>;
            iterator(const int& num, T* ptr)
            {
                order = num;
                begin = ptr;
            }

            iterator(const iterator& obj)
            {
                order = obj.order;
                begin = obj.begin;
            }
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator operator+(const int& n) const
            {
                return iterator(order + n, begin);
            }
            iterator operator-(const int& n) const
            {
                return iterator(order - n, begin);
            }
            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator& rhs) const
            {
                if (begin != rhs.begin)
                {
                    throw invalid_iterator();
                }
                return rhs.order - order > 0 ? rhs.order - order : order - rhs.order;
            }
            iterator& operator+=(const int& n)
            {
                order += n;
                return *this;
            }
            iterator& operator-=(const int& n)
            {
                order -= n;
                return *this;
            }
            /**
             * TODO iter++
             */
            iterator operator++(int)
            {
                iterator tmp = *this;
                order++;
                return tmp;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++()
            {
                order++;
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int)
            {
                iterator tmp = *this;
                order--;
                return tmp;
            }
            /**
             * TODO --iter
             */
            iterator& operator--()
            {
                order++;
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const
            {
                return begin[order];
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator& rhs) const
            {
                return begin + order == rhs.begin + rhs.order;
            }
            bool operator==(const const_iterator& rhs) const
            {
                return begin + order == rhs.begin + rhs.order;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator& rhs) const
            {
                return begin + order != rhs.begin + rhs.order;
            }
            bool operator!=(const const_iterator& rhs) const
            {
                return begin + order != rhs.begin + rhs.order;
            }
        };
        /**
         * TODO
         * has same function as iterator, just for a const object.
         */
        class const_iterator
        {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;
            using iterator_category = std::output_iterator_tag;

        private:
            int order;
            T* begin;

        public:
            const_iterator(const int& num, T* ptr)
            {
                order = num;
                begin = ptr;
            }

            const_iterator(const const_iterator& obj)
            {
                order = obj.order;
                begin = obj.begin;
            }
            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator operator+(const int& n) const
            {
                return iterator(order + n, begin);
            }
            const_iterator operator-(const int& n) const
            {
                return iterator(order - n, begin);
            }
            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const const_iterator& rhs) const
            {
                if (begin != rhs.begin)
                {
                    throw invalid_iterator();
                }
                return rhs.order - order > 0 ? rhs.order - order : order - rhs.order;
            }
            const_iterator& operator+=(const int& n)
            {
                order += n;
                return *this;
            }
            const_iterator& operator-=(const int& n)
            {
                order -= n;
                return *this;
            }
            /**
             * TODO iter++
             */
            const_iterator operator++(int)
            {
                iterator tmp = *this;
                order++;
                return tmp;
            }
            /**
             * TODO ++iter
             */
            const_iterator& operator++()
            {
                order++;
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int)
            {
                const_iterator tmp = *this;
                order--;
                return tmp;
            }
            /**
             * TODO --iter
             */
            const_iterator& operator--()
            {
                order++;
                return *this;
            }
            /**
             * TODO *it
             */
            const T& operator*() const
            {
                return begin[order];
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator& rhs) const
            {
                return begin + order == rhs.begin + rhs.order;
            }
            bool operator==(const const_iterator& rhs) const
            {
                return begin + order == rhs.begin + rhs.order;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator& rhs) const
            {
                return begin + order != rhs.begin + rhs.order;
            }
            bool operator!=(const const_iterator& rhs) const
            {
                return begin + order != rhs.begin + rhs.order;
            }
        };
        /**
         * TODO Constructs
         * At least two: default constructor, copy constructor
         */
        vector()
        {
            data = alloc.allocate(10);
            maxsize = 10;
            length = 0;
        }
        vector(const vector& other)
        {
            length = other.size();
            maxsize = other.maxsize;
            data = alloc.allocate(other.maxsize);
            for (int i = 0; i < length; i++)
            {
                new(data + i)T(other[i]);
            }
        }
        /**
         * TODO Destructor
         */
        ~vector()
        {
            for (int i = 0; i < length; i++)
            {
                data[i].~T();
            }
            alloc.deallocate(data, maxsize);
        }
        /**
         * TODO Assignment operator
         */
        vector& operator=(const vector& other)
        {
            if (this != &other)
            {
                for (int i = 0; i < length; i++)
                {
                    data[i].~T();
                }
                alloc.deallocate(data, maxsize);

                length = other.size();
                maxsize = other.maxsize;
                data = alloc.allocate(other.maxsize);
                for (int i = 0; i < length; i++)
                {
                    new(data + i)T(other[i]);
                }
            }
            return *this;
        }
        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T& at(const size_t& pos)
        {
            if (pos < 0 || pos >= length)
            {
                throw index_out_of_bound();
            }
            return data[pos];
        }
        const T& at(const size_t& pos) const
        {
            if (pos < 0 || pos >= length)
            {
                throw index_out_of_bound();
            }
            return data[pos];
        }
        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T& operator[](const size_t& pos)
        {
            if (pos < 0 || pos >= length)
            {
                throw index_out_of_bound();
            }
            return data[pos];
        }
        const T& operator[](const size_t& pos) const
        {
            if (pos < 0 || pos >= length)
            {
                throw index_out_of_bound();
            }
            return data[pos];
        }
        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T& front() const
        {
            if (length == 0)
            {
                throw container_is_empty();
            }
            return data[0];
        }
        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T& back() const
        {
            if (length == 0)
            {
                throw container_is_empty();
            }
            return data[length - 1];
        }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin()
        {
            return iterator(0, data);
        }
        const_iterator cbegin() const
        {
            return const_iterator(0, data);
        }
        /**
         * returns an iterator to the end.
         */
        iterator end()
        {
            return iterator(length, data);
        }
        const_iterator cend() const
        {
            return const_iterator(length, data);
        }
        /**
         * checks whether the container is empty
         */
        bool empty() const
        {
            return length == 0;
        }
        /**
         * returns the number of elements
         */
        size_t size() const
        {
            return length;
        }
        /**
         * clears the contents
         */
        void clear()
        {
            for (int i = 0; i < length; i++)
            {
                data[i].~T();
            }
            while (maxsize > 10)//缩减内存
            {
                narrowspace();
            }
            length = 0;
        }
        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T& value)
        {
            if (length == maxsize)
            {
                doublespace();
            }
            new(data + length)T(data[length - 1]);
            memmove(data + pos.order + 1, data + pos.order, sizeof(T) * (length - pos.order - 1));
            data[pos.order] = value;
            length++;
            return iterator(pos.order, data);
        }
        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t& ind, const T& value)
        {
            if (ind > length)
            {
                throw index_out_of_bound();
            }
            if (length == maxsize)
            {
                doublespace();
            }
            if (ind < length)
            {
                return insert(iterator(ind, data), value);
            }
            else
            {
                new(data + length)T(value);
                length++;
                return iterator(length - 1, data);
            }
        }
        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos)
        {
            memmove(data + pos.order, data + pos.order + 1, sizeof(T) * (length - pos.order - 1));
            data[length - 1].~T();
            length--;
            if (length <= maxsize / 4)
            {
                narrowspace();
            }
            return iterator(pos.order, data);
        }
        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t& ind)
        {
            if (ind >= length)
            {
                throw index_out_of_bound();
            }
            if (length <= maxsize / 4)
            {
                narrowspace();
            }
            return erase(iterator(ind, data));
        }
        /**
         * adds an element to the end.
         */
        void push_back(const T& value)
        {
            if (length == maxsize)
            {
                doublespace();
            }
            new(data + length)T(value);
            length++;
        }
        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back()
        {
            data[length - 1].~T();
            length--;
            if (length <= maxsize / 4)
            {
                narrowspace();
            }
        }
    };
}

#endif
