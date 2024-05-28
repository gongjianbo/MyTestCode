#include "ProgressSync.h"

ProgressSync::ProgressSync(QObject *parent)
    : QObject{parent}
    , mEmitter{new ProgressEmitter}
{
    connect(mEmitter.get(), &ProgressEmitter::finished, this, &ProgressSync::setStatus, Qt::QueuedConnection);
    connect(mEmitter.get(), &ProgressEmitter::valueChanged, this, &ProgressSync::setValue, Qt::QueuedConnection);
}

ProgressSync::~ProgressSync()
{
    mEmitter->setRunning(false);
}

int ProgressSync::getStatus() const
{
    return mStatus;
}

void ProgressSync::setStatus(int status)
{
    if (mStatus == status) {
        return;
    }
    mStatus = status;
    emit statusChanged(status);
}

qreal ProgressSync::getValue() const
{
    return mValue;
}

void ProgressSync::setValue(qreal value)
{
    if (qFuzzyCompare(mValue, value)) {
        return;
    }
    mValue = value;
    emit valueChanged(value);
}

std::shared_ptr<ProgressEmitter> ProgressSync::start()
{
    setValue(0.0);
    mEmitter->setRunning(true);
    setStatus(Running);
    return mEmitter;
}

void ProgressSync::cancel()
{
    mEmitter->setRunning(false);
    // 状态变更等 emitter end 去修改
}

void ProgressEmitter::setRunning(bool running)
{
    mRunning = running;
    mStatus = running ? ProgressSync::Running : ProgressSync::Cancel;
}

void ProgressEmitter::error()
{
    mStatus = ProgressSync::Error;
}

void ProgressEmitter::end()
{
    // 任务结束/退出时将状态同步到主线程
    if (mStatus == ProgressSync::Running) {
        mStatus = ProgressSync::Finish;
    }
    emit finished(mStatus);
}

void ProgressEmitter::update(qreal value)
{
    // 进度同步到主线程
    if (!getRunning())
        return;
    emit valueChanged(value);
}
