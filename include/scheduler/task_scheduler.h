#ifndef CUSTOMOS_TASK_SCHEDULER_H
#define CUSTOMOS_TASK_SCHEDULER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace customos {
namespace scheduler {

// Task priority
enum class TaskPriority {
    LOW,
    NORMAL,
    HIGH,
    URGENT
};

// Task recurrence
enum class RecurrenceType {
    ONCE,
    DAILY,
    WEEKLY,
    MONTHLY,
    CUSTOM
};

// Task status
enum class TaskStatus {
    PENDING,
    RUNNING,
    COMPLETED,
    FAILED,
    CANCELLED
};

// Scheduled task
struct ScheduledTask {
    std::string id;
    std::string title;
    std::string description;
    std::string command;  // Command to execute
    time_t scheduled_time;
    RecurrenceType recurrence;
    TaskPriority priority;
    TaskStatus status;
    time_t created;
    time_t last_run;
    std::string category;
    bool enabled;
};

// Reminder
struct Reminder {
    std::string id;
    std::string title;
    std::string message;
    time_t reminder_time;
    TaskPriority priority;
    bool dismissed;
    bool recurring;
    int snooze_minutes;
};

// Task Scheduler and Reminder System
class TaskScheduler {
public:
    static TaskScheduler& instance();

    // Initialize scheduler
    bool initialize();
    void shutdown();

    // Task scheduling
    std::string schedule_task(const std::string& title, const std::string& command,
                              time_t scheduled_time, RecurrenceType recurrence = RecurrenceType::ONCE);
    bool cancel_task(const std::string& task_id);
    bool enable_task(const std::string& task_id, bool enabled);
    bool update_task(const std::string& task_id, const ScheduledTask& task);
    ScheduledTask get_task(const std::string& task_id);
    std::vector<ScheduledTask> list_tasks(const std::string& category = "");
    std::vector<ScheduledTask> get_upcoming_tasks(int hours = 24);

    // Task execution
    bool run_task_now(const std::string& task_id);
    std::string get_task_output(const std::string& task_id);

    // Reminders
    std::string add_reminder(const std::string& title, const std::string& message,
                            time_t reminder_time, TaskPriority priority = TaskPriority::NORMAL);
    bool dismiss_reminder(const std::string& reminder_id);
    bool snooze_reminder(const std::string& reminder_id, int minutes);
    std::vector<Reminder> list_reminders();
    std::vector<Reminder> get_active_reminders();

    // Quick scheduling helpers
    std::string schedule_daily(const std::string& title, const std::string& command, int hour, int minute);
    std::string schedule_weekly(const std::string& title, const std::string& command, int day_of_week, int hour, int minute);
    std::string remind_in_minutes(const std::string& title, const std::string& message, int minutes);
    std::string remind_at_time(const std::string& title, const std::string& message, const std::string& time_str);

    // Callbacks
    using TaskCallback = std::function<void(const ScheduledTask&)>;
    using ReminderCallback = std::function<void(const Reminder&)>;
    void set_task_complete_callback(TaskCallback callback);
    void set_reminder_callback(ReminderCallback callback);

    // Categories
    std::vector<std::string> list_categories();

private:
    TaskScheduler();
    ~TaskScheduler();
    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace scheduler
} // namespace customos

#endif // CUSTOMOS_TASK_SCHEDULER_H
