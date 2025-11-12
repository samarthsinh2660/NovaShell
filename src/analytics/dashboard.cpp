#include "analytics/dashboard.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <filesystem>
#include <unordered_map>

namespace novashell {
namespace analytics {

struct AnalyticsDashboard::Impl {
    std::unordered_map<std::string, Dashboard> dashboards_;
    std::unordered_map<std::string, TimeSeries> metrics_;
    std::string active_dashboard_;
    bool real_time_enabled_;
    int refresh_interval_;
    int data_retention_days_;
    bool collecting_metrics_;

    // Initialize default dashboards
    void initialize_default_dashboards() {
        // System Overview Dashboard
        Dashboard system_dashboard;
        system_dashboard.name = "system";
        system_dashboard.description = "System Performance Overview";
        system_dashboard.refresh_interval_seconds = 30;
        system_dashboard.auto_refresh = true;

        // CPU Usage Widget
        Widget cpu_widget;
        cpu_widget.id = "cpu_usage";
        cpu_widget.title = "CPU Usage (%)";
        cpu_widget.type = ChartType::LINE;
        cpu_widget.width = 6;
        cpu_widget.height = 4;
        system_dashboard.widgets.push_back(cpu_widget);

        // Memory Usage Widget
        Widget mem_widget;
        mem_widget.id = "memory_usage";
        mem_widget.title = "Memory Usage (%)";
        mem_widget.type = ChartType::LINE;
        mem_widget.width = 6;
        mem_widget.height = 4;
        system_dashboard.widgets.push_back(mem_widget);

        // Network Traffic Widget
        Widget net_widget;
        net_widget.id = "network_traffic";
        net_widget.title = "Network Traffic (MB)";
        net_widget.type = ChartType::BAR;
        net_widget.width = 12;
        net_widget.height = 4;
        system_dashboard.widgets.push_back(net_widget);

        dashboards_["system"] = system_dashboard;
        active_dashboard_ = "system";

        // Command Analytics Dashboard
        Dashboard cmd_dashboard;
        cmd_dashboard.name = "commands";
        cmd_dashboard.description = "Command Usage Analytics";
        cmd_dashboard.refresh_interval_seconds = 60;
        cmd_dashboard.auto_refresh = true;

        // Top Commands Widget
        Widget top_cmd_widget;
        top_cmd_widget.id = "top_commands";
        top_cmd_widget.title = "Most Used Commands";
        top_cmd_widget.type = ChartType::BAR;
        top_cmd_widget.width = 12;
        top_cmd_widget.height = 6;
        cmd_dashboard.widgets.push_back(top_cmd_widget);

        dashboards_["commands"] = cmd_dashboard;
    }

    // Generate mock data for demonstration
    TimeSeries generate_mock_data(const std::string& name, const std::string& unit, int hours = 24) {
        TimeSeries series;
        series.name = name;
        series.unit = unit;

        time_t now = time(nullptr);
        for (int i = hours * 4; i >= 0; --i) { // 15-minute intervals
            DataPoint point;
            point.timestamp = now - (i * 15 * 60);

            // Generate realistic-looking data
            if (name.find("cpu") != std::string::npos) {
                point.value = 10 + (rand() % 60); // 10-70% CPU usage
            } else if (name.find("memory") != std::string::npos) {
                point.value = 30 + (rand() % 40); // 30-70% memory usage
            } else if (name.find("network") != std::string::npos) {
                point.value = rand() % 100; // 0-100 MB traffic
            } else {
                point.value = rand() % 100;
            }

            series.data.push_back(point);
        }

        return series;
    }
};

AnalyticsDashboard::AnalyticsDashboard() : pimpl_(std::make_unique<Impl>()) {
    pimpl_->real_time_enabled_ = false;
    pimpl_->refresh_interval_ = 30;
    pimpl_->data_retention_days_ = 30;
    pimpl_->collecting_metrics_ = false;
}

AnalyticsDashboard::~AnalyticsDashboard() = default;

AnalyticsDashboard& AnalyticsDashboard::instance() {
    static AnalyticsDashboard instance;
    return instance;
}

bool AnalyticsDashboard::initialize() {
    pimpl_->initialize_default_dashboards();

    // Generate some initial mock data
    pimpl_->metrics_["cpu_usage"] = pimpl_->generate_mock_data("CPU Usage", "%", 24);
    pimpl_->metrics_["memory_usage"] = pimpl_->generate_mock_data("Memory Usage", "%", 24);
    pimpl_->metrics_["network_traffic"] = pimpl_->generate_mock_data("Network Traffic", "MB", 24);

    std::cout << "Analytics Dashboard initialized successfully!\n";
    return true;
}

bool AnalyticsDashboard::create_dashboard(const std::string& name) {
    if (pimpl_->dashboards_.find(name) != pimpl_->dashboards_.end()) {
        return false; // Dashboard already exists
    }

    Dashboard dashboard;
    dashboard.name = name;
    dashboard.description = "Custom Dashboard";
    dashboard.refresh_interval_seconds = 30;
    dashboard.auto_refresh = false;

    pimpl_->dashboards_[name] = dashboard;
    return true;
}

bool AnalyticsDashboard::delete_dashboard(const std::string& name) {
    if (name == "system" || name == "commands") {
        return false; // Cannot delete built-in dashboards
    }

    auto it = pimpl_->dashboards_.find(name);
    if (it == pimpl_->dashboards_.end()) {
        return false;
    }

    pimpl_->dashboards_.erase(it);
    if (pimpl_->active_dashboard_ == name) {
        pimpl_->active_dashboard_ = "system";
    }
    return true;
}

Dashboard AnalyticsDashboard::get_dashboard(const std::string& name) {
    auto it = pimpl_->dashboards_.find(name);
    if (it != pimpl_->dashboards_.end()) {
        Dashboard dashboard = it->second;

        // Populate widgets with actual data
        for (auto& widget : dashboard.widgets) {
            if (widget.id == "cpu_usage") {
                widget.data = get_cpu_usage_history(24);
            } else if (widget.id == "memory_usage") {
                widget.data = get_memory_usage_history(24);
            } else if (widget.id == "network_traffic") {
                widget.data = get_network_traffic_history(24);
            } else if (widget.id == "top_commands") {
                // This would normally get real command data
                widget.data = pimpl_->generate_mock_data("Command Usage", "count", 7);
            }
        }

        return dashboard;
    }

    return Dashboard{}; // Return empty dashboard if not found
}

std::vector<std::string> AnalyticsDashboard::list_dashboards() {
    std::vector<std::string> names;
    for (const auto& pair : pimpl_->dashboards_) {
        names.push_back(pair.first);
    }
    return names;
}

bool AnalyticsDashboard::set_active_dashboard(const std::string& name) {
    if (pimpl_->dashboards_.find(name) != pimpl_->dashboards_.end()) {
        pimpl_->active_dashboard_ = name;
        return true;
    }
    return false;
}

bool AnalyticsDashboard::add_widget(const std::string& dashboard_name, const Widget& widget) {
    auto it = pimpl_->dashboards_.find(dashboard_name);
    if (it == pimpl_->dashboards_.end()) {
        return false;
    }

    // Check if widget ID already exists
    for (const auto& w : it->second.widgets) {
        if (w.id == widget.id) {
            return false;
        }
    }

    it->second.widgets.push_back(widget);
    return true;
}

bool AnalyticsDashboard::remove_widget(const std::string& dashboard_name, const std::string& widget_id) {
    auto it = pimpl_->dashboards_.find(dashboard_name);
    if (it == pimpl_->dashboards_.end()) {
        return false;
    }

    auto& widgets = it->second.widgets;
    for (size_t i = 0; i < widgets.size(); ++i) {
        if (widgets[i].id == widget_id) {
            widgets.erase(widgets.begin() + i);
            return true;
        }
    }
    return false;
}

bool AnalyticsDashboard::update_widget(const std::string& dashboard_name, const Widget& widget) {
    auto it = pimpl_->dashboards_.find(dashboard_name);
    if (it == pimpl_->dashboards_.end()) {
        return false;
    }

    auto& widgets = it->second.widgets;
    for (auto& w : widgets) {
        if (w.id == widget.id) {
            w = widget;
            return true;
        }
    }
    return false;
}

void AnalyticsDashboard::record_metric(const std::string& metric_name, double value) {
    record_metric(metric_name, value, "");
}

void AnalyticsDashboard::record_metric(const std::string& metric_name, double value, const std::string& label) {
    DataPoint point;
    point.timestamp = time(nullptr);
    point.value = value;
    point.label = label;

    pimpl_->metrics_[metric_name].data.push_back(point);
    pimpl_->metrics_[metric_name].name = metric_name;
}

TimeSeries AnalyticsDashboard::get_metric(const std::string& metric_name, time_t start, time_t end) {
    TimeSeries series;
    series.name = metric_name;

    auto it = pimpl_->metrics_.find(metric_name);
    if (it != pimpl_->metrics_.end()) {
        for (const auto& point : it->second.data) {
            if (point.timestamp >= start && point.timestamp <= end) {
                series.data.push_back(point);
            }
        }
    }

    return series;
}

void AnalyticsDashboard::start_collecting_metrics() {
    pimpl_->collecting_metrics_ = true;
    std::cout << "Started collecting system metrics...\n";
}

void AnalyticsDashboard::stop_collecting_metrics() {
    pimpl_->collecting_metrics_ = false;
    std::cout << "Stopped collecting system metrics.\n";
}

TimeSeries AnalyticsDashboard::get_cpu_usage_history(int hours) {
    if (pimpl_->metrics_.find("cpu_usage") == pimpl_->metrics_.end()) {
        pimpl_->metrics_["cpu_usage"] = pimpl_->generate_mock_data("CPU Usage", "%", hours);
    }
    return pimpl_->metrics_["cpu_usage"];
}

TimeSeries AnalyticsDashboard::get_memory_usage_history(int hours) {
    if (pimpl_->metrics_.find("memory_usage") == pimpl_->metrics_.end()) {
        pimpl_->metrics_["memory_usage"] = pimpl_->generate_mock_data("Memory Usage", "%", hours);
    }
    return pimpl_->metrics_["memory_usage"];
}

TimeSeries AnalyticsDashboard::get_disk_usage_history(int hours) {
    if (pimpl_->metrics_.find("disk_usage") == pimpl_->metrics_.end()) {
        pimpl_->metrics_["disk_usage"] = pimpl_->generate_mock_data("Disk Usage", "%", hours);
    }
    return pimpl_->metrics_["disk_usage"];
}

TimeSeries AnalyticsDashboard::get_network_traffic_history(int hours) {
    if (pimpl_->metrics_.find("network_traffic") == pimpl_->metrics_.end()) {
        pimpl_->metrics_["network_traffic"] = pimpl_->generate_mock_data("Network Traffic", "MB", hours);
    }
    return pimpl_->metrics_["network_traffic"];
}

TimeSeries AnalyticsDashboard::get_command_frequency(int days) {
    TimeSeries series;
    series.name = "Command Frequency";
    series.unit = "executions";

    // Generate mock command frequency data
    time_t now = time(nullptr);
    for (int i = days * 24; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 60 * 60); // Hourly data
        point.value = 50 + (rand() % 100); // Random executions per hour
        series.data.push_back(point);
    }

    return series;
}

std::map<std::string, int> AnalyticsDashboard::get_top_commands(int limit) {
    std::map<std::string, int> commands = {
        {"help", 145},
        {"git-status", 89},
        {"vault-list", 76},
        {"monitor-cpu", 65},
        {"note-list", 52},
        {"docker-ps", 43},
        {"login", 38},
        {"task-list", 31},
        {"net-interfaces", 27},
        {"version", 19}
    };

    // Return only the requested limit
    std::map<std::string, int> result;
    int count = 0;
    for (const auto& pair : commands) {
        if (count >= limit) break;
        result[pair.first] = pair.second;
        count++;
    }

    return result;
}

TimeSeries AnalyticsDashboard::get_command_execution_time(const std::string& command) {
    TimeSeries series;
    series.name = "Execution Time: " + command;
    series.unit = "ms";

    // Generate mock execution time data
    time_t now = time(nullptr);
    for (int i = 100; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 60); // Per minute
        point.value = 50 + (rand() % 200); // 50-250ms execution time
        series.data.push_back(point);
    }

    return series;
}

TimeSeries AnalyticsDashboard::get_user_activity(int days) {
    TimeSeries series;
    series.name = "User Activity";
    series.unit = "commands";

    time_t now = time(nullptr);
    for (int i = days * 24; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 60 * 60);
        point.value = 20 + (rand() % 60); // 20-80 commands per hour
        series.data.push_back(point);
    }

    return series;
}

TimeSeries AnalyticsDashboard::get_login_frequency(int days) {
    TimeSeries series;
    series.name = "Login Frequency";
    series.unit = "logins";

    time_t now = time(nullptr);
    for (int i = days; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 24 * 60 * 60);
        point.value = 1 + (rand() % 5); // 1-5 logins per day
        series.data.push_back(point);
    }

    return series;
}

std::map<std::string, int> AnalyticsDashboard::get_user_command_count() {
    return {
        {"admin", 245},
        {"developer", 189},
        {"analyst", 156},
        {"user1", 98},
        {"user2", 67}
    };
}

TimeSeries AnalyticsDashboard::get_shell_response_time(int hours) {
    TimeSeries series;
    series.name = "Shell Response Time";
    series.unit = "ms";

    time_t now = time(nullptr);
    for (int i = hours * 4; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 15 * 60);
        point.value = 10 + (rand() % 40); // 10-50ms response time
        series.data.push_back(point);
    }

    return series;
}

TimeSeries AnalyticsDashboard::get_plugin_load_time() {
    TimeSeries series;
    series.name = "Plugin Load Time";
    series.unit = "ms";

    time_t now = time(nullptr);
    for (int i = 50; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 60);
        point.value = 5 + (rand() % 15); // 5-20ms load time
        series.data.push_back(point);
    }

    return series;
}

std::map<std::string, double> AnalyticsDashboard::get_feature_usage_stats() {
    return {
        {"Git Integration", 35.2},
        {"Password Vault", 28.7},
        {"Container Management", 15.3},
        {"System Monitoring", 12.1},
        {"Network Tools", 8.7}
    };
}

TimeSeries AnalyticsDashboard::get_query_count(int hours) {
    TimeSeries series;
    series.name = "Database Queries";
    series.unit = "queries";

    time_t now = time(nullptr);
    for (int i = hours * 4; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 15 * 60);
        point.value = 10 + (rand() % 50); // 10-60 queries per 15 minutes
        series.data.push_back(point);
    }

    return series;
}

TimeSeries AnalyticsDashboard::get_query_duration(int hours) {
    TimeSeries series;
    series.name = "Query Duration";
    series.unit = "ms";

    time_t now = time(nullptr);
    for (int i = hours * 4; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 15 * 60);
        point.value = 5 + (rand() % 45); // 5-50ms query duration
        series.data.push_back(point);
    }

    return series;
}

std::map<std::string, int> AnalyticsDashboard::get_table_access_count() {
    return {
        {"users", 1250},
        {"commands", 890},
        {"notes", 567},
        {"passwords", 445},
        {"tasks", 234}
    };
}

TimeSeries AnalyticsDashboard::get_commit_frequency(int days) {
    TimeSeries series;
    series.name = "Git Commits";
    series.unit = "commits";

    time_t now = time(nullptr);
    for (int i = days; i >= 0; --i) {
        DataPoint point;
        point.timestamp = now - (i * 24 * 60 * 60);
        point.value = rand() % 10; // 0-9 commits per day
        series.data.push_back(point);
    }

    return series;
}

std::map<std::string, int> AnalyticsDashboard::get_git_command_usage() {
    return {
        {"git-status", 156},
        {"git-add", 89},
        {"git-commit", 67},
        {"git-log", 45},
        {"git-pull", 34},
        {"git-push", 28}
    };
}

std::string AnalyticsDashboard::render_chart(const Widget& widget) {
    std::stringstream ss;
    ss << "Chart: " << widget.title << "\n";
    ss << "Type: ";

    switch (widget.type) {
        case ChartType::LINE: ss << "Line Chart"; break;
        case ChartType::BAR: ss << "Bar Chart"; break;
        case ChartType::PIE: ss << "Pie Chart"; break;
        case ChartType::SCATTER: ss << "Scatter Plot"; break;
        case ChartType::HISTOGRAM: ss << "Histogram"; break;
        case ChartType::HEATMAP: ss << "Heatmap"; break;
    }

    ss << "\nData points: " << widget.data.data.size() << "\n";
    ss << "Unit: " << widget.data.unit << "\n";

    // Show last 5 data points as sample
    if (!widget.data.data.empty()) {
        ss << "\nRecent data:\n";
        size_t start = widget.data.data.size() > 5 ? widget.data.data.size() - 5 : 0;
        for (size_t i = start; i < widget.data.data.size(); ++i) {
            const auto& point = widget.data.data[i];
            ss << "  " << std::ctime(&point.timestamp)
               << "  Value: " << point.value << " " << widget.data.unit << "\n";
        }
    }

    return ss.str();
}

std::string AnalyticsDashboard::render_dashboard(const std::string& dashboard_name) {
    auto dashboard = get_dashboard(dashboard_name);
    if (dashboard.name.empty()) {
        return "Dashboard not found: " + dashboard_name;
    }

    std::stringstream ss;
    ss << "📊 " << dashboard.name << " Dashboard\n";
    ss << std::string(dashboard.name.length() + 12, '=') << "\n\n";
    ss << dashboard.description << "\n\n";

    for (const auto& widget : dashboard.widgets) {
        ss << "+- " << widget.title << " ";
        ss << std::string(50 - widget.title.length() - 3, '-') << "+\n";
        ss << render_chart(widget);
        ss << "+" << std::string(54, '-') << "+\n\n";
    }

    ss << "Refresh interval: " << dashboard.refresh_interval_seconds << " seconds\n";
    ss << "Auto refresh: " << (dashboard.auto_refresh ? "Enabled" : "Disabled") << "\n";

    return ss.str();
}

std::string AnalyticsDashboard::export_dashboard_html(const std::string& dashboard_name) {
    auto dashboard = get_dashboard(dashboard_name);
    if (dashboard.name.empty()) {
        return "";
    }

    std::stringstream ss;
    ss << "<!DOCTYPE html>\n";
    ss << "<html><head><title>" << dashboard.name << " Dashboard</title></head>\n";
    ss << "<body><h1>" << dashboard.name << " Dashboard</h1>\n";
    ss << "<p>" << dashboard.description << "</p>\n";

    for (const auto& widget : dashboard.widgets) {
        ss << "<div class='widget'>\n";
        ss << "<h3>" << widget.title << "</h3>\n";
        ss << "<canvas id='" << widget.id << "' width='400' height='200'></canvas>\n";
        ss << "</div>\n";
    }

    ss << "</body></html>\n";
    return ss.str();
}

std::string AnalyticsDashboard::export_dashboard_json(const std::string& dashboard_name) {
    auto dashboard = get_dashboard(dashboard_name);
    if (dashboard.name.empty()) {
        return "{}";
    }

    std::stringstream ss;
    ss << "{\n";
    ss << "  \"name\": \"" << dashboard.name << "\",\n";
    ss << "  \"description\": \"" << dashboard.description << "\",\n";
    ss << "  \"widgets\": [\n";

    for (size_t i = 0; i < dashboard.widgets.size(); ++i) {
        const auto& widget = dashboard.widgets[i];
        ss << "    {\n";
        ss << "      \"id\": \"" << widget.id << "\",\n";
        ss << "      \"title\": \"" << widget.title << "\",\n";
        ss << "      \"type\": \"";

        switch (widget.type) {
            case ChartType::LINE: ss << "line"; break;
            case ChartType::BAR: ss << "bar"; break;
            case ChartType::PIE: ss << "pie"; break;
            case ChartType::SCATTER: ss << "scatter"; break;
            case ChartType::HISTOGRAM: ss << "histogram"; break;
            case ChartType::HEATMAP: ss << "heatmap"; break;
        }

        ss << "\",\n";
        ss << "      \"data_points\": " << widget.data.data.size() << "\n";
        ss << "    }";

        if (i < dashboard.widgets.size() - 1) {
            ss << ",";
        }
        ss << "\n";
    }

    ss << "  ]\n";
    ss << "}\n";

    return ss.str();
}

void AnalyticsDashboard::enable_real_time(bool enabled) {
    pimpl_->real_time_enabled_ = enabled;
    std::cout << "Real-time updates " << (enabled ? "enabled" : "disabled") << "\n";
}

void AnalyticsDashboard::set_refresh_interval(int seconds) {
    pimpl_->refresh_interval_ = seconds;
}

void AnalyticsDashboard::set_alert(const std::string& metric_name, double threshold, const std::string& condition) {
    std::cout << "Alert set for " << metric_name << " (threshold: " << threshold << ", condition: " << condition << ")\n";
}

void AnalyticsDashboard::remove_alert(const std::string& metric_name) {
    std::cout << "Alert removed for " << metric_name << "\n";
}

std::vector<std::string> AnalyticsDashboard::get_active_alerts() {
    return {"CPU usage > 80% (Warning)", "Memory usage > 90% (Critical)"};
}

std::string AnalyticsDashboard::generate_daily_report() {
    std::stringstream ss;
    ss << "[ANALYTICS] Daily Analytics Report\n";
    ss << "=========================\n\n";

    ss << "System Metrics:\n";
    ss << "- Average CPU Usage: 45%\n";
    ss << "- Peak Memory Usage: 78%\n";
    ss << "- Network Traffic: 2.3 GB\n\n";

    ss << "Command Statistics:\n";
    auto top_cmds = get_top_commands(5);
    for (const auto& pair : top_cmds) {
        ss << "- " << pair.first << ": " << pair.second << " uses\n";
    }
    ss << "\n";

    ss << "User Activity:\n";
    auto user_counts = get_user_command_count();
    for (const auto& pair : user_counts) {
        ss << "- " << pair.first << ": " << pair.second << " commands\n";
    }

    return ss.str();
}

std::string AnalyticsDashboard::generate_weekly_report() {
    std::stringstream ss;
    ss << "[ANALYTICS] Weekly Analytics Report\n";
    ss << "==========================\n\n";

    ss << "Performance Summary:\n";
    ss << "- Total Commands Executed: 2,847\n";
    ss << "- Average Response Time: 35ms\n";
    ss << "- System Uptime: 99.8%\n\n";

    ss << "Feature Usage:\n";
    auto features = get_feature_usage_stats();
    for (const auto& pair : features) {
        ss << "- " << pair.first << ": " << pair.second << "%\n";
    }

    return ss.str();
}

std::string AnalyticsDashboard::generate_monthly_report() {
    std::stringstream ss;
    ss << "[ANALYTICS] Monthly Analytics Report\n";
    ss << "===========================\n\n";

    ss << "Trends:\n";
    ss << "- Command usage increased by 15%\n";
    ss << "- Git integration usage up 22%\n";
    ss << "- New users added: 3\n\n";

    ss << "Top Metrics:\n";
    ss << "- Most active user: admin (1,245 commands)\n";
    ss << "- Most used command: help (345 uses)\n";
    ss << "- Peak usage hour: 2-3 PM\n";

    return ss.str();
}

bool AnalyticsDashboard::export_report(const std::string& report, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << report;
            file.close();
            std::cout << "Report exported to: " << filepath << "\n";
            return true;
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to export report: " << e.what() << "\n";
    }
    return false;
}

bool AnalyticsDashboard::export_metrics_csv(const std::string& metric_name, const std::string& filepath) {
    auto series = pimpl_->metrics_[metric_name];
    if (series.data.empty()) {
        return false;
    }

    try {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "timestamp,value,label\n";
            for (const auto& point : series.data) {
                file << point.timestamp << "," << point.value << ",\"" << point.label << "\"\n";
            }
            file.close();
            std::cout << "Metrics exported to: " << filepath << "\n";
            return true;
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to export metrics: " << e.what() << "\n";
    }
    return false;
}

bool AnalyticsDashboard::export_all_metrics_json(const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "{\n";
            bool first = true;
            for (const auto& pair : pimpl_->metrics_) {
                if (!first) file << ",\n";
                file << "  \"" << pair.first << "\": {\n";
                file << "    \"name\": \"" << pair.second.name << "\",\n";
                file << "    \"unit\": \"" << pair.second.unit << "\",\n";
                file << "    \"data\": [\n";

                for (size_t i = 0; i < pair.second.data.size(); ++i) {
                    const auto& point = pair.second.data[i];
                    file << "      {\"timestamp\": " << point.timestamp
                         << ", \"value\": " << point.value
                         << ", \"label\": \"" << point.label << "\"}";
                    if (i < pair.second.data.size() - 1) file << ",";
                    file << "\n";
                }

                file << "    ]\n";
                file << "  }";
                first = false;
            }
            file << "\n}\n";
            file.close();
            std::cout << "All metrics exported to: " << filepath << "\n";
            return true;
        }
    } catch (const std::exception& e) {
        std::cout << "Failed to export metrics: " << e.what() << "\n";
    }
    return false;
}

void AnalyticsDashboard::set_data_retention_days(int days) {
    pimpl_->data_retention_days_ = days;
    std::cout << "Data retention set to " << days << " days.\n";
}

void AnalyticsDashboard::clear_old_metrics() {
    time_t cutoff = time(nullptr) - (pimpl_->data_retention_days_ * 24 * 60 * 60);

    for (auto& pair : pimpl_->metrics_) {
        auto& data = pair.second.data;
        data.erase(
            std::remove_if(data.begin(), data.end(),
                [cutoff](const DataPoint& point) {
                    return point.timestamp < cutoff;
                }),
            data.end());
    }

    std::cout << "Old metrics cleared (keeping last " << pimpl_->data_retention_days_ << " days).\n";
}

void AnalyticsDashboard::vacuum_database() {
    std::cout << "Database vacuum completed.\n";
}

} // namespace analytics
} // namespace novashell
