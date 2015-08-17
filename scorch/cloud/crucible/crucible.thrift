//==============================================================================
//==  CRUCIBLE SOURCE CONTROL  =================================================
//==============================================================================
namespace cpp crucible

// Represents only the memtadata of a file
struct FileInfo {
  1: string realative_path;
  2: string md5;
  3: i64 modify_timestamp;
}

// Represents a file (including it's contents)
struct File {
  1: FileInfo file_info;
  // Union (one of)
  2: optional string text_source;
  3: optional binary binary_source;
  4: optional string url_source;
}

// Onlt the metadata for a commit
struct ChangeListInfo {
  1: string commit_uuid;
  2: string user_uuid;
  3: i64 timestamp;
  4: list<FileInfo> added_files;
  5: list<FileInfo> modified_files;
  6: list<FileInfo> removed_files;
}

// Represents an entire commit, and all data needed to fulfil the ChangeList
struct ChangeList {
  1: ChangeListInfo commit_info;
  2: list<File> added_modified_files;
}

// The metadata of a repo
struct RepoInfo {
  1: string repo_uuid;
  2: string user_uuid;
  3: i64 creation_timestamp;
  4: list<ChangeListInfo> change_list_infos;

  // Only set if this repo was created as a fork
  5: optional string base_repo_uuid;
}

// Represents an entire repo, including all commits (akin to a git repo)
struct Repo {
  1: RepoInfo repo_info;
  2: list<ChangeList> commits;
}

// Represents an entire repo as it existed at the point in time
struct Snapshot {
  1: RepoInfo repo_info;
  2: ChangeListInfo commit_info;
  3: list<File> files;
}

service Crucible {
  // CREATE
  // repo_uuid: Create a blank repo (user_uuid, repo_name)
  RepoInfo CreateNewRepo (1:string user_uuid, 2:string repo_name);

  // repo_uuid: Fork repo (repo_uuid)
  RepoInfo CreateForkedRepo (1:string user_uuid, 2:string base_repo_uuid);

  // QUERY
  // list<repo_meta>: Find All Repos (user_uuid)
  list<RepoInfo> GetAllRepoInfo (1:string user_uuid);

  // MODIFY
  // RepoMeta: Commit and Upstream (repo_uuid, adds_removes_modifies)
  RepoInfo CommitAndDownstream (1:string repo_uuid, 2:ChangeList change_list);

  // DOWNLOAD
  // repo Get repo snapshot (commit_uuid)
  Repo GetRepo (1:string repo_uuid);

  // Not sure how I want to do snapshots yet...
}
