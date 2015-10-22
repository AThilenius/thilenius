//==============================================================================
//==  FIBER  ================================================================
//==============================================================================
namespace cpp fiber.proto
namespace java com.thilenius.fiber.proto

include "cloud/sentinel/sentinel.thrift"

// Note: More channels (past 3) can be used
const i32 CHANNEL_UNKNOWN = 0;
const i32 CHANNEL_STD_OUT = 1;
const i32 CHANNEL_STD_ERR = 2;

struct Cord {
  1: string uuid;
  2: string name;
  3: string user_uuid;
  4: string timestamp;
}

struct Grain {
  1: i32 channel;
  2: string data;
}

exception EndOfCord { }

exception OperationFailure {
  1: string user_message;
}

service Fiber {
  Cord CreateCord (1:sentinel.Token token, 2:string name)
    throws (1:OperationFailure operation_failure);

  Cord GetCord (1:string cord_uuid)
    throws (1:OperationFailure operation_failure);

  void WriteCord (1:Cord cord, 2:list<Grain> grains)
    throws (1:OperationFailure operation_failure, 2:EndOfCord end_of_cord);

  void CloseCord (1:Cord cord)
    throws (1:OperationFailure operation_failure, 2:EndOfCord end_of_cord);

  list<Grain> ReadCordGTEIndex (1:Cord cord, 2: i32 index)
    throws (1:OperationFailure operation_failure, 2:EndOfCord end_of_cord);
}
