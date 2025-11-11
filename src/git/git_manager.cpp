#include "git/git_manager.h"
#include <mutex>
#include <cstdlib>
#include <sstream>
#include <fstream>

namespace customos {
namespace git {

struct GitManager::Impl {
    std::string github_token;
    std::mutex mutex;

    std::string execute_git_command(const std::string& command) {
        // Execute git command and capture output
        std::string full_cmd = "git " + command + " 2>&1";
        FILE* pipe = popen(full_cmd.c_str(), "r");
        if (!pipe) return "";

        std::string result;
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        return result;
    }

    int execute_git_command_status(const std::string& command) {
        std::string full_cmd = "git " + command + " 2>&1";
        return system(full_cmd.c_str());
    }
};

GitManager::GitManager() : pimpl_(std::make_unique<Impl>()) {
}

GitManager::~GitManager() = default;

GitManager& GitManager::instance() {
    static GitManager instance;
    return instance;
}

bool GitManager::init(const std::string& path) {
    std::string cmd = "init";
    if (!path.empty() && path != ".") {
        cmd += " " + path;
    }
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::clone(const std::string& url, const std::string& destination) {
    std::string cmd = "clone " + url;
    if (!destination.empty()) {
        cmd += " " + destination;
    }
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::is_repository(const std::string& path) {
    std::string old_cmd = path.empty() ? "" : "cd " + path + " && ";
    std::string result = pimpl_->execute_git_command("rev-parse --git-dir");
    return !result.empty() && result.find("fatal") == std::string::npos;
}

std::vector<GitFileStatus> GitManager::status() {
    // TODO: Parse git status output
    return {};
}

std::string GitManager::get_current_branch() {
    std::string result = pimpl_->execute_git_command("branch --show-current");
    // Remove newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

std::string GitManager::get_remote_url(const std::string& remote) {
    std::string cmd = "remote get-url " + remote;
    std::string result = pimpl_->execute_git_command(cmd);
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

std::map<std::string, std::string> GitManager::get_config() {
    // TODO: Parse git config
    return {};
}

bool GitManager::add(const std::string& path) {
    return pimpl_->execute_git_command_status("add " + path) == 0;
}

bool GitManager::add_all() {
    return pimpl_->execute_git_command_status("add .") == 0;
}

bool GitManager::reset(const std::string& path) {
    std::string cmd = "reset";
    if (!path.empty()) {
        cmd += " " + path;
    }
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::reset_hard(const std::string& commit) {
    return pimpl_->execute_git_command_status("reset --hard " + commit) == 0;
}

bool GitManager::commit(const std::string& message) {
    std::string cmd = "commit -m \"" + message + "\"";
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::commit_amend(const std::string& message) {
    std::string cmd = "commit --amend";
    if (!message.empty()) {
        cmd += " -m \"" + message + "\"";
    } else {
        cmd += " --no-edit";
    }
    return pimpl_->execute_git_command_status(cmd) == 0;
}

std::vector<CommitInfo> GitManager::log(int limit) {
    // TODO: Parse git log output
    return {};
}

CommitInfo GitManager::get_commit(const std::string& hash) {
    // TODO: Parse git show output
    return CommitInfo{};
}

std::string GitManager::show_commit(const std::string& hash) {
    return pimpl_->execute_git_command("show " + hash);
}

std::vector<BranchInfo> GitManager::list_branches(bool include_remote) {
    // TODO: Parse git branch output
    return {};
}

bool GitManager::create_branch(const std::string& name) {
    return pimpl_->execute_git_command_status("branch " + name) == 0;
}

bool GitManager::delete_branch(const std::string& name, bool force) {
    std::string cmd = "branch " + std::string(force ? "-D " : "-d ") + name;
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::checkout(const std::string& branch_or_commit) {
    return pimpl_->execute_git_command_status("checkout " + branch_or_commit) == 0;
}

bool GitManager::checkout_new_branch(const std::string& name) {
    return pimpl_->execute_git_command_status("checkout -b " + name) == 0;
}

bool GitManager::merge(const std::string& branch) {
    return pimpl_->execute_git_command_status("merge " + branch) == 0;
}

bool GitManager::rebase(const std::string& branch) {
    return pimpl_->execute_git_command_status("rebase " + branch) == 0;
}

std::vector<RemoteInfo> GitManager::list_remotes() {
    // TODO: Parse git remote -v output
    return {};
}

bool GitManager::add_remote(const std::string& name, const std::string& url) {
    return pimpl_->execute_git_command_status("remote add " + name + " " + url) == 0;
}

bool GitManager::remove_remote(const std::string& name) {
    return pimpl_->execute_git_command_status("remote remove " + name) == 0;
}

bool GitManager::fetch(const std::string& remote) {
    return pimpl_->execute_git_command_status("fetch " + remote) == 0;
}

bool GitManager::pull(const std::string& remote, const std::string& branch) {
    std::string cmd = "pull " + remote;
    if (!branch.empty()) {
        cmd += " " + branch;
    }
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::push(const std::string& remote, const std::string& branch) {
    std::string cmd = "push " + remote;
    if (!branch.empty()) {
        cmd += " " + branch;
    }
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::push_set_upstream(const std::string& remote, const std::string& branch) {
    return pimpl_->execute_git_command_status("push -u " + remote + " " + branch) == 0;
}

bool GitManager::set_github_token(const std::string& token) {
    std::lock_guard<std::mutex> lock(pimpl_->mutex);
    pimpl_->github_token = token;
    return true;
}

bool GitManager::create_github_repo(const std::string& name, bool is_private) {
    // TODO: Use GitHub API with token
    return false;
}

bool GitManager::fork_github_repo(const std::string& owner, const std::string& repo) {
    // TODO: Use GitHub API
    return false;
}

bool GitManager::create_pull_request(const std::string& title, const std::string& description,
                                      const std::string& base, const std::string& head) {
    // TODO: Use GitHub API
    return false;
}

std::string GitManager::diff(const std::string& path) {
    std::string cmd = "diff";
    if (!path.empty()) {
        cmd += " " + path;
    }
    return pimpl_->execute_git_command(cmd);
}

std::string GitManager::diff_staged() {
    return pimpl_->execute_git_command("diff --staged");
}

std::string GitManager::diff_commit(const std::string& commit1, const std::string& commit2) {
    std::string cmd = "diff " + commit1;
    if (!commit2.empty()) {
        cmd += " " + commit2;
    }
    return pimpl_->execute_git_command(cmd);
}

bool GitManager::stash_save(const std::string& message) {
    std::string cmd = "stash push";
    if (!message.empty()) {
        cmd += " -m \"" + message + "\"";
    }
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::stash_pop() {
    return pimpl_->execute_git_command_status("stash pop") == 0;
}

bool GitManager::stash_apply() {
    return pimpl_->execute_git_command_status("stash apply") == 0;
}

std::vector<std::string> GitManager::stash_list() {
    // TODO: Parse stash list
    return {};
}

bool GitManager::stash_drop(int index) {
    return pimpl_->execute_git_command_status("stash drop stash@{" + std::to_string(index) + "}") == 0;
}

std::vector<std::string> GitManager::list_tags() {
    // TODO: Parse tag list
    return {};
}

bool GitManager::create_tag(const std::string& name, const std::string& message) {
    std::string cmd = "tag";
    if (!message.empty()) {
        cmd += " -a " + name + " -m \"" + message + "\"";
    } else {
        cmd += " " + name;
    }
    return pimpl_->execute_git_command_status(cmd) == 0;
}

bool GitManager::delete_tag(const std::string& name) {
    return pimpl_->execute_git_command_status("tag -d " + name) == 0;
}

bool GitManager::push_tags() {
    return pimpl_->execute_git_command_status("push --tags") == 0;
}

bool GitManager::generate_gitignore(const std::vector<std::string>& templates) {
    // TODO: Download from gitignore.io or use templates
    std::ofstream file(".gitignore");
    if (!file) return false;
    
    for (const auto& tmpl : templates) {
        file << "# " << tmpl << "\n";
        // Add template content
    }
    
    return true;
}

bool GitManager::add_to_gitignore(const std::string& pattern) {
    std::ofstream file(".gitignore", std::ios::app);
    if (!file) return false;
    
    file << pattern << "\n";
    return true;
}

std::vector<std::string> GitManager::get_gitignore_templates() {
    return {"Node", "Python", "Java", "C++", "VisualStudio", "macOS", "Linux", "Windows"};
}

bool GitManager::is_clean_working_tree() {
    std::string result = pimpl_->execute_git_command("status --porcelain");
    return result.empty();
}

int GitManager::get_ahead_behind_count(const std::string& branch) {
    // TODO: Parse ahead/behind count
    return 0;
}

std::string GitManager::get_last_commit_hash() {
    std::string result = pimpl_->execute_git_command("rev-parse HEAD");
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

std::string GitManager::get_repository_root() {
    std::string result = pimpl_->execute_git_command("rev-parse --show-toplevel");
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

} // namespace git
} // namespace customos
