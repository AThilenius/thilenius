//==============================================================================
//==  ANVIL PROJECT  ===========================================================
//==============================================================================

// TODO(athilenius): Move this over to SourceControl service defs
struct AnvilSourceFile {
  1: string realative_path;
  2: string md5;
}

struct AnvilSnapshot {
  1: string project_uuid;
  2: string user_token;
  3: string previous_md5;
  4: list<AnvilSourceFile> snapshot_files;
  5: list<AnvilSourceFile> frozen_files;
}
