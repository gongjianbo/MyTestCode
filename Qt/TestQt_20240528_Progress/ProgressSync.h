#pragma once
#include <QObject>
#include <memory>
#include <atomic>
class ProgressEmitter;

/**
 * @brief 进度值同步
 * @author 龚建波
 * @date 2024-05-28
 * @details
 * 在线程中修改进度值，并通过信号槽更新进度值属性
 * ProgressSync 在主线程中访问
 * ProgressEmitter 在子线程中访问
 */
class ProgressSync : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int status READ getStatus NOTIFY statusChanged)
    Q_PROPERTY(qreal value READ getValue NOTIFY valueChanged)
public:
    // 进度状态
    enum ProgressStatus {
        // 初始状态
        Init,
        // 运行中
        Running,
        // 取消
        Cancel,
        // 结束
        Finish,
        // 异常
        Error
    };
    Q_ENUM(ProgressStatus)
public:
    explicit ProgressSync(QObject *parent = nullptr);
    ~ProgressSync();

    // 当前状态
    int getStatus() const;
    void setStatus(int status);
    // 当前进度值 [0, 1]
    qreal getValue() const;
    void setValue(qreal value);

    // 开始同步任务，内部初始化状态，返回的 emitter 放到线程中用于更新进度
    // 调用 start 后变更为 Running 状态
    std::shared_ptr<ProgressEmitter> start();
    // 取消任务
    // 等操作退出后才会变更为 Cancel 状态，目前没有表示正在取消的中间状态
    void cancel();

signals:
    void statusChanged(int status);
    void valueChanged(qreal value);

private:
    // 当前状态
    int mStatus{Init};
    // 当前进度值 [0, 1]
    qreal mValue{0.0};
    // 内部状态对象
    std::shared_ptr<ProgressEmitter> mEmitter;
};

/**
 * @brief 用户子线程访问的内部状态类
 * @author 龚建波
 * @date 2024-05-28
 */
class ProgressEmitter : public QObject
{
    Q_OBJECT
public:
    // 运行状态，取消运行时running置为false
    inline bool getRunning() const { return mRunning; }
    void setRunning(bool running);
    // 遇到错误，调用 end 后发送 error 状态
    void error();
    // 结束，发送 finish 状态
    void end();
    // 更新
    void update(qreal value);

signals:
    void finished(int status);
    void valueChanged(qreal value);

private:
    // 任务运行标志
    std::atomic_bool mRunning{false};
    // 任务的最终状态，调用 end 后发送到主线程
    std::atomic_int mStatus{ProgressSync::Running};
};
