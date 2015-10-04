//==============================================================================
//==  BILLIT DEVELOPMENT ENVINROMENT  ==========================================
//==============================================================================
namespace cpp billet.proto

include "cloud/sentinel/sentinel.thrift"
include "scorch/cloud/crucible/crucible.thrift"

struct Session {
  1: string uuid;
  2: sentinel.Token session_key;
}

// Compiler message reporting
const i32 COMPILER_MESSAGE_TYPE_UNKNOWN = 0;
const i32 COMPILER_MESSAGE_TYPE_ERROR = 1;
const i32 COMPILER_MESSAGE_TYPE_WARNING = 2;
const i32 COMPILER_MESSAGE_TYPE_NOTE = 3;

struct Fixit {
  1: i32 from_line;
  2: i32 from_column;
  3: i32 to_line;
  4: i32 to_column;
  5: i32 text;
}

struct CompilerMessage {
  1: string relative_path;
  2: i32 compiler_message_type;
  3: i32 line;
  4: i32 column;
  5: string message;
  6: string full_message;
}

struct ApplicationOutput {
  1: bool did_terminate;
  2: i32 termination_code;
  3: string standard_out;
  4: string error_out;
}

exception OperationFailure {
  1: string user_message;
}

service Billet {

  Session CreateSession(1: sentinel.Token sentinel_token)
      throws (1: OperationFailure operation_failure);

  void ExecuteCMakeRepo(1: Session session,
                        2: crucible.RepoHeader repo_header,
                        3: list<crucible.ChangeList> staged_change_lists,
                        4: list<string> application_args)
      throws (1: OperationFailure operation_failure);

  ApplicationOutput QueryOutputAfterLine(1: Session session, 2: i32 line)
      throws (1: OperationFailure operation_failure);

}
