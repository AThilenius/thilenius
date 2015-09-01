//==============================================================================
//==  SENTINEL  ================================================================
//==============================================================================
namespace cpp sentinel

const i32 SECONDARY_THRESHOLD = 0;
const i32 USER_THRESHOLD = 20;
const i32 MODERATOR_THRESHOLD = 40;
const i32 ADMIN_THRESHOLD = 60;
const i32 SUPER_THRESHOLD = 80;

struct User {
  1: string uuid;
  2: string first_name;
  3: string last_name;
  4: string email_address;
  5: i32 permission_level;
}

struct Token {
  1: string user_uuid;
  2: string token_uuid;
  3: i32 permission_level;
}

exception OperationFailure {
  1: string user_message;
}

service Sentinel {
  // CREATE
  User CreateUser (1:User new_user_partial, 2:string password)
    throws (1:OperationFailure operation_failure);

  Token CreateToken (1:string email_address, 2:string password)
    throws (1:OperationFailure operation_failure);

  // permission_level: [SECONDARY_THRESHOLD, USER_THRESHOLD)
  Token CreateSecondaryToken (1:Token token, 2:i32 permission_level)
    throws (1:OperationFailure operation_failure);

  bool CheckToken (1:Token token)
    throws (1:OperationFailure operation_failure);

  // permission_level >= USER_THRESHOLD
  User FindUser (1:Token token, 2:User user_partial)
    throws (1:OperationFailure operation_failure);
}

service SentinelInternal {
  // STUB
}
