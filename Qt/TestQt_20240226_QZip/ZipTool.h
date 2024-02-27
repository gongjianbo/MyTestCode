#pragma once
#include <QtCore/qglobal.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qfile.h>
#include <QtCore/qstring.h>

class QZipReaderPrivate;

class QZipReader
{
public:
    explicit QZipReader(const QString &fileName, QIODevice::OpenMode mode = QIODevice::ReadOnly);

    explicit QZipReader(QIODevice *device);
    ~QZipReader();

    QIODevice* device() const;

    bool isReadable() const;
    bool exists() const;

    struct FileInfo
    {
        FileInfo() noexcept
            : isDir(false), isFile(false), isSymLink(false), crc(0), size(0)
        {}

        bool isValid() const noexcept { return isDir || isFile || isSymLink; }

        QString filePath;
        uint isDir : 1;
        uint isFile : 1;
        uint isSymLink : 1;
        QFile::Permissions permissions;
        uint crc;
        qint64 size;
        QDateTime lastModified;
    };

    QVector<FileInfo> fileInfoList() const;
    int count() const;

    FileInfo entryInfoAt(int index) const;
    QByteArray fileData(const QString &fileName) const;
    bool extractAll(const QString &destinationDir) const;

    enum Status {
        NoError,
        FileReadError,
        FileOpenError,
        FilePermissionsError,
        FileError
    };

    Status status() const;

    void close();

private:
    QZipReaderPrivate *d;
    Q_DISABLE_COPY_MOVE(QZipReader)
};
Q_DECLARE_TYPEINFO(QZipReader::FileInfo, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(QZipReader::Status, Q_PRIMITIVE_TYPE);

class QZipWriterPrivate;

class QZipWriter
{
public:
    explicit QZipWriter(const QString &fileName, QIODevice::OpenMode mode = (QIODevice::WriteOnly | QIODevice::Truncate));

    explicit QZipWriter(QIODevice *device);
    ~QZipWriter();

    QIODevice* device() const;

    bool isWritable() const;
    bool exists() const;

    enum Status {
        NoError,
        FileWriteError,
        FileOpenError,
        FilePermissionsError,
        FileError
    };

    Status status() const;

    enum CompressionPolicy {
        AlwaysCompress,
        NeverCompress,
        AutoCompress
    };

    void setCompressionPolicy(CompressionPolicy policy);
    CompressionPolicy compressionPolicy() const;

    void setCreationPermissions(QFile::Permissions permissions);
    QFile::Permissions creationPermissions() const;

    void addFile(const QString &fileName, const QByteArray &data);

    void addFile(const QString &fileName, QIODevice *device);

    void addDirectory(const QString &dirName);

    void addSymLink(const QString &fileName, const QString &destination);

    void close();
private:
    QZipWriterPrivate *d;
    Q_DISABLE_COPY_MOVE(QZipWriter)
};
