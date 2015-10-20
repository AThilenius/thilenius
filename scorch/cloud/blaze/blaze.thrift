//==============================================================================
//==  BLAZE MINECRAFT PROXY  ===================================================
//==============================================================================
namespace cpp blaze.proto
namespace java com.thilenius.blaze.proto

include "cloud/sentinel/sentinel.thrift"

struct MinecraftAccount {
  1: string user_uuid;
  2: string minecraft_username;
}

// Passed to the command processors
struct BlazeRequestWrapper {
  1: string user_uuid;
  2: string minecraft_username;
  3: string command_name;
  4: list<string> args_json;
}

struct BlazeResponseWrapper {
  1: bool is_exception;
  2: string json;
}

exception OperationFailure {
  1: string user_message;
}

service Blaze {

  // Returns JSON
  string ProcessBlazeRequest (1: sentinel.Token token,
                              2: string command_name,
                              3: list<string> args_json)
      throws (1: OperationFailure operation_failure);

  void SetMinecraftAccount (1: sentinel.Token token,
                            2: MinecraftAccount minecraft_account)
      throws (1: OperationFailure operation_failure);

}

service BlazeCommandProcessor {

  BlazeResponseWrapper Process (1: BlazeRequestWrapper blaze_request)
      throws (1: OperationFailure operation_failure);

}
