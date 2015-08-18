//==============================================================================
//==  CRUCIBLE SOURCE CONTROL  =================================================
//==============================================================================
namespace cpp crucible

include "utils/differencer/differencer.thrift"

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

struct FileDelta {
  1: FileInfo file_info;
  2: list<differencer.Patch> patches;
}

// Represents an entire commit, and all data needed to fulfil the ChangeList
struct ChangeList {
  1: string change_list_uuid;
  2: string user_uuid;
  3: i64 timestamp;
  4: list<File> added_files;
  5: list<FileDelta> modified_files;
  6: list<FileInfo> removed_files;
}

// Only the meta information of a repo. Used for sending a list of all repos to
// the client without needing to download everything.
struct RepoHeader {
  1: string repo_uuid;
  2: optional string base_repo_uuid;
  3: string user_uuid;
  4: string repo_name;
  5: i64 creation_timestamp;
}

// Represents an entire repo
struct Repo {
  1: RepoHeader repo_header;
  2: list<ChangeList> change_lists;
}

// Represents an entire repo as it existed at the point in time
struct Snapshot {
  1: RepoHeader repo_info;
  2: ChangeList from_change_list;
  3: list<File> files;
}

service Crucible {
  // CREATE
  Repo CreateNewRepo (1:string user_uuid, 2:string repo_name);
  Repo CreateForkedRepo (1:string user_uuid, 2:string base_repo_uuid);

  // QUERY
  list<RepoHeader> GetRepoHeadersByUser (1:string user_uuid);
  Repo GetRepoById (1:string repo_uuid);

  // MODIFY
  ChangeList CommitAndDownstream (1:string repo_uuid, 2:ChangeList change_list);

  // Not sure how I want to do snapshots yet...
}
