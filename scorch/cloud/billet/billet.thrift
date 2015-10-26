//==============================================================================
//==  BILLIT DEVELOPMENT ENVINROMENT  ==========================================
//==============================================================================
namespace cpp billet.proto

include "cloud/sentinel/sentinel.thrift"
include "cloud/fiber/fiber.thrift"
include "scorch/cloud/crucible/crucible.thrift"

struct SessionStatus {
  // Set true if there is currently an active executable
  1: bool is_running;

  // Set to the current Fiber Cord if running, or the last cord if not running,
  // or null if nothing has been run
  2: fiber.Cord current_or_last_cord;
}

exception SessionBusy {
  // Used so JS can pick out exception type
  1: bool session_busy = true;
}

exception OperationFailure {
  1: string user_message;
}

service Billet {

  // Temporary
  fiber.Cord SyncAndExec(1: sentinel.Token token,
                         2: crucible.RepoHeader repo_header,
                         3: string shell_command)
      throws (1: OperationFailure operation_failure);

  void TerminateSession(1: sentinel.Token token)
      throws (1: OperationFailure operation_failure);

  SessionStatus GetSessionStatus(1: sentinel.Token token)
      throws (1: OperationFailure operation_failure);

  string ClangFormat(1: string source)
      throws (1: OperationFailure operation_failure);

}
