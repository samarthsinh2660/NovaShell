#ifndef CUSTOMOS_GIT_MANAGER_H
#define CUSTOMOS_GIT_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace customos {
namespace git {

// Git file status
enum class FileStatus {
    UNTRACKED,
    MODIFIED,
    ADDED,
    DELETED,
    RENAMED,
    COPIED,
    UNCHANGED
};

// File status info
struct GitFileStatus {
    std::string path;
    FileStatus status;
    FileStatus staged_status;
};

// Commit information
struct CommitInfo {
    std::string hash;
    std::string author;
    std::string email;
    std::string message;
    time_t timestamp;
    std::vector<std::string> parents;
};

// Branch information
struct BranchInfo {
    std::string name;
    bool is_current;
    bool is_remote;
    std::string upstream;
    int commits_ahead;
    int commits_behind;
};

// Remote information
struct RemoteInfo {
    std::string name;
    std::string url;
    std::string push_url;
};

// Git Manager
class GitManager {
public:
    static GitManager& instance();

    // Repository operations
    bool init(const std::string& path = ".");
    bool clone(const std::string& url, const std::string& destination = "");
    bool is_repository(const std::string& path = ".");

    // Status and info
    std::vector<GitFileStatus> status();
    std::string get_current_branch();
    std::string get_remote_url(const std::string& remote = "origin");
    std::map<std::string, std::string> get_config();

    // Staging area
    bool add(const std::string& path);
    bool add_all();
    bool reset(const std::string& path = "");
    bool reset_hard(const std::string& commit = "HEAD");

    // Commits
    bool commit(const std::string& message);
    bool commit_amend(const std::string& message = "");
    std::vector<CommitInfo> log(int limit = 10);
    CommitInfo get_commit(const std::string& hash);
    std::string show_commit(const std::string& hash);

    // Branches
    std::vector<BranchInfo> list_branches(bool include_remote = false);
    bool create_branch(const std::string& name);
    bool delete_branch(const std::string& name, bool force = false);
    bool checkout(const std::string& branch_or_commit);
    bool checkout_new_branch(const std::string& name);
    bool merge(const std::string& branch);
    bool rebase(const std::string& branch);

    // Remote operations
    std::vector<RemoteInfo> list_remotes();
    bool add_remote(const std::string& name, const std::string& url);
    bool remove_remote(const std::string& name);
    bool fetch(const std::string& remote = "origin");
    bool pull(const std::string& remote = "origin", const std::string& branch = "");
    bool push(const std::string& remote = "origin", const std::string& branch = "");
    bool push_set_upstream(const std::string& remote, const std::string& branch);

    // GitHub integration
    bool set_github_token(const std::string& token);
    bool create_github_repo(const std::string& name, bool is_private = false);
    bool fork_github_repo(const std::string& owner, const std::string& repo);
    bool create_pull_request(const std::string& title, const std::string& description,
                             const std::string& base, const std::string& head);

    // Diff and changes
    std::string diff(const std::string& path = "");
    std::string diff_staged();
    std::string diff_commit(const std::string& commit1, const std::string& commit2 = "");

    // Stash operations
    bool stash_save(const std::string& message = "");
    bool stash_pop();
    bool stash_apply();
    std::vector<std::string> stash_list();
    bool stash_drop(int index = 0);

    // Tags
    std::vector<std::string> list_tags();
    bool create_tag(const std::string& name, const std::string& message = "");
    bool delete_tag(const std::string& name);
    bool push_tags();

    // .gitignore generation
    bool generate_gitignore(const std::vector<std::string>& templates);
    bool add_to_gitignore(const std::string& pattern);
    std::vector<std::string> get_gitignore_templates();

    // Utility functions
    bool is_clean_working_tree();
    int get_ahead_behind_count(const std::string& branch);
    std::string get_last_commit_hash();
    std::string get_repository_root();

private:
    GitManager();
    ~GitManager();
    GitManager(const GitManager&) = delete;
    GitManager& operator=(const GitManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace git
} // namespace customos

#endif // CUSTOMOS_GIT_MANAGER_H
