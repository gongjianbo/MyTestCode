#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>

class MyLabelPrivate;

/**
 * @brief 实现一个简单的pimpl
 * @details Qt相关宏的定义可以通过Q_DECLARE_PRIVATE等查找到
template <typename T> inline T *qGetPtrHelper(T *ptr) { return ptr; }
template <typename Ptr> inline auto qGetPtrHelper(const Ptr &ptr) -> decltype(ptr.operator->()) { return ptr.operator->(); }

#define Q_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(d_ptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d_ptr)); } \
    friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(Dptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(Dptr)); } \
    friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

#define Q_D(Class) Class##Private * const d = d_func()
#define Q_Q(Class) Class * const q = q_func()
 */
class MyLabel : public QLabel
{
public:
    explicit MyLabel(QWidget *parent=nullptr);

    void toPrev();
    void toNext();
    void addText(const QString &text);

private:
    //本来Qt的private实现可以通过构造函数来传递，
    //但是QWidgetPrivate是私有模块的没法用
    //所以起的同名变量，以使用相关宏
    QScopedPointer<MyLabelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(MyLabel)
    Q_DISABLE_COPY(MyLabel)
};

#endif // MYLABEL_H
