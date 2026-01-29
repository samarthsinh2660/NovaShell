#ifndef CUSTOMOS_PERFORMANCE_ANALYTICS_H
#define CUSTOMOS_PERFORMANCE_ANALYTICS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>

namespace customos {
namespace analytics {

struct UsagePattern {
    std::string pattern_type;  // "command_sequence", "time_based", "context_based"
    std::string pattern_name;
    double confidence;  // 0.0 to 1.0
    int frequency;
    std::chrono::system_clock::time_point last_seen;
    std::unordered_map<std::string, std::string> metadata;
};

struct ProductivityInsight {
    std::string category;  // "efficiency", "learning", "optimization", "security"
    std::string title;
    std::string description;
    std::string recommendation;
    double impact_score;  // -1.0 to 1.0 (negative = problem, positive = good)
    std::vector<std::string> related_patterns;
    std::chrono::system_clock::time_point generated_at;
};

struct PerformanceMetric {
    std::string metric_name;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> tags;
};

class PerformanceAnalytics {
public:
    PerformanceAnalytics();
    ~PerformanceAnalytics();

    // Data collection
    void record_command_execution(const std::string& command, double execution_time_ms,
                                bool success, const std::string& user = "");
    void record_ai_interaction(const std::string& interaction_type, double response_time_ms,
                             const std::string& user = "");
    void record_error(const std::string& error_type, const std::string& context,
                    const std::string& user = "");

    // Pattern analysis
    std::vector<UsagePattern> analyze_usage_patterns(const std::string& user = "");
    std::vector<UsagePattern> get_time_based_patterns();
    std::vector<UsagePattern> get_workflow_patterns(const std::string& user = "");

    // AI-powered insights
    std::vector<ProductivityInsight> generate_insights(const std::string& user = "");
    ProductivityInsight generate_personalized_insight(const std::string& user);

    // Performance metrics
    void record_metric(const std::string& name, double value, const std::string& unit = "",
                      const std::unordered_map<std::string, std::string>& tags = {});
    std::vector<PerformanceMetric> get_metrics(const std::string& metric_name = "",
                                             std::chrono::hours lookback = std::chrono::hours(24));

    // Productivity scoring
    double calculate_productivity_score(const std::string& user = "");
    std::unordered_map<std::string, double> get_efficiency_metrics(const std::string& user = "");

    // Learning recommendations
    std::vector<std::string> generate_learning_recommendations(const std::string& user);
    std::vector<std::string> suggest_productivity_improvements(const std::string& user);

    // Export/Import
    std::string export_analytics_data(const std::string& user = "");
    bool import_analytics_data(const std::string& json_data);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

    // Analysis methods
    std::vector<UsagePattern> detect_command_sequences(const std::string& user);
    std::vector<UsagePattern> detect_time_patterns(const std::string& user);
    std::vector<UsagePattern> detect_context_patterns(const std::string& user);

    // Insight generation
    ProductivityInsight analyze_command_efficiency(const std::string& user);
    ProductivityInsight analyze_learning_progress(const std::string& user);
    ProductivityInsight analyze_security_posture(const std::string& user);
    ProductivityInsight analyze_resource_usage(const std::string& user);

    // Utility methods
    double calculate_average_execution_time(const std::string& command);
    int count_unique_commands_used(const std::string& user);
    double calculate_ai_adoption_rate(const std::string& user);
    std::vector<std::string> get_most_used_commands(const std::string& user, int limit = 10);
    std::vector<std::string> get_least_efficient_workflows(const std::string& user);
};

class AnalyticsDashboard {
public:
    AnalyticsDashboard();
    ~AnalyticsDashboard();

    // Dashboard generation
    std::string generate_html_dashboard(const std::string& user = "");
    std::string generate_json_dashboard(const std::string& user = "");

    // Real-time updates
    void enable_real_time_updates(bool enable);
    std::string get_real_time_metrics();

    // Visualization data
    std::string get_usage_timeline_data(int days = 7);
    std::string get_command_frequency_data();
    std::string get_productivity_trends_data();

    // Custom dashboards
    bool create_custom_dashboard(const std::string& name, const std::vector<std::string>& metrics);
    std::string render_custom_dashboard(const std::string& name);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

    std::string generate_css_styles();
    std::string generate_javascript_charts();
    std::string create_chart_html(const std::string& chart_id, const std::string& title,
                                const std::string& data, const std::string& chart_type);
};

} // namespace analytics
} // namespace customos

#endif // CUSTOMOS_PERFORMANCE_ANALYTICS_H
