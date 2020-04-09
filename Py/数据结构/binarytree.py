class Node():
    ''' 定义节点 '''

    def __init__(self, value, left=None, right=None):
        ''' 构造节点 
        :param value:节点值
        :param left:左子树
        :param right:右子树
        '''
        self.value = value
        self.left = left
        self.right = right


class BinaryTree():
    ''' 二叉树 '''

    def __init__(self, args: list):
        self.__root = None  # 根节点
        self.create(args)

    def create(self, args: list):
        ''' 根据参数构建一棵二叉树，这里我们用最简单的层级式构建，生成完全二叉树 
        :param args:参数列表，类型为list
        '''
        if not isinstance(args, list):
            # 类型判断
            # raise ValueError('Parameter type must be list')
            return
        list_len = len(args)
        if list_len <= 0:
            return
        node_list = []
        # 根据元素生成节点
        for value in args:
            node_list.append(Node(value))
        index = 0
        # 把节点串起来，成为一棵完全二叉树
        while index < list_len:
            if index*2+1 < list_len:
                node_list[index].left = node_list[index*2+1]
            if index*2+2 < list_len:
                node_list[index].right = node_list[index*2+2]
            index += 1
        self.__root = node_list[0]

    def preorder(self):
        ''' 前序遍历 '''
        def order1(rootnode: Node):
            ''' 递归的方式遍历 '''
            if rootnode == None:
                return
            print(rootnode.value, end=' ')  # 先访问根节点
            order1(rootnode.left)  # 递归遍历左子树
            order1(rootnode.right)  # 递归遍历右子树
        print("前序遍历(递归式)：", end=' ')
        order1(self.__root)
        print()

        def order2(rootnode: Node):
            ''' 非递归的方式遍历 '''
            node_stack = []  # 用list来当stack栈
            cursor = rootnode
            while cursor != None or len(node_stack) != 0:
                if cursor != None:
                    print(cursor.value, end=' ')
                    node_stack.append(cursor)
                    cursor = cursor.left
                else:
                    cursor = node_stack.pop()
                    cursor = cursor.right
        print("前序遍历(非递归)：", end=' ')
        order2(self.__root)
        print()

    def inorder(self):
        ''' 中序遍历 '''
        def order1(rootnode: Node):
            ''' 递归的方式遍历 '''
            if rootnode == None:
                return
            order1(rootnode.left)  # 递归遍历左子树
            print(rootnode.value, end=' ')  # 左右之间访问根节点
            order1(rootnode.right)  # 递归遍历右子树
        print("中序遍历(递归式)：", end=' ')
        order1(self.__root)
        print()

        def order2(rootnode: Node):
            ''' 非递归的方式遍历 '''
            node_stack = []  # 用list来当stack栈
            cursor = rootnode
            while cursor != None or len(node_stack) != 0:
                if cursor != None:
                    node_stack.append(cursor)
                    cursor = cursor.left
                else:
                    cursor = node_stack.pop()
                    print(cursor.value, end=' ')
                    cursor = cursor.right
        print("中序遍历(非递归)：", end=' ')
        order2(self.__root)
        print()

    def postorder(self):
        ''' 后序遍历 '''
        def order1(rootnode: Node):
            ''' 递归的方式遍历 '''
            if rootnode == None:
                return
            order1(rootnode.left)  # 递归遍历左子树
            order1(rootnode.right)  # 递归遍历右子树
            print(rootnode.value, end=' ')  # 最后访问根节点
        print("后序遍历(递归式)：", end=' ')
        order1(self.__root)
        print()

        def order2(rootnode: Node):
            ''' 非递归的方式遍历 '''
            node_stack = []  # 用list来当stack栈
            cursor = rootnode  # cursor存当前节点
            prev = None  # prev存之前访问过的节点
            # 先把cursor移动到左子树最底层
            while cursor:
                node_stack.append(cursor)
                cursor = cursor.left
            while len(node_stack) != 0:
                cursor = node_stack.pop()
                # 没有右节点 或者已访问过该右节点，才去访问根节点
                if cursor.right == None or cursor.right == prev:
                    print(cursor.value, end=' ')  # 最后访问根节点
                    prev = cursor
                # 或者有为访问的右节点
                else:
                    node_stack.append(cursor)
                    # 进入右子树
                    cursor = cursor.right
                    # 把cursor移动到该节点左子树最下边
                    while cursor:
                        node_stack.append(cursor)
                        cursor = cursor.left
        print("后序遍历(非递归)：", end=' ')
        order2(self.__root)
        print()

    def breadthFirst(self):
        ''' 广度优先-层级遍历 '''
        def order(rootnode):
            if rootnode == None:
                return
            from queue import Queue
            node_queue = Queue()
            node_queue.put(rootnode)
            cursor = None
            while not node_queue.empty():
                cursor = node_queue.get()
                print(cursor.value, end=' ')
                if cursor.left != None:
                    node_queue.put(cursor.left)
                if cursor.right != None:
                    node_queue.put(cursor.right)
        print("层级遍历：", end=' ')
        order(self.__root)
        print()


if __name__ == "__main__":
    print("龚建波1992")

    mytree = BinaryTree(list(range(1, 10)))  # 元素为 1-9
    mytree.preorder()  # 前序遍历
    mytree.inorder()  # 中序遍历
    mytree.postorder()  # 后续遍历
    mytree.breadthFirst()  # 层级遍历
