//==============================================================================
//==  CRUCIBLE SOURCE CONTROL  =================================================
//==============================================================================
namespace cpp crucible

struct FileInfo {
  1: string realative_path;
  2: string md5;
  3: i64 modify_timestamp;
}

struct SourceFile {
  1: FileInfo file_info;
  2: string contents;
}

struct ExternalFile {
  1: FileInfo file_info;
  2: string url;
}

struct RepoBase {
  1: string uuid;
  2: string source_url;
  3: string source_path;
  4: i64 last_clone_timestamp;
}

struct RepoInfo {
  1: RepoBase base_id;
  2: string user_id;
  3: i64 creation_timestamp;
  4: list<SnapshotInfo> snapshot_infos;
}

struct SnapshotInfo {
  1: string snapshot_uuid;
  2: string source_repo_uuid;
  3: list<FileInfo> updated_files;

  // Not used by commiting code
  4: i64 creation_timestamp;
  5: optional i32 additions;
  6: optional i32 removals;
  7: optional i32 modifications;
}

// The Snapshot info plus ALL non-frozen files.
struct Snapshot {
  1: SnapshotInfo snapshot_info;
  2: list<SourceFile> source_files;
}

service Crucible {

  // Clone a git repo into a base image
  RepoBase CloneCrucibleBaseRepoFromGit (1:string git_url, 2:string git_path);

  // Gets (or creates) a repo from the base_uuid and returns it's meta
  // information
  RepoInfo GetRepoInfoFromBase (1:string user_uuid, 2:string base_uuid);

  // Commits a snapshot to a repo by UUID
  RepoInfo CommitSnapshot (1:string repo_uuid, 2:Snapshot snapshot);

  // Returns all non-frozen files for that snapshot
  Snapshot GetSnapshot (1:string snapshot_uuid);

  // Generates a tar file from a snapshot and returns an ExternalFile with a url
  ExternalFile GetSnapshotAsTar (1:string snapshot_id);

}
