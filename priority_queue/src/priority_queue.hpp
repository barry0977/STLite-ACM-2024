#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

    /**
     * a container like std::priority_queue which is a heap internal.
     */
    template<typename T, class Compare = std::less<T>>
    class priority_queue {
    private:
        struct node
        {
            T value;
            int degree = 0;
            node* parent = nullptr;
            node* child = nullptr;
            node* sibling = nullptr;


            node() = default;

            node(const T e) :value(e) {}
            node(const node* other)
            {
                value = other->value;
                degree = other->degree;
            }

            bool operator<(const node& other)const
            {
                return value < other.value;
            }
        };

        void link(node* a, node* b)//将两棵树合并为一棵树，其中一棵树变为另一棵树的子树
        {
            b->parent = a;
            b->sibling = a->child;
            a->child = b;
            a->degree++;
        }

        node* treemerge(node* a, node* b)//把两个二项堆连成一个
        {
            node* first;
            node* p;
            node* p1 = a;
            node* p2 = b;
            if (p1 == nullptr || p2 == nullptr)
            {
                if (p1 == nullptr)
                {
                    first = p2;
                }
                else
                {
                    first = p1;
                }
                return first;
            }
            if (p1->degree < p2->degree)
            {
                first = p1;
                p = first;
                p1 = p1->sibling;
            }
            else
            {
                first = p2;
                p = first;
                p2 = p2->sibling;
            }
            while (p1 != nullptr && p2 != nullptr)
            {
                if (p1->degree < p2->degree)
                {
                    p->sibling = p1;
                    p = p1;
                    p1 = p1->sibling;
                }
                else
                {
                    p->sibling = p2;
                    p = p2;
                    p2 = p2->sibling;
                }
            }
            if (p1 != nullptr)
            {
                p->sibling = p1;
            }
            else
            {
                p->sibling = p2;
            }
            return first;
        }

        node* treeunion(node* a, node* b)
        {
            node* first = treemerge(a, b);//头结点，treemerge把两个二项堆连成一个二项堆
            if (first == nullptr)
            {
                return first;
            }
            node* prev = nullptr;//上一个节点
            node* now = first;//当前位置节点
            node* next = first->sibling;//下一个节点
            while (next != nullptr)
            {
                if ((now->degree != next->degree) || (next->sibling != nullptr && now->degree == next->sibling->degree))
                {
                    prev = now;
                    now = next;
                    next = next->sibling;
                }
                else
                {
                    if (Compare()(next->value, now->value))
                    {
                        now->sibling = next->sibling;
                        link(now, next);
                        next = now->sibling;
                    }
                    else
                    {
                        link(next, now);
                        if (prev == nullptr) { first = next; }
                        else { prev->sibling = next; }
                        now = next;
                        next = next->sibling;
                    }
                }
            }
            return first;
        }

        node* copytree(node* obj, node* father = nullptr)
        {
            if (obj == nullptr)
            {
                return nullptr;
            }
            else
            {
                node* tmp = new node(obj->value);
                tmp->degree = obj->degree;
                tmp->parent = father;
                tmp->sibling = copytree(obj->sibling, tmp->parent);
                tmp->child = copytree(obj->child, tmp);
                return tmp;
            }
        }

        void deletetree(node* a)
        {
            if (a == nullptr)
            {
                return;
            }
            deletetree(a->child);
            deletetree(a->sibling);
            delete a;
        }

        node* head = nullptr;
        size_t length;
    public:
        priority_queue()
        {
            length = 0;
        }
        priority_queue(const priority_queue& other)
        {
            length = other.length;
            node* p = other.head;
            head = copytree(p);
        }

        ~priority_queue()
        {
            deletetree(head);
        }

        priority_queue& operator=(const priority_queue& other)
        {
            if (this == &other)
            {
                return *this;
            }
            else
            {
                deletetree(head);

                length = other.length;
                node* p = other.head;
                head = copytree(p);
                return *this;
            }
        }
        /**
         * get the top of the queue.
         * @return a reference of the top element.
         * throw container_is_empty if empty() returns true;
         */
        const T& top() const {
            if (length == 0)
            {
                throw container_is_empty();
            }
            else
            {
                node* p = head->sibling;
                node* ans = head;
                T max = head->value;
                while (p != nullptr)
                {
                    if (Compare()(max, p->value))
                    {
                        max = p->value;
                        ans = p;
                    }
                    p = p->sibling;
                }
                return ans->value;
            }
        }
        /**
         * TODO
         * push new element to the priority queue.
         */
        void push(const T& e) {
            node* tmp = new node(e);
            if (head == nullptr)
            {
                head = tmp;
                length++;
            }
            else
            {
                try
                {
                    length++;
                    tmp->sibling = head;
                    head = tmp;
                    node* prev = nullptr;
                    node* now = tmp;
                    node* next = tmp->sibling;
                    while (next != nullptr)
                    {
                        Compare()(next->value, now->value);
                        if (now->degree != next->degree || (next->sibling != nullptr && now->degree == next->sibling->degree))
                        {
                            prev = now;
                            now = next;
                            next = now->sibling;
                        }
                        else
                        {
                            if (Compare()(next->value, now->value))
                            {
                                now->sibling = next->sibling;
                                link(now, next);
                                next = now->sibling;
                            }
                            else
                            {
                                link(next, now);
                                if (prev == nullptr) { head = next; }
                                else { prev->sibling = next; }
                                now = next;
                                next = next->sibling;
                            }
                        }
                    }
                }
                catch (...)
                {
                    head = tmp->sibling;
                    delete tmp;
                    length--;
                }
            }
        }
        /**
         * TODO
         * delete the top element.
         * throw container_is_empty if empty() returns true;
         */
        void pop() {
            if (length == 0)
            {
                throw container_is_empty();
            }
            else
            {
                length--;
                node* p = head->sibling;
                node* p_prev = head;
                node* ans = head;
                node* before;//记录含最小值的头结点
                T max = head->value;
                while (p != nullptr)
                {
                    if (Compare()(max, p->value))
                    {
                        max = p->value;
                        ans = p;
                        before = p_prev;
                    }
                    p_prev = p;
                    p = p->sibling;
                }

                if (ans == head)
                {
                    head = ans->sibling;
                }
                else if (ans->sibling == nullptr)
                {
                    before->sibling = nullptr;
                }
                else
                {
                    before->sibling = ans->sibling;
                }

                node* childtree = ans->child;
                delete ans;
                if (childtree != nullptr)
                {
                    childtree->parent = nullptr;
                    node* x = childtree->sibling;
                    node* x_prev = childtree;
                    childtree->sibling = nullptr;
                    while (x != nullptr)
                    {
                        node* t = x->sibling;
                        x->sibling = x_prev;
                        x->parent = nullptr;
                        x_prev = x;
                        x = t;
                    }
                    head = treeunion(head, x_prev);
                }
            }
        }
        /**
         * return the number of the elements.
         */
        size_t size() const {
            return length;
        }
        /**
         * check if the container has at least an element.
         * @return true if it is empty, false if it has at least an element.
         */
        bool empty() const {
            return length == 0;
        }
        /**
         * merge two priority_queues with at most O(logn) complexity.
         * clear the other priority_queue.
         */
        void merge(priority_queue& other) {
            node* h1 = head;
            node* h2 = other.head;
            length += other.length;
            head = treeunion(h1, h2);
            other.length = 0;
            other.head = nullptr;
        }
    };
}

#endif