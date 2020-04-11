class Node:
    ''' 节点的结构体 '''

    def __init__(self, value=None):
        self.value = value  # 节点值
        self.prev = None   # 指向上一个节点
        self.next = None   # 指向下一个节点

    def insert(self, prev, next):
        ''' 节点插入到两个节点之间 '''
        self.prev = prev
        self.next = next
        prev.next = self
        next.prev = self

    def remove(self):
        ''' 移除该节点，并把本节点的头尾相连，
        LinkedList的head和tail不参与判断，所以不判断None '''
        # 不用自己管理内存就是好，直接修改前后节点的指向就行了
        self.prev.next = self.next
        self.next.prev = self.prev
        return self


class LinkedList:
    ''' 双向链表 '''

    def __init__(self):
        # 头，next保存第一个节点
        self.__head = Node()
        # 尾，prev保存最后一个节点
        self.__tail = Node()
        # 初始化，头尾指向彼此，有效节点在头尾之间
        self.__head.next = self.__tail
        self.__tail.prev = self.__head

    def isEmpty(self):
        ''' 判空 '''
        return self.__head.next == self.__tail

    def __len__(self):
        ''' Node个数 '''
        # 应该拿个变量保存长度，这样不用每次都去遍历
        count = 0
        cursor = self.__head
        while cursor.next != self.__tail:
            count += 1
            cursor = cursor.next
        return count

    def __str__(self):
        ''' 输出元素列表 '''
        value_list = []
        cursor = self.__head
        while cursor.next != self.__tail:
            value_list.append(cursor.next.value)
            cursor = cursor.next
        return str(value_list)

    def pushBack(self, value):
        ''' 尾部添加一个节点 '''
        new_node = Node(value)
        # 插入到在尾部和尾部的前一个节点
        new_node.insert(self.__tail.prev, self.__tail)

    def popBack(self):
        ''' 尾部删除一个节点 '''
        if not self.isEmpty():
            # 尾部的前一个节点，从尾部的前-前节点和尾部之间删除
            pop_node = self.__tail.prev.remove()
            return pop_node.value
        return None

    def pushFront(self, value):
        ''' 从头部插入节点 '''
        new_node = Node(value)
        new_node.insert(self.__head, self.__head.next)

    def popFront(self):
        ''' 从头部删除节点 '''
        if not self.isEmpty():
            pop_node = self.__head.next.remove()
            return pop_node.value
        return None

    def find(self, value):
        ''' 查找某个值第一次出现的pos '''
        # 我这里设计的是，没有找到就为None
        find_pos = None
        count = 0
        cursor = self.__head
        while cursor.next != self.__tail:
            if cursor.next.value == value:
                find_pos = count
                break
            count += 1
            cursor = cursor.next
        return find_pos

    def insert(self, pos, value):
        ''' 任意位置插入 '''
        # 对于无效的位置，可以抛出异常，或者用布尔返回值
        # （也可先判断pos离头还是尾更近，再进行循环）
        length = len(self)
        if pos >= 0 and pos <= length:
            cursor = self.__head
            count = 0
            while count < pos:
                count += 1
                cursor = cursor.next
            # 因为有效节点是从head.next开始的，所以count==pos时就是要插入的地方
            # 此时cursor指向待插入的前一个位置，cursor.next为后一个位置
            new_node = Node(value)
            new_node.insert(cursor, cursor.next)
            return True
        return False

    def remove(self, pos):
        ''' 任意位置删除 '''
        # 对于无效的位置，可以抛出异常，或者用布尔返回值
        # （也可先判断pos离头还是尾更近，再进行循环）
        length = len(self)
        if pos >= 0 and pos < length:
            cursor = self.__head.next
            count = 0
            while count < pos:
                count += 1
                cursor = cursor.next
            # 因为有效节点是从head.next开始的，所以count==pos时就是要删除的元素
            # [11,22,33]  remove(1) 这里pos为1即删除元素 22
            cursor.remove()
            return True
        return False

    def reverse(self):
        ''' 反序 '''
        # 显然至少需要2个元素
        if (self.__head.next == self.__tail or
                self.__head.next.next == self.__tail):
            return
        cursor = self.__head
        while cursor != None:
            # 交换节点的前后指向
            cursor.prev, cursor.next = cursor.next, cursor.prev
            cursor = cursor.prev
        # 原来的head和tail交换
        self.__head, self.__tail = self.__tail, self.__head

    def sort(self):
        ''' 排序，这里用冒泡排序演示下就行了 '''
        # 外部指针每次移动一个位置，内部指针每次循环一次
        outer = self.__head.next
        # 不等于tail.prev是因为比较的是 index 和 index+1 的节点，总不能拿 tail 来比较吧
        while outer != self.__tail and outer != self.__tail.prev:
            inner = outer
            while inner != self.__tail and inner != self.__tail.prev:
                if inner.value > inner.next.value:
                    # 这里根据自己的需求选择交换值还是交换节点，交换结点的话要拿变量保存节点
                    inner.value, inner.next.value = inner.next.value, inner.value
                inner = inner.next
            outer = outer.next


if __name__ == "__main__":
    print("龚建波 1992")
    mylist = LinkedList()

    print("尾部增删")
    mylist.pushBack(11)
    mylist.pushBack(22)
    mylist.pushBack(33)
    print(len(mylist))
    print(str(mylist))
    print(mylist.popBack())
    print(mylist.popBack())
    print(mylist.popBack())
    print(mylist.popBack())
    print(len(mylist))
    print(str(mylist))

    print("头部增删")
    mylist.pushFront(11)
    mylist.pushFront(22)
    mylist.pushFront(33)
    print(mylist.find(22))
    print(mylist.find(44))
    print(len(mylist))
    print(str(mylist))
    print(mylist.popFront())
    print(mylist.popFront())
    print(mylist.popFront())
    print(mylist.popFront())
    print(len(mylist))
    print(str(mylist))

    print("任意位置增删")
    mylist.pushBack(11)
    mylist.pushBack(22)
    mylist.pushBack(33)
    mylist.insert(3, 444)
    mylist.insert(0, 111)
    mylist.insert(2, 520)
    print(len(mylist))
    print(str(mylist))
    mylist.sort()
    print(str(mylist))
    mylist.remove(2)
    mylist.remove(0)
    mylist.remove(3)
    print(len(mylist))
    print(str(mylist))
    mylist.reverse()
    print(len(mylist))
    print(str(mylist))
