#include "scheduler/task_scheduler.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <sstream>
#include <fstream>

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

    std::string recurrence_to_string(RecurrenceType recurrence) {
        switch (recurrence) {
            case RecurrenceType::ONCE: return "once";
            case RecurrenceType::DAILY: return "daily";
            case RecurrenceType::WEEKLY: return "weekly";
            case RecurrenceType::MONTHLY: return "monthly";
            default: return "once";
        }
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

void TaskScheduler::execute_task(ScheduledTask& task) {
    task.status = TaskStatus::RUNNING;
    task.last_run = time(nullptr);
    
    // TODO: Actually execute the command
    // For now, simulate execution
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    task.status = TaskStatus::COMPLETED;
    task.execution_count++;
    
    // Update task in database
    auto& db = database::InternalDB::instance();
    if (task.recurrence == RecurrenceType::ONCE) {
        // Remove one-time tasks from database after completion
        db.delete_scheduled_task(task.user, task.id);
        
        // Also remove from memory
        std::lock_guard<std::mutex> lock(pimpl_->mutex);
        pimpl_->tasks.erase(task.id);
        
        LOG_INFO("One-time task '" + task.title + "' completed and removed from database");
    } else {
        // Update recurring task in database
        db.update_scheduled_task(task.user, task.id, true); // Keep enabled
        
        // Schedule next recurrence
        schedule_next_recurrence(task);
        
        LOG_INFO("Recurring task '" + task.title + "' completed, next run scheduled");
    }
    
    if (pimpl_->task_callback) {
        pimpl_->task_callback(task);
    }
}

    void schedule_next_recurrence(ScheduledTask& task) {
        // TODO: Calculate next run time based on recurrence
    }

    void check_reminders() {
        std::lock_guard<std::mutex> lock(mutex);
        
        time_t now = time(nullptr);
        std::vector<std::string> to_remove;
        
        for (auto& pair : reminders) {
            if (!pair.second.dismissed && pair.second.reminder_time <= now) {
                // Trigger reminder
                if (reminder_callback) {
                    reminder_callback(pair.second);
                }
                
                // Mark as completed in database
                auto& db = database::InternalDB::instance();
                db.complete_reminder(pair.second.user, pair.second.id);
                
                // For non-recurring reminders, mark for removal
                if (!pair.second.recurring) {
                    to_remove.push_back(pair.first);
                }
                
                LOG_INFO("Reminder '" + pair.second.title + "' triggered and completed");
            }
        }
        
        // Remove completed one-time reminders
        for (const auto& id : to_remove) {
            reminders.erase(id);
            LOG_INFO("One-time reminder removed from memory");
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

    // Load existing tasks and reminders from database
    load_from_database();

    pimpl_->running = true;
    pimpl_->scheduler_thread = std::thread(&Impl::scheduler_loop, pimpl_.get());
    
    LOG_INFO("Task scheduler initialized with database persistence");
    return true;
}

void TaskScheduler::load_from_database() {
    try {
        auto& db = database::InternalDB::instance();
        std::string user = auth::Authentication::instance().get_current_user();
        
        if (!user.empty()) {
            // Load scheduled tasks
            auto tasks_data = db.get_scheduled_tasks(user);
            for (const auto& task_data : tasks_data) {
                ScheduledTask task;
                task.id = task_data.at("id");
                task.user = user;
                task.title = task_data.at("title");
                task.command = task_data.at("command");
                // Convert schedule string back to recurrence type
                std::string schedule = task_data.at("schedule");
                if (schedule == "daily") task.recurrence = RecurrenceType::DAILY;
                else if (schedule == "weekly") task.recurrence = RecurrenceType::WEEKLY;
                else if (schedule == "monthly") task.recurrence = RecurrenceType::MONTHLY;
                else task.recurrence = RecurrenceType::ONCE;
                
                task.status = TaskStatus::PENDING;
                task.enabled = task_data.at("enabled") == "1";
                
                pimpl_->tasks[task.id] = task;
            }
            
            // Load reminders
            auto reminders_data = db.get_all_reminders(user);
            for (const auto& reminder_data : reminders_data) {
                Reminder reminder;
                reminder.id = reminder_data.at("id");
                reminder.user = user;
                reminder.title = reminder_data.at("title");
                reminder.message = reminder_data.at("message");
                reminder.reminder_time = static_cast<time_t>(std::stoll(reminder_data.at("remind_at")));
                reminder.dismissed = reminder_data.at("completed") == "1";
                
                if (!reminder.dismissed) {
                    pimpl_->reminders[reminder.id] = reminder;
                }
            }
        }
        
        LOG_INFO("Loaded " + std::to_string(pimpl_->tasks.size()) + " tasks and " + 
                std::to_string(pimpl_->reminders.size()) + " reminders from database");
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load scheduler data from database: " + std::string(e.what()));
    }
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
    
    // Get current user
    std::string user = auth::Authentication::instance().get_current_user();
    if (user.empty()) {
        return "";
    }
    
    ScheduledTask task;
    task.id = pimpl_->generate_id();
    task.user = user;
    task.title = title;
    task.command = command;
    task.scheduled_time = scheduled_time;
    task.recurrence = recurrence;
    task.priority = TaskPriority::NORMAL;
    task.status = TaskStatus::PENDING;
    task.created = time(nullptr);
    task.enabled = true;
    
    // Save to database
    auto& db = database::InternalDB::instance();
    if (!db.add_scheduled_task(user, task.id, title, command, 
                             recurrence_to_string(recurrence), scheduled_time)) {
        return "";
    }
    
    pimpl_->tasks[task.id] = task;
    LOG_INFO("Task '" + title + "' scheduled for user '" + user + "'");
    return task.id;
}

bool TaskScheduler::cancel_task(const std::string& task_id) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    
    auto it = pimpl_->tasks.find(task_id);
    if (it != pimpl_->tasks.end()) {
        it->second.status = TaskStatus::CANCELLED;
        
        // Remove from database
        auto& db = database::InternalDB::instance();
        db.delete_scheduled_task(it->second.user, task_id);
        
        LOG_INFO("Task '" + it->second.title + "' cancelled and removed from database");
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
    
    // Get current user
    std::string user = auth::Authentication::instance().get_current_user();
    if (user.empty()) {
        return "";
    }
    
    Reminder reminder;
    reminder.id = pimpl_->generate_id();
    reminder.user = user;
    reminder.title = title;
    reminder.message = message;
    reminder.reminder_time = reminder_time;
    reminder.priority = priority;
    reminder.dismissed = false;
    reminder.recurring = false;
    
    // Save to database
    auto& db = database::InternalDB::instance();
    if (!db.add_reminder(user, reminder.id, title, message, 
                        std::to_string(reminder_time))) {
        return "";
    }
    
    pimpl_->reminders[reminder.id] = reminder;
    LOG_INFO("Reminder '" + title + "' added for user '" + user + "'");
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
