/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    > class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
    private:
        struct node
        {
            value_type* data;
            node* parent;
            node* left;
            node* right;
            int height;

            node(const value_type& element)
            {
                data = new value_type(element);
                parent = left = right = nullptr;
                height = 1;
            }
            node(const value_type& element, node* pt, node* lt, node* rt, int h = 1) :data(new value_type(element)), parent(pt), left(lt), right(rt), height(h) {}
        };

        node* root = nullptr;//根结点
        int length = 0;//总结点数

        int getheight(node* t)const
        {
            return t == nullptr ? 0 : t->height;
        }


        void LL(node*& t)//插入到左结点的左孩子，使得左结点比右结点高度高2
        {
            node* l = t->left, * lr = l->right;
            t->left = l->right;
            l->parent = t->parent;
            l->right = t;
            t->parent = l;
            if (lr != nullptr)lr->parent = t;
            t->height = std::max(getheight(t->left), getheight(t->right)) + 1;
            l->height = std::max(getheight(l->left), getheight(t)) + 1;
            t = l;
        }

        void LR(node*& t)//插入到左结点的右孩子，使得左结点比右结点高度高2
        {
            RR(t->left);
            LL(t);
        }

        void RL(node*& t)//插入到右结点的左孩子，使得右结点比左结点高度高2
        {
            LL(t->right);
            RR(t);
        }

        void RR(node*& t)//插入到右结点的右孩子，使得右结点比左结点高度高2
        {
            node* r = t->right, * rl = r->left;
            t->right = r->left;
            r->parent = t->parent;
            r->left = t;
            t->parent = r;
            if (rl != nullptr) rl->parent = t;
            t->height = std::max(getheight(t->left), getheight(t->right)) + 1;
            r->height = std::max(getheight(r->right), getheight(t)) + 1;
            t = r;
        }

        bool adjust(node*& t, int subtree)//调整树的平衡
        {
            if (subtree)//在右子树上删除，使右子树变矮
            {
                if (getheight(t->left) - getheight(t->right) == 1)
                {
                    return true;
                }
                if (getheight(t->right) == getheight(t->left))
                {
                    t->height--;
                    return false;
                }
                if (getheight(t->left->right) > getheight(t->left->left))
                {
                    LR(t);
                    return false;
                }
                LL(t);
                if (getheight(t->right) == getheight(t->left))
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                if (getheight(t->right) - getheight(t->left) == 1)
                {
                    return true;
                }
                if (getheight(t->right) == getheight(t->left))
                {
                    t->height--;
                    return false;
                }
                if (getheight(t->right->left) > getheight(t->right->right))
                {
                    RL(t);
                    return false;
                }
                RR(t);
                if (getheight(t->right) == getheight(t->left))
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }

        node* Insert(const value_type& value, node*& t, node*& p, int pos, bool& is)//1为左子树，2为右子树
        {
            node* ans;
            if (t == nullptr)
            {
                t = new node(value, p, nullptr, nullptr);
                if (p != nullptr)
                {
                    if (pos == 1)
                    {
                        p->left = t;
                    }
                    else if (pos == 2)
                    {
                        p->right = t;
                    }
                }
                length++;
                ans = t;
                is = true;
            }
            else if (Compare()(t->data->first, value.first))//在右子树插入
            {
                ans = Insert(value, t->right, t, 2, is);
                if (getheight(t->right) - getheight(t->left) == 2)
                {
                    if (Compare()(value.first, t->right->data->first))//插入在右子树的左子树
                    {
                        RL(t);
                    }
                    else
                    {
                        RR(t);
                    }
                }
            }
            else if (Compare()(value.first, t->data->first))//在左子树插入
            {
                ans = Insert(value, t->left, t, 1, is);
                if (getheight(t->left) - getheight(t->right) == 2)
                {
                    if (Compare()(value.first, t->left->data->first))//插入在左子树的左子树
                    {
                        LL(t);
                    }
                    else
                    {
                        LR(t);
                    }
                }
            }
            else//该key已经存在
            {
                is = false;
                ans = t;
            }
            t->height = std::max(getheight(t->left), getheight(t->right)) + 1;
            return ans;
        }

        bool Remove(const value_type& value, node*& t)//返回树是否变矮
        {
            if (t == nullptr)
            {
                return true;
            }

            if (Compare()(t->data->first, value.first))//要删除的结点在右子树
            {
                if (Remove(value, t->right))//如果高度没变化
                {
                    return true;
                }
                return adjust(t, 1);
            }
            else if (Compare()(value.first, t->data->first))//要删除的结点在左子树
            {
                if (Remove(value, t->left))
                {
                    return true;
                }
                return adjust(t, 0);
            }
            else//找到要删除的结点
            {
                if (t->left == nullptr && t->right == nullptr)//被删结点是叶结点
                {
                    delete t->data;
                    delete t;
                    t = nullptr;
                    length--;
                    return false;
                }
                else if (t->left == nullptr || t->right == nullptr)//被删结点只有一个儿子
                {
                    node* oldnode = t;
                    node* p = t->parent;
                    t = (t->left != nullptr) ? t->left : t->right;//因为t为引用，所以相当于直接改了父结点的儿子指针
                    t->parent = p;
                    delete oldnode->data;
                    delete oldnode;
                    length--;
                    return false;
                }
                else//被删结点有两个儿子
                {
                    //注意这里t就等于parent的儿子
                    node* old = t;
                    node* tmp = t->right;//找到右子树的最小结点作为替身
                    if (tmp->left == nullptr)//替身就是右子树
                    {
                        tmp->parent = t->parent;
                        t->right = tmp->right;
                        if (tmp->right != nullptr)
                        {
                            tmp->right->parent = t;
                        }
                        t->left->parent = tmp;//t一定有左子树
                        tmp->left = t->left;
                        tmp->right = t;
                        t->parent = tmp;
                        t->left = nullptr;
                        int h = t->height;
                        t->height = tmp->height;
                        tmp->height = h;
                        t = tmp;
                        if (Remove(*(old->data), t->right))//删除后右子树没有变矮
                        {
                            return true;
                        }
                        return adjust(t, 1);
                    }
                    else
                    {
                        while (tmp->left != nullptr)
                        {
                            tmp = tmp->left;
                        }
                        node* oldp = tmp->parent;
                        if (oldp->left == tmp)
                        {
                            oldp->left = t;
                        }
                        else
                        {
                            oldp->right = t;
                        }
                        tmp->parent = t->parent;
                        t->parent = oldp;
                        node* tmpr = tmp->right;//替身结点可能有右子树
                        tmp->left = t->left;
                        tmp->right = t->right;
                        t->left->parent = tmp;//t一定有左子树
                        t->right->parent = tmp;
                        if (old->right != nullptr)
                        {
                            old->right->parent = tmp;
                        }
                        t->left = nullptr;
                        t->right = tmpr;
                        if (tmpr != nullptr)
                        {
                            tmpr->parent = t;
                        }
                        int h = old->height;
                        old->height = tmp->height;
                        tmp->height = h;
                        t = tmp;//把t的父结点的儿子指针指向替身
                        if (Remove(*(old->data), t->right))//删除后右子树没有变矮
                        {
                            return true;
                        }
                        return adjust(t, 1);
                    }
                }
            }
        }

        void Clear(node* t)
        {
            if (t == nullptr)
            {
                return;
            }
            else
            {
                Clear(t->left);
                Clear(t->right);
                delete t->data;
                delete t;
                length--;
            }
        }

        node* copytree(node* obj, node* parent = nullptr)
        {
            if (obj == nullptr)
            {
                return nullptr;
            }
            else
            {
                node* tmp = new node(*(obj->data));
                tmp->parent = parent;
                tmp->height = obj->height;
                tmp->left = copytree(obj->left, tmp);
                tmp->right = copytree(obj->right, tmp);
                return tmp;
            }
        }
    public:
        class const_iterator;
        class iterator {
            friend class map;
        private:
            map* m;
            node* x;
        public:
            iterator() {
                m = nullptr;
                x = nullptr;
            }
            iterator(map* _map, node* _node)
            {
                m = _map;
                x = _node;
            }
            iterator(const iterator& other) {
                m = other.m;
                x = other.x;
            }
            /**
             * TODO iter++
             */
            iterator operator++(int)
            {
                if (x == nullptr)//迭代器为end()
                {
                    throw invalid_iterator();
                }
                iterator tmp = *this;
                node* n = x;
                if (n->right != nullptr)//存在右子树
                {
                    n = n->right;
                    while (n->left != nullptr)
                    {
                        n = n->left;
                    }
                    x = n;
                }
                else
                {
                    node* p = n->parent;
                    if (n->parent != nullptr)//存在父结点
                    {
                        while (p != nullptr && n != p->left)
                        {
                            n = p;
                            p = p->parent;
                        }
                    }
                    x = p;
                }
                return tmp;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++()
            {
                if (x == nullptr)//迭代器为end()
                {
                    throw invalid_iterator();
                }
                iterator tmp = *this;
                node* n = x;
                if (n->right != nullptr)//存在右子树
                {
                    n = n->right;
                    while (n->left != nullptr)
                    {
                        n = n->left;
                    }
                    x = n;
                }
                else
                {
                    node* p = n->parent;
                    if (n->parent != nullptr)//存在父结点
                    {
                        while (p != nullptr && n != p->left)
                        {
                            n = p;
                            p = p->parent;
                        }
                    }
                    x = p;
                }
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int)
            {
                if (m->length == 0)
                {
                    throw invalid_iterator();
                }
                node* r = m->root;
                while (r->left != nullptr)
                {
                    r = r->left;
                }
                if (r == x)//begin()--
                {
                    throw invalid_iterator();
                }
                iterator tmp = *this;
                node* n = x;
                if (n == nullptr)//end()--等于map中最后一个元素
                {
                    node* r = m->root;
                    if (r != nullptr)
                    {
                        while (r->right != nullptr)
                        {
                            r = r->right;
                        }
                        x = r;
                    }
                }
                else if (n->left != nullptr)//存在左子树
                {
                    n = n->left;
                    while (n->right != nullptr)
                    {
                        n = n->right;
                    }
                    x = n;
                }
                else
                {
                    node* p = n->parent;
                    if (n->parent != nullptr)//存在父结点
                    {
                        while (p != nullptr && n != p->right)
                        {
                            n = p;
                            p = p->parent;
                        }
                    }
                    x = p;
                }
                return tmp;
            }
            /**
             * TODO --iter
             */
            iterator& operator--()
            {
                if (m->length == 0)
                {
                    throw invalid_iterator();
                }
                node* r = m->root;
                while (r->left != nullptr)
                {
                    r = r->left;
                }
                if (r == x)//begin()--
                {
                    throw invalid_iterator();
                }
                iterator tmp = *this;
                node* n = x;
                if (n == nullptr)//end()--等于map中最后一个元素
                {
                    node* r = m->root;
                    if (r != nullptr)
                    {
                        while (r->right != nullptr)
                        {
                            r = r->right;
                        }
                        x = r;
                    }
                }
                else if (n->left != nullptr)//存在左子树
                {
                    n = n->left;
                    while (n->right != nullptr)
                    {
                        n = n->right;
                    }
                    x = n;
                }
                else
                {
                    node* p = n->parent;
                    if (n->parent != nullptr)//存在父结点
                    {
                        while (p != nullptr && n != p->right)
                        {
                            n = p;
                            p = p->parent;
                        }
                    }
                    x = p;
                }
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type& operator*() const
            {
                return *(x->data);
            }
            bool operator==(const iterator& rhs) const
            {
                return ((m == rhs.m) && (x == rhs.x));
            }
            bool operator==(const const_iterator& rhs) const
            {
                return ((m == rhs.m) && (x == rhs.x));
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator& rhs) const
            {
                return !(*this == rhs);
            }
            bool operator!=(const const_iterator& rhs) const
            {
                return !(*this == rhs);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type* operator->() const noexcept
            {
                return x->data;
            }
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            const map* m;
            const node* x;
            // data members.
        public:
            friend class map;
            const_iterator() {
                m = nullptr;
                x = nullptr;
            }
            const_iterator(const map* _map, const node* _node)
            {
                m = _map;
                x = _node;
            }
            const_iterator(const const_iterator& other) {
                m = other.m;
                x = other.x;
            }
            const_iterator(const iterator& other) {
                m = other.m;
                x = other.x;
            }
            // And other methods in iterator.
            const_iterator operator++(int)
            {
                if (x == nullptr)//迭代器为cend()
                {
                    throw invalid_iterator();
                }
                const_iterator tmp = *this;
                const node* n = x;
                if (n->right != nullptr)//存在右子树
                {
                    n = n->right;
                    while (n->left != nullptr)
                    {
                        n = n->left;
                    }
                    x = n;
                }
                else
                {
                    const node* p = n->parent;
                    if (n->parent != nullptr)//存在父结点
                    {
                        while (p != nullptr && n != p->left)
                        {
                            n = p;
                            p = p->parent;
                        }
                    }
                    x = p;
                }
                return tmp;
            }
            /**
             * TODO ++iter
             */
            const_iterator& operator++()
            {
                if (x == nullptr)//迭代器为cend()
                {
                    throw invalid_iterator();
                }
                const_iterator tmp = *this;
                const node* n = x;
                if (n->right != nullptr)//存在右子树
                {
                    n = n->right;
                    while (n->left != nullptr)
                    {
                        n = n->left;
                    }
                    x = n;
                }
                else
                {
                    const node* p = n->parent;
                    if (n->parent != nullptr)//存在父结点
                    {
                        while (p != nullptr && n != p->left)
                        {
                            n = p;
                            p = p->parent;
                        }
                    }
                    x = p;
                }
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int)
            {
                if (m->length == 0)
                {
                    throw invalid_iterator();
                }
                node* r = m->root;
                while (r->left != nullptr)
                {
                    r = r->left;
                }
                if (r == x)//cbegin()--
                {
                    throw invalid_iterator();
                }
                const_iterator tmp = *this;
                const node* n = x;
                if (n == nullptr)//end()--等于map中最后一个元素
                {
                    const node* r = m->root;
                    if (r != nullptr)
                    {
                        while (r->right != nullptr)
                        {
                            r = r->right;
                        }
                        x = r;
                    }
                }
                else if (n->left != nullptr)//存在左子树
                {
                    n = n->left;
                    while (n->right != nullptr)
                    {
                        n = n->right;
                    }
                    x = n;
                }
                else
                {
                    const node* p = n->parent;
                    if (n->parent != nullptr)//存在父结点
                    {
                        while (p != nullptr && n != p->right)
                        {
                            n = p;
                            p = p->parent;
                        }
                    }
                    x = p;
                }
                return tmp;
            }
            /**
             * TODO --iter
             */
            const_iterator& operator--()
            {
                if (m->length == 0)
                {
                    throw invalid_iterator();
                }
                node* r = m->root;
                while (r->left != nullptr)
                {
                    r = r->left;
                }
                if (r == x)//cbegin()--
                {
                    throw invalid_iterator();
                }
                const_iterator tmp = *this;
                const node* n = x;
                if (n == nullptr)//end()--等于map中最后一个元素
                {
                    const node* r = m->root;
                    if (r != nullptr)
                    {
                        while (r->right != nullptr)
                        {
                            r = r->right;
                        }
                        x = r;
                    }
                }
                else if (n->left != nullptr)//存在左子树
                {
                    n = n->left;
                    while (n->right != nullptr)
                    {
                        n = n->right;
                    }
                    x = n;
                }
                else
                {
                    const node* p = n->parent;
                    if (n->parent != nullptr)//存在父结点
                    {
                        while (p != nullptr && n != p->right)
                        {
                            n = p;
                            p = p->parent;
                        }
                    }
                    x = p;
                }
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            const value_type& operator*() const
            {
                return *(x->data);
            }
            bool operator==(const iterator& rhs) const
            {
                return ((m == rhs.m) && (x == rhs.x));
            }
            bool operator==(const const_iterator& rhs) const
            {
                return ((m == rhs.m) && (x == rhs.x));
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator& rhs) const
            {
                return !(*this == rhs);
            }
            bool operator!=(const const_iterator& rhs) const
            {
                return !(*this == rhs);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type* operator->() const noexcept
            {
                return x->data;
            }
        };
        /**
         * TODO two constructors
         */
        map() {}
        map(const map& other)
        {
            root = copytree(other.root);
            length = other.length;
        }
        /**
         * TODO assignment operator
         */
        map& operator=(const map& other)
        {
            if (this == &other)
            {
                return *this;
            }
            else
            {
                clear();
                root = copytree(other.root);
                length = other.length;
                return *this;
            }
        }
        /**
         * TODO Destructors
         */
        ~map()
        {
            Clear(root);
        }
        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T& at(const Key& key)
        {
            iterator it = find(key);
            if (it.x == nullptr)
            {
                throw index_out_of_bound();
            }
            else
            {
                return it.x->data->second;
            }
        }
        const T& at(const Key& key) const
        {
            const_iterator it = find(key);
            if (it.x == nullptr)
            {
                throw index_out_of_bound();
            }
            else
            {
                return it.x->data->second;
            }
        }
        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T& operator[](const Key& key)
        {
            iterator it = find(key);
            if (it.x == nullptr)
            {
                it = insert(value_type(key, T())).first;
            }
            return it.x->data->second;
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T& operator[](const Key& key) const
        {
            node* t = root;
            while (t != nullptr && (Compare()(t->data->first, key) || Compare()(key, t->data->first)))
            {
                if (Compare()(t->data->first, key))
                {
                    t = t->right;
                }
                else
                {
                    t = t->left;
                }
            }
            if (t == nullptr)
            {
                throw index_out_of_bound();
            }
            return t->data->second;
        }
        /**
         * return a iterator to the beginning
         */
        iterator begin()
        {
            if (length == 0)
            {
                return iterator(this, nullptr);
            }
            else
            {
                node* t = root;
                while (t->left != nullptr)
                {
                    t = t->left;
                }
                return iterator(this, t);
            }
        }
        const_iterator cbegin() const
        {
            if (length == 0)
            {
                return const_iterator(this, nullptr);
            }
            else
            {
                node* t = root;
                while (t->left != nullptr)
                {
                    t = t->left;
                }
                return const_iterator(this, t);
            }
        }
        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end()
        {
            return iterator(this, nullptr);
        }
        const_iterator cend() const
        {
            return const_iterator(this, nullptr);
        }
        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const
        {
            return root == nullptr;
        }
        /**
         * returns the number of elements.
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
            Clear(root);
            root = nullptr;
        }
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type& value)
        {
            bool is;
            node* p = nullptr;
            node* ans = Insert(value, root, p, 0, is);
            return pair(iterator(this, ans), is);
        }
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos)
        {
            if (pos.m != this)//不是这个map的迭代器
            {
                throw invalid_iterator();
            }
            else if (pos.x == nullptr)//pos == this->end()
            {
                throw invalid_iterator();
            }
            else
            {
                Remove(*(pos.x->data), root);
            }
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key& key) const
        {
            node* t = root;
            while (t != nullptr && (Compare()(t->data->first, key) || Compare()(key, t->data->first)))
            {
                if (Compare()(t->data->first, key))
                {
                    t = t->right;
                }
                else
                {
                    t = t->left;
                }
            }
            if (t == nullptr)//没有找到
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key& key)
        {
            node* t = root;
            while (t != nullptr && (Compare()(t->data->first, key) || Compare()(key, t->data->first)))
            {
                if (Compare()(t->data->first, key))
                {
                    t = t->right;
                }
                else
                {
                    t = t->left;
                }
            }
            return iterator(this, t);
        }
        const_iterator find(const Key& key) const
        {
            const node* t = root;
            while (t != nullptr && (Compare()(t->data->first, key) || Compare()(key, t->data->first)))
            {
                if (Compare()(t->data->first, key))
                {
                    t = t->right;
                }
                else
                {
                    t = t->left;
                }
            }
            return const_iterator(this, t);
        }
    };

}

#endif
