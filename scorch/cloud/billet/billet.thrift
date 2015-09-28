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

exception OperationFailure {
  1: string user_message;
}

service Billet {

  Session CreateSession(1:sentinel.Token sentinel_token)
      throws (1: OperationFailure operation_failure);

  /*void Execute(1:Session session, 2:crucible.RepoHeader)*/
}
