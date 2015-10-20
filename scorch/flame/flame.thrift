//==============================================================================
//==  FLAME MINECRAFT PLUGIN    ================================================
//==============================================================================
namespace cpp flame.proto
namespace java com.thilenius.flame.proto

struct Entity {
  1: string minecraft_username;
  2: string entity_uuid;
}

struct EntityMessage {
  1: Entity entity;
  2: string action_path;
  3: string message_json;
}

struct EntityResponse {
  1: string message_json;
}

exception OperationFailure {
  1: string user_message;
}

service Flame {
  list<Entity> GetAllEntites (1: string minecraft_username)
      throws (1: OperationFailure operation_failure);

  EntityResponse SendEntityMessage (1:Entity entity,
                                    2:EntityMessage entity_message)
      throws (1: OperationFailure operation_failure);
}
