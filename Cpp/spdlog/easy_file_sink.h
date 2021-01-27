#pragma once

#include <spdlog/common.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/os.h>
#include <spdlog/details/circular_q.h>
#include <spdlog/details/synchronous_factory.h>

#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>
#include <tuple>

namespace spdlog {
namespace sinks {

//根据大小和时间生成日志文件
//龚建波 2021-1-18
//参照rotating_file_sink和daily_file_sink自定义
//（生成的时间也可以修改为任意间隔）
//（还有个需求没完成，就是清理旧文件，按照个数或者时间，即max_files参数目前相关逻辑是没用的）
template<typename Mutex>
class easy_file_sink final : public base_sink<Mutex>
{
public:
    easy_file_sink(filename_t base_filename, size_t max_size, size_t max_files = 0)
        : base_filename_(std::move(base_filename))
        , max_size_(max_size)
        , max_files_(max_files)
        , filenames_q_()
    {
        auto now = log_clock::now();
        auto filename = daily_filename_(base_filename_, now_tm(now));
        file_helper_.open(filename, false);
        //记录大小
        current_size_ = file_helper_.size();
        //记录新建时间，24h
        rotation_tp_ = next_rotation_tp_();

        if (max_files_ > 0)
        {
            init_filenames_q_();
        }
    }

    filename_t filename()
    {
        std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        return file_helper_.filename();
    }

protected:
    void sink_it_(const details::log_msg &msg) override
    {
        memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);
        current_size_ += formatted.size();

        auto time = msg.time;
        //超过大小原文件改后缀，超过日期新文件改日期名
        bool should_rotate = false;
        if(time >= rotation_tp_)
        {
            should_rotate = true;
            file_helper_.close();
            auto filename = daily_filename_(base_filename_, now_tm(time));
            file_helper_.open(filename, false);
            current_size_ = file_helper_.size();
            rotation_tp_ = next_rotation_tp_();
        }
        else if(current_size_ >= max_size_)
        {
            should_rotate = true;
            file_helper_.close();
            auto src_name = daily_filename_(base_filename_, now_tm(time));
            auto target_name = calc_filename_(base_filename_, now_tm(time));
            //如果ranme失败就用targetname作为当前文件名
            if (!rename_file_(src_name, target_name))
            {
                details::os::sleep_for_millis(200);
                if (!rename_file_(src_name, target_name))
                {
                    src_name = target_name;
                }
            }
            file_helper_.open(src_name, false);
            current_size_ = file_helper_.size();
            rotation_tp_ = next_rotation_tp_();
        }

        file_helper_.write(formatted);

        // Do the cleaning only at the end because it might throw on failure.
        if (should_rotate && max_files_ > 0)
        {
            delete_old_();
        }
    }

    void flush_() override
    {
        file_helper_.flush();
    }

private:
    void init_filenames_q_()
    {
        using details::os::path_exists;

        filenames_q_ = details::circular_q<filename_t>(static_cast<size_t>(max_files_));
        std::vector<filename_t> filenames;
        auto now = log_clock::now();
        while (filenames.size() < max_files_)
        {
            auto filename = calc_filename_(base_filename_, now_tm(now));
            if (!path_exists(filename))
            {
                break;
            }
            filenames.emplace_back(filename);
            now -= std::chrono::hours(24);
        }
        for (auto iter = filenames.rbegin(); iter != filenames.rend(); ++iter)
        {
            filenames_q_.push_back(std::move(*iter));
        }
    }

    tm now_tm(log_clock::time_point tp)
    {
        time_t tnow = log_clock::to_time_t(tp);
        return spdlog::details::os::localtime(tnow);
    }

    log_clock::time_point next_rotation_tp_()
    {
        auto now = log_clock::now();
        tm date = now_tm(now);
        date.tm_hour = 0;
        date.tm_min = 0;
        date.tm_sec = 0;
        auto rotation_time = log_clock::from_time_t(std::mktime(&date));
        if (rotation_time > now)
        {
            return rotation_time;
        }
        return {rotation_time + std::chrono::hours(24)};
    }

    // Delete the file N rotations ago.
    // Throw spdlog_ex on failure to delete the old file.
    void delete_old_()
    {
        using details::os::filename_to_str;
        using details::os::remove_if_exists;

        filename_t current_file = file_helper_.filename();
        if (filenames_q_.full())
        {
            auto old_filename = std::move(filenames_q_.front());
            filenames_q_.pop_front();
            bool ok = remove_if_exists(old_filename) == 0;
            if (!ok)
            {
                filenames_q_.push_back(std::move(current_file));
                throw_spdlog_ex("Failed removing daily file " + filename_to_str(old_filename), errno);
            }
        }
        filenames_q_.push_back(std::move(current_file));
    }

    //每日名称
    static filename_t daily_filename_(const filename_t &filename, const tm &now_tm)
    {
        filename_t basename, ext;
        std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
        return fmt::format(SPDLOG_FILENAME_T("{}_{:04d}_{:02d}_{:02d}{}"),
                           basename,
                           now_tm.tm_year + 1900,
                           now_tm.tm_mon + 1,
                           now_tm.tm_mday,
                           ext);
    }

    //超过大小后重命名，后面加上时分秒
    static filename_t calc_filename_(const filename_t &filename, const tm &now_tm)
    {
        filename_t basename, ext;
        std::tie(basename, ext) = details::file_helper::split_by_extension(filename);
        return fmt::format(SPDLOG_FILENAME_T("{}_{:04d}_{:02d}_{:02d}_{:02d}{:02d}{:02d}{}"),
                           basename,
                           now_tm.tm_year + 1900,
                           now_tm.tm_mon + 1,
                           now_tm.tm_mday,
                           now_tm.tm_hour,
                           now_tm.tm_min,
                           now_tm.tm_sec,
                           ext);
    }

    static bool rename_file_(const filename_t &src_filename, const filename_t &target_filename)
    {
        (void)details::os::remove(target_filename);
        return details::os::rename(src_filename, target_filename) == 0;
    }

    filename_t base_filename_;
    log_clock::time_point rotation_tp_;
    details::file_helper file_helper_;
    std::size_t max_size_;
    std::size_t max_files_;
    std::size_t current_size_;
    details::circular_q<filename_t> filenames_q_;
};

using easy_file_sink_mt = easy_file_sink<std::mutex>;
using easy_file_sink_st = easy_file_sink<details::null_mutex>;
}

template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> easy_logger_mt(
        const std::string &logger_name, const filename_t &filename, size_t max_size, size_t max_files = 0)
{
    return Factory::template create<sinks::easy_file_sink_mt>(logger_name, filename, max_size, max_files);
}

template<typename Factory = spdlog::synchronous_factory>
inline std::shared_ptr<logger> easy_logger_st(
        const std::string &logger_name, const filename_t &filename, size_t max_size, size_t max_files = 0)
{
    return Factory::template create<sinks::easy_file_sink_st>(logger_name, filename, max_size, max_files);
}
}
