#include "scheduler/task_scheduler.h"
#include <map>
#include <mutex>
#include <thread>
#include <chrono>

namespace customos {
namespace scheduler {

struct TaskScheduler::Impl {
    std::map<std::string, ScheduledTask> tasks;
    std::map<std::string, Reminder> reminders;
    bool running = false;
    std::thread scheduler_thread;
    TaskCallback task_callback;
    ReminderCallback reminder_callback;
    std::mutex mutex;
    int next_id = 1;

    std::string generate_id() {
        return "task_" + std::to_string(next_id++);
    }

    void scheduler_loop() {
        while (running) {
            check_scheduled_tasks();
            check_reminders();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void check_scheduled_tasks() {
        std::lock_guard<std::mutex> lock(mutex);
        
        time_t now = time(nullptr);
        for (auto& pair : tasks) {
            if (pair.second.enabled && 
                pair.second.status == TaskStatus::PENDING &&
                pair.second.scheduled_time <= now) {
                // Execute task
                execute_task(pair.second);
            }
        }
    }

    void execute_task(ScheduledTask& task) {
        task.status = TaskStatus::RUNNING;
        task.last_run = time(nullptr);
        
        // TODO: Actually execute the command
        
        task.status = TaskStatus::COMPLETED;
        
        if (task_callback) {
            task_callback(task);
        }
        
        // Handle recurrence
        if (task.recurrence != RecurrenceType::ONCE) {
            schedule_next_recurrence(task);
        }
    }

    void schedule_next_recurrence(ScheduledTask& task) {
        // TODO: Calculate next run time based on recurrence
    }

    void check_reminders() {
        std::lock_guard<std::mutex> lock(mutex);
        
        time_t now = time(nullptr);
        for (auto& pair : reminders) {
            if (!pair.second.dismissed && pair.second.reminder_time <= now) {
                if (reminder_callback) {
                    reminder_callback(pair.second);
                }
            }
        }
    }
};

TaskScheduler::TaskScheduler() : pimpl_(std::make_unique<Impl>()) {
}

TaskScheduler::~TaskScheduler() {
    shutdown();
}

TaskScheduler& TaskScheduler::instance() {
    static TaskScheduler instance;
    return instance;
}

bool TaskScheduler::initialize() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    if (pimpl_->running) {
        return false;
    }

    pimpl_->running = true;
    pimpl_->scheduler_thread = std::thread(&Impl::scheduler_loop, pimpl_.get());
    
    return true;
}

void TaskScheduler::shutdown() {
    {
        std::lock_guard<std::mutex> lock(pimpl_->mutex);
        pimpl_->running = false;
    }
    
    if (pimpl_->scheduler_thread.joinable()) {
        pimpl_->scheduler_thread.join();
    }
}

std::string TaskScheduler::schedule_task(const std::string& title, const std::string& command,
                                          time_t scheduled_time, RecurrenceType recurrence) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    ScheduledTask task;
    task.id = pimpl_->generate_id();
    task.title = title;
    task.command = command;
    task.scheduled_time = scheduled_time;
    task.recurrence = recurrence;
    task.priority = TaskPriority::NORMAL;
    task.status = TaskStatus::PENDING;
    task.created = time(nullptr);
    task.enabled = true;
    
    pimpl_->tasks[task.id] = task;
    return task.id;
}

bool TaskScheduler::cancel_task(const std::string& task_id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->tasks.find(task_id);
    if (it != pimpl_->tasks.end()) {
        it->second.status = TaskStatus::CANCELLED;
        return true;
    }
    return false;
}

bool TaskScheduler::enable_task(const std::string& task_id, bool enabled) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->tasks.find(task_id);
    if (it != pimpl_->tasks.end()) {
        it->second.enabled = enabled;
        return true;
    }
    return false;
}

bool TaskScheduler::update_task(const std::string& task_id, const ScheduledTask& task) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->tasks.find(task_id);
    if (it != pimpl_->tasks.end()) {
        it->second = task;
        return true;
    }
    return false;
}

ScheduledTask TaskScheduler::get_task(const std::string& task_id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->tasks.find(task_id);
    if (it != pimpl_->tasks.end()) {
        return it->second;
    }
    return ScheduledTask{};
}

std::vector<ScheduledTask> TaskScheduler::list_tasks(const std::string& category) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<ScheduledTask> result;
    for (const auto& pair : pimpl_->tasks) {
        if (category.empty() || pair.second.category == category) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<ScheduledTask> TaskScheduler::get_upcoming_tasks(int hours) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    time_t now = time(nullptr);
    time_t future = now + (hours * 3600);
    
    std::vector<ScheduledTask> result;
    for (const auto& pair : pimpl_->tasks) {
        if (pair.second.enabled && 
            pair.second.status == TaskStatus::PENDING &&
            pair.second.scheduled_time >= now &&
            pair.second.scheduled_time <= future) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool TaskScheduler::run_task_now(const std::string& task_id) {
    // TODO: Execute task immediately
    return false;
}

std::string TaskScheduler::get_task_output(const std::string& task_id) {
    // TODO: Get task output
    return "";
}

std::string TaskScheduler::add_reminder(const std::string& title, const std::string& message,
                                        time_t reminder_time, TaskPriority priority) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    Reminder reminder;
    reminder.id = pimpl_->generate_id();
    reminder.title = title;
    reminder.message = message;
    reminder.reminder_time = reminder_time;
    reminder.priority = priority;
    reminder.dismissed = false;
    reminder.recurring = false;
    
    pimpl_->reminders[reminder.id] = reminder;
    return reminder.id;
}

bool TaskScheduler::dismiss_reminder(const std::string& reminder_id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->reminders.find(reminder_id);
    if (it != pimpl_->reminders.end()) {
        it->second.dismissed = true;
        return true;
    }
    return false;
}

bool TaskScheduler::snooze_reminder(const std::string& reminder_id, int minutes) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->reminders.find(reminder_id);
    if (it != pimpl_->reminders.end()) {
        it->second.reminder_time += (minutes * 60);
        it->second.snooze_minutes = minutes;
        return true;
    }
    return false;
}

std::vector<Reminder> TaskScheduler::list_reminders() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<Reminder> result;
    for (const auto& pair : pimpl_->reminders) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<Reminder> TaskScheduler::get_active_reminders() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::vector<Reminder> result;
    for (const auto& pair : pimpl_->reminders) {
        if (!pair.second.dismissed) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::string TaskScheduler::schedule_daily(const std::string& title, const std::string& command, int hour, int minute) {
    // TODO: Calculate next occurrence
    time_t next_run = time(nullptr);  // Simplified
    return schedule_task(title, command, next_run, RecurrenceType::DAILY);
}

std::string TaskScheduler::schedule_weekly(const std::string& title, const std::string& command, int day_of_week, int hour, int minute) {
    // TODO: Calculate next occurrence
    time_t next_run = time(nullptr);  // Simplified
    return schedule_task(title, command, next_run, RecurrenceType::WEEKLY);
}

std::string TaskScheduler::remind_in_minutes(const std::string& title, const std::string& message, int minutes) {
    time_t reminder_time = time(nullptr) + (minutes * 60);
    return add_reminder(title, message, reminder_time);
}

std::string TaskScheduler::remind_at_time(const std::string& title, const std::string& message, const std::string& time_str) {
    // TODO: Parse time string
    time_t reminder_time = time(nullptr);  // Simplified
    return add_reminder(title, message, reminder_time);
}

void TaskScheduler::set_task_complete_callback(TaskCallback callback) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->task_callback = callback;
}

void TaskScheduler::set_reminder_callback(ReminderCallback callback) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->reminder_callback = callback;
}

std::vector<std::string> TaskScheduler::list_categories() {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    std::set<std::string> categories;
    for (const auto& pair : pimpl_->tasks) {
        if (!pair.second.category.empty()) {
            categories.insert(pair.second.category);
        }
    }
    return std::vector<std::string>(categories.begin(), categories.end());
}

} // namespace scheduler
} // namespace customos
