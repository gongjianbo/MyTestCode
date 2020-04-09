class Node:
    ''' 节点的结构体 '''
    def __init__(self,value=None):
        self.value=value
        self.next=None

class SingleLinkedList:
    ''' 单链表类 '''
    def __init__(self):
        self.__head=None

    def isEmpty(self):
        ''' 判空 '''
        return self.__head==None

    def __len__(self):
        ''' Node个数 '''
        count=0
        cursor=self.__head
        while cursor!=None:
            count+=1
            cursor=cursor.next
        return count

    def __str__(self):
        ''' 输出元素列表 '''
        value_list=[]
        cursor=self.__head
        while cursor!=None:
            value_list.append(cursor.value)
            cursor=cursor.next
        return str(value_list)

    def pushBack(self,value):
        ''' 尾部添加一个节点 '''
        new_node=Node(value)
        if self.isEmpty():
            self.__head=new_node
        else:
            cursor=self.__head
            while cursor.next!=None:
                cursor=cursor.next
            cursor.next=new_node

    def popBack(self):
        ''' 尾部删除一个节点 ''' 
        pop_node=None
        #三种情况，无节点，单个节点，多个节点
        if self.__head==None:
            pass
        elif self.__head.next==None:
            pop_node=self.__head
            self.__head=None
        else:
            cursor=self.__head
            pop_node=self.__head.next
            while pop_node.next!=None:
                cursor=cursor.next
                pop_node=cursor.next
            cursor.next=None
        if pop_node!=None:
            return pop_node.value
        return None

    def pushFront(self,value):
        ''' 从头部插入节点 '''
        new_node=Node(value)
        new_node.next=self.__head
        self.__head=new_node

    def popFront(self):
        ''' 从头部删除节点 '''
        pop_node=self.__head
        if self.__head!=None:
            self.__head=self.__head.next
        if pop_node!=None:
            return pop_node.value
        return None

    def find(self,value):
        ''' 查找某个值第一次出现的pos '''
        find_pos=None
        count=0
        cursor=self.__head
        while cursor!=None:
            if cursor.value==value:
                find_pos=count
                break
            count+=1
            cursor=cursor.next
        return find_pos

    def insert(self,pos,value):
        ''' 任意位置插入 '''
        # 对于无效的位置，可以抛出异常，或者用布尔返回值
        length=len(self)
        if pos>=0 and pos<=length:
            # 0则头部插入，len则尾部插入
            if pos==0:
                self.pushFront(value)
            elif pos==length:
                self.pushBack(value)
            else:
                cursor=self.__head
                count=0
                while count<pos-1:
                    count+=1
                    cursor=cursor.next
                # 循环结束后，cursor指向pos-1的位置
                # [11,33]  insert(1,22) 这里pos为1，即33的位置，所以需要在0的后面插入
                new_node=Node(value)
                new_node.next=cursor.next
                cursor.next=new_node
            return True
        return False

    def remove(self,pos):
        ''' 任意位置删除 '''
        # 对于无效的位置，可以抛出异常，或者用布尔返回值
        length=len(self)
        if pos>=0 and pos<length:
            # 判断是否为头节点
            if pos==0:
                self.popFront()
            else:
                cursor=self.__head
                count=0
                while count<pos-1:
                    count+=1
                    cursor=cursor.next
                # 循环结束后，cursor指向pos-1的位置
                # [11,22,33]  remove(1) 这里pos为1，但是我们也要修改 11的next，所以停在0处
                if cursor.next!=None:
                    cursor.next=cursor.next.next
            return True
        return False

    def reverse(self):
        ''' 反序 '''
        # 显然至少需要2个元素
        if self.__head==None or self.__head.next==None:
            return
        # 三个连续节点的指针，用中间那个来进行反序操作
        pre_node=self.__head
        cur_node=pre_node.next
        nex_node=None
        while cur_node!=None:
            nex_node=cur_node.next
            cur_node.next=pre_node
            pre_node=cur_node
            cur_node=nex_node
        self.__head.next=None
        self.__head=pre_node


if __name__=="__main__":
    print("龚建波1992")

    nodes=SingleLinkedList()
    print(len(nodes))
    print(str(nodes))

    print('从尾部插入')
    nodes.pushBack(11)
    nodes.pushBack(12)
    nodes.pushBack(13)
    print(len(nodes))
    print(str(nodes))

    print('从尾部删除')
    print(nodes.popBack())
    print(nodes.popBack())
    print(len(nodes))
    print(str(nodes))
    print(nodes.popBack())
    print(nodes.popBack())
    print(len(nodes))
    print(str(nodes))

    print('从头部插入')
    nodes.pushFront(11)
    nodes.pushFront(12)
    nodes.pushFront(13)
    print(len(nodes))
    print(str(nodes))

    print('从头部删除')
    print(nodes.find(12))
    print(nodes.find(15))
    print(nodes.popFront())
    print(nodes.popFront())
    print(len(nodes))
    print(str(nodes))

    print('任意增删')
    nodes.insert(0,22)
    print(str(nodes))
    nodes.insert(1,33)
    print(str(nodes))
    nodes.insert(2,44)
    print(str(nodes))
    nodes.insert(3,55)
    print(str(nodes))
    nodes.reverse()
    print(str(nodes))
    nodes.remove(0)
    print(str(nodes))
    nodes.remove(1)
    print(str(nodes))
    nodes.remove(len(nodes)-1)
    print(str(nodes))
    nodes.reverse()
    print(str(nodes))