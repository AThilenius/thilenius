//==============================================================================
//==  CRUCIBLE SOURCE CONTROL  =================================================
//==============================================================================
namespace cpp crucible.proto
namespace java com.thilenius.crucible.proto

include "cloud/sentinel/sentinel.thrift"
include "utils/differencer/differencer.thrift"

// Represents only the memtadata of a file
struct FileInfo {
  1: string relative_path;
  2: bool is_locked;
  3: string md5;
  4: string modify_timestamp;
}

const i32 FILE_TYPE_UNKNOWN = 0;
const i32 FILE_TYPE_TEXT = 1;
const i32 FILE_TYPE_URL = 2;

// Represents a file (including it's contents)
struct File {
  1: FileInfo file_info;
  2: i32 file_type;
  3: string source;
}

struct FileDelta {
  1: FileInfo file_info;
  2: list<differencer.Patch> patches;
}

// Represents an entire commit, and all data needed to fulfil the ChangeList
struct ChangeList {
  1: string change_list_uuid;
  2: string user_uuid;
  3: string timestamp;
  4: list<File> added_files;
  5: list<FileDelta> modified_files;
  6: list<FileInfo> removed_files;
}

// Only the meta information of a repo. Used for sending a list of all repos to
// the client without needing to download everything.
struct RepoHeader {
  1: string repo_uuid;
  2: string base_repo_uuid;
  3: string user_uuid;
  4: string repo_name;
  5: string creation_timestamp;
  6: string latest_change_list_uuid;
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

exception OperationFailure {
  1: string user_message;
}

service Crucible {
  // CREATE
  Repo CreateBaseRepo (1:sentinel.Token user_stoken, 2:string repo_name)
      throws (1: OperationFailure operation_failure);

  Repo CreateForkedRepo (1:sentinel.Token user_stoken, 2:string base_repo_name)
      throws (1: OperationFailure operation_failure);

  // QUERY
  list<RepoHeader> GetRepoHeadersByUser (1:sentinel.Token user_stoken)
      throws (1: OperationFailure operation_failure);

  Repo GetRepoById (1:string repo_uuid)
      throws (1: OperationFailure operation_failure);

  RepoHeader GetRepoHeaderById (1:sentinel.Token user_stoken,
                                2:string repo_uuid)
      throws (1: OperationFailure operation_failure);

  // MODIFY
  ChangeList CommitAndDownstream (1:sentinel.Token user_stoken,
                                  2:RepoHeader repo_header,
                                  3:ChangeList change_list)
      throws (1: OperationFailure operation_failure);

  // Not sure how I want to do snapshots yet...
}
