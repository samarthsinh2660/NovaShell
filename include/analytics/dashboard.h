#ifndef NOVASHELL_ANALYTICS_DASHBOARD_H
#define NOVASHELL_ANALYTICS_DASHBOARD_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace novashell {
namespace analytics {

// Metric data point
struct DataPoint {
    time_t timestamp;
    double value;
    std::string label;
};

// Time series data
struct TimeSeries {
    std::string name;
    std::vector<DataPoint> data;
    std::string unit;
};

// Chart types
enum class ChartType {
    LINE,
    BAR,
    PIE,
    SCATTER,
    HISTOGRAM,
    HEATMAP
};

// Dashboard widget
struct Widget {
    std::string id;
    std::string title;
    ChartType type;
    TimeSeries data;
    int width;
    int height;
    std::map<std::string, std::string> options;
};

// Dashboard layout
struct Dashboard {
    std::string name;
    std::string description;
    std::vector<Widget> widgets;
    int refresh_interval_seconds;
    bool auto_refresh;
};

// Advanced Analytics Dashboard
class AnalyticsDashboard {
public:
    static AnalyticsDashboard& instance();

    // Initialize analytics
    bool initialize();

    // Dashboard management
    bool create_dashboard(const std::string& name);
    bool delete_dashboard(const std::string& name);
    Dashboard get_dashboard(const std::string& name);
    std::vector<std::string> list_dashboards();
    bool set_active_dashboard(const std::string& name);

    // Widget management
    bool add_widget(const std::string& dashboard_name, const Widget& widget);
    bool remove_widget(const std::string& dashboard_name, const std::string& widget_id);
    bool update_widget(const std::string& dashboard_name, const Widget& widget);

    // Metrics collection
    void record_metric(const std::string& metric_name, double value);
    void record_metric(const std::string& metric_name, double value, const std::string& label);
    TimeSeries get_metric(const std::string& metric_name, time_t start, time_t end);
    
    // Built-in metrics
    void start_collecting_metrics();
    void stop_collecting_metrics();
    
    // System metrics
    TimeSeries get_cpu_usage_history(int hours = 24);
    TimeSeries get_memory_usage_history(int hours = 24);
    TimeSeries get_disk_usage_history(int hours = 24);
    TimeSeries get_network_traffic_history(int hours = 24);
    
    // Command metrics
    TimeSeries get_command_frequency(int days = 7);
    std::map<std::string, int> get_top_commands(int limit = 10);
    TimeSeries get_command_execution_time(const std::string& command);
    
    // User activity
    TimeSeries get_user_activity(int days = 7);
    TimeSeries get_login_frequency(int days = 30);
    std::map<std::string, int> get_user_command_count();
    
    // Performance metrics
    TimeSeries get_shell_response_time(int hours = 24);
    TimeSeries get_plugin_load_time();
    std::map<std::string, double> get_feature_usage_stats();
    
    // Database metrics (if DB module is used)
    TimeSeries get_query_count(int hours = 24);
    TimeSeries get_query_duration(int hours = 24);
    std::map<std::string, int> get_table_access_count();
    
    // Git metrics (if Git module is used)
    TimeSeries get_commit_frequency(int days = 30);
    std::map<std::string, int> get_git_command_usage();
    
    // Visualization
    std::string render_chart(const Widget& widget);
    std::string render_dashboard(const std::string& dashboard_name);
    std::string export_dashboard_html(const std::string& dashboard_name);
    std::string export_dashboard_json(const std::string& dashboard_name);
    
    // Real-time updates
    void enable_real_time(bool enabled);
    void set_refresh_interval(int seconds);
    
    // Alerts and thresholds
    void set_alert(const std::string& metric_name, double threshold, const std::string& condition);
    void remove_alert(const std::string& metric_name);
    std::vector<std::string> get_active_alerts();
    
    // Reports
    std::string generate_daily_report();
    std::string generate_weekly_report();
    std::string generate_monthly_report();
    bool export_report(const std::string& report, const std::string& filepath);
    
    // Data export
    bool export_metrics_csv(const std::string& metric_name, const std::string& filepath);
    bool export_all_metrics_json(const std::string& filepath);
    
    // Configuration
    void set_data_retention_days(int days);
    void clear_old_metrics();
    void vacuum_database();

private:
    AnalyticsDashboard();
    ~AnalyticsDashboard();
    AnalyticsDashboard(const AnalyticsDashboard&) = delete;
    AnalyticsDashboard& operator=(const AnalyticsDashboard&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace analytics
} // namespace novashell

#endif // NOVASHELL_ANALYTICS_DASHBOARD_H
