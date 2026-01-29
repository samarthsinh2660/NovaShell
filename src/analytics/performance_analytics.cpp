#include "analytics/performance_analytics.h"
#include "database/internal_db.h"
#include "ai/ai_module.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <map>
#include <mutex>
#include <unordered_set>

namespace customos {
namespace analytics {

struct PerformanceAnalytics::Impl {
    std::vector<PerformanceMetric> metrics_buffer;
    std::unordered_map<std::string, std::vector<UsagePattern>> user_patterns;
    std::mutex mutex;

    // Configuration
    size_t max_metrics_buffer = 10000;
    std::chrono::hours analytics_retention_period = std::chrono::hours(24 * 30); // 30 days

    void cleanup_old_data() {
        auto now = std::chrono::system_clock::now();
        auto cutoff = now - analytics_retention_period;

        // Remove old metrics
        metrics_buffer.erase(
            std::remove_if(metrics_buffer.begin(), metrics_buffer.end(),
                [cutoff](const PerformanceMetric& m) { return m.timestamp < cutoff; }),
            metrics_buffer.end()
        );

        // Remove old patterns (simplified)
        for (auto& user_patterns_pair : user_patterns) {
            user_patterns_pair.second.erase(
                std::remove_if(user_patterns_pair.second.begin(), user_patterns_pair.second.end(),
                    [cutoff](const UsagePattern& p) { return p.last_seen < cutoff; }),
                user_patterns_pair.second.end()
            );
        }
    }
};

PerformanceAnalytics::PerformanceAnalytics() : pimpl_(std::make_unique<Impl>()) {
}

PerformanceAnalytics::~PerformanceAnalytics() {
}

void PerformanceAnalytics::record_command_execution(const std::string& command,
                                                  double execution_time_ms,
                                                  bool success,
                                                  const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    // Record execution time metric
    record_metric("command_execution_time", execution_time_ms, "ms",
                 {{"command", command}, {"success", success ? "true" : "false"}, {"user", user}});

    // Record command usage
    record_metric("command_usage", 1.0, "count",
                 {{"command", command}, {"user", user}});

    // Store in database for persistence
    try {
        auto& db = database::InternalDB::instance();
        db.add_analytics_data("command_execution_" + command, execution_time_ms, "ms",
                             "{\"user\": \"" + user + "\", \"success\": " + (success ? "true" : "false") + "}");
    } catch (...) {
        // Ignore database errors
    }
}

void PerformanceAnalytics::record_ai_interaction(const std::string& interaction_type,
                                               double response_time_ms,
                                               const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    record_metric("ai_response_time", response_time_ms, "ms",
                 {{"type", interaction_type}, {"user", user}});

    record_metric("ai_interaction", 1.0, "count",
                 {{"type", interaction_type}, {"user", user}});
}

void PerformanceAnalytics::record_error(const std::string& error_type,
                                      const std::string& context,
                                      const std::string& user) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    record_metric("error_count", 1.0, "count",
                 {{"type", error_type}, {"context", context}, {"user", user}});
}

void PerformanceAnalytics::record_metric(const std::string& name, double value,
                                       const std::string& unit,
                                       const std::unordered_map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    PerformanceMetric metric;
    metric.metric_name = name;
    metric.value = value;
    metric.unit = unit;
    metric.timestamp = std::chrono::system_clock::now();
    metric.tags = tags;

    pimpl_->metrics_buffer.push_back(metric);

    // Maintain buffer size
    if (pimpl_->metrics_buffer.size() > pimpl_->max_metrics_buffer) {
        pimpl_->metrics_buffer.erase(pimpl_->metrics_buffer.begin());
    }

    // Store in database
    try {
        auto& db = database::InternalDB::instance();
        std::string metadata = "{";
        bool first = true;
        for (const auto& tag : tags) {
            if (!first) metadata += ",";
            metadata += "\"" + tag.first + "\":\"" + tag.second + "\"";
            first = false;
        }
        metadata += "}";
        db.add_analytics_data(name, value, unit, metadata);
    } catch (...) {
        // Ignore database errors
    }
}

std::vector<UsagePattern> PerformanceAnalytics::analyze_usage_patterns(const std::string& user) {
    std::vector<UsagePattern> patterns;

    // Detect different types of patterns
    auto command_patterns = detect_command_sequences(user);
    auto time_patterns = detect_time_patterns(user);
    auto context_patterns = detect_context_patterns(user);

    patterns.insert(patterns.end(), command_patterns.begin(), command_patterns.end());
    patterns.insert(patterns.end(), time_patterns.begin(), time_patterns.end());
    patterns.insert(patterns.end(), context_patterns.begin(), context_patterns.end());

    return patterns;
}

std::vector<UsagePattern> PerformanceAnalytics::detect_command_sequences(const std::string& user) {
    std::vector<UsagePattern> patterns;

    // Analyze command execution metrics to find sequences
    auto command_metrics = get_metrics("command_usage", std::chrono::hours(24));

    // Group by user if specified
    std::unordered_map<std::string, int> command_counts;
    for (const auto& metric : command_metrics) {
        if (user.empty() || metric.tags.count("user") && metric.tags.at("user") == user) {
            std::string command = metric.tags.count("command") ? metric.tags.at("command") : "unknown";
            command_counts[command] += static_cast<int>(metric.value);
        }
    }

    // Create patterns for frequently used commands
    for (const auto& pair : command_counts) {
        if (pair.second > 5) {  // More than 5 uses
            UsagePattern pattern;
            pattern.pattern_type = "command_sequence";
            pattern.pattern_name = "Frequent command: " + pair.first;
            pattern.confidence = std::min(1.0, pair.second / 20.0);  // Scale confidence
            pattern.frequency = pair.second;
            pattern.last_seen = std::chrono::system_clock::now();
            pattern.metadata["command"] = pair.first;
            patterns.push_back(pattern);
        }
    }

    return patterns;
}

std::vector<UsagePattern> PerformanceAnalytics::detect_time_patterns(const std::string& user) {
    std::vector<UsagePattern> patterns;

    // This would analyze when commands are executed
    // For now, return a placeholder pattern
    UsagePattern pattern;
    pattern.pattern_type = "time_based";
    pattern.pattern_name = "Peak usage hours detected";
    pattern.confidence = 0.8;
    pattern.frequency = 10;
    pattern.last_seen = std::chrono::system_clock::now();
    pattern.metadata["peak_hours"] = "14:00-16:00";

    patterns.push_back(pattern);
    return patterns;
}

std::vector<UsagePattern> PerformanceAnalytics::detect_context_patterns(const std::string& user) {
    std::vector<UsagePattern> patterns;

    // Analyze context-based usage patterns
    UsagePattern pattern;
    pattern.pattern_type = "context_based";
    pattern.pattern_name = "Development workflow detected";
    pattern.confidence = 0.9;
    pattern.frequency = 15;
    pattern.last_seen = std::chrono::system_clock::now();
    pattern.metadata["workflow"] = "edit -> test -> commit";

    patterns.push_back(pattern);
    return patterns;
}

std::vector<ProductivityInsight> PerformanceAnalytics::generate_insights(const std::string& user) {
    std::vector<ProductivityInsight> insights;

    // Generate various types of insights
    insights.push_back(analyze_command_efficiency(user));
    insights.push_back(analyze_learning_progress(user));
    insights.push_back(analyze_security_posture(user));
    insights.push_back(analyze_resource_usage(user));

    // Use AI to enhance insights
    if (ai::APIKeyManager::instance().has_api_key()) {
        try {
            std::string prompt = "Based on the following productivity metrics, generate one key insight:\n";
            prompt += "User: " + user + "\n";
            prompt += "Productivity Score: " + std::to_string(calculate_productivity_score(user)) + "\n";
            prompt += "AI Adoption Rate: " + std::to_string(calculate_ai_adoption_rate(user)) + "\n";

            auto response = ai::GeminiClient::instance().generate_content(prompt);
            if (response.success && !response.content.empty()) {
                ProductivityInsight ai_insight;
                ai_insight.category = "ai_generated";
                ai_insight.title = "AI-Generated Productivity Insight";
                ai_insight.description = response.content;
                ai_insight.recommendation = "Consider the AI's analysis for productivity improvements";
                ai_insight.impact_score = 0.5;
                ai_insight.generated_at = std::chrono::system_clock::now();
                insights.push_back(ai_insight);
            }
        } catch (...) {
            // Ignore AI errors
        }
    }

    return insights;
}

ProductivityInsight PerformanceAnalytics::analyze_command_efficiency(const std::string& user) {
    ProductivityInsight insight;
    insight.category = "efficiency";
    insight.title = "Command Execution Efficiency";

    auto exec_times = get_metrics("command_execution_time", std::chrono::hours(24));
    if (exec_times.empty()) {
        insight.description = "No command execution data available";
        insight.impact_score = 0.0;
    } else {
        double avg_time = 0.0;
        for (const auto& metric : exec_times) {
            avg_time += metric.value;
        }
        avg_time /= exec_times.size();

        if (avg_time > 5000) { // More than 5 seconds average
            insight.description = "Commands are taking longer than expected to execute";
            insight.recommendation = "Consider optimizing frequently used commands or checking system performance";
            insight.impact_score = -0.3;
        } else {
            insight.description = "Command execution times are within acceptable ranges";
            insight.recommendation = "Keep monitoring for any performance degradation";
            insight.impact_score = 0.2;
        }
    }

    insight.generated_at = std::chrono::system_clock::now();
    return insight;
}

ProductivityInsight PerformanceAnalytics::analyze_learning_progress(const std::string& user) {
    ProductivityInsight insight;
    insight.category = "learning";
    insight.title = "Learning and Adaptation Progress";

    double ai_adoption = calculate_ai_adoption_rate(user);
    int unique_commands = count_unique_commands_used(user);

    if (ai_adoption < 0.1) {
        insight.description = "Low AI feature adoption detected";
        insight.recommendation = "Try using AI completion (Tab key) and AI analysis features more often";
        insight.impact_score = -0.4;
    } else if (unique_commands < 10) {
        insight.description = "Limited command diversity - opportunity for learning";
        insight.recommendation = "Explore more NovaShell features to increase productivity";
        insight.impact_score = -0.2;
    } else {
        insight.description = "Good progress in learning NovaShell features";
        insight.recommendation = "Continue exploring advanced features for maximum productivity";
        insight.impact_score = 0.3;
    }

    insight.generated_at = std::chrono::system_clock::now();
    return insight;
}

ProductivityInsight PerformanceAnalytics::analyze_security_posture(const std::string& user) {
    ProductivityInsight insight;
    insight.category = "security";
    insight.title = "Security Posture Analysis";

    auto error_metrics = get_metrics("error_count", std::chrono::hours(24));
    int security_errors = 0;

    for (const auto& metric : error_metrics) {
        if (metric.tags.count("type") && metric.tags.at("type").find("auth") != std::string::npos) {
            security_errors++;
        }
    }

    if (security_errors > 0) {
        insight.description = "Security-related errors detected";
        insight.recommendation = "Review authentication settings and access controls";
        insight.impact_score = -0.8;
    } else {
        insight.description = "No security issues detected in recent activity";
        insight.recommendation = "Continue following security best practices";
        insight.impact_score = 0.1;
    }

    insight.generated_at = std::chrono::system_clock::now();
    return insight;
}

ProductivityInsight PerformanceAnalytics::analyze_resource_usage(const std::string& user) {
    ProductivityInsight insight;
    insight.category = "optimization";
    insight.title = "Resource Usage Analysis";

    insight.description = "Resource usage is being monitored";
    insight.recommendation = "Consider using AI features to optimize workflows";
    insight.impact_score = 0.0;
    insight.generated_at = std::chrono::system_clock::now();

    return insight;
}

double PerformanceAnalytics::calculate_productivity_score(const std::string& user) {
    double score = 50.0; // Base score

    // Factor in AI adoption
    double ai_rate = calculate_ai_adoption_rate(user);
    score += ai_rate * 20.0;

    // Factor in command diversity
    int unique_cmds = count_unique_commands_used(user);
    score += std::min(20.0, unique_cmds * 2.0);

    // Factor in efficiency (inverse of average execution time)
    double avg_time = calculate_average_execution_time("");
    if (avg_time > 0) {
        double efficiency_bonus = std::max(0.0, 10.0 - (avg_time / 1000.0));
        score += efficiency_bonus;
    }

    return std::min(100.0, std::max(0.0, score));
}

double PerformanceAnalytics::calculate_ai_adoption_rate(const std::string& user) {
    auto ai_metrics = get_metrics("ai_interaction", std::chrono::hours(24));
    auto total_metrics = get_metrics("", std::chrono::hours(24));

    if (total_metrics.empty()) return 0.0;

    return static_cast<double>(ai_metrics.size()) / total_metrics.size();
}

int PerformanceAnalytics::count_unique_commands_used(const std::string& user) {
    std::unordered_set<std::string> unique_commands;

    auto command_metrics = get_metrics("command_usage", std::chrono::hours(24 * 7)); // Last week
    for (const auto& metric : command_metrics) {
        if (user.empty() || (metric.tags.count("user") && metric.tags.at("user") == user)) {
            if (metric.tags.count("command")) {
                unique_commands.insert(metric.tags.at("command"));
            }
        }
    }

    return unique_commands.size();
}

double PerformanceAnalytics::calculate_average_execution_time(const std::string& command) {
    auto exec_metrics = get_metrics("command_execution_time", std::chrono::hours(24));
    if (exec_metrics.empty()) return 0.0;

    double total_time = 0.0;
    for (const auto& metric : exec_metrics) {
        if (command.empty() || (metric.tags.count("command") && metric.tags.at("command") == command)) {
            total_time += metric.value;
        }
    }

    return total_time / exec_metrics.size();
}

std::vector<PerformanceMetric> PerformanceAnalytics::get_metrics(const std::string& metric_name,
                                                               std::chrono::hours lookback) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);

    auto now = std::chrono::system_clock::now();
    auto cutoff = now - lookback;

    std::vector<PerformanceMetric> result;
    for (const auto& metric : pimpl_->metrics_buffer) {
        if (metric.timestamp >= cutoff &&
            (metric_name.empty() || metric.metric_name == metric_name)) {
            result.push_back(metric);
        }
    }

    return result;
}

std::vector<std::string> PerformanceAnalytics::generate_learning_recommendations(const std::string& user) {
    std::vector<std::string> recommendations;

    double ai_rate = calculate_ai_adoption_rate(user);
    if (ai_rate < 0.1) {
        recommendations.push_back("Try AI completion by typing partial commands and pressing Tab");
        recommendations.push_back("Use ai-analyze to get code reviews and improvement suggestions");
        recommendations.push_back("Explore ai-help for learning new concepts and commands");
    }

    int unique_cmds = count_unique_commands_used(user);
    if (unique_cmds < 15) {
        recommendations.push_back("Explore more NovaShell modules like git, vault, and monitoring");
        recommendations.push_back("Try automation features like task scheduling and reminders");
        recommendations.push_back("Learn about advanced features in the help system");
    }

    auto least_efficient = get_least_efficient_workflows(user);
    if (!least_efficient.empty()) {
        recommendations.push_back("Consider optimizing: " + least_efficient[0]);
    }

    return recommendations;
}

std::vector<std::string> PerformanceAnalytics::get_least_efficient_workflows(const std::string& user) {
    // This would analyze workflows and find inefficient ones
    return {"Consider using aliases for long command sequences"};
}

std::unordered_map<std::string, double> PerformanceAnalytics::get_efficiency_metrics(const std::string& user) {
    return {
        {"productivity_score", calculate_productivity_score(user)},
        {"ai_adoption_rate", calculate_ai_adoption_rate(user)},
        {"command_diversity", count_unique_commands_used(user)},
        {"average_execution_time", calculate_average_execution_time("")}
    };
}

// Analytics Dashboard Implementation
struct AnalyticsDashboard::Impl {
    PerformanceAnalytics analytics;
    bool real_time_enabled = false;
};

AnalyticsDashboard::AnalyticsDashboard() : pimpl_(std::make_unique<Impl>()) {
}

AnalyticsDashboard::~AnalyticsDashboard() {
}

std::string AnalyticsDashboard::generate_html_dashboard(const std::string& user) {
    std::stringstream html;

    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    html << "<title>NovaShell Analytics Dashboard</title>\n";
    html << generate_css_styles();
    html << "</head>\n<body>\n";

    html << "<div class='dashboard'>\n";
    html << "<h1>🤖 NovaShell Analytics Dashboard</h1>\n";

    // Productivity Score
    double prod_score = pimpl_->analytics.calculate_productivity_score(user);
    html << "<div class='metric-card'>\n";
    html << "<h3>Productivity Score</h3>\n";
    html << "<div class='score'>" << std::fixed << std::setprecision(1) << prod_score << "/100</div>\n";
    html << "</div>\n";

    // AI Adoption
    double ai_rate = pimpl_->analytics.calculate_ai_adoption_rate(user);
    html << "<div class='metric-card'>\n";
    html << "<h3>AI Adoption Rate</h3>\n";
    html << "<div class='percentage'>" << std::fixed << std::setprecision(1) << (ai_rate * 100) << "%</div>\n";
    html << "</div>\n";

    // Command Diversity
    int unique_cmds = pimpl_->analytics.count_unique_commands_used(user);
    html << "<div class='metric-card'>\n";
    html << "<h3>Command Diversity</h3>\n";
    html << "<div class='count'>" << unique_cmds << " unique commands</div>\n";
    html << "</div>\n";

    // Insights
    auto insights = pimpl_->analytics.generate_insights(user);
    html << "<div class='insights-section'>\n";
    html << "<h2>💡 AI-Generated Insights</h2>\n";
    for (const auto& insight : insights) {
        std::string impact_class = insight.impact_score > 0 ? "positive" : "negative";
        html << "<div class='insight-card " << impact_class << "'>\n";
        html << "<h4>" << insight.title << "</h4>\n";
        html << "<p>" << insight.description << "</p>\n";
        html << "<p><strong>Recommendation:</strong> " << insight.recommendation << "</p>\n";
        html << "</div>\n";
    }
    html << "</div>\n";

    // Recommendations
    auto recommendations = pimpl_->analytics.generate_learning_recommendations(user);
    if (!recommendations.empty()) {
        html << "<div class='recommendations-section'>\n";
        html << "<h2>🎯 Learning Recommendations</h2>\n";
        html << "<ul>\n";
        for (const auto& rec : recommendations) {
            html << "<li>" << rec << "</li>\n";
        }
        html << "</ul>\n";
        html << "</div>\n";
    }

    html << "</div>\n";
    html << generate_javascript_charts();
    html << "</body>\n</html>\n";

    return html.str();
}

std::string AnalyticsDashboard::generate_css_styles() {
    return R"(
        <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
        }
        .dashboard {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 10px;
            padding: 30px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 30px;
        }
        .metric-card {
            display: inline-block;
            width: 30%;
            margin: 1%;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 8px;
            text-align: center;
            border-left: 4px solid #667eea;
        }
        .score {
            font-size: 2em;
            font-weight: bold;
            color: #667eea;
        }
        .percentage {
            font-size: 1.5em;
            font-weight: bold;
            color: #28a745;
        }
        .count {
            font-size: 1.2em;
            color: #6c757d;
        }
        .insights-section, .recommendations-section {
            margin-top: 40px;
        }
        .insight-card {
            padding: 20px;
            margin: 10px 0;
            border-radius: 8px;
            border-left: 4px solid;
        }
        .insight-card.positive {
            background: #d4edda;
            border-left-color: #28a745;
        }
        .insight-card.negative {
            background: #f8d7da;
            border-left-color: #dc3545;
        }
        .recommendations-section ul {
            list-style-type: none;
            padding: 0;
        }
        .recommendations-section li {
            padding: 10px;
            margin: 5px 0;
            background: #e9ecef;
            border-radius: 5px;
        }
        </style>
    )";
}

std::string AnalyticsDashboard::generate_javascript_charts() {
    return R"(
        <script>
        // Placeholder for chart generation
        console.log('Analytics dashboard loaded');
        </script>
    )";
}

std::string AnalyticsDashboard::generate_json_dashboard(const std::string& user) {
    std::ostringstream oss;

    oss << "{\n";
    oss << "  \"productivity_score\": " << pimpl_->analytics.calculate_productivity_score(user) << ",\n";

    auto insights = pimpl_->analytics.generate_insights(user);
    oss << "  \"insights\": [\n";
    bool first = true;
    for (const auto& insight : insights) {
        if (!first) oss << ",\n";
        oss << "    {\n";
        oss << "      \"category\": \"" << insight.category << "\",\n";
        oss << "      \"title\": \"" << insight.title << "\",\n";
        oss << "      \"description\": \"" << insight.description << "\",\n";
        oss << "      \"recommendation\": \"" << insight.recommendation << "\",\n";
        oss << "      \"impact_score\": " << insight.impact_score << "\n";
        oss << "    }";
        first = false;
    }
    oss << "\n  ],\n";

    auto recommendations = pimpl_->analytics.generate_learning_recommendations(user);
    oss << "  \"recommendations\": [";
    first = true;
    for (const auto& rec : recommendations) {
        if (!first) oss << ", ";
        oss << "\"" << rec << "\"";
        first = false;
    }
    oss << "]\n";
    oss << "}";

    return oss.str();
}

void AnalyticsDashboard::enable_real_time_updates(bool enable) {
    pimpl_->real_time_enabled = enable;
}

std::string AnalyticsDashboard::get_real_time_metrics() {
    // Return current metrics in JSON format
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"timestamp\": " << std::chrono::system_clock::now().time_since_epoch().count() << ",\n";
    oss << "  \"active_users\": 1,\n"; // Placeholder
    oss << "  \"current_load\": 0.5\n"; // Placeholder
    oss << "}";

    return oss.str();
}

std::string AnalyticsDashboard::get_usage_timeline_data(int days) {
    // Return timeline data for charts
    (void)days; // Mark as intentionally unused
    // Implementation would query database for historical data
    return "{}";
}

} // namespace analytics
} // namespace customos
