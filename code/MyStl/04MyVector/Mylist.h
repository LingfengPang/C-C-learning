
template<class T,class Ref,class Ptr>
struct __list_iterator{
    typedef __list_iterator<T,T&,T*> iterator;
    typedef __list_iterator<T,Ref,Ptr> self;

    typedef bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef __list_node<T>* link_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    link_type node;//迭代器内部的普通指针

    __list_iterator(link_type x):node(x){}
    __list_iterator(){}
    __list_iterator(const iterator& x):node(x.node){}

    bool operator==(const self& x)const{return node == x.node;}
    bool operator!=(const self& x)const{return node != x.node;}

    reference operator*() const{return &(operator*());}

    self& operator++(){
        node = (link_type)((*node).next);
        return *this;
    }
    self operator++(int){
        selft tmp = *this;
        ++*this;
        return temp;
    }

    self& operator--(){
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }
};

template<class T,class Alloc = alloc>
class list{
    protected:
    typedef __list_node<T> list_node;
    typedef simple_alloc<list_node,Alloc> list_node_allocator;//专属空间配置器，每次配置一个大小
    public:
    typedef list_node* link_type;
    typedef __list_iterator iterator;

    protected:
    link_type node;
    protected:
    link_type get_node(){return list_node_allocator::allocate();}
    //释放节点
    void put_node(){list_node_allocator::deallocate(p);}
    //产生并配置一个节点
    link_type create_node(const T&x){
        link_type p = get_node();
        construct(&p->data,x);
        return p;
    }
    void destroy_node(link_type p){
        destroy(&p->data);
        put_node(p);
    }
    void empty_initialize(){
        node = get_node();
        node->next =node;
        node->prev = node;
    }
    public:
    //构造函数
    list(){empty_initialize();}
    public:
    iterator begin(){return (link_type)((*node).next);}
    iterator end(){return node;}
    bool empty() const {return node->next == node;}
    size_type size(){
        size_type result = 0;
        distance(begin(),end(),result);
        return result;
    }
    reference front(){return *begin();}
    reference back(){return *(--end());}

    iterator insert(iterator pos,const T&x){
        link_type tmp = create_node(x);
        tmp->next = pos.node;
        tmp->prev = pos.node->prev;
        (link_type(pos.node->prev))->next = tmp;
        pos.node0>prev = tmp;
        return tmp;
    }
    void push_back(const T& x){insert(end(),x);}
    void push_front(const T&x){insert(begin(),x);}
    
    iterator erase(iterator pos){
        link_type next_node = link_type(pos.node->next);
        link_type prev_node = link_type(pos.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy(pos.node);
        return (iterator(next_node));
    }

    void pop_front(){erase(begin());}
    void pop_back(){
        iterator tmp = end();
        erase(--tmp);
    }

    void clear(){
        link_type cur = (link_type)node->next;
        while(cur!=node){
            link_type tmp = cur;
            cur = (link_type)cur->next;
            destroy_node(tmp);
        }
        node->next = node;
        node->prev = node;
    }

    void remove(const T& value){
        iterator first = begin();
        iterator last = end();
        while(first != last){
            iterator next = first;
            ++next;
            if(*first == value)
                erase(first);
            first = next;
        }
    }
    //移除连续相同的
    void unique(){
        iterator first = begin();
        iterator last = end();
        if(first == last) return;
        iterator next = first;
        while(++next != last){
            if(*first == *next)
                erase(next);
            else
                first = next;
            next = first;
        }

    }
    //迁移，pos之前
    //前闭后开
    void transfer(iterator pos,iterator first,iterator last){
        (*(link_type((*last.node).prev))).next = pos.node;
        (*(link_type((*first.node).prev))).next = last.node;
        (*(link_type((*pos.node).prev))).next = first.node;
        link_type tmp = link_type((*pos.node).prev);
        (*pos.node).prev = (*last.node).prev;
        (*first.node).prev = tmp;
    }

    //将x结合与pos之前
    void splice(iterator pos,list& x){
        if(!x.empty())
            transfer(pos,x.begin(),x.end());
    }

    //将i所指的元素接合与pos之前,i可以是指向list的一个迭代器
    void splice(iterator pos,list&,iterator i){
        iterator j = i;
        ++j;
        if(pos == i || pos == j) return;
        transfer(pos,i,j);
    }

    void splice(iterator pos,list&,iterator first,iterator last){
        if(first != last)
            transfer(pos,first,last);
    }

    //合并x到*this上
    void merge(list<T,Alloc>& x){
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();

        while(first1!=last1 && first2!=last2){
            if(*first2<*first1){
                iterator next = first2;
                transfer(first1,first2,++next);
                first2 = next;
            }
            else{
                ++first1;
            }
        }
        if(first2 != last2) transfer(last1,first2,last2);
    }

    void reverse(){
        if(node->next == node||link_type(node->next)->next == node)
            return;
        iterator first = begin();
        ++first;
        while(first!=end()){
            iterator old = first;
            ++first;
            transfer(begin(),old,first);
        }
    }

    void sort(){
        if(node->next == node||link_type(node->next)->next == node)
            return;
        list<T,Alloc> carry;
        list<T,Alloc> counter[64];
        int fill = 0;

        while(!empty()){
            carry.splice(carry.begin(),*this,begin());
            int i = 0;
            while(i<fill &&!counter[i].empty()){
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            carry.swap(counter[i]);
            if(i == fill) ++fill;
        }

        for(int i = 1;i<fill;++i){
            counter[i].merge(counter[i-1]);
        swap(counter[fill-1]);
        }
    }

};